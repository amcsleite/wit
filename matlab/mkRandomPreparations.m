function [ p ] = mkRandomPreparations(set, preparations, states )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

   for n = 1 : preparations
        rho = zeros(states);
        rho(2,2) = 1;
        
        for i = 1 : states
            for j = 1 : states
                
                if j > i
                     l(i, j ) = (rand()*pi)/2;
                end
                if j < i
                    if strcmp(set, 'real')
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
   end
    
end

