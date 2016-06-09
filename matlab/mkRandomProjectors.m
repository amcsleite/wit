function [ p ] = mkRandomProjectors(field, nProjectors, d)
% creates nProjectors of dimension d
% creates nProjectors of dimension d using a Random unitary.
% field can be'real' or 'Complex'

for n = 1 : nProjectors
    rho = zeros(d);
    rho(2, 2) = 1;
    
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
    p{n} = u'*rho*u;
    assert(abs(trace(p{n}) - 1) < 1e-10);
    p2 =trace(p{n}*p{n});
    assert(abs(trace(p2) - 1) < 1e-10);
    
    h = trace(p{n}' - p{n}) ;
    assert( h == 0);
    
end
end

