#pragma once
#include "buffer_manager.h"
#include <unordered_map>

using std::unordered_map;

//hash + 双向链表
class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file), capacity(DEFBUFSIZE), biList() {
		LOG_DEBUG("enter LRU");
		//重写head的元素frame_id意义，代表双向链表长度，初始双向链表长度置为0
		biList.head.frame_id = 0;
		frameId2nodePtr = new BiNode * [capacity]();
	};
	
	~LRU() {
		LOG_DEBUG("exit LRU");
		delete[]frameId2nodePtr;
	};

	//读页，返回对应fid
	int ReadPageFromBMgr(int page_id, bFrame* reader);

	//写页，返回对应fid
	int WritePageFromBMgr(int page_id, bFrame* writer);

	virtual int FixNewPage(bFrame* tmp);

	//得到磁盘io次数
	count_n GetCountIO()const { return countIO; };

	//得到buffer命中次数
	count_n GetCountHit()const { return countHit; };

private:
	//将page_id对应的page加载到buffer，如果下一步操作不更新内容，应该读入内存，如果更新，无需IO
	int FillFrame(int page_id, bool willUpdating = false);

	//向LRU链表新增frame，返回插入操作后的双向链表长度（头插法）
	int InsertLRU(int frame_id);

	//选择一个将被替换的frame，如果dirty生效，写回disk，返回置换的frame_id（删除尾部）
	virtual int SelectVictim();

	//当缓存中某页被命中时调用，更新LRU表
	void UpdateLRU(int frame_id);



	//双向链表的容量
	const int capacity;

	//双向链表
	BiList biList;

	//hash，key:frame_id，value:ptr(BiNode)
	BiNode** frameId2nodePtr;
};