#include "part1.h"

using namespace std;

int main(int argc , char *argv[]){

	if(!check_arg(argc, argv))
		exit(EXIT_FAILURE);
   
   init_pt();
   init_tlb();
   mmap_store();

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

	int counter = 0;

    ofstream outfile("OUTPUT.txt");

	while(getline(infile, line)){

        /*cout << " virtual addr is " << line << endl;
        std::cout << std::bitset<32>(atoi(line.c_str())) << endl;*/

		int offset = get_offset(line);
		int page_num = get_page_num(line);

		counter++;

		/*cout << " offset is " << offset << endl;
		std::cout << std::bitset<8>(offset);*/

		/*cout << " page num is " << page_num << endl;
		std::cout << std::bitset<8>(page_num);*/

		int frame_num = find_in_tlb(page_num);

		/* TLB hit */
		int phys_addr;
		if(frame_num != -1){

				/* check this */
			    phys_addr = frame_num * FRAME_SIZE + offset;
                final_value = phys_mem[phys_addr];
		}

		else{

			frame_num = find_in_page_table(page_num);
			/* found in page table */
			if(frame_num != -1){

				phys_addr = frame_num * FRAME_SIZE + offset;
				update_tlb(page_num, frame_num);
				final_value = phys_mem[phys_addr];
			}
			// not found in page table : demand paging
			else{
				swap_in(page_num);
			    // update page table
				page_table[page_num] = current_frame;

				phys_addr = current_frame * FRAME_SIZE + offset;
				final_value = phys_mem[phys_addr];

				/* Check This */
				update_tlb(page_num, current_frame);
				current_frame ++;
			}
		}
		outfile << "virtual address : "<< line << " ,physical addr :" << phys_addr << " ,value :" << final_value << endl;			
	}

	 infile.close();
	 outfile.close();
	 print_statistics();
}

int get_offset(string addr){
 
 	return ( atoi(addr.c_str()) & 255);  
}

int get_page_num(string addr){

	return ((atoi(addr.c_str()  )/256) & (255));
}

int find_in_tlb(int page_num){

	bool hit = false;
	int i = 0;

	// TLB Walk
	for(i = 0; i < TLB_ENTRIES; i++){

		if(tlb[i][0] == page_num){

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

int find_in_page_table(int page_table_num){

	int val = page_table[page_table_num];

    if (val == -1) {
		num_of_page_faults++;
    }
    
    return val;
}

void print_statistics(){

	
	cout.precision(16);

	double pageFaultRate = ((num_of_page_faults / (double)num_of_tests));

	cout << "Page Fault Rate for #" << num_of_tests 
			<< " addresses is : "<<pageFaultRate << endl;

	double hitRate = ((num_of_tlb_hits / (double)num_of_tests) );

	cout << " Hits : " << num_of_tlb_hits << endl;


	cout << "Hit Rate for #" << num_of_tests 
			<< " addresses is : "<<hitRate << endl;

	//Check this
	/*
		tlb hit : reads from TLB , reads value from physical memory
		tlb-miss , page-table hit : reads from TLB , reads from page-table , reads from phys_mem , updates tlb
		pageFault : reads from tlb, reads from page-table, reads from disk, writes to phys_mem, updates page table
					reads from phys_mem
	 */		
	double overhead = (hitRate) * (tlb_rw + mm_rw) + (1 - (pageFaultRate + hitRate)) * (2*tlb_rw + 2*mm_rw) +
					  (pageFaultRate) * (tlb_rw + 4*mm_rw + disk_rw);
	cout << "Total Overhead for #"<<num_of_tests<<" addresses is : "<<overhead << " nanoseconds"<<endl;
}

void generate_rands(){

	cout << "Enter seed for random address generation " << endl;
    int seed;
	cin>> seed;
	srand(seed);

	ofstream myfile;
    myfile.open(MYADDR, ios_base::app);

    for(int i = 0; i < num_of_tests; i++){

   		int  temp_out = fRand(PHYS_MEM_SIZE);
   		int ratio = fRand(2);
   		long out = (ratio+1) * temp_out;
    	if (myfile.is_open()){

    		myfile << out << endl;
    	}
	  else
   			exit(0);

  }
   myfile.close(); 
}

int fRand(int fMax){
   
    int f = rand() % fMax;
    return f;
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

void update_tlb(int page_num, int frame_num){

    if (tlb_ptr == -1) {

       tlb_ptr = 0;
        tlb[tlb_ptr][0] = page_num;
        tlb[tlb_ptr][1] = frame_num;
    }
    else {

        tlb_ptr = (tlb_ptr + 1) % TLB_ENTRIES;

        tlb[tlb_ptr][0] = page_num;
        tlb[tlb_ptr][1] = frame_num;
    }

}

void swap_in(int page_num){
    
    //cout << "current frame = "<<current_frame<<" page num = " << page_num << endl; 
	memcpy(phys_mem + current_frame*FRAME_SIZE, storage + page_num*PAGE_SIZE, FRAME_SIZE);
}

void generate_rands_with_locality(int mode){

	ofstream outfile;
	outfile.open("myaddresses.txt");

	cout << "Enter seed for random address generation" << endl;
	int seed;
	cin >> seed;
	srand(seed);

	if(mode == 0){

		int counter = 0;

		for(int i = 0 ; i < 1000 ; i++){

    	 int temp_f = rand() % PHYS_MEM_SIZE;

     	for(int j = 0 ; j < 10 ; j++){
    		 int f = (temp_f + j) % PHYS_MEM_SIZE;
    		 int ratio = fRand(2);
    		 long out = f * (ratio + 1);
    		 outfile << out << endl;
    		 counter ++;
		 }    
 	 }
  }

  else if (mode == 1){

  	int counter = 0;

		for(int i = 0 ; i < 100 ; i++){

    	 int temp_f = rand() % PHYS_MEM_SIZE;

     	for(int j = 0 ; j < 100 ; j++){
    		 int f = (temp_f + j) % PHYS_MEM_SIZE;
    		 int ratio = fRand(2);
    		 long out = f * (ratio + 1);
    		 outfile << out << endl;
    		 counter ++;
		 }    
      }
   }

}



void mmap_store(){

	    /* Open the store file. */
        /* Map the store file to memory. */
        /* Initialize the file descriptor. */
        int store_fd = open(BACKING_STORE_ADDR, O_RDONLY);
        void* store_data = mmap(0, PHYS_MEM_SIZE, PROT_READ, MAP_SHARED, store_fd, 0);
        /* Check that the mmap call succeeded. */
        if (store_data == MAP_FAILED) {
            close(store_fd);
            printf("Error mmapping the backing store file!");
            exit(EXIT_FAILURE);
        }

        storage = static_cast<char*> (store_data);
}

