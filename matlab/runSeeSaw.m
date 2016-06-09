function [ out ] = runSeeSaw(file)
rng('shuffle', 'twister');
pwd

[a, c] = eval(file);

[~, nWit] = size(a);

states = 2;

[preparations, measurements, vars] = size(a{1});
% vars == independent outcomes they are the ones on the witness

outcomes = vars + 1;

for n = 1 : nWit
    
    P = mkRandomPreparations('real', preparations, states);
    P{1} = zeros(states);
    P{1}(1, 1) = 1;
    
    M = {};
    
    vs{n}{1} = c{n};
    [vs{n}{2}, vs{n}{3}, vs{n}{4}] = mSeeSaw(a{n}, states, 5, P, M, 'real', true, false);
    [vs{n}{5}, vs{n}{6}, vs{n}{7}] = mSeeSaw(a{n}, states, 5, P, M, 'real', false, false);
    
    
    disp('----------------');
    v(n,1) = vs{n}{1}; 
    v(n,2) = vs{n}{2};
    v(n,3) = vs{n}{5}
    
end
% vs variable contains both values and the states
% v value contains only values

%rename and save
vsName = ['ssAll_' file];

eval( [vsName '= vs']);
save( vsName, vsName);

vName = ['ssValues_' file];
eval( [vName '= v']);
save( vName, vName);
disp('--- Results ----')
disp('    classic   NPA       projective POVM')
v
end
