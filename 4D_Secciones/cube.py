import numpy as np

vs = '''1.  1.  1.  1.
 1.  1.  1. -1.
 1.  1. -1. -1.
 1.  1. -1.  1.
 1. -1.  1.  1.
 1. -1.  1. -1.
 1. -1. -1. -1.
 1. -1. -1.  1.
-1.  1.  1.  1.
-1.  1.  1. -1.
-1.  1. -1. -1.
-1.  1. -1.  1.
-1. -1.  1.  1.
-1. -1.  1. -1.
-1. -1. -1. -1.
-1. -1. -1.  1.'''

vs = vs.split('\n')

vs = [
	np.fromstring(v, dtype=float, sep=' ') for v in vs
]

a = 3.1416/4
mat = np.array([
	[ np.cos(a),-np.sin(a), 0, 0 ],
	[ np.sin(a), np.cos(a), 0, 0 ],
	[ 0, 0, 1, 0 ],
	[ 0, 0, 0, 1 ]
])
mat = np.array([
	[ np.cos(a), 0, np.sin(a), 0 ],
	[ 0, 1, 0, 0 ],
	[-np.sin(a), 0, np.cos(a), 0 ],
	[ 0, 0, 0, 1 ]
])@mat
mat = np.array([
	[ 1, 0, 0, 0 ],
	[ 0, np.cos(a), 0, np.sin(a) ],
	[ 0, 0, 1, 0 ],
	[ 0,-np.sin(a), 0, np.cos(a) ]
])@mat


for v in vs:
	v2 = tuple(mat@v)
	print('%.2f %.2f %.2f %.2f'%v2)
