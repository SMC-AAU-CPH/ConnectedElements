%{
function [maxTotEnergy] = 
connectedStringPlate (fs,
                      stringlength, 
                      damping coefficients string, 
                      stiffness string,
                      frequency string (gamma),
                      damping coefficients plate,
                      stiffness plate,
                      excitation location,
                      excitation width,
                      connection location string,
                      connection location plate,
                      spring coefficients connection)
%}

clear all;
close all;
clc;

fs = 44100;      % Sampling rate
k = 1 / fs;     % Time step


%% Create objects 
objectVars{1, 1} = "string";
freq1 = 110.0;
gamma = freq1 * 2;
kappaS1 = 0;
objectVars{1, 2} = [gamma, kappaS1, 1e99, 0];

objectVars{2, 1} = "string";
freq2 = 110.0;
gamma = freq2 * 2;
kappaS2 = 0;
objectVars{2, 2} = [gamma, kappaS2, 1e99, 0];

%initialise cells/vectors depending on the number of objects
Bpre = cell(2, 1);
Cpre = cell(2, 1);

bB = cell(2, 1);
bC = cell(2, 1);

N = zeros(2, 1);
h = zeros(2, 1);
matIdx = ones(2, 1);

s0 = zeros(2, 1);
s1 = zeros(2, 1);

whatExciter = "raisedCos";

[Bpre{1}, Cpre{1}, N(1), h(1), s0(1)] = createString (objectVars{1, 2}, fs);  
[Bpre{2}, Cpre{2}, N(2), h(2), s0(2)] = createString (objectVars{2, 2}, fs);

Ntot = sum(N);

for q = 1:2
    vec{q} = matIdx(q):matIdx(q)+N(q)-1;
end

matIdx(2) = N(1) + 1;

%% Create full matrices
B = zeros(Ntot);
C = zeros(Ntot);

for q = 1:2
    B(matIdx(q):matIdx(q)+N(q) - 1, matIdx(q):matIdx(q)+N(q) - 1) = Bpre{q};
    C(matIdx(q):matIdx(q)+N(q) - 1, matIdx(q):matIdx(q)+N(q) - 1) = Cpre{q};
end


%% Initialise state vectors
u = zeros(Ntot, 1);
excitePlate = false;
exciteString = true;

