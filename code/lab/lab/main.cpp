#include "LRU.h"
#include <iostream>
#include <string>

using std::to_string;
using std::cout;
using std::endl;

void init() {
	LOG_ON_OFF = true;
	FILE *streamLog, *streamData;
	freopen_s(&streamLog, "log.out", "w", stderr);
	freopen_s(&streamData, "data.out", "w", stdout);
	fflush(stdout);
	LRU obj(DATAFILE, true);
	//创造50000个新页
	for (int cnt = 0; cnt < 50000; cnt++) {
		bFrame tmp;
		memcpy(&tmp, ("test" + to_string(cnt)).c_str(), sizeof(bFrame));
		obj.FixNewPage(&tmp);
	}
}



int main() {
	init();
	
	
	return 0;
}