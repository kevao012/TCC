from sets import Set
import subprocess
import random

numberInstances = 10
Cases = 16

instancias = Set()

while len(instancias) < numberInstances:
	instancias.add(int(random.random()*980))

for j in range(11, Cases + 1):
	#for i in range(6549,numberInstances + 1):
	for i in instancias:
		instancia = '../NSPLib/N60/' + str(i) + '.nsp'
		case = '../NSPLib/Cases/' + str(j) + '.gen'
		print "Executando " + instancia + " com " + case
		subprocess.call(['./exec.out',instancia,case])
	
	#while len(instancias) < 100:
	#	instancias.add(int(random.random()*980))
