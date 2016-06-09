function [bestEverResult, bestEverP, bestEverM] = mSeeSaw(w, states, repeat, P, M, field, proj, useEV)
% see / saw
% w      - is the witnness
% states - is the Hilbert space dimension
% repeat - number of iterations (result will be the best one)
% P      - initial preparations it is as cell array with p states x states matrices
% M      - initial measurements it is a cell array with m measurents each m is a
%          set of outcomes - 1 matrices
% field  - real or complex
% proj   - set to true to force projective measurements or false for POVMs
% useEV  - set to true to use expected values false for probabilities
%-- outputs --
%bestEverResult - well that
%bestEverP      - as a cell array
%bestEverM      - cell array

solver = 'sedumi';
%solver = 'mosek';  %faster

constraints = [];
options = sdpsettings('solver', solver);
options = sdpsettings(options, 'verbose', 0);

    function clearCt()
        constraints = [];
    end
    function addCt(c)
        constraints = [constraints, c];
    end

[preparations, measurements, vars] = size(w);
outcomes = vars + 1;

    function setAllProjective(m)
        for o1 = 1 : states - 1
            addCt(trace(M{m}{o1}) == 1);
        end
        addCt(trace(M{m}{states}) <= 1);
    end
    function setAtLeasOneNotProjective(m, o)
        
        i = (m - 1)*measurements + o;
        
        if i == iSel
            addCt(trace(M{m}{o}) <= 0.90);
        else
            addCt(trace(M{m}{o}) <= 1);
        end
    end

    function [bestP, bestM, result, errorCode] = SDPMeasurements(w, states, p, mes, result)
        
        bestP = p;
        bestM = mes;
        
        [preparations, measurements, vars] = size(w);
        outcomes = vars + 1;
        
        clearCt();
        
        for m = 1 : measurements
            sumM = 0;
            
            for o = 1 : outcomes
                
                M{m}{o} = sdpvar(states, states, 'hermitian', field);
                
                if ~isempty(mes)
                    assign(M{m}{o}, mes{m}{o});
                end
                addCt(M{m}{o} >= 0);
                setAtLeasOneNotProjective(m, o);
                
                sumM = sumM + M{m}{o};
            end
            
            if iSel == 0
                setAllProjective(m);
            end
            
            addCt(sumM == eye(states));
            
        end
        
        valid = false;
        
        oM = 0;
        
        for r = 1 : preparations
            for c = 1 : measurements
                
                if (vars == useEV)
                    oM = oM + w(r, c, 1)  * (2*trace(p{r} * M{c}{1}) - 1 );
                else
                    for o = 1 : vars
                        oM = oM + w(r, c, o) * trace(p{r} * M{c}{o});
                    end
                    
                end
            end
        end
        
        errorCode = optimize(constraints, -(real(oM)), options);
        
        if (errorCode.problem ~= 0)
            error(errorCode.info);
            return;
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
    function [bestP, bestM,result, errorCode] = SDPStates(w, states, p, mop, result)
        
        we = witEvaluate(p, mop, w);
        
        bestP = p;
        bestM = mop;
        
        [preparations, measurements, vars] = size(w);
        outcomes = vars + 1;
        
        clearCt();
        
        for n = 1 : preparations
            P{n} = sdpvar(states, states, 'hermitian', field);
            assign(P{n}, p{n});
            addCt(P{n} >= 0);
            addCt(trace(P{n}) == 1);
        end
        
        valid = false;
        
        oP = 0;
        for r = 1 : preparations
            for c = 1 : measurements
                
                if (vars == useEV)
                    oP = oP + w(r, c, 1)  * (2*trace(P{r} * mop{c}{1}) - 1 );
                else
                    for o = 1 : vars
                        oP = oP + w(r, c, o) * trace(P{r} * mop{c}{o});
                    end
                    
                end
            end
        end
        
        errorCode = optimize(constraints, -(real(oP)), options);
        
        if (errorCode.problem ~= 0)
            error(errorCode.info);
            return;
        end
        
        if value(oP) > result
            result = value(oP);
            
            for n = 1 : preparations
                bestP{n} = value(P{n});
            end
            valid = true;
        end
    end

% main

try
    bestEverResult = -1e9;
    
    if isempty(M)
        mFirst = true;
    else
        mFirst = false;
    end
    
    if proj
        iAllSel = 0;
    else
        iAllSel = measurements*outcomes;
    end
    
    for iSel = 0 : iAllSel
        for rep = 1 : repeat
            
            old = -100;
            delta = 100;
            bestResult = -1e9;
            
            while (delta > 0.00001)
                
                if (mFirst)
                    [P, M, bestResult, errorCode] = SDPMeasurements(w, states,  P, M, bestResult);
                    [P, M, bestResult, errorCode] = SDPStates(w, states, P, M, bestResult);
                else
                    [P, M, bestResult, errorCode] = SDPStates(w, states, P, M, bestResult);
                    [P, M, bestResult, errorCode] = SDPMeasurements(w, states,  P, M, bestResult);
                end
                
                delta = abs(old - bestResult);
                old = bestResult;
                
                we = witEvaluate(P, M, w);
            end
            
            if bestResult > bestEverResult;
                bestEverP = P;
                bestEverM = M;
                bestEverResult = bestResult;
            end
        end
    end
catch e
    disp('----');
    e
end
end

