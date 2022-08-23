import numpy as np
from sympy.utilities.iterables import multiset_permutations

sq2 = np.sqrt(2)

l = []

for p in multiset_permutations([1,1,0,0]):
	l.append(np.array(p))
for p in multiset_permutations([1,-1,0,0]):
	l.append(np.array(p))
for p in multiset_permutations([-1,-1,0,0]):
	l.append(np.array(p))

print(len(l))
for p in l:
	print("%i %i %i %i"%tuple(p))

e = set()

for i,a in enumerate(l):
	for j,b in enumerate(l):
		if abs(np.linalg.norm(a-b) - sq2) < .0001:
			e.add(frozenset([i,j]))

print(len(e))

for i,j in e:
	print(i,j)
	
print(0)
