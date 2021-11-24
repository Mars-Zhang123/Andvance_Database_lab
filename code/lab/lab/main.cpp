#include "LRU.h"
#include <iostream>
#include <string>
#include <list>

using std::to_string;
using std::list;
using std::pair;
using std::make_pair;
using std::stoi;

void Check() {
	//读写测试，测试了3500页和3501页读写测试
	//前提：CreateFile完成，进行页读取和页写入，selectVictim在createfile过程调用FixNewPage已经测试完成！
	LOG_ON_OFF = false;
	LRU obj(DATAFILE, false);
	bFrame tmp, AAA;
	printf("读页3500,对应fid：%d\n", obj.ReadPageFromBMgr(3500, &tmp));
	printf("%s\n", tmp.field);
	printf("读页3501,对应fid：%d\n", obj.ReadPageFromBMgr(3501, &tmp));
	printf("%s\n", tmp.field);
	memcpy(tmp.field, "update3500", sizeof(tmp));
	printf("写页3500,对应fid：%d\n", obj.WritePageFromBMgr(3500, &tmp));
	printf("读页3500,对应fid：%d\n", obj.ReadPageFromBMgr(3500, &AAA));
	printf("%s\n", AAA.field);
	memcpy(tmp.field, "update3501", sizeof(tmp));
	printf("写页3501,对应fid：%d\n", obj.WritePageFromBMgr(3501, &tmp));
	printf("读页3501,对应fid：%d\n", obj.ReadPageFromBMgr(3501, &AAA));
	printf("%s\n", AAA.field);
}

void CreateFile() {
	LRU obj(DATAFILE, true);
	//创造50000个新页
	for (int cnt = 0; cnt < 50064; cnt++) {
		bFrame tmp;
		memcpy(&tmp, ("test" + to_string(cnt)).c_str(), sizeof(bFrame));
		obj.FixNewPage(&tmp);
	}
}

void TestLRU() {
	FILE* in_fp;
	if (fopen_s(&in_fp, "G:/Advanced_database/Andvance_Database_lab/data-5w-50w-zipf.txt", "r+"))
		FAIL;
	list<pair<int, int>> cmds;
	while (true) {
		char line[12] = {};
		fgets(line, sizeof(line), in_fp);
		if (feof(in_fp))
			break;
		line[1] = '\0';
		auto tmp_data = make_pair(stoi(line), (stoi(line + 2) - 1));
		cmds.push_back(tmp_data);
	}
	fclose(in_fp);
	clock_t start, end;
	LRU obj(DATAFILE, false);
	bFrame writer, reader;
	memcpy(writer.field, "update", sizeof(writer));
	start = clock();
	int cnt = 0;//访问计数
	for (auto& it : cmds) {
		cnt++;
		//fprintf(stderr, "vis page:%d\n", it.second);
		if (it.first == 0) {//读
			obj.ReadPageFromBMgr(it.second, &reader);
		} else {//写
			obj.WritePageFromBMgr(it.second, &writer);
		}
		//写入磁盘io次数和buffer命中次数
		printf("%u,%u\n", obj.GetCountIO(), obj.GetCountHit());
	}
	end = clock();
	printf("%lf", double((end - start) / double(CLOCKS_PER_SEC)));
}

void Init() {
	LOG_ON_OFF = false;
	FILE *streamLog, *streamData;
	freopen_s(&streamLog, "log.out", "w", stderr);
	freopen_s(&streamData, "data.out", "w", stdout);
	fflush(stdout);
}



int main() {
	Init();
	CreateFile();
	TestLRU();
	return 0;
}