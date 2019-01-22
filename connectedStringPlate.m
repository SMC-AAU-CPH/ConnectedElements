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
objectVars{2, 2} = [3, 1, 0.05, 1e99, 0];

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

if objectVars{1, 1} == "bar"
        [Bpre{1}, Cpre{1}, N(1), h(1), s0(1)] = createBar(objectVars{1, 2}, fs);
    elseif objectVars{1, 1} == "string"
        [Bpre{1}, Cpre{1}, N(1), h(1), s0(1)] = createString(objectVars{1, 2}, fs);
    elseif objectVars{1, 1} == "bowedString"
        whatExciter = "bowed";
        [Bpre{1}, Cpre{1}, bB{1}, bC{1}, N(1), h(1), s0(1)] = createBowedString(objectVars{1, 2}, fs);
end
       
    
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

if excitePlate
    exciterPosX = 0.5;
    exciterPosY = 0.5;
    rcW = 4;
    excitationMat = zeros(rcW+1, rcW+1);
    scaler = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
    for x = 1:rcW+1
        excitationMat(x,:) = scaler(x) * (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
    end
    startIdxX = floor(Nx * exciterPosX - rcW/2) - 1;
    startIdxY = floor(Ny * exciterPosY - rcW/2) - 1;
    for i = 1 : rcW 
       u(N(1) + (startIdxY + i) * Ny + startIdxX : N(1) + (startIdxY + i) * Ny + startIdxX + rcW) = ...  
            excitationMat(i,:);
        %         u(N(1) + startIdxX * (Nx-1) * i + startIdxY : N(1)+ startIdxX * (Nx-1) * i + startIdxY + rcW) = ...

    end
    
else
    exciterPos = 0.45;
    rcW = 10;
    u(matIdx(1) + floor(exciterPos*N(1)-rcW/2):matIdx(1) + floor(exciterPos*N(1)+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
end
uPrev = u;
uNext = u;

%% Length of the sound
lengthSound = fs;

%% Exciter
if whatExciter == "bowed"
    %Bow Model
    a = 100;                % free parameter
    BM = sqrt(2*a)*exp(1/2);
    Fb = 50;
    Vb = 0.2;
    Ee = zeros(Ntot, 1);
    Ee(N(1) + floor(N(2)/4)) = 1;
    K = k^2 * Ee / (1 + s0(2)*k);
    vrelPrev = 0;
    tol = 1e-4;
else
    Fmax = 500;
    exciterLength = floor(1e-3 * fs);
    v = 2; 
    Fe = Fmax/2*(1-cos(v*pi*(0:exciterLength)/exciterLength));
    exciter = [Fe, zeros(1,lengthSound-exciterLength - 1)];

    OTE = 2; % Object To Excite
    exciterPos = 1/2;
    rcW = 3;

    Ee = zeros(Ntot, 1);
    Ee (matIdx(OTE) + exciterPos*N(OTE)-floor(rcW/2):matIdx(OTE) + exciterPos*N(OTE)+ceil(rcW/2)) = 1-cos(v*pi*(0:rcW)/rcW);
    K = k^2 * Ee / (1 + s0(OTE)*k);
end
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

plateVecHor = 2:(Nx+1);
plateVecVert = (1+Nx):(Nx + 2):(Nx+2)*(Ny+2);
k = 1;
indices = zeros(Nx * Ny, 1);
for i = 1:Ny
    for j = 1:Nx
        indices(k) = plateVecVert(i) + plateVecHor(j);
        k = k + 1;
    end
end
potEnergyString = zeros(lengthSound, 1);
kinEnergyString = zeros(lengthSound, 1);
potEnergyPlate = zeros(lengthSound, 1);
kinEnergyPlate = zeros(lengthSound, 1);
testMatZeros = zeros(Nx + 2, Ny + 2);
testMatZerosPrev = zeros(Nx + 2, Ny + 2);
uE = zeros((Nx + 2) * (Ny + 2), 1);
uEPrev = zeros((Nx + 2) * (Ny + 2), 1);

for n = 1 : lengthSound
    
    % calculate relative displacement
    etaR = L*u;
    
    % update r and p
    rn = (2*sx/k - w0.^2 - w1.^4.*(etaR).^2)./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    pn = -2./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    
    Rn = eye(1).*rn; 
    Pn = eye(1).*pn; 
    
    if whatExciter == "bowed"
        % Newton-Raphson
        b = 2/k * Vb + 2 * s0(2) * Vb + Ee(vec{2})' * bB{2} * u(vec{2}) + Ee(vec{2})' * bC{2} * uPrev(vec{2});
        eps = 1;
        i = 0;
        while eps>tol
            vrel=vrelPrev-(Fb*BM*vrelPrev*exp(-a*vrelPrev^2)+2*vrelPrev/k+2*s0(2)*vrelPrev+b)/...
             (Fb*BM*(1-2*a*vrelPrev^2)*exp(-a*vrelPrev^2)+2/k+2*s0(2));
            eps = abs(vrel-vrelPrev);
            vrelPrev = vrel;
            i = i + 1;
            if i > 10000
                disp('Nope');
            end
        end
        vrelSave(n) = vrel;
        excitation = k^2*Ee'*1/h(2)*Fb*BM*vrel*exp(-a*vrel^2);
        uTemp = B * u + C * uPrev - excitation';
    else
        % temporary value for the next u (without the spring forces)
        uTemp = B * u + C * uPrev; % + K*exciter(n);
    end 
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
    testMatZeros(2:end-1, 2:end-1) = testmat;
    testMatZerosPrev(2:end-1, 2:end-1) = testmatPrev;
        
    for i = 1:Ny
        uE(1 + (i-1) * (Nx + 2) : i * (Nx + 2)) = testMatZeros(:,i); 
        uEPrev(1 + (i-1) * (Nx + 2) : i * (Nx + 2)) = testMatZerosPrev(:,i); 
    end
    % energystuff
    potEnergyString(n) = gamma^2 / 2 * sum (1 / h(1) * (u(stringVec + 1) - u(stringVec)) .* (uPrev(stringVec + 1) - uPrev(stringVec))) ...
        + kappaS^2 / 2 * 1/h(1)^3 * sum((u(stringVec + 1) - 2 * u(stringVec) + u(stringVec - 1)) ...
        .* (uPrev(stringVec + 1) - 2 * uPrev(stringVec) + uPrev(stringVec - 1)));
    kinEnergyString(n) = 1 / 2 * sum (h(1) * ((1 / k * (u(stringVec) - uPrev(stringVec))).^2));
    
    kinEnergyPlate(n) = 1 / 2 * h(2)^2 * sum(1/k^2 * (u(plateVec) - uPrev(plateVec)).^2);
    potEnergyPlate(n) = kappaP / (2 * h(2)^2) * sum(uE(indices + 1)...
        + uE(indices - 1) + uE(indices + (Nx + 2)) + uE(indices - (Nx + 2)) - 4 * uE(indices))...
        * sum (uEPrev(indices + 1) + uEPrev(indices - 1)...
        + uEPrev(indices + (Nx + 2)) + uEPrev(indices - (Nx + 2)) - 4 * uEPrev(indices));

%     potEnergyPlate(n) = kappaP / (2 * h(2)^2) * sum(u(indices + 1)...
%         + u(indices - 1) + u(indices + Nx) + u(indices - Nx) - 4 * u(indices))...
%         * sum (uPrev(indices + 1) + uPrev(indices - 1)...
%         + uPrev(indices + Nx) + uPrev(indices - Nx) - 4 * uPrev(indices))...
%         +...%edges
%         + (u(indices(2)) + u(indices((Nx-2) + 1)) - 4 * u(indices(1)))...
%         * (uPrev(indices(2)) + uPrev(indices((Nx-2)+ 1)) - 4 * uPrev(indices(1)))...
%         + (u(indices(2)) + u(indices((Nx-2) + 1)) - 4 * u(indices(1)))...
%         * (uPrev(indices(2)) + uPrev(indices((Nx-2)+ 1)) - 4 * uPrev(indices(1)))...
%         ; %corners
%     potEnergyPlate(n) = kappaP / (2 * h(2)^2) * sum(
%     for x = 1:Nx

%         for y = 1:Ny
%             if x == 1
%                 if y == 1 || y == Ny % in a corner
%                     potEnergyPlate(n) = potEnergyPlate(n) + 
%                 end
%             elseif x == Nx - 1
%             end
%             if 
%             end
%         end
%     end
    out(n) = uNext(floor(N(1)/2));
    out2(n) = u(round(Ntot - 15));
    if mod(n,1) == 0 && drawBar == true
        subplot(2,1,1)
        plot(u(vec{1}))
%         u = ones(length(u),1);
        subplot(2,1,2)
%         plot(u(vec{2}))
        for q = 0:Nx-1
            testmat(q+1, 1:Ny) = u(N(1)+1+q*Ny:N(1)+(q+1)*Ny);
        end
        testMatZeros(2:end-1, 2:end-1) = testmat;
        mesh(testMatZeros);
        drawnow;
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