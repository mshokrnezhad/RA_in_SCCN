set N := {read "S01_set_N.txt" as "<1n>"};
set A := {read "S01_set_A.txt" as "<1n>"};
set K := {read "S02_set_K.txt" as "<1n>"};
param p_bar := 10000;
param M := 10000;
param gamma[N] := read "S03_sir.txt" as "<1n>2n";
param nu := 1;
param relay_node[N] := read "S03_nn.txt" as "<1n>2n";
param xl[A] := read "S04_x.txt" as "<1n>2n";
param yl[A] := read "S05_y.txt" as "<1n>2n";

defnumb dist(a,b) := sqrt((xl[a]-xl[b])^2 + (yl[a]-yl[b])^2);
defnumb h(a,b) := 0.09*(dist(a,b)^(-3));

var p[N*K];
var x[N*K] binary;

minimize f: sum <n> in N: sum <k> in K: p[n,k];

subto const1: forall <n> in N: forall <k> in K: p[n,k] >= 0;
subto const2: forall <n> in N: forall <k> in K: p[n,k] <= p_bar;
subto const3: forall <n> in N: sum <k> in K: x[n,k] == 1;
subto const4: forall <n> in N: forall <k> in K: p[n,k] >= ((((sum <i> in N without {<n>,<relay_node[n]>}:p[i,k]*h(i,relay_node[n]))+nu)/h(n,relay_node[n]))*gamma[n])-(1-x[n,k])*M;


