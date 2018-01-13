#include "part2.h"

using namespace std;

int main(int argc , char *argv[]){

	if(!check_arg(argc, argv))
		exit(EXIT_FAILURE);
   
   init_pt();
   init_tlb();
   mmap_store();

   run_vmm(argv[1]);
   print_statistics();

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
        page_table[i][0] = -1;
        page_table[i][1] = 0;
    }
}

void init_tlb(){

	for (int i = 0; i < TLB_ENTRIES; i++) {
        tlb[i][0] = -1;
        tlb[i][1] = -1;
        tlb[i][2] = 0;
    }
}

void init_counter_usage_frame(){
	for ( int i = 0 ; i < NUM_OF_FRAMES; i++) {
		counter_usage_frame[i] = 0;
	}
}

void init_state_phys_mem(){
	for ( int i = 0 ; i < NUM_OF_FRAMES; i++) {
		state_phys_mem[i] = 0;
	}
}
void init_second_chance(){
	for ( int i = 0 ; i < NUM_OF_FRAMES ; i++) {
		second_chance[i] = 0;
	}
}

void run_vmm(char* addr){

	int choice = menu();
	page_replacement_policy = menu_PRP();

	if(page_replacement_policy == 4){

		int seed = get_seed_for_rand_replacement();
		srand(seed);
	}

	if ( page_replacement_policy == 3 ){

		init_second_chance();
	}

	if ( page_replacement_policy == 2 ){
		init_counter_usage_frame();
		init_state_phys_mem();
	}

	ifstream infile;
	
	string line;
	if(choice == 1) 
		 infile.open(addr);
	else
	{
    	generate_rands_with_locality(0);
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

		int offset = get_offset(line);
		int page_num = get_page_num(line);

		counter++;

		int frame_num = find_in_tlb(page_num);

		/* TLB hit */
		int phys_addr;
		if(frame_num != -1){

				/* check this */
			    phys_addr = frame_num * FRAME_SIZE + offset;
                final_value = phys_mem[phys_addr];
                update_counter_usage_frame(frame_num);
                if ( page_replacement_policy == 3)
                	second_chance[frame_num] = 1;
                //outfile << "HIT" << endl;
		}

		else{

			frame_num = find_in_page_table(page_num);
			/* found in page table */
			if(frame_num != -1){

				phys_addr = frame_num * FRAME_SIZE + offset;

				//cout << " phys addr =  " << phys_addr << endl;
				update_tlb(page_num, frame_num);
				final_value = phys_mem[phys_addr];

				if ( page_replacement_policy == 2 )
					update_counter_usage_frame(frame_num);
				if ( page_replacement_policy == 3)
                	second_chance[frame_num] = 1;

                //outfile << "PT HIT" << endl;

			}
			// not found in page table : demand paging
			else{

				//outfile << "PAGE FAULT" << endl;

				swap_in(page_num);
			    // Add this page and its allocated frame to page table
				add_entry_to_page_table(page_num, current_frame);


				phys_addr = current_frame * FRAME_SIZE + offset;
				final_value = phys_mem[phys_addr];

				/* Check This */
				invalidate_entry_in_tlb(current_frame);
				update_tlb(page_num, current_frame);

				if ( page_replacement_policy == 1 || page_replacement_policy == 3 ) {// fifo , second chance
					current_frame ++;
				}
				else if(page_replacement_policy == 4){  // Random

					if(!mem_is_full){

						current_frame++;
						//cout<< " not full and current frame = "<<current_frame << endl;
					}
					/*else 
						cout << " Memory full "<< endl;*/
				}
			}
		}
		outfile << "Virtual address: "<< line << " Physical address: " << phys_addr << " Value: " << final_value << endl;			
	}

	 infile.close();
	 outfile.close();
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

		if(tlb[i][0] == page_num && tlb[i][2] != 0){

			hit = true;
			num_of_tlb_hits ++;
			break;
		}
	}
	
	if(!hit)
		return -1;
	else
		return tlb[i][1];
}