%% Excite
if excitePlate
    exciterPosX = 0.5;
    exciterPosY = 0.5;
    rcW = floor(min(Nx,Ny) / 2);
    excitationMat = zeros(rcW+1, rcW+1);
    scaler = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
    for x = 1:rcW+1
        excitationMat(x,:) = scaler(x) * (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
    end
    startIdxX = floor(Nx * exciterPosX - rcW/2) - 1;
    startIdxY = floor(Ny * exciterPosY - rcW/2) - 1;
    for i = 1 : rcW 
       u(N(1) + (startIdxY + i) * Nx + startIdxX : N(1) + (startIdxY + i) * Nx + startIdxX + rcW) = ...  
            excitationMat(i,:);
        %         u(N(1) + startIdxX * (Nx-1) * i + startIdxY : N(1)+ startIdxX * (Nx-1) * i + startIdxY + rcW) = ...

    end  
end
if exciteString
    exciterPos = 0.45;
    rcW = 10;
    u(matIdx(1) + floor(exciterPos*N(1)-rcW/2):matIdx(1) + floor(exciterPos*N(1)+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
end
uPrev = u;
uNext = u;

%% Length of the sound
lengthSound = fs / 10;

%% Connections

%{ 
    1st, 2nd object, 
    location of string connection,
    x-location of plate connection,
    y-location of plate connection,
    1st, 2nd width of connection
%}
conn = [1, 2, 0.2, 0.2, 1, 1];

J = zeros(Ntot, 1); % second term is the amount of connections
massRatio = 1;

L = zeros(1, Ntot);
[J(:,1), L(1,:), E] = createConnectionStringString(conn, [N(conn(1)) N(conn(2)) Ntot], k, [h(1) h(2)], massRatio, [s0(conn(1)) s0(conn(2))], [matIdx(conn(1)) matIdx(conn(2))]);
%% Spring coefficients 
sx = 0;
w0 = 0; 
w1 = 0; 

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);
drawBar = false;

etaRPrev = zeros(1, 1); %first term is amount of connections  
connected = true;

stringVec1 = vec{1}(2:N(1)-1);
stringVec2 = vec{2}(N(1)+2:N(2)-2);


% k = 1;
% indices = zeros(Nx-2 * Ny-2, 1);
% for i = 1:Ny
%     for j = 1:Nx
%         indices(k) = plateVecVert(i) + plateVecHor(j);
%         k = k + 1;
%     end
% end

potEnergyString1 = zeros(lengthSound, 1);
kinEnergyString1 = zeros(lengthSound, 1);
potEnergyString2 = zeros(lengthSound, 1);
kinEnergyString2 = zeros(lengthSound, 1);
JFc = zeros(length(u), 1);

for n = 1 : lengthSound
    
    % calculate relative displacement
%     etaR = L*u;
    
    % update r and p
%     rn = (2*sx/k - w0.^2 - w1.^4.*(etaR).^2)./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
%     pn = -2./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
%     
%     Rn = eye(1).*rn; 
%     Pn = eye(1).*pn; 
    
    uTemp = B * u + C * uPrev;
    
    % find Fc
    if connected
        Fc = (u(40) - u(240)) / (-1/h(1) - 1/h(2));
%         bn = L * uTemp; 
%         an = Rn * etaRPrev; 
%         Fc = (L*J-Pn)\(an - bn);
        JFc(40) = k^2 * Fc * 1/h(1);
        JFc(240) = -k^2 * Fc * 1/h(2);
    else 
        JFc = 0;
    end
    uNext = uTemp + JFc;
%     clf
%     subplot(2,1,1)
%     plot(u(1:N(1)));
%     hold on;
%     plot(u(N(1)+1:end));
%     drawnow;
%     for q = 0:Nx-1
%         testmat(q+1, 1:Ny) = u(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
%         testmatPrev(q+1, 1:Ny) = uPrev(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
%     end
%     uE(2:end-1, 2:end-1) = testmat;
%     uEPrev(2:end-1, 2:end-1) = testmatPrev;

    potEnergyString1(n) = gamma^2 / 2 * sum (1 / h(1) * (u(stringVec1 + 1) - u(stringVec1)) .* (uPrev(stringVec1 + 1) - uPrev(stringVec1))) ...
        + kappaS1^2 / 2 * 1/h(1)^3 * sum((u(stringVec1 + 1) - 2 * u(stringVec1) + u(stringVec1 - 1)) ...
        .* (uPrev(stringVec1 + 1) - 2 * uPrev(stringVec1) + uPrev(stringVec1 - 1)));
    kinEnergyString1(n) = 1 / 2 * sum (h(1) * ((1 / k * (u(stringVec1) - uPrev(stringVec1))).^2));
     
     potEnergyString2(n) = gamma^2 / 2 * sum (1 / h(1) * (u(stringVec2 + 1) - u(stringVec2)) .* (uPrev(stringVec2 + 1) - uPrev(stringVec2))) ...
        + kappaS2^2 / 2 * 1/h(1)^3 * sum((u(stringVec2 + 1) - 2 * u(stringVec2) + u(stringVec2 - 1)) ...
        .* (uPrev(stringVec2 + 1) - 2 * uPrev(stringVec2) + uPrev(stringVec2 - 1)));
    kinEnergyString2(n) = 1 / 2 * sum (h(1) * ((1 / k * (u(stringVec2) - uPrev(stringVec2))).^2));
   
    clf
    subplot(2,1,1)
    plot(u(1:N(1)))
    hold on;
    plot(E(1:N(1)))
    plot(u(N(2):end) * 1e10);
    subplot(2,1,2)
    totEnergyString1 = potEnergyString1 + kinEnergyString1;
    totEnergyString2 = potEnergyString2 + kinEnergyString2;
    totEnergy = totEnergyString1 + totEnergyString2;
    totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
    plot(totEnergy(1:n))
    drawnow;
    
    
    out(n) = uNext(floor(N(1)/2));
    out2(n) = u(round(Ntot - 15));
    if mod(n,1) == 0 && drawBar == true
        for q = 0:Nx-1
            testmat(q+1, 1:Ny) = u(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        end
%         subplot(2,2,4)
%         imagesc(testmat)
        plot(u)
        drawnow;
%         pause;
    end
    uPrev = u;
    u = uNext;
%     etaRPrev = etaR;
end
subplot(2,1,1)
plot(out)
subplot(2,1,2)
plot(out2)

figure;
totEnergyString1 = kinEnergyString1 + potEnergyString1;
totEnergyStringPlot = (totEnergyString1-totEnergyString1(1))/totEnergyString1(1);
plot(totEnergyStringPlot)

figure;
totEnergyString2 = kinEnergyString2 + potEnergyString2;
totEnergyPlatePlot = (totEnergyString2-totEnergyString2(1))/totEnergyString2(1);
plot(totEnergyPlatePlot)
figure;
totEnergy = totEnergyString1+totEnergyString2;
totEnergy = (totEnergy-totEnergy(1))/totEnergy(1);
plot(totEnergy)