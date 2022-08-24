# Sections
import numpy as np
import cmath

def f(x):
	c = x[0] +1j*x[1]
# 	if c == 0+0j:
# 		return 0.,0.
	sq = .5*c**2
	return sq.real,sq.imag

ar = [-1.,-.75,-.5,-.25,.0,.25,.5,.75,1.]
ar = np.arange(-1,1,.05)

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

print((N-1)**2)
for a in range(N-1):
	for b in range(N-1):
		print(t(a,b),t(a,b+1),t(a+1,b+1),t(a+1,b),-1)
# 		print(t(a+1,b+1),t(a,b+1),t(a+1,b),-1)
# print(0)
