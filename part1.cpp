
#include "part1.h"

using namespace std;

int main(int argc , char *argv[]){

	if(!check_arg(argc, argv))
		exit(EXIT_FAILURE);
   
   init_pt();
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

	while(infile >> line){

		//cout << " addr number " << counter << endl;
		counter ++;

        /*cout << " virtual addr is " << line << endl;
        std::cout << std::bitset<32>(atoi(line.c_str())) << endl;*/

		int offset = get_offset(line);
		int page_num = get_page_num(line);

		/*cout << " offset is " << offset << endl;
		std::cout << std::bitset<8>(offset);*/

		/*cout << " page num is " << page_num << endl;
		std::cout << std::bitset<8>(page_num);*/

		int frame_num = find_in_tlb(page_num);

		/* TLB hit */
		if(frame_num != -1){

				/* check this */
			    //cout << " TLB hit " << endl;
			    int phys_addr = frame_num * FRAME_SIZE + offset;
                final_value = phys_mem[phys_addr];
		}

		else{

			frame_num = find_in_page_table(page_num);
			/* found in page table */
			if(frame_num != -1){

				int phys_addr = frame_num * FRAME_SIZE + offset;
				update_tlb(page_num, frame_num);
			}
			// not found in page table : demand paging
			else{

				swap_in(page_num);
			    // update page table
				page_table[page_num] = current_frame;
				current_frame ++;

			}
		}			
	}

	 infile.close();
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

	double hitRate = ((num_of_tlb_hits / (double)num_of_tests));

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

void update_tlb(int page_num, int frame_num){

    if (tlb_front == -1) {

        tlb_front = 0;
        tlb_back = 0;

        tlb[tlb_back][0] = page_num;
        tlb[tlb_back][1] = frame_num;
    }
    else {

        tlb_front = (tlb_front + 1) % TLB_ENTRIES;
        tlb_back = (tlb_back + 1) % TLB_ENTRIES;

        tlb[tlb_back][0] = page_num;
        tlb[tlb_back][1] = frame_num;
    }
}

void swap_in(int page_num){

	char buf[256];
	int index = 0;

	//cout << " page number " << page_num << " frame number " << current_frame << endl;

	 //cout << " PAGE FAULT : SWAPPING IN  ... "<<endl;

	 //cout << " current frame = " << current_frame << endl;

	 FILE *backingStore = fopen("BACKING_STORE.bin", "rb");

	 fseek(backingStore, current_frame*256, SEEK_SET);
     fread(buf, sizeof(char), 256, backingStore);

     //cout << "here "<< endl;

     for(index = 0; index < 256; index++) {

     	//cout << " writing " << buf[index] << " to phys_mem " << current_frame * 256 + index << endl;
        phys_mem[current_frame* 256 + index] = buf[index];
      }
}