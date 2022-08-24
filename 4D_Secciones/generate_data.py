import random

N = 50
lim = 5.

print(N)
def ran():
	return -lim + 2*lim*random.random()
for i in range(N):
	print("%.2f %.2f %.2f"%(ran(),ran(),ran()))
