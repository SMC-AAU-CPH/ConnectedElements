clear all;
close all
numRuns = 100;
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
connP = zeros(numRuns, 1);
connS = zeros(numRuns, 1);
maxTotEnergyDiff = zeros(numRuns,1);
for i = 1:numRuns
    disp(i)
    fs(i) = randomiser(22050, 44100);
    rhoS(i) = randomiser(785, 7850);
    r(i) = randomiser(0.001, 0.005);
    T(i) = randomiser(50, 1000);
    kappaS(i) = randomiser(0.1, 10);
    rhoP(i) = randomiser(4400, 7850);
    H(i) = randomiser(0.001, 0.005);
    EP(i) = randomiser(2E8, 2E12);
    Lx(i) = randomiser(0.5, 1);
    Ly(i) = Lx(i);
    connS(i) = randomiser(0.2, 0.8);
    connP(i) = randomiser(0.2, 0.8);
    maxTotEnergyDiff(i) = connectedStringPlateLoop (fs(i), rhoS(i), r(i),...
        T(i), kappaS(i), rhoP(i), H(i), EP(i), Lx(i), Ly(i), connS(i), connP(i));
end
plot(maxTotEnergyDiff)