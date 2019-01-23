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
kappaS = 5;
objectVars{1, 2} = [gamma, kappaS, 1e99, 0];

objectVars{2, 1} = "plate";
objectVars{2, 2} = [1, 0.5, 0.01, 1e99, 0];

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

[Bpre{1}, Cpre{1}, N(1), h(1), s0(1)] = createString(objectVars{1, 2}, fs);  
[Bpre{2}, Cpre{2}, N(2), s0(2), Nx, Ny, h(2), kappaP] = createPlate (objectVars{2, 2}, fs);
Nx = Nx - 1;
Ny = Ny - 1;
Ntot = sum(N);
vec = cell(2, 1);
matIdx(2) = N(1) + 1;

for q = 1:2
    vec{q} = matIdx(q):matIdx(q)+N(q)-1;
end

%% Create full matrices
B = zeros(Ntot);
C = zeros(Ntot);

for q = 1:2
    B(matIdx(q):matIdx(q)+N(q) - 1, matIdx(q):matIdx(q)+N(q) - 1) = Bpre{q};
    C(matIdx(q):matIdx(q)+N(q) - 1, matIdx(q):matIdx(q)+N(q) - 1) = Cpre{q};
end


%% Initialise state vectors
u = zeros(Ntot, 1);
excitePlate = true;
exciteString = true;

%% Excite
if excitePlate
    exciterPosX = 0.5;
    exciterPosY = 0.5;
    rcW = 2;
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
    rcW = 2;
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
conn = [1, 2, 0.2, 0.4, 0.4, 1, 1];

J = zeros(Ntot, 1); % second term is the amount of connections
massRatio = 1;

L = zeros(1, Ntot);
[J(:,1), L(1,:)] = createConnection(conn, [N(conn(1)) N(conn(2)) Ntot], k, [h(1) h(2)], massRatio, [s0(conn(1)) s0(conn(2))], [matIdx(conn(1)) matIdx(conn(2))]);

%% Spring coefficients 
sx = 1;
w0 = 1000; 
w1 = 1000000; 

out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);
drawBar = false;

etaRPrev = zeros(1, 1); %first term is amount of connections
testmat = ones(Nx, Ny); %or other way around   
testmatPrev = ones(Nx, Ny); %or other way around   
connected = false;

stringVec = vec{1}(2:N(1)-1);
plateVec = vec{2};
phVec = 2:(Nx+1);
% pvVec = (1+Nx):(Nx - 2):(Nx-2)*(Ny-2);
pvVec = 2:(Ny+1);

% k = 1;
% indices = zeros(Nx-2 * Ny-2, 1);
% for i = 1:Ny
%     for j = 1:Nx
%         indices(k) = plateVecVert(i) + plateVecHor(j);
%         k = k + 1;
%     end
% end

% lm = 1 : N(2);
% l1m = lm(Ny+1:end);
% lneg1m = lm(1:end-Ny);
% lm1 = lm;
% lm1(mod(lm-1,Ny) == 0) = [];
% lmneg1 = lm;
% lmneg1(mod(lm-Ny,Ny) == 0) = [];
% 
% lm = lm + N(1);
% l1m = l1m + N(1);
% lneg1m = lneg1m + N(1);
% lm1 = lm1 + N(1);
% lmneg1 = lmneg1 + N(1);

