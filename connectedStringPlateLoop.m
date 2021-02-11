function [maxTotEnergyDiff] = connectedStringPlateLoop (fs, rhoS, r, T, rhoP, H, EP, Lx, Ly, connS, connPX, connPY, w0, w1, exciteType)


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
if exciteType == 0
    excitePlate = true;
    exciteString = true;
elseif exciteType == 1
    excitePlate = true;
    exciteString = false;
elseif exciteType == 2
    excitePlate = false;
    exciteString = true;
end

%% Excite
if excitePlate
    exciterPosX = 0.75;
    exciterPosY = 0.75;
%     rcW = floor(min(Nx,Ny) / 5);
    rcW = 2;
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
    rcW = 2;
    u(1 + floor(exciterPos*NS-rcW/2):1 + floor(exciterPos*NS+rcW/2)) = 100 * (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
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
J(1 + floor(connS*NS-cWS/2):1 + floor(connS*NS+cWS/2)) = (1-cos(2*pi*(0:cWS)/cWS)) * 0.5 * k^2 / (rhoS * A * hS * (1 + s0S * k));

cWP = 2;
startIdxX = floor(Nx * connPX - cWP/2);
if startIdxX == 0
    startIdxX = 1;
end
startIdxY = floor(Ny * connPY - cWP/2);
if startIdxY == 0
    startIdxY = 1;
end

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

%%For area of connection
% Itest(startIdxX : startIdxX+cWP, startIdxY : startIdxY+cWP) = -connMat;
% Jtest(startIdxX : startIdxX+cWP, startIdxY : startIdxY+cWP) = -connMat * k^2 / (rhoP * H * hP^2 * (1 + s0P * k));

Itest(startIdxX + 1, startIdxY + 1) = -1;
Jtest(startIdxX + 1, startIdxY + 1) = -1 * k^2 / (rhoP * H * hP^2 * (1 + s0P * k));
I(NS+1 : end) = reshape(Itest, NP, 1);
J(NS+1 : end) = reshape(Jtest, NP, 1);
% end  
%     
% I(floor(1+NS + connP * NP)) = -1;
% 
% J(floor(NS + 1 + connP * NP)) = -k^2 / (rhoP * H * hP^2);


%% Length of the sound
lengthSound = round(fs/50);

potEnergyString = zeros(lengthSound, 1);
kinEnergyString = zeros(lengthSound, 1);
totEnergyString = zeros(lengthSound, 1);
totEnergyStringPlot = zeros(lengthSound, 1);
potEnergyPlate = zeros(lengthSound, 1);
kinEnergyPlate = zeros(lengthSound, 1);
totEnergyPlate = zeros(lengthSound, 1);
totEnergyPlatePlot = zeros(lengthSound, 1);
totEnergy = zeros(lengthSound, 1);
totEnergyPlot = zeros(lengthSound, 1);
energyConnection = zeros(lengthSound, 1);
totEnergyPlusConn = zeros(lengthSound, 1);
totEnergyPlusConnPlot = zeros(lengthSound, 1);

curve = zeros(lengthSound, 1);

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);

% w0 = 100;
% w1 = 1000000;
sx = 0;

stringVec = 2:NS-1;

connected = false;

% JFc = zeros(Ntot);
% testmat = ones(Nx, Ny);
% testmatPrev = ones(Nx, Ny);

drawState = true;
drawEnergy = false;
% figure;
idx = find(J~=0);
cIX = connPX * Nx;
cIY = connPY * Ny;

% %predraw
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
etaPrev = 0;
eta = 0;
L = [I(1:NS) * hS; I(NS+1:end) * hP^2]';

for n = 1 : lengthSound
    eta = I' * u;
    uNext = B * u + C * uPrev; 
    if connected
