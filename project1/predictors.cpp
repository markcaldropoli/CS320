#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string always(int,char*);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Invalid Args - Please use ./predictors <input_trace.txt> <output.txt>" << endl;
        exit(EXIT_FAILURE);
    }
    
    ofstream outfile(argv[2]);

    outfile << always(0, argv[1]) << endl;
    outfile << always(1, argv[1]) << endl;

    return 0;
}

string always(int type, char* arg) {
    ifstream infile(arg);
    unsigned int count, correct;
    unsigned long long addr;
    string behavior, line;

    count = 0, correct = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> std::hex >> addr >> behavior;

        if(type == 0) {
            if(behavior == "T") correct++;
        } else {
            if(behavior != "T") correct++;
        }
        count++;
    }

    string str = to_string(correct) + "," + to_string(count) + ";";
    return str;
}