int find_in_page_table(int page_table_num){

	int val = page_table[page_table_num][0];
	int valid = page_table[page_table_num][1];

    if (val == -1 || valid == 0) {
		num_of_page_faults++;
		return -1;
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
int menu_PRP(){

	cout << "***** choose page replacement policy ***** " << endl;
	cout << "Mode 1 : FIFO"<<endl;
	cout << "Mode 2 : LRU"<<endl;
	cout << "Mode 3 : Second Chance"<<endl;   
	cout << "Mode 4 : Random"<<endl;
	
	int choice;
	cin >> choice;
	if( choice != 1 && choice != 2 && choice != 4 && choice != 3 ){

		cout <<" invalid choice";
		exit(EXIT_FAILURE);
	}
	return choice;
}

void update_tlb(int page_num, int frame_num){

    // If there is an empty slot in TLB
	int index = find_empty_index_in_tlb();
	if(index != -1){

		tlb[index][0] = page_num;
		tlb[index][1] = frame_num;
		tlb[index][2] = 1;
		return;
	}
    // else 
    if (tlb_ptr == -1) {

        tlb_ptr = 0;
        tlb[tlb_ptr][0] = page_num;
        tlb[tlb_ptr][1] = frame_num;
        tlb[tlb_ptr][2] = 1;
    }
    else {

        tlb_ptr = (tlb_ptr + 1) % TLB_ENTRIES;

        tlb[tlb_ptr][0] = page_num;
        tlb[tlb_ptr][1] = frame_num;
        tlb[tlb_ptr][2] = 1;
    }

}

void swap_in(int page_num){

	 
	 if ( page_replacement_policy == 2)  // LRU
	 {
	 	 int available_frame;
	 	 bool mem_state = is_memory_full(available_frame);
	 	 if ( mem_state == true )
	 	 {
	 	 	 //cout << " memory is full " << endl;
	 	 	 current_frame =find_LRU();
	 	 	 remove_entry_from_page_table_framenum(current_frame);
	 	 }
	 	 else if ( mem_state == false )
	 	 {
	 	 	current_frame = available_frame;
	 	 }
	 }
	 
	 else if ( page_replacement_policy == 1) // FIFO
	 {
	 	 if(current_frame >=  NUM_OF_FRAMES){
	 	 current_frame = 0;
	 	 //rolls back to the begining of the queue
	     }
	     remove_entry_from_page_table_framenum(current_frame);
	 }

	 else if ( page_replacement_policy == 3) // second chance 
	 {
	 	 if(current_frame >=  NUM_OF_FRAMES)
	 	 {
	 	 	mem_is_full = true;
	 	 	current_frame = 0;
	 	 }
	 	 if ( mem_is_full )
	 	 {

	 	 	while( second_chance[current_frame] != 0 )
	 	 	{
	 	 		second_chance[current_frame] = 0;
	 	 		current_frame++;
	 	 		if ( current_frame >=  NUM_OF_FRAMES)
	 	 			current_frame = 0;
	 	 	} 
	 	 	remove_entry_from_page_table_framenum(current_frame);
	 	 }
	 }
	 else if(page_replacement_policy == 4){  //Random Replacement

	 	if(current_frame >= NUM_OF_FRAMES){

	 		mem_is_full = true; 
	 		/* after all the free frames are used, this gets true and we use random replacing */ 
	 	}

	 	if(mem_is_full){

	 		//current_frame = fRand;
	 		int rand_index ;
	 		while(true){

	 			rand_index = fRand(PAGE_TABLE_ENTRIES);
	 			//cout << " index = " << rand_index << endl;
	 			if(page_table[rand_index][1])   /* if this entry is valid (has not been swapped out) */
	 				break;
	 		}

	 		current_frame = page_table[rand_index][0];
	 		remove_entry_from_page_table(rand_index);
	 		//cout<< " here " << endl;
	 		//cout << " Random current frame = " << current_frame << endl;
	 	}
	 }

       //cout << "current frame = "<<current_frame<<" page num = " << page_num << endl; 
	   memcpy(phys_mem + current_frame*FRAME_SIZE,storage + page_num*PAGE_SIZE, FRAME_SIZE);

       state_phys_mem[current_frame] = 1;
       update_counter_usage_frame(current_frame); 
       if ( page_replacement_policy == 3 )
       	second_chance[current_frame] = 0; 
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

		for(int i = 0 ; i < num_of_tests/10 ; i++){

    	 int temp_f = rand() % PHYS_MEM_SIZE;

     	for(int j = 0 ; j < num_of_tests/1000 ; j++){
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

		for(int i = 0 ; i < num_of_tests/100 ; i++){

    	 int temp_f = rand() % PHYS_MEM_SIZE;

     	for(int j = 0 ; j < num_of_tests/100 ; j++){
    		 int f = (temp_f + j) % PHYS_MEM_SIZE;
    		 int ratio = fRand(2);
    		 long out = f * (ratio + 1);
    		 outfile << out << endl;
    		 counter ++;
		 }    
      }
   }
}


void update_counter_usage_frame(int frame_num){

	counter_usage_frame[frame_num]++;
}

bool is_memory_full(int& available_frame){
	
	for ( int i = 0 ; i < NUM_OF_FRAMES;i++)
	{
		if ( state_phys_mem[i] == 0 )
		{
			available_frame = i;
			return false; // mem is not full
		} 
	}
	return true; // mem is full
}

int find_LRU(){
	
	int done = 0;
	int min = counter_usage_frame[0];
	int frame;
	for ( int i = 1 ; i < NUM_OF_FRAMES; i++)
	{
		if ( counter_usage_frame[i] <  min )
		{
			min = counter_usage_frame[i];
			done = 1;
			frame = i;
		}
	}
	if ( done == 1) {
		counter_usage_frame[frame] = 0;
		return frame;
	}
	else if ( done == 0) {
		counter_usage_frame[0] = 0;
		return 0;
	}
}

int get_seed_for_rand_replacement(){

	cout << " Enter seed for Random Page frame replacement"<<endl;
	int seed;
	cin >> seed;
	return seed;
}

void mmap_store(){

	    /* Open the store file. */
        /* Map the store file to memory. */
        /* Initialize the file descriptor. */
        int store_fd = open(BACKING_STORE_ADDR, O_RDONLY);
        void* store_data = mmap(0, PHYS_MEM_SIZE*2, PROT_READ, MAP_SHARED, store_fd, 0);
        
        if (store_data == MAP_FAILED) {
            close(store_fd);
            printf("Error mmapping the backing store file!");
            exit(EXIT_FAILURE);
        }

        storage = static_cast<char*> (store_data);
}

void remove_entry_from_page_table(int page_num){

	page_table[page_num][1] = 0;
}

void add_entry_to_page_table(int page_num, int frame_num){

	//cout << " adding entry" << frame_num << " to page number " << page_num << endl;

	page_table[page_num][0] = frame_num;
	page_table[page_num][1] = 1;
}

void remove_entry_from_page_table_framenum(int frame_num){

	for(int i = 0 ; i < PAGE_TABLE_ENTRIES ; i++){

		if(page_table[i][0] == frame_num)  // if this entry is in frame frame_num
			page_table[i][1] = 0;   
	}
}

void invalidate_entry_in_tlb(int frame_num){

	for(int i = 0 ; i < TLB_ENTRIES ; i++){

		if(tlb[i][1] == frame_num){ 
		 	tlb[i][2] = 0;
		}
	}
    // The frame was not in tlb so swap with next ptr 
	//update_tlb(page_num, frame_num);
}

int find_empty_index_in_tlb(){

	for(int i = 0 ; i < TLB_ENTRIES ; i++){

		if(tlb[i][2] == 0)
			return i;
	}
	return -1;
}
