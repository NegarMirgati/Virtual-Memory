#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>

#define PAGE_SIZE 256
#define TLB_ENTRIES 16
#define MM_SIZE 128
#define VM_SIZE 256

void init_vm();
void init_phys_mem();
void init_tlb();
bool check_arg(int argc, char* argv[]);
void print_statistics();
void run_vmm(char* addr);
