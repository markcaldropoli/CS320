#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string always(int,char*);
string bimodal1(int,char*);
string bimodal2(int,char*);
string gshare(int,char*);
string tournament(char*);
string perceptron(int,int,char*);

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
    // Tournament
    outfile << tournament(argv[1]) << endl;

    /**
     * This line is for the extra credit perceptron predictor.
     * Do not uncomment this when grading the assignment.
     **/
    // Perceptron - Extra Credit
    //outfile << perceptron(2048,11,argv[1]) << endl;

    return 0;
}

// Implementation for Always Taken & Always Non-Taken
string always(int type, char* arg) {
    ifstream infile(arg);
    string behavior, line;
    unsigned int count, correct;
    unsigned long long addr;

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
    int table[tsize];
    string behavior, line;
    unsigned int count, correct;
    unsigned long long addr;
    
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
    int table[tsize];
    string behavior, line;
    unsigned int count, correct;
    unsigned long long addr;

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

// Implementation of Tournament Predictor
string tournament(char* arg) {
    ifstream infile(arg);
    int bimodalTable[2048];
    int gshareTable[2048];
    int selectorTable[2048];
    string behavior, line;
    unsigned int count, bcount, gcount, correct, history, mask;
    unsigned long long addr;

    count = 0, bcount = 0, gcount = 0, correct = 0, history = 0;
    mask = 0b11111111111;
    for(int i = 0; i < 2048; i++) {
        bimodalTable[i] = 1;
        gshareTable[i] = 1;
        selectorTable[i] = 2;
    }

    while(getline(infile, line)) {
        stringstream s(line);
        s >> std::hex >> addr >> behavior;
        int bsIndex = addr % 2048;
        int gIndex = bsIndex ^ (history & mask);
        int bval = bimodalTable[bsIndex];
        int gval = gshareTable[gIndex];
        int sval = selectorTable[bsIndex];

        // bimodal
        if(behavior == "NT") {
            if(bval == 0 || bval == 1) bcount = 1;
            if(bval != 0) bimodalTable[bsIndex]--;
        } else if(behavior == "T") {
            if(bval == 2 || bval == 3) bcount = 1;
            if(bval != 3) bimodalTable[bsIndex]++;
        }

        // gshare
        if(behavior == "NT") {
            if(gval == 0 || gval == 1) gcount = 1;
            if(gval > 0) gshareTable[gIndex]--;
            history <<= 1;
        } else if(behavior == "T") {
            if(gval == 2 || gval == 3) gcount = 1;
            if(gval < 3) gshareTable[gIndex]++;
            history <<= 1;
            history |= 1;
        }

        switch(sval) {
            case 0:
                if((bcount == 1 && gcount == 1) || (bcount == 0 && gcount == 1)) correct++;
                if(bcount == 1 && gcount == 0) selectorTable[bsIndex] = 1;
                break;
            case 1:
                if((bcount == 1 && gcount == 1) || (bcount == 0 && gcount == 1)) correct++;
                if(bcount == 0 && gcount == 1) selectorTable[bsIndex] = 0;
                if(bcount == 1 && gcount == 0) selectorTable[bsIndex] = 2;
                break;
            case 2:
                if((bcount == 1 && gcount == 1) || (bcount == 1 && gcount == 0)) correct++;
                if(bcount == 0 && gcount == 1) selectorTable[bsIndex] = 1;
                if(bcount == 1 && gcount == 0) selectorTable[bsIndex] = 3;
                break;
            case 3:
                if((bcount == 1 && gcount == 1) || (bcount == 1 && gcount == 0)) correct++;
                if(bcount == 0 && gcount == 1) selectorTable[bsIndex] = 2;
                break;
            default:
                fprintf(stderr, "Invalid selector state.\n");
                exit(EXIT_FAILURE);
        }
        bcount = 0, gcount = 0;
        count++;
    }

    return (to_string(correct) + "," + to_string(count) + ";");
}

/**
 * Perceptron Predictor - Extra Credit
 * Based on: https://www.cs.utexas.edu/~lin/papers/hpca01.pdf
 **/
string perceptron(int tsize, int bits, char* arg) {
    ifstream infile(arg);
    int bias[tsize];
    int t, y; // t = -1 if NT, 1 if T; y = perceptron output
    string behavior, line;
    unsigned int count, correct;
    unsigned long long addr;
    vector<int> ghistory;
    vector<int> weights;
    vector<vector<int>> ptable;

    int theta = 1.93*bits + 14; // theta = threshold
    /**
     * This is because adding another weight to a perceptron
     * increases its average output by some constant, so the
     * threshold must be increased by a constant, yielding a
     * linear relationship between history length and threshold
     **/

    // initialization
    count = 0, correct = 0, y = 0;
    bias[0] = 1; //x0 is always set to 1, providing a “bias” input
    for(int i = 1; i < tsize; i++) bias[i] = -1;
    for(int i = 0; i < bits; i++) weights.push_back(0);
    ghistory.push_back(1);
    for(int i = 1; i < bits; i++) ghistory.push_back(-1);
    for(int i = 0; i < tsize; i++) ptable.push_back(weights);

    while(getline(infile, line)) {
        stringstream s(line);
        s >> std::hex >> addr >> behavior;
        int index = addr % tsize;
        vector<int> perceptron = ptable[index];
        y = bias[index];

        // dot product of perceptron and global history register
        for(int i = 1; i < bits; i++) y += (ghistory[i]*perceptron[i]);

        // NT when y is negative, T otherwise
        if((y >= 0 && behavior == "T") || (y < 0 && behavior == "NT")) correct++;

        // set value of t
        if(behavior == "NT") t = -1;
        else if(behavior == "T") t = 1;

        // training algorithm
        if(((y >= 0 && t == -1) || (y < 0 && t == 1)) || (abs(y) <= theta)) {
            bias[index] += t;

            for(int i = 1; i < bits; i++) {
                if(t == ghistory[i]) perceptron[i]++;
                else perceptron[i]--;
            }
        }

        // write back to ith entry
        std::rotate(ghistory.begin(), ghistory.begin()+ghistory.size()-1, ghistory.end());
        ghistory[0] = t;

        count++;
    }

    return (to_string(correct) + "," + to_string(count) + ";");
}
