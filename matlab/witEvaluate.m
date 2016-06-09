function [ result ] = witEvaluate(P, M, w)

result = 0;

[preparations, measurements, vars] = size(w);

if vars == 1
    for p = 1 : preparations
        for m = 1 : measurements
            result = result + w(p, m, 1)  * (2*trace(P{p} * M{m}{1}) - 1 );
        end
    end
else
    for p = 1 : preparations
        for m = 1 : measurements
            for o = 1 : vars
                result = result + w(p, m, o)  * trace(P{p} * M{m}{o});
            end
        end
    end
end
end

