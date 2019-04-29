#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <utility>
#include <vector>

using namespace std;

// Cache Function Definitions here
string directmap(int, char*);
string setAssociative(int, char*);
string fullyAssociativeLRU(char*);
string fullyAssociativeHC(char*);
string setAssociativeNoAlloc(int, char*);
string setAssociativePrefetch(int, char*);
string setAssociativeCMPrefetch(int, char*);
string extraCreditLFU(int, char*);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Invalid Args - Please use ./cache-sim <input_trace.txt> <output.txt>" << endl;
        exit(EXIT_FAILURE);
    }

    ofstream outfile(argv[2]);

    // Direct-Mapped Cache
    outfile << directmap(1,argv[1]) << " ";
    outfile << directmap(4,argv[1]) << " ";
    outfile << directmap(16,argv[1]) << " ";
    outfile << directmap(32,argv[1]) << endl;

    // Set-Associative Cache
    outfile << setAssociative(2,argv[1]) << " ";
    outfile << setAssociative(4,argv[1]) << " ";
    outfile << setAssociative(8,argv[1]) << " ";
    outfile << setAssociative(16,argv[1]) << endl;

    // Fully-Associative Cache w/ LRU
    outfile << fullyAssociativeLRU(argv[1]) << endl;

    // Fully-Associative Cache w/ Hot/Cold
    outfile << fullyAssociativeHC(argv[1]) << endl;

    // Set-Associative Cache w/ No Allocation on a Write Miss
    outfile << setAssociativeNoAlloc(2,argv[1]) << " ";
    outfile << setAssociativeNoAlloc(4,argv[1]) << " ";
    outfile << setAssociativeNoAlloc(8,argv[1]) << " ";
    outfile << setAssociativeNoAlloc(16,argv[1]) << endl;

    // Set-Associative Cache w/ Next-line Prefetching
    outfile << setAssociativePrefetch(2,argv[1]) << " ";
    outfile << setAssociativePrefetch(4,argv[1]) << " ";
    outfile << setAssociativePrefetch(8,argv[1]) << " ";
    outfile << setAssociativePrefetch(16,argv[1]) << endl;

    // Prefetch-on-a-Miss
    outfile << setAssociativeCMPrefetch(2,argv[1]) << " ";
    outfile << setAssociativeCMPrefetch(4,argv[1]) << " ";
    outfile << setAssociativeCMPrefetch(8,argv[1]) << " ";
    outfile << setAssociativeCMPrefetch(16,argv[1]) << endl;

    // Extra Credit LFU
#if 0 // Set this to 1 to run extra credit
    outfile << extraCreditLFU(2,argv[1]) << " ";
    outfile << extraCreditLFU(4,argv[1]) << " ";
    outfile << extraCreditLFU(8,argv[1]) << " ";
    outfile << extraCreditLFU(16,argv[1]) << endl;
#endif

    return 0;
}

