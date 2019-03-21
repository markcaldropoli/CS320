#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string always(int,char*);
string bimodal1(int,char*);
string bimodal2(int,char*);
string gshare(int,char*);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Invalid Args - Please use ./predictors <input_trace.txt> <output.txt>" << endl;
        exit(EXIT_FAILURE);
    }
    
    ofstream outfile(argv[2]);

    // Always Taken
    outfile << always(0, argv[1]) << endl;
    // Always Non-Taken
    outfile << always(1, argv[1]) << endl;
    // Bimodal Predictor w/ 1 bit history - 7 variations
    outfile << bimodal1(16,argv[1]) << " ";
    outfile << bimodal1(32,argv[1]) << " ";
    outfile << bimodal1(128,argv[1]) << " ";
    outfile << bimodal1(256,argv[1]) << " ";
    outfile << bimodal1(512,argv[1]) << " ";
    outfile << bimodal1(1024,argv[1]) << " ";
    outfile << bimodal1(2048,argv[1]) << endl;
    // Bimodal Predictor w/ 2 bit counters - 7 variations
    outfile << bimodal2(16,argv[1]) << " ";
    outfile << bimodal2(32,argv[1]) << " ";
    outfile << bimodal2(128,argv[1]) << " ";
    outfile << bimodal2(256,argv[1]) << " ";
    outfile << bimodal2(512,argv[1]) << " ";
    outfile << bimodal2(1024,argv[1]) << " ";
    outfile << bimodal2(2048,argv[1]) << endl;
    // Gshare - 9 variations
    outfile << gshare(3,argv[1]) << " ";
    outfile << gshare(4,argv[1]) << " ";
    outfile << gshare(5,argv[1]) << " ";
    outfile << gshare(6,argv[1]) << " ";
    outfile << gshare(7,argv[1]) << " ";
    outfile << gshare(8,argv[1]) << " ";
    outfile << gshare(9,argv[1]) << " ";
    outfile << gshare(10,argv[1]) << " ";
    outfile << gshare(11,argv[1]) << endl;
    // TODO Tournament

    return 0;
}

// Implementation for Always Taken & Always Non-Taken
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
            // Always Taken
            if(behavior == "T") correct++;
        } else {
            // Always Non-Taken
            if(behavior != "T") correct++;
        }
        count++;
    }
    
    return (to_string(correct) + "," + to_string(count) + ";");
}

// Implementation for 1-bit Bimodal Predictor
string bimodal1(int tsize, char* arg) {
    ifstream infile(arg);
    unsigned int count, correct;
    unsigned long long addr;
    string behavior, line;
    int table[tsize];
    
    count = 0, correct = 0;
    for(int i = 0; i < tsize; i++) table[i] = 0;

    while(getline(infile, line)) {
        stringstream s(line);
        s >> std::hex >> addr >> behavior;
        int index = addr % tsize;
        int tval = table[index];

        if(behavior == "NT") {
            if(tval == 0) correct++;
            else table[index] = 0;
        } else if(behavior == "T") {
            if(tval == 1) correct++;
            else table[index] = 1;
        }
        count++;
    }

    return (to_string(correct) + "," + to_string(count) + ";");
}

// Implementation for 2-bit Bimodal Predictor
string bimodal2(int tsize, char* arg) {
    ifstream infile(arg);
    unsigned int count, correct;
    unsigned long long addr;
    string behavior, line;
    int table[tsize];

    count = 0, correct = 0;
    for(int i = 0; i < tsize; i++) table[i] = 1;

    while(getline(infile, line)) {
        stringstream s(line);
        s >> std::hex >> addr >> behavior;
        int index = addr % tsize;
        int tval = table[index];

        if(behavior == "NT") {
            if(tval == 0 || tval == 1) correct++;
            if(tval != 0) table[index]--;
        } else if(behavior == "T") {
            if(tval == 2 || tval == 3) correct++;
            if(tval != 3) table[index]++;
        }
        count++;
    }

    return (to_string(correct) + "," + to_string(count) + ";");
}

// Implementation for Gshare Predictor
unsigned int getMaxHistory(unsigned int bits) {
    unsigned int result = 0;

    switch(bits) {
        case 3:
            result = 0b00000000111;
            break;
        case 4:
            result = 0b00000001111;
            break;
        case 5:
            result = 0b00000011111;
            break;
        case 6:
            result = 0b00000111111;
            break;
        case 7:
            result = 0b00001111111;
            break;
        case 8:
            result = 0b00011111111;
            break;
        case 9:
            result = 0b00111111111;
            break;
        case 10:
            result = 0b01111111111;
            break;
        case 11:
            result = 0b11111111111;
            break;
        default:
            fprintf(stderr, "Incorrect bit value.\n");
            exit(EXIT_FAILURE);
    }
    return result;
}


string gshare(int bits, char* arg) {
    ifstream infile(arg);
    int table[2048];
    string behavior, line;
    unsigned int count, correct, history, mask;
    unsigned long long addr;

    count = 0, correct = 0, history = 0;
    mask = getMaxHistory(bits);
    for(int i = 0; i < 2048; i++) table[i] = 1;

    while(getline(infile, line)) {
        stringstream s(line);
        s >> std::hex >> addr >> behavior;
        int index = (addr % 2048) ^ (history & mask);
        int tval = table[index];

        if(behavior == "NT") {
            if(tval == 0 || tval == 1) correct++;
            if(tval > 0) table[index]--;
            history <<= 1;
        } else if(behavior == "T") {
            if(tval == 2 || tval == 3) correct++;
            if(tval < 3) table[index]++;
            history <<= 1;
            history |= 1;
        }
        count++;
    }

    return (to_string(correct) + "," + to_string(count) + ";");
}
