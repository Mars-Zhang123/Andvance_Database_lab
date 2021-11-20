#pragma once
#include "buffer_manager.h"

//hash + 双向链表
class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file), capacity(DEFBUFSIZE), BiList() {};
	
	~LRU() {
	//delete 双向链表
	};

	//将page读入buffer，并返回frame_id
	int FixPage(int page_id);

	//删除LRU列表中删除frame_id对应元素
	void RemoveLRUEle(int frame_id);

	//选择一个将被替换的frame，如果dirty生效，写回disk，返回置换的frame_id
	int SelectVictim();

private:
	//双向链表的容量
	const int capacity;

	LRU_List BiList;
};