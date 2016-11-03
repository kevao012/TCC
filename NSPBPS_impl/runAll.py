from sets import Set
import subprocess
import random

numberInstances = 129
Cases = 8

instancias = Set()



for j in range(2, Cases + 1):
	#for i in range(6549,numberInstances + 1):
	existentes = Set()
	instancias = Set()
	f = open('../teste/N50/Case' + str(j) + '.txt')

	for line in f:
		existentes.add(int(line.split(' ')[0]))	

	while len(instancias) < numberInstances:
		newNum = int(random.random()*7290)
		if newNum not in existentes:
			instancias.add(newNum)

	f.close()
	for i in instancias:
		instancia = '../NSPLib/N50/' + str(i) + '.nsp'
		case = '../NSPLib/Cases/' + str(j) + '.gen'
		print "Executando " + instancia + " com " + case
		subprocess.call(['./exec.out',instancia,case])

	#while len(instancias) < 10:
	#	instancias.add(int(random.random()*960))
