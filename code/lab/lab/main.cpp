#include "buffer_manager.h"
#include <iostream>
using std::cout;
using std::endl;
void init() {
	LOG_ON_OFF = true;
	FILE *streamLog, *streamData;
	freopen_s(&streamLog, "log.out", "a+", stderr);
	freopen_s(&streamData, "data.out", "w", stdout);
	fflush(stdout);
}

int main() {
	cout << sizeof(long long) << endl;
	return 0;
}