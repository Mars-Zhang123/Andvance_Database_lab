#pragma once
#include "buffer_manager.h"

class TwoQueues :protected BMgr {
public:
	TwoQueues(string filename, bool create_file = false) :BMgr(filename, create_file), 
														  cold(), hot() ,capacity(DEFBUFSIZE){
		LOG_DEBUG("eneter TwoQueues");
		//tail元素的frame_id意义重构，代表当前队列长度上限
		//2Q算法通常热页缓存时冷页的3倍
		int maxNumcoldPages = capacity / 4;
		cold.tail.frame_id = maxNumcoldPages;
		hot.tail.frame_id = capacity - maxNumcoldPages;

		frameId2coldPtr = new BiNode * [capacity]();
		frameId2hotPtr = new BiNode * [capacity]();
	}

	~TwoQueues() {
		LOG_DEBUG("exit TwoQueues");

		delete[]frameId2coldPtr;
		delete[]frameId2hotPtr;
	}

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

	//向2Q新增frame，返回插入后缓存中frame个数
	//如果插入在冷页，则willInsertCold为true，否则为false
	int Insert2Q(int frame_id, bool willInsertCold);

	//选择一个将被替换的frame,根据成员变量willDeleteColdFrame决定待删除Frame所在的表
	virtual int SelectVictim();

	//当缓存中某页被命中时调用，更新2Q
	void Update2Q(int frame_id);

	//配合Insert2Q和SelectVictim使用，用于指定待淘汰页所在的链表
	//如果淘汰冷页，则willDeleteColdFrame为true，否则为false
	bool willDeleteColdFrame;

	//2Q本质还是LRU，与LRU2不同，两个队列frame_id都在buffer。
	//冷页：最近只访问1次。热页：最近访问次数大于1
	BiList cold, hot;

	//缓存容量
	const int capacity;

	//冷页hash, key:frame_id，value:ptr(BiNode),FIFO
	BiNode** frameId2coldPtr;

	//热页hash, key:frame_id，value:ptr(BiNode)
	BiNode** frameId2hotPtr;
};