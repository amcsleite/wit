
Dimension witness characterization - classic vs quantum
#Generating witness scenarios

#WPoly  \<p\>  \<m\> \<d\> \<o\>
  where
  
* **p**  preparations   
* **m**  measurements   
*  **d**  dimension  \(i.e.  d==2  for a bit d==3 for a trit\)
*  **o**  outcomes

For d==2 two polytopes corresponding to expected values and probabilities polytope. Otherwsise only the 
probababilities polytope is created

Polytopes are created in folder   ~/wit/scenarios/p-m-d-o  ( example:   ~/wit/scenarios/3-2-2-3 )

by default panda is used to find the facets of the polytope

####  using the probabilities polytope:
panda pandaP.poi > panda.ieq

#### using expected values: (possible if outcomes == 2) 
panda pandaE.poi > panda.ieq

## wFilter
filters the list of witness

## wSelect
select the best witnesses

## wMat
The file  **a\<p\>\<m\>\<d\>\<o\>.m ** (Example: a3223.m) is created with all witnesses in matlab's cell array format
#Generating witness scenarios

#Running quantum limits
- mSeeSaw computes limits using projective measurement or POVMS
- prepareNPADimNeumark - prepares momentum matrix base for a scenario
- runNPADimNeumark - run SDP for a specific witness using the base bound with prepareNPADimNeumark

