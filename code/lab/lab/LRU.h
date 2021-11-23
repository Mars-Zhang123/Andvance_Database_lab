#pragma once
#include "buffer_manager.h"
#include <unordered_map>

using std::unordered_map;

//hash + ˫������
class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file), capacity(DEFBUFSIZE), BiList() {
		//��дhead��Ԫ��frame_id���壬����˫�������ȣ���ʼ˫����������Ϊ0
		BiList.head.frame_id = 0;
		frameId2nodePtr = new LRU_Node * [capacity]();
	};
	
	~LRU() {
		delete[]frameId2nodePtr;
	};

	//��ҳ
	int ReadPageFromBMgr(int page_id, bFrame*& reader);

	//дҳ
	int WritePageFromBMgr(int page_id, bFrame* writer);

	virtual int FixNewPage(bFrame* tmp);

protected:
	//��page_id��Ӧ��page���ص�buffer
	int FillFrame(int page_id);

	//��LRU��������frame�����ز���������˫�������ȣ�ͷ�巨��
	int InsertLRU(int frame_id);

	//ѡ��һ�������滻��frame�����dirty��Ч��д��disk�������û���frame_id��ɾ��β����
	int SelectVictim();

	//��������ĳҳ������ʱ���ã�����LRU��
	void UpdateLRU(int frame_id);

private:
	//˫�����������
	const int capacity;

	//˫������
	LRU_List BiList;

	//hash��key:frame_id��value:ptr(LRU_Node)
	LRU_Node** frameId2nodePtr;
};