#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;

 std::vector<std::string> split(std::string statement, char delimeter);

 bool wrong = false;

int main(){
	
	ifstream inf_correct;
	ifstream my_out;
	ofstream out;
	inf_correct.open("correct.txt");
	my_out.open("OUTPUT.txt");
	out.open("test.txt");

	string my_out_line , correct_line;

	while(getline(my_out, my_out_line)){

		getline(inf_correct, correct_line);

			
			vector<string> my_tokens = split(my_out_line, ' ');
			vector<string> corr_tokens = split(correct_line, ' ');

			//cout << corr_tokens.size() << endl;
			//cout << my_tokens.size() << endl;

			if(corr_tokens[7] != my_tokens[7]){
				cout << my_out_line << endl;
				cout << correct_line << endl;
				wrong = true;
			}


	}

	//cout << " phew" << endl;
	if(!wrong)
		cout << " Good job!" << endl;
	out.close();
	inf_correct.close();
	my_out.close();
	return 0;
}

vector<string> split(string statement, char delimeter) {
	vector<string> result;
	string token;
	for(int i=0; i<statement.length(); i++)
		if(statement[i] != delimeter)
			token += statement[i];
		else if(token.length()) {
			result.push_back(token);
			token = "";
		}
	if(token.length())
		result.push_back(token);
	return result;
}
