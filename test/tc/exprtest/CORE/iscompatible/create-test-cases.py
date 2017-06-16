start="""
class Test

types

  A = nat;
 
  B = A * A | C | bool | rat;
 
  C = real
  inv c == c < 7;
 
  D = inmap nat1 to G;
 
  E = set of A;
 
  F = seq of B;
 
  G = seq1 of A;
 
  H = map A to F;
 
  I = [D];
 
  J = ((<RED> | <YELLOW> | <GREEN>) | A) * A | real | bool;    

  K = A * B * C * D * E * G * D * (<RED> * nat);
 
  L = B * J * real * I * set of real * F * H * J
functions

Af: A -> A
Af(x) ==
  x;

Bf: B -> B
Bf(x) ==
  x;

Cf: C -> C
Cf(x) ==
  x;

Df: D -> D
Df(x) ==
  x;

Ef: E -> E
Ef(x) ==
  x;

Ff: F -> F
Ff(x) ==
  x;

Gf: G -> G
Gf(x) ==
  x;

Hf: H -> H
Hf(x) ==
  x;

If: I -> I
If(x) ==
  x;

Jf: J -> J
Jf(x) ==
  x;

Kf: K -> K
Kf(x) ==
  x;

Lf: L -> L
Lf(x) ==
  x;
"""

i=1
for x in ['A','B','C','D','E','F','G','H','I','J','K','L']:
  for y in ['A','B','C','D','E','F','G','H','I','J','K','L']:

    if i<10:
      f="subtype-00"+`i`+".vdm"
    elif i < 100:
      f = "subtype-0"+`i`+".vdm"
    else:
      f="recursive-"+`i`+".vdm"
    FID=open(f,"w")
    FID.write(start)

    FID.write(x+y+": " + x +"->"+y+"\n")
    FID.write(x+y+"(x) ==\n")
    FID.write("\t"+y+"f(x);\n")

    FID.write("end Test\n")
    i=i+1
