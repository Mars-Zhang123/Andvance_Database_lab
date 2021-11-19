#pragma once
#include "buffer_manager.h"

class LRU :protected BMgr {
public:
	LRU(string filename, bool create_file = false) :BMgr(filename, create_file) {};
	
	~LRU() {};

	//
	int FixPage(int page_id);

	//ɾ��LRU�б���ɾ��frid��ӦԪ��
	void RemoveLRUEle(int frid);

	//ѡ��һ�������滻��frame�����dirty��Ч��д��disk�������û���frame_id
	int SelectVictim();

private:

};