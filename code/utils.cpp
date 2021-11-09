#include "utils.h"

using namespace std;

void FixNewPage(int pageSum = 50000){
    ofstream heapFile(DATAFILE, ios::out);  
    if (!heapFile.good()) {
        throw "Err: can not create .dbf!";
    }

    for (int page_id = 0; page_id < pageSum; page_id++) {
        char *padding = new char[FRAMESIZE];
        heapFile << padding; 
        delete []padding;
    }
    heapFile.close();
}