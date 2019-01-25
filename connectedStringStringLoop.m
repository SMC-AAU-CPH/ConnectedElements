function [maxTotEnergyDiff] = connectedStringStringLoop (fs, rhoS, r, T, kappaS, conn1, conn2)


%% String
k = 1/fs;
A = r^2 * pi;
L = 1; % Length of the string [m]
s0S = 0; % Frequency independent damping coefficient
s1S = 0; % Frequency dependent damping coefficient
c = sqrt(T / (rhoS * A));

[B1, C1, N1, h1, D1, D41] = newCreateString (c, kappaS, L, s0S, s1S, k);
[B2, C2, N2, h2, D2 D42] = newCreateString (c, kappaS, L, s0S, s1S, k);


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
    exciterPos = 0.25;
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
J(1 + floor(conn1*N1-cW1/2):1 + floor(conn1*N1+cW1/2)) = (1-cos(2*pi*(0:cW1)/cW1)) * 0.5 * k^2 / (rhoS * A * h1);

cW2 = 2;
I(N1 + 1 + floor(conn2*N2-cW2/2):N1 + 1 + floor(conn2*N2+cW2/2)) = -(1-cos(2*pi*(0:cW2)/cW2)) * 0.5;
J(N1 + 1 + floor(conn2*N2-cW2/2):N1 + 1 + floor(conn2*N2+cW2/2)) = -(1-cos(2*pi*(0:cW2)/cW2)) * 0.5 * k^2 / (rhoS * A * h2);

% end  
%     
% I(floor(1+N1 + connP * NP)) = -1;
% 
% J(floor(N1 + 1 + connP * NP)) = -k^2 / (rhoP * H * hP^2);


%% Length of the sound
lengthSound = round(fs / 50);

potEnergyString1 = zeros(lengthSound, 1);
kinEnergyString1 = zeros(lengthSound, 1);
potEnergyString2 = zeros(lengthSound, 1);
kinEnergyString2 = zeros(lengthSound, 1);

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);
drawBar = false;

stringVec1 = 2:N1-1;
stringVec2 = stringVec1 + N1; %N1+2:N1+N2-1;

connected = true;

JFc = zeros(Ntot);

drawState = true;
figure;
idx = find(J~=0);

for n = 1 : lengthSound
    if connected
        %             - kappaS^2 * k^2 * I(1:N1)' * D14 * u(1:N1) ...
        Fc = (c^2 * k^2 * I(1:N1)' * (D1 / h1^2) * u(1:N1) ...
            + c^2 * k^2 * I(N1+1:end)' * (D2 / h2^2) * u(N1+1:end))...
            / -(I(1:N1)' * J(1:N1) + I(N1+1:end)' * J(N1+1:end));
%         Fc = (c^2 * k^2 / hS^2 * (u(idx(1) + 1) - 2 * u(idx(1)) + u(idx(1)-1)) ...
%             - kappaP^2 * k^2 / hP^4 * -1 * (...
%             -(testmat(cIX + 2, cIY) + testmat(cIX - 2, cIY) + testmat(cIX, cIY + 2) + testmat(cIX, cIY - 2))...
%             - 2 * (testmat(cIX + 1, cIY + 1) + testmat(cIX + 1, cIY - 1) + testmat(cIX - 1, cIY + 1) + testmat(cIX - 1, cIY - 1))...
%             + 8 * (testmat(cIX + 1, cIY) + testmat(cIX - 1, cIY) + testmat(cIX, cIY + 1) + testmat(cIX, cIY - 1))...
%             - 20 * testmat(cIX, cIY)))...
%             / -(I(1:N1)' * J(1:N1) - I(N1+1:end)' * J(N1+1:end));
%         Fc1 - Fc
% if testmat(cIX, cIY) ~=0
%     
% end
        JFc = J*Fc;
    else 
        JFc = 0;
    end
    uNext = B * u + C * uPrev + JFc; 

%     u(idx(1)) - u(idx(2))
    
    potEnergyString1(n) = c^2 / 2 * sum (1 / h1 * (u(stringVec1 + 1) - u(stringVec1)) .* (uPrev(stringVec1 + 1) - uPrev(stringVec1))) ...
        + kappaS^2 / 2 * 1/h1^3 * sum(D1 * u(1:N1) .* D1 * uPrev(1:N1));
    kinEnergyString1(n) = 1 / 2 * sum (h1 * ((1 / k * (u(stringVec1) - uPrev(stringVec1))).^2));
    
    potEnergyString2(n) = c^2 / 2 * sum (1 / h2 * (u(stringVec2 + 1) - u(stringVec2)) .* (uPrev(stringVec2 + 1) - uPrev(stringVec2))) ...
        + kappaS^2 / 2 * 1/h2^3 * sum(D2 * u(N1+1:end) .* D2 * uPrev(N1+1:end));
    kinEnergyString2(n) = 1 / 2 * sum (h2 * ((1 / k * (u(stringVec2) - uPrev(stringVec2))).^2));
    
    uPrev = u;
    u = uNext;
    if drawState && mod(n, 10) == 0
        clf
        subplot(2,1,1)
        plot (u(1:N1))
         subplot(2,1,2)
        plot(u(N1+1:end))
        drawnow;
    end
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

figure;
totEnergyString2 = kinEnergyString2 + potEnergyString2;
totEnergyString2Plot = (totEnergyString2-totEnergyString2(1))/totEnergyString2(1);
plot(totEnergyString2Plot)

figure;
totEnergy = totEnergyString1+totEnergyString2;
totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
maxTotEnergyDiff = (max(totEnergy) + abs(min(totEnergy)));
plot(totEnergy)
drawnow;