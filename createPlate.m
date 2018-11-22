function [B, C, ss, X, sigma, Nx, Ny] = createPlate(objectVars, fs)    
    Lx = objectVars(1);
    Ly = objectVars(2);
    H = objectVars(3);
    T60 = objectVars(4);
    
    k = 1/fs;
    E = 2e11;
    nu = 0.3;
    rho = 7850;
    K_over_A = sqrt(E*H^2/(12*rho*(1-nu^2)))/(Lx*Ly); % stiffness parameter/area
    LWRatio = Lx/Ly;

    sigma = 6*log(10)/T60;
    X = 2*sqrt(K_over_A*k); %gridspacing
    Nx = floor(sqrt(LWRatio)/X);
    Ny = floor(1/(sqrt(LWRatio)*X));
    X = sqrt(LWRatio)/Nx;
    ss = (Nx-1)*(Ny-1);
    % generate scheme matrices
    Dxx = sparse(toeplitz([-2;1;zeros(Nx-3,1)]));
    Dyy = sparse(toeplitz([-2;1;zeros(Ny-3,1)]));
    D = kron(speye(Nx-1), Dyy)+kron(Dxx, speye(Ny-1)); 
    DD = D*D/X^4; 
    B = sparse((2*speye(ss)-K_over_A^2*k^2*DD)/(1+sigma*k));
    C = ((1-sigma*k)/(1+sigma*k))*speye(ss); 
    
end