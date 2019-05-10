clear all
close all

fs = 44100;
k = 1/fs;
lengthSound = fs;


%% Drawing functions
drawMasses = true;

if drawMasses
    figure('Renderer', 'painters', 'Position', [100 100 700 900])
    drawSpeed = 1;
end

%% Initialise Mass 1
M1 = 0.01;                  % Mass
K1 = 10000;                  % Spring Coefficient
omega1 = sqrt(K1 / M1);     % Angular frequency of oscillation
offset1 = 1;                % Center of oscillation (where the mass oscillates around)
u1Next = 0;
u1 = offset1 + 0.25;        % Initial offset
u1Prev = u1;

%% Initialise Mass 2
M2 = 0.01;                  % Mass
K2 = 1000;                 % Spring Coefficient
omega2 = sqrt(K2 / M2);     % Angular frequency of oscillation
offset2 = 0;                % Center of oscillation (where the mass oscillates around)
u2Next = 0;
u2 = offset2 - 0.25;        % Initial offset
u2Prev = u2;

%% Connection
w0 = 100;                   % Linear spring coefficient
w1 = 10;                    % Non-linear spring coefficient
sx = 0.00;                  % Damping coefficient (creates pitch glides here)

%% Set previous relative displacement
etaPrev = u1 - u2;

totEnergy = zeros(lengthSound, 1);

for n = 1:lengthSound
    
    %% Calculate relative displacement
    eta = u1 - u2;
    
    %% Calculate known part of FDSs
    u1Next = 2 * u1 - u1Prev - k^2 * omega1^2 * (u1 - offset1);
    u2Next = 2 * u2 - u2Prev - k^2 * omega2^2 * (u2 - offset2);
    
    %% Calculate forces
    a = w0^2 + w1^4*eta^2 + 2 * sx / k;
    r = (-w0^2 - w1^4*eta^2 + 2 * sx / k) / a;
    if w0 == 0 && w1 == 0
        Fc = 0;
    else
        Fc = (u1Next - u2Next - r * etaPrev) / (-k^2 / M1 - k^2 / M2 - 2 / a);
    end
    
    %% Add to the previously calculated FDSs
    u1Next = u1Next + k^2 * Fc / M1;
    u2Next = u2Next - k^2 * Fc / M2;
    
    %% Energy analysis
    
    % Mass1
    kinEnergy1(n) = M1 / 2 * (1/k * (u1 - u1Prev)).^2;
    potEnergy1(n) = -K1 / 2 * ((u1 - offset1) * (u1Prev - offset1));
    energy1(n) = kinEnergy1(n) - potEnergy1(n);
    
    % Mass2
    kinEnergy2(n) = M2 / 2 * (1/k * (u2 - u2Prev)).^2;
    potEnergy2(n) = -K2 / 2 * ((u2 - offset2) * (u2Prev - offset2));
    energy2(n) = kinEnergy2(n) - potEnergy2(n);
    
    % Connection
    linConnEnergy(n) = w0^2 / 4 * (eta^2 + etaPrev^2);
    nonLinConnEnergy(n) = w1^4 / 2 * (1/2 * eta^2 * etaPrev^2);
    totEnergy(n) = energy1(n) + energy2(n) + linConnEnergy(n) + nonLinConnEnergy(n);
    
    %% Save states
    u1Save(n) = u1Next;
    u2Save(n) = u2Next;
    
    %% Draw
    limX = floor(n*2);
    if drawMasses && mod(n, drawSpeed) == 0
        subplot(3,1,1)
        cla
        hold on;
        
        % plot mass trajectories
        plot(u1Save(1:n), 'b');
        plot(u2Save(1:n), 'r');
        
        % plot connection with colour depending on eta
        c = -(abs(eta) + 1)^2 / ((abs(eta) + 1)^2 -1) + 2;
        if c < 0
            c = 1;
        else
            c = 1-c;
        end
        plot([n n], [u1Next u2Next], '-.', 'Color', [c,c,c], 'Linewidth', 2);
        
        % scatter current states of masses
        scatter(n, u1Next, 400, '.', 'b')
        scatter(n, u2Next, 400, '.', 'r')
        
        % plot centers of oscillation
        plot([1:limX+2], ones(1, limX+2) * offset1, '--b')
        plot([1:limX+2], ones(1, limX+2) * offset2, '--r')
        
        % extra function
        xlim([1 limX+1])
        grid on
        legend(["Trajectory u_1", "Trajectory u_2", "Connection", "u_1^n", "u_2^n"])
        title("Current and past states of the system")
        
        subplot(3,1,2)
        cla
        hold on;
        plot(energy1(1:n)+energy2(1:n))
        plot(linConnEnergy(1:n) + nonLinConnEnergy(1:n))
        legend(["Energy masses", "Energy connection"])
        subplot(3,1,3)
        plot(totEnergy(1:n) / totEnergy(1) - 1);
        title("Normalised Energy (should be within machine precision (10^{-15}) if s_x = 0")
        drawnow;
    end
    
    
    %% Update states
    u1Prev = u1;
    u1 = u1Next;
    
    u2Prev = u2;
    u2 = u2Next;
    
    etaPrev = eta;
end

if ~drawMasses
    subplot(2,1,1);
    plot(u1Save)
    title("Output u_1")
    subplot(2,1,2);
    plot(u2Save)
    title("Output u_2")
end