function [ out ] = wMat(prefix, w, states, wPath, repeat, set)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

rng('shuffle', 'twister');
logFile = fopen('runMat.log', 'w');

[preparations, measurements, vars] = size(w);


    function [ bestP, bestM, result, errorCode] = SDPMeasurements(set, w, states, logFileID, path, repeat, p, mes, result)
        %UNTITLED Summary of this function goes here
        %   Detailed explanation goes here
        mkpath(path);
        options = sdpsettings('solver', 'mosek');
        options = sdpsettings(options, 'verbose', 0);
        
        bestP = p;
        bestM = mes;
        
        [preparations, measurements, vars] = size(w);
        outcomes = vars + 1;
        
        cMeasurements = [];
        
        for m = 1 : measurements
            sumM = 0;
            
            for o = 1 : outcomes
                
                M{m}{o} = sdpvar(states, states, 'hermitian', set);
                if ~isempty(mes)
                    assign(M{m}{o}, mes{m}{o});
                end
                cMeasurements = [cMeasurements,M{m}{o} - 1e-6>=0];
                
                %cMeasurements = [cMeasurements (M{m, o} - eye(states)) <= -1e-6];
                
                sumM = sumM + M{m}{o};
            end
            
            cMeasurements = [cMeasurements,sumM == eye(states)];
            
        end
        
        valid = false;
        
        for rep = 0 : repeat
            old = -100;
            delta = 1;
            
            oM = 0;
            
            for r = 1 : preparations
                for c = 1 : measurements
                    
                    if (vars == 1)
                        oM = oM + w(r, c, 1)  * (2*trace(p{r} * M{c}{1}) - 1 );
                    else
                        for o = 1 : vars
                            oM = oM + w(r, c, o) * trace(p{r} * M{c}{o});
                        end
                        
                    end
                end
            end
            
            errorCode = optimize(cMeasurements, -(real(oM)), options);
            
            if (errorCode.problem ~= 0)
                break;
            end
            
            if value(oM) > result
                result = value(oM);
                bestP = p;
                
                for c = 1 : measurements
                    for o = 1 : outcomes
                        bestM{c}{o} = value(M{c}{o});
                    end
                end
                
                valid = true;
            end
            
        end
        
        
    end
    function [bestP, bestM,result, errorCode] = SDPStates(set, w, states, logFileID, path, repeat, p, mop, result)
        %UNTITLED Summary of this function goes here
        %   Detailed explanation goes here
        mkpath(path);
        options = sdpsettings('solver','mosek');
        options = sdpsettings(options, 'verbose', 0);
        
        we = witEvaluate(p, mop,w);
        
        bestP = p;
        bestM = mop;
        
        [preparations, measurements, vars] = size(w);
        outcomes = vars + 1;
        
        cStates = [];
        
        for n = 1 : preparations
            P{n} = sdpvar(states, states, 'hermitian', set);
            assign(P{n}, p{n});
            cStates = [cStates,P{n}>=1e-6];
            cStates = [cStates,trace(P{n})==1];
        end
        
        valid = false;
        
        for rep = 0 : repeat
            oP = 0;
            for r = 1 : preparations
                for c = 1 : measurements
                    
                    if (vars == 1)
                        oP = oP + w(r, c, 1)  * (2*trace(P{r} * mop{c}{1}) - 1 );
                    else
                        for o = 1 : vars
                            oP = oP + w(r, c, o) * trace(P{r} * mop{c}{o});
                        end
                        
                    end
                end
            end
            
            errorCode = optimize(cStates, -(real(oP)), options);
            
            if (errorCode.problem ~= 0)
                errorCode.problem
                break;
            end
            
            if value(oP) > result
                result = value(oP)
                
                for n = 1 : preparations
                    bestP{n} = value(P{n});
                end
                valid = true;
            end
            
        end
        
    end

% main
M={}

fprintf(logFile, '\n----------------------------------------------------------------\n');
fprintf(logFile, 'Preparations: %d Measurements: %d States: %d vars: %d ', preparations, measurements, states, vars);

bestResult = -1e9;
bestEverResult = -1e9;
for rep = 1 : repeat
    old = -100;
    delta = 100;
    
    P = mkRandomPreparations(set, preparations, states);
    
    while (delta > 0.00001)
        
        [P, M, bestResult, errorCode] = SDPMeasurements(set, w, states, logFile, wPath, 1, P, M, bestResult);
        
        we = witEvaluate(P, M, w)
        
        [P, M, bestResult, errorCode] = SDPStates(set, w, states, logFile, wPath, 1, P, M, bestResult);
        
        delta = abs(old - bestResult);
        old = bestResult;
        
        we = witEvaluate(P, M, w)
        bestResult
        
        %if we <0
        %   aaa = 1;
        %end
    end
    
    if bestResult > bestEverResult;
        bestEverP = P;
        bestEverM = M;
        bestEverResult = bestResult;
    end
end

%save([wPath 'M'], 'M');
%save([wPath 'P'], 'P');
%save([wPath 'bestResult'], 'bestResult');

saveStatesAndMeasurements([prefix], wPath, w,  bestEverP,  bestEverM, bestEverResult, errorCode.problem);

fclose(logFile);

end

