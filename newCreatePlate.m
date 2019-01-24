function [B, C, N, Nx, Ny, h, kappa, D] = newCreatePlate (Lx, Ly, rho, H, D, s0, s1, k)
    
    kappa = sqrt(D / (rho * H))/(Lx*Ly);
    LWRatio = Lx/Ly;
    h = 2*sqrt(k*(s1^2+sqrt(kappa^2+s1^2)));

    Nx = floor(sqrt(LWRatio)/h);
    Ny = floor(1/(sqrt(LWRatio)*h));
    
    h = max(sqrt(LWRatio)/Nx, 1/(sqrt(LWRatio)*Ny));
    N = (Nx-1)*(Ny-1);
    
    % generate scheme matrices
    Dxx = sparse(toeplitz([-2;1;zeros(Nx-3,1)]));
    Dyy = sparse(toeplitz([-2;1;zeros(Ny-3,1)]));
    D = kron(speye(Nx-1), Dyy)+kron(Dxx, speye(Ny-1)); 
    DD = D*D/h^4; 
    B = sparse((2*speye(N)-kappa^2*k^2*DD)/(1+s0*k));
    C = -((1-s0*k)/(1+s0*k))*speye(N); 
    
end