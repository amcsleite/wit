function [out] = runNPADimNeumark(permutation, constraints, mM, witness, dimension)
rng('shuffle', 'twister');

[preparations, measurements, vars] = size(witness);
outcomes = vars + 1;
iOutcomes = vars;

z = zeros(outcomes);
z(1, 1) = 1;

options = sdpsettings('solver', 'sedumi');
%options = sdpsettings('solver', 'mosek');
options = sdpsettings(options, 'verbose', 1);
options = sdpsettings(options, 'debug', 1);

startP = 1;

    function [of] = mkObjF1()
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = permutation(o);
            
            if m > measurements || oPer >= outcomes
                out = 0;
                return
            end
            
            out = witness(p, m, oPer);
            
        end
        
        % prepare objective function
        for iP = 1 : preparations
            for iM = 1 : measurements
                for iO = 1 : outcomes - 1
                    row = startP + iP;
                    col = startP + preparations + (iM - 1) * iOutcomes + iO;
                    
                    wVal = permutedWitness(iP, iM, iO);
                    
                    if first
                        of = wVal * mM(row, col);
                        first = false;
                    else
                        of = of + wVal * mM(row, col);
                    end
                end
            end
        end
    end
    function [of] = evaluate1(permu)
        permutation = permu;
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = permutation(o);
            
            if m > measurements || oPer >= outcomes
                out = 0;
                return
            end
            
            out = witness(p, m, oPer);
            
        end
        
        % prepare objective function
        for iP = 1 : preparations
            for iM = 1 : measurements
                for iO = 1 : outcomes - 1
                    row = startP + iP;
                    col = startP + preparations + (iM - 1) * iOutcomes + iO;
                    
                    wVal = permutedWitness(iP, iM, iO);
                    
                    if first
                        of = wVal * mM(row, col);
                        first = false;
                    else
                        of = of + wVal * mM(row, col);
                    end
                    disp([num2str(iP) ' ' num2str(iM) ' ' num2str(iO ) '    ' num2str(wVal) ' x  ' num2str(value(mM(row, col))) ' = ' num2str(value(wVal * mM(row, col)))]);
                    
                end
            end
        end
    end

%----------------- main --------------------------------

objF = mkObjF1();
errorCode = optimize(constraints, -real(objF), options);

result = value(objF);
mxs = value(mM);

disp(errorCode);


disp(mxs);

disp('result:');
disp(result);
out = result;
end
