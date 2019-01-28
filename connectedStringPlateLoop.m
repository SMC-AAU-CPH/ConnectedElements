function [maxTotEnergyDiff] = connectedStringPlateLoop (fs, rhoS, r, T, rhoP, H, EP, Lx, Ly, connS, connPX, connPY)


%% String
k = 1/fs;
A = r^2 * pi;
L = 1; % Length of the string [m]
s0S = 0; % Frequency independent damping coefficient
s1S = 0; % Frequency dependent damping coefficient
c = sqrt(T / (rhoS * A));
IS = r^4 * pi/4;
ES = 2e11;
kappaS = sqrt(ES * IS / (rhoS * A));
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
excitePlate = true;
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
    exciterPos = 0.75;
    rcW = 5;
    u(1 + floor(exciterPos*NS-rcW/2):1 + floor(exciterPos*NS+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
end
uPrev = u;
uNext = u;

%% Connection points
% connS = 0.5;
% connP = 0.5;

I = zeros(Ntot, 1);
J = zeros(Ntot, 1);

cWS = 2;
I(1 + floor(connS*NS-cWS/2):1 + floor(connS*NS+cWS/2)) = (1-cos(2*pi*(0:cWS)/cWS)) * 0.5;
J(1 + floor(connS*NS-cWS/2):1 + floor(connS*NS+cWS/2)) = (1-cos(2*pi*(0:cWS)/cWS)) * 0.5 * k^2 / (rhoS * A * hS);

cWP = 2;
startIdxX = floor(Nx * connPX - cWP/2);
startIdxY = floor(Ny * connPY - cWP/2);

connMat = zeros(cWP+1, cWP+1);
scaler = (1-cos(2*pi*(0:cWP)/cWP)) * 0.5;
for x = 1:cWP+1
    connMat(x,:) = scaler(x) * (1-cos(2*pi*(0:cWP)/cWP)) * 0.5;
end
Itest = reshape(I(NS+1:end), Nx, Ny);
Jtest = reshape(J(NS+1:end), Nx, Ny);
% for i = 1 : cWP
%     I(NS + (startIdxY + i) * Nx + startIdxX : NS + (startIdxY + i) * Nx + startIdxX + cWP) = -connMat(i,:); 
%     J(NS + (startIdxY + i) * Nx + startIdxX : NS + (startIdxY + i) * Nx + startIdxX + cWP) = ...
%     -connMat(i,:) * k^2 / (rhoP * H * hP^2);
Itest(startIdxX : startIdxX+cWP, startIdxY : startIdxY+cWP) = -connMat;
Jtest(startIdxX : startIdxX+cWP, startIdxY : startIdxY+cWP) = -connMat * k^2 / (rhoP * H * hP^2);
I(NS+1 : end) = reshape(Itest, NP, 1);
J(NS+1 : end) = reshape(Jtest, NP, 1);
% end  
%     
% I(floor(1+NS + connP * NP)) = -1;
% 
% J(floor(NS + 1 + connP * NP)) = -k^2 / (rhoP * H * hP^2);


%% Length of the sound
lengthSound = round(fs / 50);

potEnergyString = zeros(lengthSound, 1);
kinEnergyString = zeros(lengthSound, 1);
potEnergyPlate = zeros(lengthSound, 1);
kinEnergyPlate = zeros(lengthSound, 1);

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);

stringVec = 2:NS-1;

connected = true;

JFc = zeros(Ntot);
testmat = ones(Nx, Ny);
testmatPrev = ones(Nx, Ny);

drawState = false;
% figure;
idx = find(J~=0);
cIX = connPX * Nx;
cIY = connPY * Ny;
for n = 1 : lengthSound
    if connected
        Fc = (c^2 * k^2 * I(1:NS)' * (DS / hS^2) * u(1:NS) ...
            - kappaS^2 * k^2 * I(1:NS)' * (DS4 / hS^4) * u(1:NS)...
            - kappaP^2 * k^2 * I(NS+1:end)' * ((DP * DP) / hP^4) * u(NS+1:end))...
            / -(I' * J);
%         testmat = reshape(u(NS+1:end), Ny, Nx);
        JFc = J*Fc;
    else 
        JFc = 0;
    end
    uNext = B * u + C * uPrev + JFc; 

    I' * u
    
    kinEnergyString(n) = (rhoS * A) / 2 * sum (hS * ((1 / k * (u(stringVec) - uPrev(stringVec))).^2));
    potEnergyString(n) = T / 2 * sum (1 / hS * (u(stringVec + 1) - u(stringVec)) .* (uPrev(stringVec + 1) - uPrev(stringVec))) ...
        + (ES * IS) / 2 * 1/hS^3 * sum((DS * u(1:NS)) .* (DS * uPrev(1:NS)));
    
    kinEnergyPlate(n) = (rhoP * H) / 2 * hP^2 * sum(sum(1/k^2 * (u(NS + 1 : end) - uPrev(NS + 1 : end)).^2));
    potEnergyPlate(n) = D / (2 * hP^2) * sum((DP * u(NS + 1 : end)) .* (DP * uPrev(NS + 1 : end)));

    clf
    subplot(3,2,1)
    plot(kinEnergyString(1:n))
    hold on;
    plot(potEnergyString(1:n));
    subplot(3,2,2)
    totEnergyString(n) = kinEnergyString(n) + potEnergyString(n);
    totEnergyStringPlot(n) = (totEnergyString(n)-totEnergyString(1))/totEnergyString(1);
    plot(totEnergyStringPlot(1:n))

    subplot(3,2,3)
    plot(kinEnergyPlate(1:n))
    hold on;
    plot(potEnergyPlate(1:n));
    
    subplot(3,2,4)
    totEnergyPlate(n) = kinEnergyPlate(n) + potEnergyPlate(n);
    totEnergyPlatePlot(n) = (totEnergyPlate(n)-totEnergyPlate(1))/totEnergyPlate(1);
    plot(totEnergyPlatePlot(1:n))
    title("Total Plate Energy")
    
    subplot(3,2,5)
    totEnergy = totEnergyString + totEnergyPlate;
    totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
%     maxTotEnergyDiff = (max(totEnergy) + abs(min(totEnergy)));
    plot(totEnergy)
    title("Total Energy")
    drawnow;

    uPrev = u;
    u = uNext;
    if drawState% && mod(n, 10) == 0
        clf
        subplot(2,1,1)
        plot (u(1:NS))
        hold on;
        scatter(floor(connS * NS), 0);
        subplot(2,1,2)
        if sum(u(NS+1:end))~=0
            
        end
        testmat = reshape(u(NS+1:end), Ny, Nx);
%         for q = 0:Nx-1
%             testmat(q+1, 1:Ny) = u(NS+1+q*Ny:NS+(q+1)*Ny);
%         end
        imagesc(testmat)
%         zlim([-1e-7, 1e-7]);
        
    %     plot(u(NS+1:end))
        drawnow;
    end
    
%     clf;
%     scatter(uNext(idx(1)), 1)
%     hold on;
%     scatter(uNext(idx(2)), 2)
%     drawnow;
end
% figure;
totEnergyString = kinEnergyString + potEnergyString;
totEnergyPlate = kinEnergyPlate + potEnergyPlate;
totEnergy = totEnergyString + totEnergyPlate;
subplot(3, 1, 2)
plot(totEnergyString);
title("String")
subplot(3, 1, 2)
plot(totEnergyPlate);
title("Plate")
subplot(3,1,3)
totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
plot(totEnergy);
title("Total")
maxTotEnergyDiff = (max(totEnergy) + abs(min(totEnergy)));
drawnow;