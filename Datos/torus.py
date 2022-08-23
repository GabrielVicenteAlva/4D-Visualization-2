import math

N,M,P = 6,6,6
R1,R2,R3 = 4, 2, 1
points = [ [ [ 0. for k in range(P) ] for j in range(M) ] for i in range(N) ]

for i in range(N):
	for j in range(M):
		for k in range(P):
			a = 2*math.pi*i/N
			b = 2*math.pi*j/M
			c = 2*math.pi*k/P
			points[i][j][k] = (
				.25*(R1 + (R2+R3*math.cos(a))*math.cos(b))*math.cos(c),
				.25*(R1 + (R2+R3*math.cos(a))*math.cos(b))*math.sin(c),
				.25*(R2 + R3*math.cos(a))*math.sin(b),
				.25*R3*math.sin(a)
			)

print(N*M*P)

for i in range(N):
	for j in range(M):
		for k in range(P):
			print('%.2f %.2f %.2f %.2f'%points[i][j][k])

edges = [ ]

ind = lambda i,j,k : i*M*P + j*P + k
for i in range(N):
	for j in range(M):
		for k in range(P):
			I = (i+1)%N
			J = (j+1)%M
			K = (k+1)%P
			edges.append((ind(i,j,k),ind(I,j,k)))
			edges.append((ind(i,j,k),ind(i,J,k)))
			edges.append((ind(i,j,k),ind(i,j,K)))
			
# 			edges.append((ind(I,j,k),ind(I,J,k)))
# 			edges.append((ind(I,j,k),ind(I,j,K)))
# 			
# 			edges.append((ind(i,J,k),ind(I,J,k)))
# 			edges.append((ind(i,j,k),ind(i,J,K)))
# 			
# 			edges.append((ind(i,j,K),ind(I,j,K)))
# 			edges.append((ind(i,j,K),ind(i,J,K)))
# 			
# 			edges.append((ind(i,J,K),ind(I,J,K)))
# 			edges.append((ind(I,j,K),ind(I,J,K)))
# 			edges.append((ind(I,J,k),ind(I,J,K)))

print(len(edges))
for edge in edges:
	print('%i %i'%edge)

faces = []

for i in range(N):
	for j in range(M):
		for k in range(P):
			I = (i+1)%N
			J = (j+1)%M
			K = (k+1)%P
			a = 2*math.pi*i/N
			b = 2*math.pi*j/M
			c = 2*math.pi*k/P
			faces.append((
				ind(i,j,k),ind(i,J,k),ind(I,J,k),ind(I,j,k),ind(i,j,K),ind(i,J,K),ind(I,J,K),ind(I,j,K),
				math.cos(a)*math.cos(b)*math.cos(c),
				math.cos(a)*math.cos(b)*math.sin(c),
				math.cos(a)*math.sin(b),
				math.sin(a)
			))

print(len(faces))
for face in faces:
	print('%i %i %i %i %i %i %i %i -1 %.2f %.2f %.2f %.2f'%face)

# print(0)
