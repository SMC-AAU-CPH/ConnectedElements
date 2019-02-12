function [maxTotEnergyDiff] = connectedStringStringLoop (fs, rhoS1, rhoS2, r1, r2, T1, T2, conn1, conn2)


%% String
k = 1/fs;
A1 = r1^2 * pi;
A2 = r2^2 * pi;
L1 = 1; % Length of the string [m]
L2 = 1; % Length of the string [m]
s0S = 0.1; % Frequency independent damping coefficient
s1S = 0.005; % Frequency dependent damping coefficient
c1 = sqrt(T1 / (rhoS1 * A1));
c2 = sqrt(T2 / (rhoS2 * A2));
I1 = pi/4 * r1^4;
I2 = pi/4 * r2^4;
E = 2e11;
kappaS1 = sqrt(E*I1 / (rhoS1 * A1));
kappaS2 = sqrt(E*I2 / (rhoS2 * A2));
[B1, C1, N1, h1, D1, D41] = newCreateString (c1, kappaS1, L1, s0S, s1S, k);
[B2, C2, N2, h2, D2, D42] = newCreateString (c2, kappaS2, L2, s0S, s1S, k);

%% Create full matrices
Ntot = N1 + N2;
B = zeros(Ntot);
C = zeros(Ntot);

B(1:N1, 1:N1) = B1;
B(N1+1:N1+N2, N1+1:N1+N2) = B2;
C(1:N1, 1:N1) = C1;
C(N1+1:N1+N2, N1+1:N1+N2) = C2;


%% Initialise state vectors
u = zeros(Ntot, 1);
exciteString1 = true;
exciteString2 = true;

