function [ ] = saveStatesAndMeasurements(prefix, path, w,  bestP, bestM, result, errorCode)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

[preparations, measurements, vars] = size(w);
outcomes = vars + 1;
[states, s3, s3] = size(bestP{1,1});

f = fopen([ path prefix '_results.m'], 'w');

valid = true;


if valid
    
    if (errorCode == 0)
        fprintf(f, '\nvalue = %f;', result);
        fprintf(f, '\nstatus = %f;', 1);
        
        writeDouble(path, [prefix 'Value'], result);
        writeInt(path, [prefix 'Status'], 1);
    else
        fprintf(f, '\nerror = %d;', errorCode);
        fprintf(f, '\nstatus = %f;', -1);
        
        writeInt(path, [perfix 'Status'], -1);
    end
    
    for prep = 1 : preparations
        varName = sprintf('P{%d}', prep);
        writeMat(f, varName, value(bestP{prep}));

        if (states == 2)
        a = matrixToAngles(bestP{prep});

        sNameThe = sprintf('%sP%dThe', prefix, prep);
        sNamePhi = sprintf('%sP%dPhi', prefix, prep);
        writeDouble(path, sNameThe, a(1));
        writeDouble(path, sNamePhi, a(2));
        end
    end
    
    for mes = 1 : measurements
        for o = 1 : outcomes
            
            varName = sprintf('M{%d}{%d}', mes, o);
            writeMat(f, varName, value(bestM{mes}{o}));

            % is projective ?
            prod = bestM{mes}{o}*bestM{mes}{o};
            isProj = abs(trace(prod) - 1) < 1e-5;
            fprintf(f, '\nM%d_%d_isProj = %d;', mes, o, isProj);

            if (states == 2)
            a = matrixToAngles(bestM{mes}{o});
            mNameThe = sprintf('%sM%d_%dThe', prefix, mes, o);
            mNamePhi = sprintf('%sM%d_%dPhi', prefix, mes, o);
            writeDouble(path, mNameThe, a(1));
            writeDouble(path, mNamePhi, a(2));
            end

        end
        
    end
end
fclose(f);
result
end

