clear all;
close all
numRuns = 500;
fs = zeros(numRuns, 1);
rhoS = zeros(numRuns, 1);
r = zeros(numRuns, 1);
T = zeros(numRuns, 1);
kappaS = zeros(numRuns, 1);
rhoP = zeros(numRuns, 1);
H = zeros(numRuns, 1);
EP = zeros(numRuns, 1);
Lx = zeros(numRuns, 1);
Ly = zeros(numRuns, 1);
maxTotEnergyDiff = zeros(numRuns,1);
for i = 1:numRuns
    disp(i)
    fs(i) = randomiser(22050, 44100);
    rhoS(i) = randomiser(7850, 7850);
    r(i) = randomiser(0.0005, 0.001);
    T(i) = randomiser(50, 1000);
    kappaS(i) = randomiser(0.1, 10);
    rhoP(i) = randomiser(7850, 7850);
    H(i) = randomiser(0.001, 0.005);
    EP(i) = randomiser(2E11, 2E11);
    Lx(i) = randomiser(0.5, 1);
    Ly(i) = Lx(i);
    maxTotEnergyDiff(i) = connectedStringPlateLoop (fs(i), rhoS(i), r(i),...
        T(i), kappaS(i), rhoP(i), H(i), EP(i), Lx(i), Ly(i));
end
plot(maxTotEnergyDiff)