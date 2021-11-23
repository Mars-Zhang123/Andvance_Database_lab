#pragma once
#include "buffer_manager.h"
#include <unordered_map>

using std::unordered_map;

//hash + 双向链表
class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file), capacity(DEFBUFSIZE), BiList() {
		//重写head的元素frame_id意义，代表双向链表长度，初始双向链表长度置为0
		BiList.head.frame_id = 0;
		frameId2nodePtr = new LRU_Node * [capacity]();
	};
	
	~LRU() {
		delete[]frameId2nodePtr;
	};

	//读页
	int ReadPageFromBMgr(int page_id, bFrame*& reader);

	//写页
	int WritePageFromBMgr(int page_id, bFrame* writer);

	virtual int FixNewPage(bFrame* tmp);

protected:
	//将page_id对应的page加载到buffer
	int FillFrame(int page_id);

	//向LRU链表新增frame，返回插入操作后的双向链表长度（头插法）
	int InsertLRU(int frame_id);

	//选择一个将被替换的frame，如果dirty生效，写回disk，返回置换的frame_id（删除尾部）
	int SelectVictim();

	//当缓存中某页被命中时调用，更新LRU表
	void UpdateLRU(int frame_id);

private:
	//双向链表的容量
	const int capacity;

	//双向链表
	LRU_List BiList;

	//hash，key:frame_id，value:ptr(LRU_Node)
	LRU_Node** frameId2nodePtr;
};