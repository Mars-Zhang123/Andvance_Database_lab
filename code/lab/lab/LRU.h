#pragma once
#include "buffer_manager.h"

//hash + ˫������
class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file), capacity(DEFBUFSIZE), BiList() {};
	
	~LRU() {
	//delete ˫������
	};

	//��page����buffer��������frame_id
	int FixPage(int page_id);

	//ɾ��LRU�б���ɾ��frame_id��ӦԪ��
	void RemoveLRUEle(int frame_id);

	//ѡ��һ�������滻��frame�����dirty��Ч��д��disk�������û���frame_id
	int SelectVictim();

private:
	//˫�����������
	const int capacity;

	LRU_List BiList;
};