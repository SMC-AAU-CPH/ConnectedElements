function [j, l, E] = createConnectionStringString(connection, N, k, h, massRatio, s0, matIdx)
    
    EcA = zeros(N(1), 1);
    connPoint = floor(connection(3) * N(1));
    if connection(5) == 1
        EcA (connPoint) = 1;
    else
        rcW = connection(5);
        EcA(connPoint-floor(rcW/2):connPoint+ceil(rcW/2)) = (1-cos(2*pi*(0:rcW)/rcW))/2;
    end
    jA = (k^2 * EcA) / (1 + s0(1) * k);
    
    EcB = zeros(N(2), 1);
    connPoint = floor(connection(4) * N(2));
    if connection(6) == 1
        EcB (connPoint) = 1;
    else
        rcW = connection(6);
        EcB(connPoint-floor(rcW/2):connPoint+floor(rcW/2)) = (1-cos(2*pi*(1:rcW)/rcW))/2;
    end
    jB = -(k^2 * EcB * massRatio) / (1 + s0(2) * k);

    j = zeros(N(3), 1);
    
    j(matIdx(1):matIdx(1) + N(1) - 1) = jA;
    j(matIdx(2):matIdx(2) + N(2) - 1) = jB;
    
    l = zeros(N(3), 1);
    l(matIdx(1):matIdx(1) + N(1) - 1) = 1/h(1) * EcA';
    l(matIdx(2):matIdx(2) + N(2) - 1) = -1/h(2) * EcB';
    
    E = [EcA; EcB];
end