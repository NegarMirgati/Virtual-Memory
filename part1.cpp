
#include "part1.h"

 int phys_mem[MM_SIZE];
 int v_mem[VM_SIZE][2];
 int tlb[TLB_ENTRIES][2];


using namespace std;


int main(int argc , char *argv[]){

	if(!check_arg(argc, argv))
		exit(EXIT_FAILURE);
   

   init_vm();
   init_phys_mem();
   init_tlb();

   run_vmm(argv[0]);

	return 0;
}


bool check_arg(int argc, char* argv[]){

	if(argc != 2){

		cout << " valid operaton " << argv[0] << " [addresses.txt] " << endl;
		return false;

	}

	return true;

}

void init_vm(){}
void init_phys_mem(){}
void init_tlb(){}

void run_vmm(char* addr){

	ifstream infile;
	infile.open(addr);
}
