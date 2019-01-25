function [B, C, ss, sigma0, Nx, Ny, h, kappa, D] = createPlate (objectVars, fs)    
    Lx = objectVars(1);
    Ly = objectVars(2);
    H = objectVars(3);
    rho = objectVars(4);
    T60 = objectVars(5);
    sigma1 = objectVars(6);
    
    k = 1/fs;
    E = 2e11;
    nu = 0.3;
    rho = 7850;
    kappa = sqrt((2e11*H^2)/(12*7850*(1-0.3^2)))/(Lx*Ly);
    
%     K_over_A = kappa/(Lx*Ly); % stiffness parameter/area
    LWRatio = Lx/Ly;
    h = 2*sqrt(k*(sigma1^2+sqrt(kappa^2+sigma1^2)));
    sigma0 = 6*log(10)/T60;
%     X = 2*sqrt(K_over_A*k); %gridspacing
    
    Nx = floor(sqrt(LWRatio)/h);
    Ny = floor(1/(sqrt(LWRatio)*h));
%     X = sqrt(LWRatio)/Nx;
    h = max(sqrt(LWRatio)/Nx, 1/(sqrt(LWRatio)*Ny));
    ss = (Nx-1)*(Ny-1);
    % generate scheme matrices
    Dxx = sparse(toeplitz([-2;1;zeros(Nx-3,1)]));
    Dyy = sparse(toeplitz([-2;1;zeros(Ny-3,1)]));
    D = kron(speye(Nx-1), Dyy)+kron(Dxx, speye(Ny-1)); 
    DD = D*D/h^4; 
    B = sparse((2*speye(ss)-kappa^2*k^2*DD)/(1+sigma0*k));
    C = -((1-sigma0*k)/(1+sigma0*k))*speye(ss); 
    
end