%         rn = (2*sx/k - w0.^2 - w1.^4.*(eta).^2)./(2*sx/k + w0.^2 + w1.^4.*(eta).^2);
%         pn = -2./(2*sx/k + w0.^2 + w1.^4.*(eta).^2);
%         Fc = (c^2 * k^2 * I(1:NS)' * (DS / hS^2) * u(1:NS) ...
%             - kappaS^2 * k^2 * I(1:NS)' * (DS4 / hS^4) * u(1:NS)...
%             - kappaP^2 * k^2 * I(NS+1:end)' * ((DP * DP) / hP^4) * u(NS+1:end))...
%             / -(I' * J);
        %this will only screw up if the connection is on or next to the
        %point of excitation
        varPhi = w0/4 + (w1 * eta^2)/2;
        Fc = (-I' * uNext * varPhi - (w0 * eta) / 2 - etaPrev * varPhi) ...
            / (I' * J * varPhi + 1);

%         Fc = (-I' * uNext - 2 * eta - etaPrev) / (I' * J + 4 / w0);
%         testmat = reshape(u(NS+1:end), Ny, Nx);
        JFc = J*Fc;
    else 
        JFc = 0;
    end
    uNext = uNext + JFc;
    
    kinEnergyString(n) = ((rhoS * A) / 2) * sum (hS * ((1 / k * (u(stringVec) - uPrev(stringVec))).^2));
    potEnergyString(n) = T / 2 * sum (1 / hS * (u(stringVec + 1) - u(stringVec)) .* (uPrev(stringVec + 1) - uPrev(stringVec))) ...
        + (ES * IS) / 2 * 1/hS^3 * sum((DS * u(1:NS)) .* (DS * uPrev(1:NS)));
%     curve(n) = -s0S * sum (hS * ((1 / k * (u(stringVec) - uPrev(stringVec))).^2));
    
    kinEnergyPlate(n) = ((rhoP * H) / 2) * hP^2 * sum(sum(1/k^2 * (u(NS + 1 : end) - uPrev(NS + 1 : end)).^2));
    potEnergyPlate(n) = D / (2 * hP^2) * sum((DP * u(NS + 1 : end)) .* (DP * uPrev(NS + 1 : end)));

    
%     energyConnection(n) = w0 / 2 * (1/2 * (eta + etaPrev))^2;

    energyConnection(n) = w0/8 * ((eta + etaPrev))^2 + w1 / 4 * (eta^2 * etaPrev^2);
    
    etaPrev = eta;
    
    totEnergyString(n) = kinEnergyString(n) + potEnergyString(n);
    totEnergyStringPlot(n) = (totEnergyString(n)-totEnergyString(1))/totEnergyString(1);
    totEnergyPlate(n) = kinEnergyPlate(n) + potEnergyPlate(n);
    totEnergyPlatePlot(n) = (totEnergyPlate(n)-totEnergyPlate(1))/totEnergyPlate(1);
    totEnergy(n) = totEnergyString(n) + totEnergyPlate(n);
    totEnergyPlot(n) = (totEnergy(n)-totEnergy(1))/totEnergy(1);
    totEnergyPlusConn(n) = totEnergyString(n) + totEnergyPlate(n) + energyConnection(n);
    totEnergyPlusConnPlot(n) = (totEnergyPlusConn(n)-totEnergyPlusConn(1))/totEnergyPlusConn(1);
        
        
    if drawEnergy == true && mod(n, 10) == 0

        clf
        subplot(3,2,1)
%         plot(kinEnergyString(1:n))
%         hold on;
%         plot(potEnergyString(1:n));
%         title("Kinetic and Potential Energy String ")
%         subplot(3,2,2)
        plot(totEnergyString(1:n))
        title("Total String Energy")

        
        subplot(3,2,2)
%         plot(kinEnergyString2(1:n))
%         hold on;
%         plot(potEnergyString2(1:n));
%         title("Kinetic and Potential Energy String 2")
%         subplot(3,2,4)
        plot(totEnergyPlate(1:n))
        title("Total Plate Energy")

        subplot(3,2,3)
        plot(energyConnection(1:n))
        title("Connection Energy")
        
        subplot(3,2,4);
        plot(totEnergy(1:n))
        title("TotEnergy Without Connection")
        
        subplot(3,2,5)
        plot(totEnergyPlusConnPlot(1:n))
        title("Total Energy")
        drawnow;
       
%        subplot(3, 1, 1)
%        plot(energyConnection(1:n))
%        title("Connection Energy");
%        subplot(3, 1, 2);
%        plot(totEnergy(1:n))
%        title("Tot Energy Min Connection");
%        subplot(3, 1, 3);
%        plot(totEnergyPlusConn(1:n))
%        title("Tot Energy Plus Connection");
%        drawnow;
    end
    
    out(n) = u(round(NS / pi));
    out2(n) = u(round(NS + 1 + NP / pi));
    uPrev = u;
    u = uNext;
    if drawState == true% && mod(n, 100) == 0
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
plot(totEnergyString)
energyTest = (totEnergyString(2:end) - totEnergyString(1:end-1)) * 1/k;
plot(energyTest);
energyTest = log(totEnergyString(2:end) - totEnergyString(1:end-1));
plot(energyTest - energyTest(1) / energyTest(1));

totEnergyPlate = kinEnergyPlate + potEnergyPlate;
totEnergy = totEnergyString + totEnergyPlate + energyConnection;
subplot(3, 1, 1)
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