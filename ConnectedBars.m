clear all;
close all;
clc;

fs = 44100;      % Sampling rate
k = 1 / fs;     % Time step
L = 1;          % Bar length

% kappa = sqrt((E * inertia) / (rho * A * L^2));  % Stiffness
kappaA = 10;
s0A = 0.1;
s1A = 0.005;
hA = sqrt(2 * k * (s1A^2 + sqrt(kappaA^2 + s1A^2)));
NA = floor(1/hA); % Number of gridpoints
hA = 1/NA; % Recalculate gridspacing

kappaB = 50;
s0B = 0.1;
s1B = 0.005;
hB = sqrt(2 * k * (s1B^2 + sqrt(kappaB^2 + s1B^2)));
NB = floor(1/hB); % Number of gridpoints
hB = 1/NB; % Recalculate gridspacing
% muSq = (k * kappa / h^2)^2; % Courant number squared (always 0.25)
% lambdaSq = (k^2*kappa^2) / h^4;

P = 1/5; % plucking position

%% Raised cosine input
% cosWidth = floor(NA / 10);
% raisedCos = 0.5 * (cos(pi:(2*pi)/cosWidth:3*pi) + 1);
% PIdx = floor (P * NA);

%% Initialise state vectors
u = zeros(NA + NB, 1);
% u(ceil(NA * P - cosWidth / 2 : ceil(NA * P + cosWidth / 2))) = raisedCos;
% u(ceil(NA + NA * P - cosWidth / 2 : NA + ceil(NA * P + cosWidth / 2))) = raisedCos;
uPrev = u;
uNext = zeros(NA + NB, 1);

lengthSound = fs*5;


%% Exciter
Fmax = 50000000000;
exciterLength = floor(1e-3 * fs);
q = 2; 
Fe = Fmax/2*(1-cos(q*pi*(0:exciterLength)/exciterLength));
exciter = [Fe, zeros(1,lengthSound-exciterLength - 1)];

rcW = 3;
Ee = zeros(NA + NB, 1);
Ee(floor(NA/4):floor(NA/4)+rcW) = 1-cos(q*pi*(0:rcW)/rcW);
K = k^2 * Ee / (1 + s0A*k);

%% Connections
Q = 1; % amount of connections
EcA = zeros(NA, 1);
connectionWidth = 5;

EcA(floor(NA/5):floor(NA/5)+connectionWidth) = (1-cos(2*pi*(0:connectionWidth)/connectionWidth));
jA = (k^2 * EcA) / (1 + s0A * k);

% connA = find(EcA > 0);
EcB = zeros(NB, 1);
EcB(5) = 1;
massratio = 1; 
% EcB(floor(NA/5):floor(NA/5)+connectionWidth) = (1-cos(2*pi*(0:connectionWidth)/connectionWidth));
jB = -(k^2 * EcB * massratio) / (1 + s0B * k);
J = [jA; jB];

%% Spring coefficients 

sx = 0; 
w0 = 1; 
w1 = 4000; 




%% Matrix representation
NA = NA - 4;
NB = NB - 4;

DxxxxA = (sparse(3:NA, 1:NA-2, ones(1, NA-2), NA, NA) + ...
        sparse(2:NA, 1:NA-1, -4 * ones(1, NA-1), NA, NA) + ...
        sparse(1:NA, 1:NA, 6 * ones(1, NA), NA, NA) + ...
        sparse(1:NA-1, 2:NA, -4 * ones(1, NA-1), NA, NA) + ...
        sparse(1:NA-2, 3:NA, ones(1, NA-2), NA, NA));
DxxA =   (sparse(2:NA, 1:NA-1, ones(1, NA-1), NA, NA) + ...
        sparse(1:NA, 1:NA, -2 * ones(1, NA), NA, NA) + ...
        sparse(1:NA-1, 2:NA, ones(1, NA-1), NA, NA));
    
DxxxxB = (sparse(3:NB, 1:NB-2, ones(1, NB-2), NB, NB) + ...
        sparse(2:NB, 1:NB-1, -4 * ones(1, NB-1), NB, NB) + ...
        sparse(1:NB, 1:NB, 6 * ones(1, NB), NB, NB) + ...
        sparse(1:NB-1, 2:NB, -4 * ones(1, NB-1), NB, NB) + ...
        sparse(1:NB-2, 3:NB, ones(1, NB-2), NB, NB));
DxxB =   (sparse(2:NB, 1:NB-1, ones(1, NB-1), NB, NB) + ...
        sparse(1:NB, 1:NB, -2 * ones(1, NB), NB, NB) + ...
        sparse(1:NB-1, 2:NB, ones(1, NB-1), NB, NB));
 
NA = NA + 4;
NB = NB + 4;

DxxxxA2 = zeros(NA);
DxxxxA2(3:end-2, 3:end-2) = DxxxxA;
DxxxxA = DxxxxA2;

DxxA2 = zeros(NA);
DxxA2(3:end-2, 3:end-2) = DxxA;
DxxA = DxxA2;

DxxxxB2 = zeros(NB);
DxxxxB2(3:end-2, 3:end-2) = DxxxxB;
DxxxxB = DxxxxB2;

DxxB2 = zeros(NB);
DxxB2(3:end-2, 3:end-2) = DxxB;
DxxB = DxxB2;

BA = (2 * eye(NA) - kappaA^2 * k^2 * DxxxxA / hA^4 + 2 * s1A * k * DxxA / hA^2) / (1 + s0A * k);
CA = -((1 - s0A * k) * eye(NA) + 2 * s1A * k * DxxA / hA^2) / (1 + s0A * k);

BB = (2 * eye(NB) - kappaB^2 * k^2 * DxxxxB / hB^4 + 2 * s1B * k * DxxB / hB^2) / (1 + s0B * k);
CB = -((1 - s0B * k) * eye(NB) + 2 * s1B * k * DxxB / hB^2) / (1 + s0B * k);

zeroMat = zeros(NA, NB);


B = [BA zeroMat; zeroMat' BB]; 
C = [CA zeroMat; zeroMat' CB]; 

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);
drawBar = false;

etaRPrev = 0;
L = [hA * EcA', -hB * EcB']; % Add rows for more connections 

for n = 1 : lengthSound
    
    % calculate relative displacement
    etaR = L*u;
    
    % update r and p
    rn = (2*sx/k - w0^2 - w1^4*(etaR)^2)/(2*sx/k + w0^2 + w1^4*(etaR)^2);
    pn = -2/(2*sx/k + w0^2 + w1^4*(etaR)^2);
    
    Rn = eye(Q)*rn; 
    Pn = eye(Q)*pn; 
    
    % temporary value for the next u (without the spring forces)
    uTemp = B * u + C * uPrev + K*exciter(n);
    
    % find Fc
    bn = L * uTemp; 
    an = Rn * etaRPrev; 
    
    Fc = (an - bn)/(L*J-Pn);
 
    uNext = uTemp + J * Fc; 
    
    out(n) = u(round(NA/5));
    out2(n) = u(round(NA + NB/5));
    if mod(n,1) == 0 && drawBar == true
        subplot(2,1,1)
        plot(u(1:NA))
        hold on;
        
        subplot(2,1,2)
        plot(u(NA+1:end));
%         plot (J)
%         ylim([-1 1])
        drawnow;
    end
    uPrev = u;
    u = uNext;
    etaRPrev = etaR;
end

plot(out2)