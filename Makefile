all : vmm1.o vmm2.o

vmm1.o : part1.cpp part1.h
	g++ -g part1.cpp -o vmm1.o && echo success
vmm2.o :
	g++ -g part2.cpp -o vmm2.o && echo success
clean :
	rm  -f addresses.txt myaddresses.txt *.o *.out 
