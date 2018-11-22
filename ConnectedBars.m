clear all;
close all;
clc;

fs = 44100;      % Sampling rate
k = 1 / fs;     % Time step


%% Create objects 
% Bars: stiffness, freq. dep. damping, freq. indep. damping
% Strings: wavespeed, stiffness, freq. dep. damping, freq. indep. damping

objectVars{1, 1} = "string";
freq1 = 196.0;
objectVars{1, 2} = [freq1*2, 2, 0.1, 0.005];

objectVars{2, 1} = "string";
freq2 = 196.0*2^(7/12);
objectVars{2, 2} = [freq2*2, 2, 0.1, 0.005];

% objectVars{3, 1} = "plate";
% objectVars{3, 2} = [1, 1, 0.005, 4];

Q = length(objectVars); % amount of objects

%initialise cells/vectors depending on the number of objects
Bpre = cell(Q, 1);
Cpre = cell(Q, 1);
N = zeros(Q,1);
h = zeros(Q,1);
matIdx = zeros(Q,1);

Ntot = 0;
numPlates = 0;
for q = 1:Q
    
    if objectVars{q, 1} == "bar"
        [Bpre{q}, Cpre{q}, N(q), h(q), s0(q)] = createBar(objectVars{q, 2}, fs);
    elseif objectVars{q, 1} == "string"
        [Bpre{q}, Cpre{q}, N(q), h(q), s0(q)] = createString(objectVars{q, 2}, fs);
    elseif objectVars{q, 1} == "plate"
        numPlates = numPlates + 1;
        [Bpre{q}, Cpre{q}, N(q), h(q), s0(q), Nx(numPlates), Ny(numPlates)] = createPlate(objectVars{q, 2}, fs);
    end
    matIdx(q) = Ntot + 1;
    Ntot = Ntot + N(q);
end


%% Create full matrices
B = zeros(Ntot);
C = zeros(Ntot);

for q = 1:Q
    B(matIdx(q):matIdx(q)+N(q) - 1, matIdx(q):matIdx(q)+N(q) - 1) = Bpre{q};
    C(matIdx(q):matIdx(q)+N(q) - 1, matIdx(q):matIdx(q)+N(q) - 1) = Cpre{q};
end


%% Initialise state vectors
u = zeros(Ntot, 1);
uPrev = u;
uNext = u;

%% Length of the sound
lengthSound = fs*5;

%% Exciter
Fmax = 500;
exciterLength = floor(1e-3 * fs);
v = 2; 
Fe = Fmax/2*(1-cos(v*pi*(0:exciterLength)/exciterLength));
exciter = [Fe, zeros(1,lengthSound-exciterLength - 1)];

OTE = 2; % Object To Excite
exciterPos = 1/2;
rcW = 3;

if objectVars{OTE,1} == "plate"
%     Ee = zeros(Ntot, 1);
%     Ee (matIdx(OTE) + exciterPos*N(OTE)-floor(rcW/2):matIdx(OTE) + exciterPos*N(OTE)+ceil(rcW/2)) = 1-cos(v*pi*(0:rcW)/rcW);
%     K = k^2 * Ee / (1 + s0(OTE)*k);
else
    Ee = zeros(Ntot, 1);
    Ee (matIdx(OTE) + exciterPos*N(OTE)-floor(rcW/2):matIdx(OTE) + exciterPos*N(OTE)+ceil(rcW/2)) = 1-cos(v*pi*(0:rcW)/rcW);
    K = k^2 * Ee / (1 + s0(OTE)*k);
end
%% Connections

% 1st, 2nd object,
% location of connection at 1st, 2nd object
% width of connection at 1st, 2nd object
conn = [1, 2, 0.5, 0.3, 5, 5];
%         1, 2, 0.9, 0.6, 1, 1];
%         1, 2, 0.9, 0.5, 1, 1];
    
Qc = length(conn(:,1)); % amount of connections
J = zeros(Ntot, Qc);
massRatio = 1; 

L = zeros(Qc, Ntot);

for qc = 1:Qc
    curConn = conn(qc,:);
    if curConn(3) * N(curConn(1)) + floor(curConn(5)/2) > N(curConn(1)) || curConn(3) * N(curConn(1)) - floor(curConn(5)/2) < 1
        error(['Connection ', num2str(qc), ' at component ', num2str(curConn(1)), ' is out of bounds. Change excitation position or width.'])  
    end
    
    if curConn(4) * N(curConn(2)) + floor(curConn(6)/2) > N(curConn(2)) || curConn(4) * N(curConn(2)) - floor(curConn(6)/2) < 1
        error(['Connection ', num2str(qc), ' at component ', num2str(curConn(1)), ' is out of bounds. Change excitation position or width.'])
    end

    [J(:,qc), L(qc,:)] = createConnection(curConn, [N(curConn(1)) N(curConn(2)) Ntot], k, [h(1) h(2)], massRatio, [s0(curConn(1)) s0 curConn(2)], [matIdx(curConn(1)) matIdx(curConn(2))]);

end

%% Spring coefficients 
sx = zeros(Qc, 1);
w0 = zeros(Qc, 1);
w1 = zeros(Qc, 1);

sx(1) = 0;
w0(1) = 1; 
w1(1) = 1; 

% sx(2) = 0;
% w0(2) = 1; 
% w1(2) = 1; 
% 
% sx(3) = 3;
% w0(3) = 1000; 
% w1(3) = 10; 


out = zeros(lengthSound, 1);
out2 = zeros(lengthSound, 1);
drawBar = false;

etaRPrev = zeros(Qc, 1);

% Plotstuff
% if length(connA) == 1
%     scatA = true;
% else
%     scatA = false;
% end 
% 
% if length(connB) == 1
%     scatB = true;
% else
%     scatB = false;
% end 
% plateMat = zeros(Ny-1, Nx-1);
for n = 1 : lengthSound
    
    % calculate relative displacement
    etaR = L*u;
    
    % update r and p
    rn = (2*sx/k - w0.^2 - w1.^4.*(etaR).^2)./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    pn = -2./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    
    Rn = eye(Qc).*rn; 
    Pn = eye(Qc).*pn; 
    
    % temporary value for the next u (without the spring forces)
    uTemp = B * u + C * uPrev + K*exciter(n);
    
    % find Fc
    bn = L * uTemp; 
    an = Rn * etaRPrev; 
    
    Fc = (L*J-Pn)\(an - bn);
 
    uNext = uTemp + J * Fc;
    
    out(n) = u(round(N(1)/5));
    out2(n) = u(round(Ntot - 5));
    if mod(n,1) == 0 && drawBar == true
        subplot(2,1,1)
        plot(u(matIdx(1):matIdx(1)+N(1)-1))
%         u = ones(length(u),1);
        subplot(2,1,2)
        plot(u(matIdx(2):matIdx(2)+N(2)-1))
%         for it = 1:Nx-1
%             plateMat(:,it) = u(matIdx(3) + (it - 1) * (Ny-1): matIdx(3) + it * (Ny-1) - 1);
%         end
%         surf(plateMat, 'FaceAlpha', 0.5);
%         zlim([-0.5e50 0.5e50])
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