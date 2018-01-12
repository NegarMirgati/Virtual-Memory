#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <time.h>
#include <bitset>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

#define PAGE_SIZE 256
#define PAGE_TABLE_ENTRIES 256
#define FRAME_SIZE 256
#define TLB_ENTRIES 16
#define NUM_OF_FRAMES 128
#define PHYS_MEM_SIZE 32768


#define tlb_rw 0.5
#define mm_rw 1000
#define disk_rw 250000

#define num_of_tests 10000

#define MYADDR "myaddresses.txt"
#define BACKING_STORE_ADDR "BACKING_STORE.bin"


char phys_mem[PHYS_MEM_SIZE];  /// 1 byte 
int page_table[PAGE_TABLE_ENTRIES];
int tlb[TLB_ENTRIES][2];
int counter_usage_frame[NUM_OF_FRAMES];  //recently used page in frame will have a bigger number
int state_phys_mem[NUM_OF_FRAMES]; // 0 => empty ,  1=> full 
int second_chance[NUM_OF_FRAMES];

/* statistics variables */
int num_of_tlb_hits = 0;
int num_of_page_faults = 0;

/* MMPAP variables */
char* storage;
int store_fd;
void* store_data;

int final_value;    /* value read from phys mem */

int tlb_ptr = -1;   /* FIFO tlb replacement policy */

int current_frame = 0;

int page_replacement_policy;  /* 1.FIFO 2.LRU 3.second chance 4.random replacement */

bool mem_is_full = false;  /* Random Replacement */

/* VMM Funcs */


void init_pt();
void init_tlb();
void init_counter_usage_frame();
void init_state_phys_mem();
void init_second_chance();

void run_vmm(char* addr);
int get_offset(std::string addr);
int get_page_num(std::string addr);
int find_in_tlb(int page_num);  /*modified*/
int find_in_page_table(int page_table_num); /*modified*/
void update_tlb(int page_num, int frame_num); 
void update_counter_usage_frame(int frame_num);
bool is_memory_full(int& available_frame);
int find_LRU();
void mmap_store();
void swap_in(int page_num);


/* Other Funcs*/
void generate_rands();
void generate_rands_with_locality(int mode);
int get_seed_for_rand_replacement();
int fRand(int fMax);
int menu();
int menu_PRP();
void print_statistics();
bool check_arg(int argc, char* argv[]);



 