// Direct-Mapped Cache
string directmap(int size, char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned long long addr;

    int csize = (size * 1024)/32;
    int cache[csize];

    for(int i = 0; i < csize; i++) cache[i] = 0;

    accesses = 0, hits = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        int index = (addr/32) % csize;

        if(cache[index] == 32 * (addr/32)) hits++;
        else cache[index] = 32 * (addr/32);

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

// Set-Associative Cache
string setAssociative(int size, char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int timer;
    unsigned long long addr;

    int rows = 16*32 / size;
    int cache[rows][size];
    int lru[rows][size];

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < size; j++) {
            cache[i][j] = 0;
            lru[i][j] = 0;
        }
    }

    accesses = 0, hits = 0;
    timer = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        int index = (addr/32) % rows;

        bool hit = false;
        for(int i = 0; i < size; i++) {
            if(cache[index][i] == 32 * (addr/32)) {
                hits++;
                lru[index][i] = ++timer;
                hit = true;
                break;
            }
        }

        if(!hit) {
            int val = lru[index][0];
            int way = 0;

            for(int i = 0; i < size; i++) {
                if(lru[index][i] < val) {
                    val = lru[index][i];
                    way = i;
                }
            }

            lru[index][way] = ++timer;
            cache[index][way] = 32 * (addr/32);
        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

// Fully-Associative Cache w/ LRU
string fullyAssociativeLRU(char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int timer;
    unsigned int addr;

    int col = 512;
    int cache[col];
    int lru[col];

    for(int i = 0; i < col; i++) {
        cache[i] = 0;
        lru[i] = 0;
    }

    accesses = 0, hits = 0;
    timer = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        addr = 32 * (addr/32);

        bool hit = false;
        for(int i = 0; i < col; i++) {
            if(cache[i] == addr) {
                hits++;
                lru[i] = ++timer;
                hit = true;
                break;
            }
        }

        if(!hit) {
            int index = 0;
            int val = lru[0];

            for(int i = 0; i < col; i++) {
                if(lru[i] < val) {
                    val = lru[i];
                    index = i;
                }
            }

            lru[index] = ++timer;
            cache[index] = addr;
        }

        hit = false;
        for(int i = 0; i < col; i++) {

        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

// Fully-Associative Cache w/ Hot/Cold
int* updateBits(int* array, int len, int index) {
    int lvl = (len+1)/2;
    int val = len/2;

    while(lvl > 0) {
        if(lvl == 1) {
            if(index == val) {
                array[val] = 0;
            } else {
                array[val] = 1;
            }
            break;
        }

        lvl /= 2;

        if(index <= val) {
            array[val] = 0;
            val -= lvl;
        } else {
            array[val] = 1;
            val += lvl;
        }
    }

    return array;
}

int getColdBit(int* array, int len, int offset) {
    int arr[len/2];

    if(len == 1) {
        if(array[0] == 1) return offset;
        else return offset + 1;
    }

    if(array[len/2] == 1) {
        memcpy(arr, array, (len/2) * sizeof(int));
        return getColdBit(arr, len/2, offset);
    } else {
        memcpy(arr, array+(len/2 + 1), (len/2) * sizeof(int));
        return getColdBit(arr, len/2, offset + (len/2 + 1));
    }

    return 0;
}

string fullyAssociativeHC(char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int addr;

    int col = 512;
    int cache[col];
    int bitArray[col-1];

    for(int i = 0; i < col; i++) {
        cache[i] = 0;
        bitArray[i] = 0;
    }

    accesses = 0, hits = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        addr = 32 * (addr/32);

        bool hit = false;
        for(int i = 0; i < col; i++) {
            if(cache[i] == addr) {
                hits++;
                hit = true;
                memcpy(bitArray, updateBits(bitArray, 511, i), 511*sizeof(int));
            }
        }

        if(!hit) {
            int index = getColdBit(bitArray, 511, 0);
            memcpy(bitArray, updateBits(bitArray, 511, index), 511*sizeof(int));
            cache[index] = addr;
        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

// Set-Associative Cache w/ No Allocation on a Write Miss
string setAssociativeNoAlloc(int size, char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int timer;
    unsigned int addr;

    int rows = 16*32 / size;
    int cache[rows][size];
    int lru[rows][size];

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < size; j++) {
            cache[i][j] = 0;
            lru[i][j] = 0;
        }
    }

    accesses = 0, hits = 0;
    timer = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        int index = (addr/32) % rows;

        bool hit = false;
        for(int i = 0; i < size; i++) {
            if(cache[index][i] == 32 * (addr/32)) {
                hits++;
                lru[index][i] = ++timer;
                hit = true;
                break;
            }
        }

        if(!hit && instr == "L") {
            int val = lru[index][0];
            int way = 0;

            for(int i = 0; i < size; i++) {
                if(lru[index][i] < val) {
                    val = lru[index][i];
                    way = i;
                }
            }

            lru[index][way] = ++timer;
            cache[index][way] = 32 * (addr/32);
        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

// Set-Associative Cache w/ Next-line Prefetching
string setAssociativePrefetch(int size, char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int timer;
    unsigned int addr;

    int rows = 16*32 / size;
    int cache[rows][size];
    int lru[rows][size];

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < size; j++) {
            cache[i][j] = 0;
            lru[i][j] = 0;
        }
    }

    accesses = 0, hits = 0;
    timer = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        int index = (addr/32) % rows;

        bool hit = false;
        for(int i = 0; i < size; i++) {
            if(cache[index][i] == 32 * (addr/32)) {
                hits++;
                lru[index][i] = ++timer;
                hit = true;
                break;
            }
        }

        if(!hit) {
            int val = lru[index][0];
            int way = 0;

            for(int i = 0; i < size; i++) {
                if(lru[index][i] < val) {
                    val = lru[index][i];
                    way = i;
                }
            }

            lru[index][way] = ++timer;
            cache[index][way] = 32 * (addr/32);
        }

        int index2 = (addr/32 + 1) % rows;

        hit = false;
        for(int i = 0; i < size; i++) {
            if(cache[index2][i] == 32 * (addr/32 + 1)) {
                hit = true;
                lru[index2][i] = ++timer;
                break;
            }
        }

        if(!hit) {
            int val = lru[index2][0];
            int way = 0;

            for(int i = 0; i < size; i++) {
                if(lru[index2][i] < val) {
                    val = lru[index2][i];
                    way = i;
                }
            }

            lru[index2][way] = ++timer;
            cache[index2][way] = 32 * ((addr/32) + 1);
        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

// Prefetch-on-a-Miss
string setAssociativeCMPrefetch(int size, char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int timer;
    unsigned int addr;

    int rows = 16*32 / size;
    int cache[rows][size];
    int lru[rows][size];

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < size; j++) {
            cache[i][j] = 0;
            lru[i][j] = 0;
        }
    }

    accesses = 0, hits = 0;
    timer = 0;
    while(getline(infile, line)) {
        stringstream s(line);
        s >> instr >> std::hex >> addr;
        int index = (addr/32) % rows;

        bool hit = false;
        for(int i = 0; i < size; i++) {
            if(cache[index][i] == 32 * (addr/32)) {
                hits++;
                lru[index][i] = ++timer;
                hit = true;
                break;
            }
        }

        if(!hit) {
            int val = lru[index][0];
            int way = 0;

            for(int i = 0; i < size; i++) {
                if(lru[index][i] < val) {
                    val = lru[index][i];
                    way = i;
                }
            }

            lru[index][way] = ++timer;
            cache[index][way] = 32 * (addr/32);

            int index2 = (addr/32 + 1) % rows;

            hit = false;
            for(int i = 0; i < size; i++) {
                if(cache[index2][i] == 32 * (addr/32 + 1)) {
                    hit = true;
                    lru[index2][i] = ++timer;
                    break;
                }
            }

            if(!hit) {
                int val = lru[index2][0];
                int way = 0;

                for(int i = 0; i < size; i++) {
                    if(lru[index2][i] < val) {
                        val = lru[index2][i];
                        way = i;
                    }
                }

                lru[index2][way] = ++timer;
                cache[index2][way] = 32 * ((addr/32) + 1);
            }
        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}

string extraCreditLFU(int size, char* arg) {
    ifstream infile(arg);
    string instr, line;
    unsigned int accesses, hits;
    unsigned int addr;

    int rows = 16*32 / size;
    int cache[rows][size];
    vector<int> tags; // store tags
    vector<int> freq; // store frequencies

    // initialize array with zeroes
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < size; j++) {
            cache[i][j] = 0;
        }
    }

    // initialize variables
    accesses = 0, hits = 0;

    // parse input trace
    while(getline(infile, line)) {
        stringstream s(line);
        // instr = "L" or "S", addr = tag (in hex)
        s >> instr >> std::hex >> addr;
        int index = (addr/32) % rows;

        bool hit = false;
        for(int i = 0; i < size; i++) {
            if(cache[index][i] == 32 * (addr/32)) {
                hits++;
                hit = true;

                // update freq if tag is present
                for(int j = 0; j < tags.size(); j++) {
                    if(tags[j] == 32 * (addr/32)) {
                        freq[j]++;
                        break;
                    }
                }
                break;
            }
        }

        // if tag not present, need to replace
        if(!hit) {
            int freq2[size];

            for(int i = 0; i < size; i++) freq2[i] = 0;

            // populate temporary freq array from vector
            for(int i = 0; i < tags.size(); i++) {
                for(int j = 0; j < size; j++) {
                    if(tags[i] == cache[index][j]) {
                        freq2[j] = freq[i];
                    }
                }
            }

            int max = 0;
            int max_val = cache[index][0];

            // obtain the smallest frequency
            for(int i = 0; i < size; i++) {
                if(freq2[i] < max_val) {
                    max = i;
                    max_val = freq2[i];
                }
            }

            // replace smallest frequency
            cache[index][max] = 32 * (addr/32);

            // check if tag is present
            bool hit = false;
            for(int i = 0; i < tags.size(); i++) {
                if(tags[i] == 32 * (addr/32)) {
                    hit = true;
                    break;
                }
            }

            // if not present, update frequency
            if(!hit) {
                tags.push_back(32 * (addr/32));
                freq.push_back(1);
            }
        }

        accesses++;
    }

    return (to_string(hits) + "," + to_string(accesses) + ";");
}