potEnergyString = zeros(lengthSound, 1);
kinEnergyString = zeros(lengthSound, 1);
potEnergyPlate = zeros(lengthSound, 1);
kinEnergyPlate = zeros(lengthSound, 1);
uE = zeros(Nx + 2, Ny + 2);
uEPrev = zeros(Nx + 2, Ny + 2);
% uE = zeros((Nx + 2) * (Ny + 2), 1);
% uEPrev = zeros((Nx + 2) * (Ny + 2), 1);
NxZeros = zeros(Nx, 1);
NyZeros = zeros(Ny, 1);
ulm1 = zeros(N(2),1);
ulmneg1 = zeros(N(2),1);
ulm1Prev = zeros(N(2),1);
ulmneg1Prev = zeros(N(2),1);
for n = 1 : lengthSound
    
    % calculate relative displacement
    etaR = L*u;
    
    % update r and p
    rn = (2*sx/k - w0.^2 - w1.^4.*(etaR).^2)./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    pn = -2./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    
    Rn = eye(1).*rn; 
    Pn = eye(1).*pn; 
    
    uTemp = B * u + C * uPrev;
    
    % find Fc
    if connected
        bn = L * uTemp; 
        an = Rn * etaRPrev; 

        Fc = (L*J-Pn)\(an - bn);
        JFc = J * Fc;
    else 
        JFc = 0;
    end
    uNext = uTemp + JFc;
    
    
    for q = 0:Nx-1
        testmat(q+1, 1:Ny) = u(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        testmatPrev(q+1, 1:Ny) = uPrev(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
    end
    uE(2:end-1, 2:end-1) = testmat;
    uEPrev(2:end-1, 2:end-1) = testmatPrev;

    potEnergyString(n) = gamma^2 / 2 * sum (1 / h(1) * (u(stringVec + 1) - u(stringVec)) .* (uPrev(stringVec + 1) - uPrev(stringVec))) ...
        + kappaS^2 / 2 * 1/h(1)^3 * sum((u(stringVec + 1) - 2 * u(stringVec) + u(stringVec - 1)) ...
        .* (uPrev(stringVec + 1) - 2 * uPrev(stringVec) + uPrev(stringVec - 1)));
    kinEnergyString(n) = 1 / 2 * sum (h(1) * ((1 / k * (u(stringVec) - uPrev(stringVec))).^2));
    
    kinEnergyPlate(n) = 1 / 2 * h(2)^2 * sum(sum(1/k^2 * (uE - uEPrev).^2));
    potEnergyPlate(n) = kappaP^2 / (2 * h(2)^2) * sum(sum(...
        (uE(phVec + 1, pvVec)...
        + uE(phVec - 1, pvVec) + uE(phVec, pvVec + 1) + uE(phVec, pvVec - 1) ...
        - 4 * uE(phVec, pvVec)) ...
        .* (uEPrev(phVec + 1, pvVec)...
        + uEPrev(phVec - 1, pvVec) + uEPrev(phVec, pvVec + 1) + uEPrev(phVec, pvVec - 1) ...
        - 4 * uEPrev(phVec, pvVec))));
%     u = 1:length(u);
%     for i = 1:Nx
%         if i == 23
%             
%         end
%         ulm1(2+(Ny+1)*(i-1):(Ny+1)*i) = u(N(1)+1+(i-1)*Ny:N(1)+(i)*Ny);
% %         ulmneg1(2+(Ny+2)*(i-1):(Ny+2)*i-1) = u(N(1)+1+(i-1)*Ny:N(1)+(i)*Ny);
%     end
%     ulm1 = u(N(1)+1:end);
%     ulm1(mod(lm -  N(1) - 1, Ny) == 0) = 0;
%     ulmneg1 = u(N(1)+1:end);
%     ulmneg1(mod(lm -  N(1) - Ny, Ny) == 0) = 0;

%     ulm1(lm1 - N(1)) = u(lm1);
%     ulm1Prev(lm1 - N(1)) = uPrev(lm1);
    
%     ulm1Prev = u(N(1)+1:end);
%     ulm1Prev(mod(lm -  N(1) - 1, Ny) == 0) = 0;
%     ulmneg1Prev = u(N(1)+1:end);
%     ulmneg1Prev(mod(lm -  N(1) - Ny, Ny) == 0) = 0;

%     ulmneg1(lmneg1 - N(1)) = u(lmneg1);
%     ulmneg1Prev(lmneg1 - N(1)) = uPrev(lmneg1);
    
%     potEnergyPlate(n) = kappaP^2 / (2 * h(2)^2) * sum(([NyZeros; u(l1m)] + [u(lneg1m); NyZeros] + ulm1 + ulmneg1 - 4 * u(lm)) ...
%         .* ([NyZeros; uPrev(l1m)] + [uPrev(lneg1m); NyZeros] + ulm1Prev + ulmneg1Prev - 4 * uPrev(lm)));
%     clf
% %     plot(kinEnergyPlate(1:n))
% %     hold on;
%     plot(potEnergyPlate(1:n))
%     surf(uE)
%     drawnow;
    out(n) = uNext(floor(N(1)/2));
    out2(n) = u(round(Ntot - 15));
    if mod(n,1) == 0 && drawBar == true
        uE = u;
        uE(lm) = 0;
        for q = 0:Nx-1
            testmat(q+1, 1:Ny) = uE(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        end
        subplot(2,2,1)
        imagesc(testmat)
%         plot(u(vec{1}))
%         u = ones(length(u),1);
        uE = u;
        uE(lm1) = 0;
        for q = 0:Nx-1
            testmat(q+1, 1:Ny) = uE(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        end
        subplot(2,2,2)
        imagesc(testmat)
       
        uE = u;
        uE(lmneg1) = 0;
        for q = 0:Nx-1
            testmat(q+1, 1:Ny) = uE(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        end
        subplot(2,2,3)
        imagesc(testmat)
        
        uE = u;
        for q = 0:Nx-1
            testmat(q+1, 1:Ny) = uE(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        end
        subplot(2,2,4)
        imagesc(testmat)
        
        drawnow;
        pause;
    end
    uPrev = u;
    u = uNext;
    etaRPrev = etaR;
end
subplot(2,1,1)
plot(out)
subplot(2,1,2)
plot(out2)

figure;
totEnergyString = kinEnergyString + potEnergyString;
totEnergyString = (totEnergyString-totEnergyString(1))/totEnergyString(1);
plot(totEnergyString)

totEnergyPlate = kinEnergyPlate + potEnergyPlate;
totEnergyPlate = (totEnergyPlate-totEnergyPlate(1))/totEnergyPlate(1);
plot(totEnergyPlate)