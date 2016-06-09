function [mProj] = mkProjectorSet(field, measurements, d, outcomes)
% creates a full set for dimension
% creates nProjectors of dimension d using a Random unitary.
% field can be'real' or 'Complex'

for mes = 1 : measurements
    
    rho = zeros(d);
    rho(1, 1) = 1;
    
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
    
    u = UC(l);
    
    for n = 1 : d
        m = zeros(d);
        m(n, n) = 1;
        
        mp{n} = u' * m * u;
        
        t = trace(mp{n});
        
        assert(abs(t) - 1 < 1e-10);
        p2 = trace(mp{n}' * mp{n});
        assert(abs(trace(p2) - 1) < 1e-10);
        
        if n == 1
            sum = mp{n};
        else
            sum = sum + mp{n};
        end
    end
    
    for n = d + 1 : outcomes
        mp{n} = zeros(d);
    end
    
    sum = sum - eye(d);
    assert(trace(sum'*sum)  < 1e-10);

    mProj{mes} = mp(1 : outcomes - 1);
    
%     mp0 = mp([1 2 3]);
%     
%     mp1{1} = mp0{1} + mp0{2};
%     mp1{2} = mp0{2};
%     mp1{3} = mp0{3};
%     
%     mProj{mes} = mp1([2 1]);
%     disp(value(mProj));
end
end

