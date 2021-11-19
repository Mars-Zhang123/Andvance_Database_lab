#pragma once
#include "buffer_manager.h"

class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file) {};
	
	~LRU() {};

	//
	int FixPage(int page_id);

	//删除LRU列表中删除frid对应元素
	void RemoveLRUEle(int frid);

	//选择一个将被替换的frame，如果dirty生效，写回disk，返回置换的frame_id
	int SelectVictim();

private:

};