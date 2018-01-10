all : vmm.o

vmm.o : part1.cpp part1.h
	g++ -g part1.cpp -o vmm.o && echo success

clean :
	rm  -f addresses.txt myaddresses.txt *.o *.out 