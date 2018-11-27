clear all;
close all;
clc;

fs = 44100;      % Sampling rate
k = 1 / fs;     % Time step


%% Create objects 
% Bars: stiffness, freq. dep. damping, freq. indep. damping
% Strings: wavespeed, stiffness, freq. dep. damping, freq. indep. damping

objectVars{1, 1} = "string";
freq1 = 110.0;
objectVars{1, 2} = [freq1*2, 2, 1, 0.005];

objectVars{2, 1} = "bowedString";
freq2 = freq1 * 2^(7/12);
objectVars{2, 2} = [freq2*2, 2, 1, 0.005];

% objectVars{3, 1} = "plate";
% objectVars{3, 2} = [1, 1, 0.005, 4];

Q = length(objectVars); % amount of objects

%initialise cells/vectors depending on the number of objects
Bpre = cell(Q, 1);
Cpre = cell(Q, 1);

bB = cell(Q, 1);
bC = cell(Q, 1);

N = zeros(Q,1);
h = zeros(Q,1);
matIdx = zeros(Q,1);

Nx = cell(Q, 1);
Ny = cell(Q, 1);

Ntot = 0;

whatExciter = "raisedCos";

for q = 1:Q
    
    if objectVars{q, 1} == "bar"
        [Bpre{q}, Cpre{q}, N(q), h(q), s0(q)] = createBar(objectVars{q, 2}, fs);
    elseif objectVars{q, 1} == "string"
        [Bpre{q}, Cpre{q}, N(q), h(q), s0(q)] = createString(objectVars{q, 2}, fs);
    elseif objectVars{q, 1} == "bowedString"
%         whatExciter = "bowed";
        [Bpre{q}, Cpre{q}, bB{q}, bC{q}, N(q), h(q), s0(q)] = createBowedString(objectVars{q, 2}, fs);
    elseif objectVars{q, 1} == "plate"
        [Bpre{q}, Cpre{q}, N(q), h(q), s0(q), Nx{q}, Ny{q}] = createPlate(objectVars{q, 2}, fs);
    end
    matIdx(q) = Ntot + 1;
    Ntot = Ntot + N(q);
end

vec = cell(Q, 1);
for q = 1:Q
    vec{q} = matIdx(q):matIdx(q)+N(q)-1;
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
rcW = 2;
exciterPos = 0.5;
% u(matIdx(1) + floor(exciterPos*N(1)-rcW/2):matIdx(1) + floor(exciterPos*N(1)+rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW)) * 0.5;
u(floor(exciterPos*N(1))) = 1;
uPrev = u;
uNext = u;

%% Length of the sound
lengthSound = fs*5;


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

% 1st, 2nd object, 1st, 2nd location of connection and 1st, 2nd width of connection
conn = [1, 2, 0.5, 0.4, 1, 1];
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

sx(1) = 1;
w0(1) = 10; 
w1(1) = 1000000; 

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
            

for n = 1 : lengthSound
    
    % calculate relative displacement
    etaR = L*u;
    
    % update r and p
    rn = (2*sx/k - w0.^2 - w1.^4.*(etaR).^2)./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    pn = -2./(2*sx/k + w0.^2 + w1.^4.*(etaR).^2);
    
    Rn = eye(Qc).*rn; 
    Pn = eye(Qc).*pn; 
    
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
    bn = L * uTemp; 
    an = Rn * etaRPrev; 
    
    Fc = (L*J-Pn)\(an - bn);
    JFc = J * Fc;
    uNext = uTemp + JFc;
    
    out(n) = uNext(floor(N(1)/2));
    out2(n) = u(round(Ntot - 15));
    if mod(n,1) == 0 && drawBar == true
        subplot(2,1,1)
        plot(u(vec{1}))
%         u = ones(length(u),1);
        subplot(2,1,2)
        plot(u(vec{2}))
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