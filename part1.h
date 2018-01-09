#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <string>

#define PAGE_SIZE 256
#define FRAME_SIZE 256

#define TLB_ENTRIES 16

#define MM_SIZE 128
#define VM_SIZE 256


#define tlb_rw 0.5
#define mm_rw 100
#define disk_rw 250000

int phys_mem[MM_SIZE];
int v_mem[VM_SIZE][2];
int tlb[TLB_ENTRIES][2];

int num_of_tlb_hits = 0;
int num_of_page_faults = 0;

void init_vm();
void init_phys_mem();
void init_tlb();
bool check_arg(int argc, char* argv[]);
void print_statistics();
void run_vmm(char* addr);
int get_offset(std::string addr);
int get_page_num(std::string addr);
bool is_in_tlb(int page_num);
int calc_phys_addr(int page_num, int offset);
