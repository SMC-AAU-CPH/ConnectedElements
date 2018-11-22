function [B, C, N, h, s0] = createBar(objectVars, fs)
    kappa = objectVars(1);
    s0 = objectVars(2);
    s1 = objectVars(3);
    
    k = 1/fs;
    h = sqrt(2 * k * (s1^2 + sqrt(kappa^2 + s1^2)));
    N = floor(1/h); % Number of gridpoints
    h = 1/N; % Recalculate gridspacing
    
    N = N - 4;

    Dxxxx = (sparse(3:N, 1:N-2, ones(1, N-2), N, N) + ...
            sparse(2:N, 1:N-1, -4 * ones(1, N-1), N, N) + ...
            sparse(1:N, 1:N, 6 * ones(1, N), N, N) + ...
            sparse(1:N-1, 2:N, -4 * ones(1, N-1), N, N) + ...
            sparse(1:N-2, 3:N, ones(1, N-2), N, N));
    Dxx =   (sparse(2:N, 1:N-1, ones(1, N-1), N, N) + ...
            sparse(1:N, 1:N, -2 * ones(1, N), N, N) + ...
            sparse(1:N-1, 2:N, ones(1, N-1), N, N));

    B = (2 * eye(N) - kappa^2 * k^2 * Dxxxx / h^4 + 2 * s1 * k * Dxx / h^2) / (1 + s0 * k);
    C = -((1 - s0 * k) * eye(N) + 2 * s1 * k * Dxx / h^2) / (1 + s0 * k);
    
    N = N + 4;
    
    B2 = zeros(N);
    B2(3:end-2, 3:end-2) = B;
    B = B2;

    C2 = zeros(N);
    C2(3:end-2, 3:end-2) = C;
    C = C2;
end