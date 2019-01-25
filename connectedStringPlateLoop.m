
function [maxTotEnergyDiff] = connectedStringPlateLoop (fs, rhoS, r, T, kappaS, rhoP, H, EP, Lx, Ly)


%% String
k = 1/fs;
A = r^2 * pi;
L = 1; % Length of the string [m]
s0S = 0; % Frequency independent damping coefficient
s1S = 0; % Frequency dependent damping coefficient
c = sqrt(T / (rhoS * A));

[BS, CS, NS, hS, DS, DS4] = newCreateString (c, kappaS, L, s0S, s1S, k);

%% Plate
nu = 0.3; % Poissons ratio [unitless]
D = (EP * H^3) / (12 * (1-nu^2));
s0P = 0;
s1P = 0;

[BP, CP, NP, Nx, Ny, hP, kappaP, DP] = newCreatePlate (Lx, Ly, rhoP, H, D, s0P, s1P, k);
Nx = Nx - 1;
Ny = Ny - 1;

%% Create full matrices
Ntot = NS + NP;
B = zeros(Ntot);
C = zeros(Ntot);

B(1:NS, 1:NS) = BS;
B(NS+1:NS+NP, NS+1:NS+NP) = BP;
C(1:NS, 1:NS) = CS;
C(NS+1:NS+NP, NS+1:NS+NP) = CP;


%% Initialise state vectors
u = zeros(Ntot, 1);
excitePlate = false;
exciteString = true;

%% Excite
if excitePlate
    exciterPosX = 0.75;
    exciterPosY = 0.75;
    rcW = floor(min(Nx,Ny) / 10);
    excitationMat = zeros(rcW+1, rcW+1);
    scaler = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
    for x = 1:rcW+1
        excitationMat(x,:) = scaler(x) * (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
    end
    startIdxX = floor(Nx * exciterPosX - rcW/2) - 1;
    startIdxY = floor(Ny * exciterPosY - rcW/2) - 1;
    for i = 1 : rcW 
       u(NS + (startIdxY + i) * Nx + startIdxX : NS + (startIdxY + i) * Nx + startIdxX + rcW) = ...  
            excitationMat(i,:);
    end  
end
if exciteString
    exciterPos = 0.25;
    rcW = 5;
    u(1 + floor(exciterPos*NS-rcW/2):1 + floor(exciterPos*NS+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
end
uPrev = u;
uNext = u;

%% Connection points
connS = 0.5;
connP = 0.5;

I = zeros(Ntot, 1);
I(floor(1+connS * NS)) = 1;
I(floor(1+NS + connP * NP)) = -1;

J = zeros(Ntot, 1);
J(floor(1+connS * NS)) = k^2 / (rhoS * A * hS);
J(floor(NS + 1 + connP * NP)) = -k^2 / (rhoP * H * hP^2);


%% Length of the sound
lengthSound = round(fs / 50);

potEnergyString = zeros(lengthSound, 1);
kinEnergyString = zeros(lengthSound, 1);
potEnergyPlate = zeros(lengthSound, 1);
kinEnergyPlate = zeros(lengthSound, 1);

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);
drawBar = false;

stringVec = 2:NS-1;

connected = true;

JFc = zeros(Ntot);
testmat = ones(Nx, Ny);
testmatPrev = ones(Nx, Ny);

for n = 1 : lengthSound
    Fc = -(c^2 * k^2 * I(1:NS)' * DS * u(1:NS) ...
        - kappaS^2 * k^2 * I(1:NS)' * DS4 * u(1:NS) ...
        - kappaP^2 * k^2 * I(NS+1:end)' * DP * u(NS+1:end))...
        / (I(1:NS)' * J(1:NS) + I(NS+1:end)' * J(NS+1:end));

    JFc = J*Fc;

    uNext = B * u + C * uPrev + JFc; 

    potEnergyString(n) = c^2 / 2 * sum (1 / hS * (u(stringVec + 1) - u(stringVec)) .* (uPrev(stringVec + 1) - uPrev(stringVec))) ...
        + kappaS^2 / 2 * 1/hS^3 * sum(DS * u(1:NS) .* DS * uPrev(1:NS));
    kinEnergyString(n) = 1 / 2 * sum (hS * ((1 / k * (u(stringVec) - uPrev(stringVec))).^2));
    
    kinEnergyPlate(n) = 1 / 2 * hP^2 * sum(sum(1/k^2 * (u(NS + 1 : end) - uPrev(NS + 1 : end)).^2));
    potEnergyPlate(n) = kappaP^2 / (2 * hP^2) * sum(DP * u(NS + 1 : end) .* (DP * uPrev(NS + 1 : end)));

    uPrev = u;
    u = uNext;

end
% subplot(2,1,1)
% plot(out)
% subplot(2,1,2)
% plot(out2)


% figure;
totEnergyString = kinEnergyString + potEnergyString;
% totEnergyStringPlot = (totEnergyString-totEnergyString(1))/totEnergyString(1);
% plot(totEnergyStringPlot)
% 
% figure;
totEnergyPlate = kinEnergyPlate + potEnergyPlate;
% totEnergyPlatePlot = (totEnergyPlate-totEnergyPlate(1))/totEnergyPlate(1);
% plot(totEnergyPlatePlot)
% 
% figure;
totEnergy = totEnergyString+totEnergyPlate;
totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
maxTotEnergyDiff = (max(totEnergy) + abs(min(totEnergy)));
% plot(totEnergy)