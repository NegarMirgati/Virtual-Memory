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
#include <sstream>

using namespace std;

#define num_of_tests 10000
#define PHYS_MEM_SIZE 65536

string addr;
void generate_rands();
void generate_rands_with_locality(int mode);
int fRand(int fMax);
int mystoi(string number);
string itos(int number);

int main(){

	for(int i = 0 ; i <= 9 ; i++){

		 addr = "myaddr" + itos(i) + ".txt";
		//ofstream outfile;
		//outfile.open(addr.c_str());

		if( i == 8)
			generate_rands_with_locality(0);
		else if (i == 9)
			generate_rands_with_locality(1);
		else
			generate_rands();
	}

	return 0;
}

void generate_rands(){

	cout << "Enter seed for random address generation " << endl;
    int seed ;
	cin>> seed;
	srand(seed);

	ofstream myfile;
    myfile.open(addr.c_str(), std::ofstream::out | std::ofstream::trunc);

    for(int i = 0; i < num_of_tests; i++){

   		int  temp_out = fRand(65536);
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

void generate_rands_with_locality(int mode){

	ofstream outfile;
	//outfile.open("myaddresses.txt");
	outfile.open(addr.c_str(), std::ofstream::out | std::ofstream::trunc);

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

int fRand(int fMax){
   
    int f = rand() % fMax;
    return f;
}

int mystoi(string number) {

  int n;
  stringstream ss;
  ss << number;
  ss >> n;
  return n;
}

string itos(int number) {
  ostringstream oss;
  oss << number;
  return oss.str();
}