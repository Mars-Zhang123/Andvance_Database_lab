#pragma once
#include "buffer_manager.h"

class TwoQueues :protected BMgr {
public:
	TwoQueues(string filename, bool create_file = false) :BMgr(filename, create_file), 
														  cold(), hot() ,capacity(DEFBUFSIZE){
		LOG_DEBUG("eneter TwoQueues");
		//tailԪ�ص�frame_id�����ع�������ǰ���г�������
		//2Q�㷨ͨ����ҳ����ʱ��ҳ��3��
		int maxNumcoldPages = capacity / 4;
		cold.tail.frame_id = maxNumcoldPages;
		hot.tail.frame_id = capacity - maxNumcoldPages;

		frameId2coldPtr = new LRU_Node * [capacity]();
		frameId2hotPtr = new LRU_Node * [capacity]();
	}

	~TwoQueues() {
		LOG_DEBUG("exit TwoQueues");

		delete[]frameId2coldPtr;
		delete[]frameId2hotPtr;
	}

	//��ҳ�����ض�Ӧfid
	int ReadPageFromBMgr(int page_id, bFrame* reader);

	//дҳ�����ض�Ӧfid
	int WritePageFromBMgr(int page_id, bFrame* writer);

	virtual int FixNewPage(bFrame* tmp);

	//�õ�����io����
	count_n GetCountIO()const { return countIO; };

	//�õ�buffer���д���
	count_n GetCountHit()const { return countHit; };

private:
	//��page_id��Ӧ��page���ص�buffer�������һ���������������ݣ�Ӧ�ö����ڴ棬������£�����IO
	int FillFrame(int page_id, bool willUpdating = false);

	//��2Q����frame�����ز���󻺴���frame����
	int Insert2Q(int frame_id);

	//ѡ��һ�������滻��frame
	virtual int SelectVictim();

	//��������ĳҳ������ʱ���ã�����2Q
	void Update2Q(int frame_id);

	//2Q���ʻ���LRU����LRU2��ͬ����������frame_id����buffer��
	//��ҳ�����ֻ����1�Ρ���ҳ��������ʴ�������1
	LRU_List cold, hot;

	//��������
	const int capacity;

	//��ҳhash, key:frame_id��value:ptr(LRU_Node)
	LRU_Node** frameId2coldPtr;

	//��ҳhash, key:frame_id��value:ptr(LRU_Node)
	LRU_Node** frameId2hotPtr;
};