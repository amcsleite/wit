function [ R, M ] = randomNeumarkAlternative(field, states, measurements, outcomes, d)

% creates projectors and measurements
%
rho = zeros(d);
rho(1, 1) = 1;

    function [l] = mkl(d)
        for i = 1 : d
            for j = 1 : d
                
                if j > i
                    l(i, j ) = (rand()*pi)/2;
                end
                if j < i
                    if strcmp(field, 'real')
                        l(i, j ) = 0;
                    else
                        l(i, j ) = 2*pi*rand();
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
    u1 = UC(mkl(d));
    G{s} = u1' * rho * u1;
    b = zeros(outcomes);
    b(1, 1) = 1;
    R{s} = kron(b, G{s});
end

for m = 1 : measurements
    u2 = UC(mkl(outcomes * d));
 
    for o = 1 : outcomes
        a = zeros(outcomes);
        a(o, o) = 1;
        z = kron(a, eye(d));
        
        M{m}{o} = u2' * z * u2;
        %H{m}{o} = M{m}{o}(1 : 1 + d - 1, 1 : 1 + d - 1);
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

    for s = 1 : states
        for m = 1 : measurements
            sum = zeros(d);
            
            for o = 1 : outcomes
                
                sum = sum +  H{m}{o};
            end
            assert(trace(abs(sum - eye(d))) <= 1e-12);
        end
    end

end

end
