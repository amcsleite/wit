function [ R, M, param ] = randomNeumarkAlternative(field, d, x, param, states, measurements, outcomes)
% creates projectors and measurements
%
rho = zeros(d);
rho(2, 2) = 1;

    function [v] = getRandomParam()
        v = x(param);
        param = param + 1;
    end
    function [l] = mkl(dim)
        for i = 1 : dim
            for j = 1 : dim
                
                if j > i
                    l(i, j ) = (getRandomParam()*pi)/2;
                end
                if j < i
                    if strcmp(field, 'real')
                        l(i, j ) = 0;
                    else
                        l(i, j ) = 2*pi*getRandomParam();
                    end
                end
                
                if i == j
                    l(i, j ) = 0;
                end
                
            end
        end
    end
    function [b] = isPositiveOrZero(M)
        e = eig(M);
        b = all( e > -1e-17);
    end

for s = 1 : states
    
    u1 = UC(mkl(2));
    G{s} = u1'*rho*u1;
    b = zeros(outcomes);
    b(1, 1) = 1;
    
    R{s} = kron(b, G{s});
end

for m = 1 : measurements
    u2 = UC(mkl(6));
    ix = 1;
    for o = 1 : outcomes
        z = zeros(outcomes*d);
        
        z(ix, ix) = 1;
        ix = ix + 1;
        z(ix, ix) = 1;
        ix = ix + 1;
        
        M{m}{o} = u2'* z * u2;
    end
end

if false
    for s = 1 : states
        for m = 1 : measurements
            sum = zeros(outcomes * d);
            
            for o = 1 : outcomes
                
                sum = sum +  M{m}{o};
            end
            assert(trace(abs(sum - eye(outcomes * d))) <= 1e-12);
        end
    end
end

end