%% Excite
if exciteString1
    exciterPos = 0.25;
    rcW = 10;
    u(1 + floor(exciterPos*N1-rcW/2):1 + floor(exciterPos*N1+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;

end
if exciteString2
    exciterPos = 0.35;
    rcW = 10;
    u(N1 + 1 + floor(exciterPos*N2-rcW/2):N1 + 1 + floor(exciterPos*N2+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
end
uPrev = u;
uNext = u;

%% Connection points
% connS = 0.5;
% connP = 0.5;

I = zeros(Ntot, 1);
J = zeros(Ntot, 1);

cW1 = 2;
I(1 + floor(conn1*N1-cW1/2):1 + floor(conn1*N1+cW1/2)) = (1-cos(2*pi*(0:cW1)/cW1)) * 0.5;
J(1 + floor(conn1*N1-cW1/2):1 + floor(conn1*N1+cW1/2)) = (1-cos(2*pi*(0:cW1)/cW1)) * 0.5 * k^2 / (rhoS1 * A1 * h1);

cW2 = 2;
I(N1 + 1 + floor(conn2*N2-cW2/2):N1 + 1 + floor(conn2*N2+cW2/2)) = -(1-cos(2*pi*(0:cW2)/cW2)) * 0.5;
J(N1 + 1 + floor(conn2*N2-cW2/2):N1 + 1 + floor(conn2*N2+cW2/2)) = -(1-cos(2*pi*(0:cW2)/cW2)) * 0.5 * k^2 / (rhoS2 * A2 * h2);

%% Length of the sound
lengthSound = round(fs);

potEnergyString1 = zeros(lengthSound, 1);
kinEnergyString1 = zeros(lengthSound, 1);
totEnergyString1 = zeros(lengthSound, 1);
totEnergyString1Plot = zeros(lengthSound, 1);
potEnergyString2 = zeros(lengthSound, 1);
kinEnergyString2 = zeros(lengthSound, 1);
totEnergyString2 = zeros(lengthSound, 1);
totEnergyString2Plot = zeros(lengthSound, 1);
totEnergy = zeros(lengthSound, 1);
totEnergyPlot = zeros(lengthSound, 1);
energyConnection = zeros(lengthSound, 1);
totEnergyPlusConn = zeros(lengthSound, 1);
totEnergyPlusConnPlot = zeros(lengthSound, 1);

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);

stringVec1 = 2:N1-1;
stringVec2 = N1+2:N1+N2-1;

connected = true;

JFc = zeros(Ntot);

drawState = false;
drawEnergy = true;
idx = find(J~=0);

w0 = 100;
w1 = 10000000;
sx = 0;

L = [I(1:N1) * h1; I(N1+1:end) * h2]';

etaPrev = 0;
eta = 0;
for n = 1 : lengthSound
    eta = I' * u;
    uNext = B * u + C * uPrev;
    if connected
%         rn = (2*sx/k - w0.^2 - w1.^4.*(eta).^2)./(2*sx/k + w0.^2 + w1.^4.*(eta).^2);
%         pn = -2./(2*sx/k + w0.^2 + w1.^4.*(eta).^2);
%         
%         Fc = (c1^2 * k^2 * I(1:N1)' * (D1 / h1^2) * u(1:N1) ...
%             - kappaS1^2 * k^2 * I(1:N1)' * (D41 / h1^4) * u(1:N1) ...
%             + c2^2 * k^2 * I(N1+1:end)' * (D2 / h2^2) * u(N1+1:end) ...
%             - kappaS2^2 * k^2 * I(N1+1:end)' * (D42 / h2^4) * u(N1+1:end))...
%             / -(I'*J);

%         Fc = (-I' * uNext - 2 * eta - etaPrev) / (I' * J + 4 / w0);
%         Fc = (-I' * uNext - etaPrev) / (I' * J + 2 / (w1 * eta^2));
    varPhi = w0/4 + (w1 * eta^2)/2;
    Fc = (-I' * uNext * varPhi - (w0 * eta) / 2 - etaPrev * varPhi) / (I' * J * varPhi + 1);
%         Fc = (I'*uNext) / -(I' * J);
        
        JFc = J*Fc;
    else 
        JFc = 0;
    end
    uNext = uNext + JFc; 
%     I' * u
    potEnergyString1(n) = T1 / 2 * sum (1 / h1 * (u(stringVec1 + 1) - u(stringVec1)) .* (uPrev(stringVec1 + 1) - uPrev(stringVec1))) ...
        + (E * I1) / 2 * 1/h1^3 * sum(D1 * u(1:N1) .* D1 * uPrev(1:N1));
    kinEnergyString1(n) = (rhoS1 * A1) / 2 * sum (h1 * ((1 / k * (u(stringVec1) - uPrev(stringVec1))).^2));
    
    potEnergyString2(n) = T2 / 2 * sum (1 / h2 * (u(stringVec2 + 1) - u(stringVec2)) .* (uPrev(stringVec2 + 1) - uPrev(stringVec2))) ...
        + (E * I2) / 2 * 1/h2^3 * sum(D2 * u(N1+1:end) .* D2 * uPrev(N1+1:end));
    kinEnergyString2(n) = (rhoS2 * A2) / 2 * sum (h2 * ((1 / k * (u(stringVec2) - uPrev(stringVec2))).^2));
    
%     energyConnection(n) = w0/4 * (eta^2 + etaPrev^2); % when using center time difference 
%     energyConnection(n) = w0/2 * (1/2 * (eta + etaPrev))^2; % when using forward * backward time difference (mu_tt}
%     energyConnection(n) = w1 / 2 * (1/2 * eta^2 * etaPrev^2); 
    energyConnection(n) = w0/2 * (1/2 * (eta + etaPrev))^2 + w1 / 2 * (1/2 * eta^2 * etaPrev^2);
    etaPrev = eta;
    
    totEnergyString1(n) = kinEnergyString1(n) + potEnergyString1(n);
    totEnergyString1Plot(n) = (totEnergyString1(n)-totEnergyString1(1))/totEnergyString1(1);
    totEnergyString2(n) = kinEnergyString2(n) + potEnergyString2(n);
    totEnergyString2Plot(n) = (totEnergyString2(n)-totEnergyString2(1))/totEnergyString2(1);
    totEnergy(n) = totEnergyString1(n) + totEnergyString2(n);
    totEnergyPlot(n) = (totEnergy(n)-totEnergy(1))/totEnergy(1);
    totEnergyPlusConn(n) = totEnergyString1(n) + totEnergyString2(n) + energyConnection(n);
    totEnergyPlusConnPlot(n) = (totEnergyPlusConn(n)-totEnergyPlusConn(1))/totEnergyPlusConn(1);
        
        
    if drawEnergy == false && mod(n, 10) == 0

        clf
        subplot(3,2,1)
%         plot(kinEnergyString1(1:n))
%         hold on;
%         plot(potEnergyString1(1:n));
%         title("Kinetic and Potential Energy String ")
%         subplot(3,2,2)
        plot(totEnergyString1(1:n))
        title("Total String1 Energy")

        
        subplot(3,2,2)
%         plot(kinEnergyString2(1:n))
%         hold on;
%         plot(potEnergyString2(1:n));
%         title("Kinetic and Potential Energy String 2")
%         subplot(3,2,4)
        plot(totEnergyString2(1:n))
        title("Total String2 Energy")

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
    
    uPrev = u;
    u = uNext;
    if drawState == true % && mod(n, 10) == 0
        clf
        subplot(2,1,1)
        plot (u(1:N1))
         subplot(2,1,2)
        plot(u(N1+1:end))
        drawnow;
    end
    
    out(n) = uNext(floor(N1/pi));
    out2(n) = u(round(Ntot - 15));
%     clf;
%     scatter(uNext(idx(1)), 1)
%     hold on;
%     scatter(uNext(idx(2)), 2)
%     drawnow;
end
% subplot(2,1,1)
% plot(out)
% subplot(2,1,2)
% plot(out2)


figure;
totEnergyString1 = kinEnergyString1 + potEnergyString1;
totEnergyString1Plot = (totEnergyString1-totEnergyString1(1))/totEnergyString1(1);
plot(totEnergyString1Plot)

hold on;
totEnergyString2 = kinEnergyString2 + potEnergyString2;
totEnergyString2Plot = (totEnergyString2-totEnergyString2(1))/totEnergyString2(1);
plot(totEnergyString2Plot)

figure;
totEnergy = totEnergyString1+totEnergyString2+energyConnection;
totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
maxTotEnergyDiff = (max(totEnergy) + abs(min(totEnergy)));
plot(totEnergy)
drawnow;
        