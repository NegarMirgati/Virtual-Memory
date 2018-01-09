
#include "part1.h"

using namespace std;


int main(int argc , char *argv[]){

	if(!check_arg(argc, argv))
		exit(EXIT_FAILURE);
   

   init_pt();
   init_phys_mem();
   init_tlb();

   run_vmm(argv[1]);

	return 0;
}


bool check_arg(int argc, char* argv[]){

	if(argc != 2){

		cout << " valid operaton " << argv[0] << " [addresses.txt] " << endl;
		return false;

	}

	return true;

}

void init_pt(){

	 for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        page_table[i] = -1;
    }
}
void init_phys_mem(){

	//TODO
}
void init_tlb(){

	for (int i = 0; i < TLB_ENTRIES; i++) {
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }
}

void run_vmm(char* addr){

	int choice = menu();

	ifstream infile;
	string line;
	if(choice == 1) 
		 infile.open(addr);
	else
	{
    	generate_rands();
		infile.open(MYADDR);
	}

	if (!infile){
    cerr << "Unable to open file " << addr << endl;
    exit(EXIT_FAILURE);   
	}

	cout << "Running Virtual Memory Manger ..." << endl;


	while(infile >> line){


		int offset = get_offset(line);
		int page_table_num = get_page_num(line);

		//TO-DO
	}

	 infile.close();
}

int get_offset(string addr){
 
 	return ( atoi(addr.c_str()) & 127);  
}

int get_page_num(string addr){

	return (atoi(addr.c_str() ) & (127 >> 8));
}

int find_in_tlb(int page_num){

	bool hit = false;
	int i = 0;

	// TLB Walk

	for(i = 0; i < TLB_ENTRIES; i++){

		if(tlb[i][0] == page_num){

			cout << " page number #"<<page_num << " was found in TLB " << endl;
			hit = true;
			num_of_tlb_hits ++;
			break;
		}
	}
	
	if(!hit)
		return -1;
	else
		return i;
}

int calc_phys_addr(int page_table_num, int offset){

	int phys_addr=( phys_mem[page_table[page_table_num]] * FRAME_SIZE) + offset;

	cout << " calculated phys addr is " << phys_addr << endl;

	return phys_addr;

}

int find_in_page_table(int page_table_num){

	int val = page_table[page_table_num];

    if (val == -1) {
		num_of_page_faults++;
    }
    
    return val;

}

void print_statistics(){

	double pageFaultRate = (double) ( (num_of_page_faults / num_of_tests) * 100);

	cout << "Page Fault Rate for #" << num_of_tests 
			<< " addresses is : "<<pageFaultRate << endl;

	double hitRate = (double) ((num_of_tlb_hits / num_of_tests) * 100);

	cout << "Hit Rate for #" << num_of_tests 
			<< " addresses is : "<<hitRate << endl;


	//TODO overhead
}

void generate_rands(){

	cout << "Enter seed for random number generation " << endl;
    int seed;
	cin>> seed;
	srand(seed);

    for(int i = 0; i < num_of_tests; i++){

    int  out = fRand(PHYS_MEM_SIZE);
    write_on_file(out);
  }
}

int fRand(int fMax){
   
    int f = rand() % fMax;
    return f;
}

void write_on_file(int data){

  ofstream myfile;
  myfile.open(MYADDR, ios_base::app);

  if (myfile.is_open())
    myfile << data << " ";
  else
    exit(0);

  myfile.close();
  
}

int menu(){

	cout << "***** MENU ***** " << endl;
	cout << "Mode 1 : Execute VMM with the file provided"<<endl;
	cout << "Mode 2 : Execute VMM with random addresses"<<endl;
	
	int choice;
	cin >> choice;
	if( choice != 1 && choice != 2){

		cout <<" invalid choice";
		exit(EXIT_FAILURE);
	}

	return choice;

}


