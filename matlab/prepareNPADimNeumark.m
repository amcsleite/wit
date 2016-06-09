function [constraints, mM] = prepareNPADimNeumark(preparations, measurements, dimension, vars)
rng('shuffle', 'twister');
clc;

nAllInList = 1;
sList =  {};

field = 'real';

outcomes = vars + 1;
iOutcomes = vars;

z = zeros(outcomes);
z(1, 1) = 1;
idp = kron(z, eye(dimension));

options = sdpsettings('solver', 'mosek');
options = sdpsettings(options, 'verbose', 1);
options = sdpsettings(options, 'debug', 1);

startP = 1;
basis = {};

constraints = [];

    function [bEnd] = doGramSchmidt(M)
        [~, mxSz] = size(M);
        
        bEnd = false;
        rest = M;
        [~, count] = size(basis);
        %Md = M';
        
        for e = 1 : count
            rest = rest - basis{e} * trace(rest'*basis{e});
        end
        
        if rest - zeros(mxSz) <= 1e-11
            bEnd = true;
            return;
        end
        mod = sqrt(trace(rest'*rest));
        basis{count + 1} = rest / mod;
        count = count + 1;
        disp(count);
    end
    function [ts] = mkCrosses(pTerms, mTerms)
        [~, nPTerms] = size(pTerms);
        [~, nMTerms] = size(mTerms);
        
        ts = {};
        tc = {};
        
        for i = 1 : nPTerms
            for j = 1 : nMTerms
                t1 = pTerms{i} * mTerms{j};
                ts = [ts t1];
            end
        end
    end
    function [M] = evaluateSet(h, pTerms, mTerms)
        terms = {};
        [~, nTotal] = size(h);
        
        for n = 1 : nTotal
            [~, sz] = size(h{n});
            
            term = eye(dimension*outcomes);
            
            for t = 1 : sz
                s = h{n}{t};
                
                c1 = h{n}{t}(1);
                
                if (c1 == 'P')
                    c2 = h{n}{t}(2);
                    p = str2num(c2);
                    term = term * pTerms{p};
                end
                if (c1 == 'M')
                    c2 = h{n}{t}(2);
                    c3 = h{n}{t}(3);
                    m = str2num(c2);
                    o = str2num(c3);
                    
                    term = term * mTerms{m}{o};
                end
                if (c1 == 'I')
                    c2 = h{n}{t}(2);
                    
                    if (c2 == 'p')
                        term = term * idp;
                    end
                end
            end
            
            terms = [terms term];
            
        end
        
        [~,nProj2] = size(terms);
        
        M = [];
        for i = 1 : nProj2
            for j = 1 : nProj2
                M(i, j) = trace(terms{i}' * terms{j});
            end
        end
    end
    function mkBasis()
        first = true;
        
        while true
            
            %pTerms = mkRandomProjectors(field, preparations, dimension);
            %mTerms = mkProjectorSet(field, measurements, dimension, outcomes);
            
            [pTerms, mTerms] = randomNeumarkAlternative(field, preparations, measurements, outcomes, dimension);
            
            M = evaluateSet(set, pTerms, mTerms);
            
            if first
                sumM = M;
                first = false;
            else
                sumM = sumM + M;
            end
            
            
            %disp(value(M));
            bEnd = doGramSchmidt(M);
            
            if bEnd
                break;
            end;
            
        end
        % testPSD = chol(sumM);
        
    end
    function checkBasis
        [~, basisSize] = size(basis);
        for n = 1 : basisSize
            for m = 1 : basisSize
                t = trace(basis{n}'*basis{m});
                
                if (n == m)
                    assert( t >= 1 - 1e-8 && t <= 1 + 1 - 1e-8);
                    %disp(t);
                else
                    if abs(t) > 1e-8
                        t = t;
                        %assert(false);
                    end
                    %disp(t);
                end
            end
            
            %disp(basis{n});
        end
    end
    function addCt(c)
        constraints = [constraints, c];
    end
    function onesMx()
        
        count = startP + preparations*measurements*iOutcomes;
        
        %[~, count] = size(mM);
        
        %count = 1;
        
        disp('---- ones set -----');
        for n1 = 1 : count
            for n2 = 1 : count
                if n1 == n2
                    
                    if n1 == 1
                        addCt(mM(n1, n2) == 1);
                    end
                    
                end
                
            end
        end
    end
    function [set, z] = mkSet()
        
        function join(newSet)
            
            [~, sz] = size(newSet);
            
            for n = 1 : sz
                key = [newSet{n}{:}];
                if ~isKey(mMap, key)
                    setMap(key, nTotal);
                    set{nTotal} = newSet{n};
                    nTotal = nTotal + 1;
                end
            end
        end
        function [out] = invertTerm(t)
            [~, sizeOfTerm] = size(t);
            
            out = {};
            
            for c = 1 : sizeOfTerm
                out(c) = t(sizeOfTerm - c + 1);
            end
        end
        function setMap(k, v)
            mMap(k) = v;
            iMap{v} = k;
        end
        function[m] = mCell(i,j)
            m = zeros(mxSize);
            m(i,j) = 1;
        end
        function [tmpSet] = mkSet1()
            i = 1;
            tmpSet = {};
            for p = 1 : preparations
                tmpSet{i}{1} = strcat('P', num2str(p), '_');
                i = i + 1;
            end
            for m = 1 : measurements
                for o = 1 : iOutcomes
                    tmpSet{i}{1} = strcat('M', num2str(m), num2str(o), '_');
                    i = i + 1;
                end
            end
        end
        function [tmpSet] = mkPreps()
            i = 1;
            tmpSet = {};
            for p = 1 : preparations
                tmpSet{i}{1} = strcat('P', num2str(p), '_');
                i = i + 1;
            end
        end
        function [tmpSet] = mkMes()
            i = 1;
            tmpSet = {};
            for m = 1 : measurements
                for o = 1 : iOutcomes
                    tmpSet{i}{1} = strcat('M', num2str(m), num2str(o), '_');
                    i = i + 1;
                end
            end
        end
        function [res] = isNull(a, b)
            res = false;
            if a(1) ~= 'M' || b(1) ~= 'M'
                return;
            end
            if a(2) ~= b(2)
                return;
            end
            if a(3) ~= b(3)
                res = true;
                return;
            end
        end
        function [sT, res] = simplifyTerm(t)
            tLast = '';
            
            sT = {};
            [~, tSize] = size(t);
            sgn = 1;
            
            for n = 1 : tSize
                el = t{n};
                if el == '-'
                    sgn = -sgn;
                    continue;
                end
                
                if ~isempty(tLast) && isNull(tLast, el)
                    res = false;
                    return;
                end
                if ~strcmp(el, 'I_') && ~strcmp(tLast, el)
                    sT = [sT t{n} ];
                    tLast = el;
                end
            end
            
            if isempty(sT)
                if sgn == -1
                    sT{1} = '-';
                    sT{2} = 'I_';
                else
                    sT = 'I_';
                end
            else
                if sgn == -1
                    sT = [ '-' sT];
                end
            end
            res = true;
        end
        function [out] = cProd(set1, set2)
            [~, size1] = size(set1);
            [~, size2] = size(set2);
            k = {};
            nItems = 1;
            tmpMap = containers.Map();
            
            for c1 = 1 : size1
                for c2 = 1 : size2
                    tmp = {};
                    nItem = 1;
                    [~, sz] = size(set1{c1});
                    for e = 1 : sz
                        s = set1{c1}{e};
                        tmp{nItem} = s;
                        nItem = nItem + 1;
                    end
                    [~, sz] = size(set2{c2});
                    for e = 1 : sz
                        s = set2{c2}{e};
                        tmp{nItem} = s;
                        nItem = nItem + 1;
                    end
                    
                    [k, res] = simplifyTerm(tmp);
                    
                    if res
                        key = [k{:}];
                        
                        addIt =false;
                        
                        if ~isKey(tmpMap, key)
                            [~, szK] = size(k);
                            addIt = true;
                            
                            if szK == 2 && k{1}(1) == 'M' && k{2}(1) == 'M' && k{1}(2)  ~= k{2}(2)
                                iK = invertTerm(k);
                                iKey = [iK{:}];
                                
                                addIt = ~isKey(tmpMap, iKey);
                            end
                        end
                        
                        if addIt
                            tmpMap(key) =  nTotal;
                            out{nItems} = k;
                            nItems = nItems + 1;
                        end
                        
                    end
                end
            end
        end
        function [out] = cOnce(set1)
            [~, size1] = size(set1);
            [~, size2] = size(set1);
            k = {};
            nItems = 1;
            tmpMap = containers.Map();
            
            for c1 = 1 : size1
                for c2 = c1 : size2
                    tmp = {};
                    nItem = 1;
                    [~, sz] = size(set1{c1});
                    for e = 1 : sz
                        s = set1{c1}{e};
                        tmp{nItem} = s;
                        nItem = nItem + 1;
                    end
                    [~, sz] = size(set1{c2});
                    for e = 1 : sz
                        s = set1{c2}{e};
                        tmp{nItem} = s;
                        nItem = nItem + 1;
                    end
                    
                    [k, res] = simplifyTerm(tmp);
                    
                    if res
                        key = [k{:}];
                        
                        if ~isKey(tmpMap, key)
                            tmpMap(key) =  nTotal;
                            out{nItems} = k;
                            nItems = nItems + 1;
                        end
                    end
                end
            end
        end
        function appendProdToSetAndMap(set1, set2)
            [~, size1] = size(set1);
            [~, size2] = size(set2);
            k = {};
            
            for c1 = 1 : size1
                for c2 = 1 : size2
                    tmp = {};
                    tmp{1} = set1{c1}{1};
                    tmp{2} = set2{c2}{1};
                    [k, res] = simplifyTerm(tmp);
                    
                    if res
                        key = [k{:}];
                        
                        if ~isKey(mMap, key)
                            setMap(key,  nTotal);
                            set{nTotal} = k;
                            nTotal = nTotal + 1;
                        end
                    end
                end
            end
        end
        function addToList(s)
            sList{nAllInList} = s;
            nAllInList = nAllInList + 1;
        end
        function [z] =  zerosMx(h)
            [~, mxSize] = size(h);
            
            z = {};
            zeroPos = 1;
            disp('---- zeros -----');
            for n1 = 1 : mxSize
                for n2 = 1 : mxSize
                    
                    left = h(n1);
                    right = h(n2);
                    allL = [left{:}];
                    allR = [right{:}];
                    
                    left = left(1);
                    right = right(1);
                    
                    invertedRight = invertTerm(allR);
                    
                    all = [allL invertedRight];
                    
                    
                    %all = [tmp{:}];
                    
                    [left, res] = simplifyTerm(all);
                    
                    if ~res
                        z{zeroPos}{1} = n1;
                        z{zeroPos}{2} = n2;
                        
                        zeroPos = zeroPos + 1;
                        disp([ '(' num2str(n1) ',' num2str(n2) ')'   allL ' x ' invertedRight]);
                        
                    end
                end
            end
            disp('---- end zeros -----');
        end
        
        nAllInList = 1;
        sList =  {};
        mMap = containers.Map();
        iMap = {};
        nTotal = 1;
        idSet{1}{1} = 'Ip_';
        
        set = {};
        ps = mkPreps();
        ms = mkMes();
        p3 = [ps ms];
        
        join(idSet);
        join(ps);
        join(ms);
        
        join(cProd(cProd(ps, ms), ms));
        join(cProd(cProd(ps, ps), ps));
        
        for n = 1 : nTotal - 1
            [~, sz] = size(set{n});
            
            s = '';
            for t = 1 : sz
                s = [s ' ' set{n}{t}];
            end
            disp(s);
        end
        
        z = zerosMx(set);
    end
    function [of] = mkObjF1(permu)
        currentPermutation = permu;
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = currentPermutation(o);
            
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
        currentPermutation = permu;
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = currentPermutation(o);
            
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
    function [of] = mkObjF2(permu)
        currentPermutation = permu;
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = currentPermutation(o);
            
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
                    row = startP;
                    col = startP + (iP -1) * measurements*iOutcomes + (iM - 1) * iOutcomes + iO;
                    
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
    function [of] = mkObjF3(permu)
        currentPermutation = permu;
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = currentPermutation(o);
            
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
                    row = startP + (iP -1) * measurements*iOutcomes + (iM - 1) * iOutcomes + iO;
                    col = row;
                    
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
    function [of] = evaluate2(permu)
        currentPermutation = permu;
        
        first = true;
        function[out] = permutedWitness(p, m, o)
            oPer = currentPermutation(o);
            
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
                    row = startP;
                    col = startP + (iP -1) * measurements*iOutcomes + (iM - 1) * iOutcomes + iO;
                    
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
    function isOptInBasis()
        [pTerms, mTerms ] = readPM();
        M = evaluateSet(set, pTerms, mTerms);
        
        [~, count] = size(basis);
        total = 0;
        for e = 1 : count
            t = trace(M' * basis{e});
            disp(t);
            total = total + t;
        end
        disp(total);
    end
    function crossPMs()
        s = ''
        for p = 1 : preparations
            for m = 1 : measurements
                for o = 1 : iOutcomes
                    rowP = startP + p;
                    colP = startP + preparations + (m - 1)*iOutcomes + o;
                    
                    rowPM = startP + preparations + measurements*iOutcomes + (p - 1)*measurements*iOutcomes + (m - 1)*iOutcomes + o;
                    colPM = rowPM;
                    
                    addCt(mM(rowP, colP) == mM(rowPM, colPM));
                    [s num2str(rowP) ',' num2str(colP)  '=='  num2str(rowPM) ',' num2str(colPM) ]
                    disp(s);
                end
            end
        end
    end
    function writeZeroConstraints()
        [~, sz] = size(z);
        for n = 1 : sz
            n1 = z{n}{1};
            n2 = z{n}{2};
            addCt(mM(n1, n2) == 0);
        end
    end
%----------------- main --------------------------------
[set, z] = mkSet();
mkBasis();
checkBasis();
%isOptInBasis();

[~, basisSize] = size(basis);
disp([ 'basis has ' num2str(count) ' elements']);

spanC = [];
first = true;
uV = {};

for c = 1 : basisSize
    uV{c} = sdpvar(1, 1, 'hermitian', field);
    
    if first
        spanC =  uV{c}*basis{c};
        first = false;
    else
        spanC =  spanC + uV{c}*basis{c};
    end
end

mM = spanC;
addCt(spanC >= 0);
addCt(mM(1, 1) == dimension);

writeZeroConstraints();

permutations = [1 : outcomes];

end
