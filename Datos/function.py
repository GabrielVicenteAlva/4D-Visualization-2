import numpy as np
import cmath

def f(x):
	c = x[0] +1j*x[1]
	sq = c**2
	sq = cmath.log(c)
	return sq.real,sq.imag

ar = [-1.5,-1.,-.5,.00001,.5,1.,1.5]

X = np.array([
	[(i,j) for i in ar] for j in ar
])

N = len(ar)
print(N**2)
for k in range(N**2):
	i,j = k%N,k//N
	p = X[i,j]
	fp = f(X[i,j])
	print("%.2f %.2f %.2f %.2f"%(p[0],p[1],fp[0],fp[1]))
print(2*N*(N-1))

def t(a,b):
	return a*N+b

for a in range(N):
	for b in range(N-1):
		print(t(a,b),t(a,b+1))
		print(t(b,a),t(b+1,a))

print(0)
