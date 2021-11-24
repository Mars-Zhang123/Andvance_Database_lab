#include "LRU.h"

int LRU::FixNewPage(bFrame* tmp) {//д��ҳ�����뻺��
	LOG_DEBUG("execute LRU::FixNewPage");

	int page_id = BMgr::FixNewPage(tmp);
	int frame_id = GetFreeFrameId();
	if (frame_id == -1) {//�����ڿ���frame
		frame_id = SelectVictim();
		if (frame_id == -1) { FAIL; };
	}
	else {//���ڿ���frame
		;
	}

	memcpy(GetFramePtr(frame_id)->field, tmp->field, FRAMESIZE);
	BCB* bcb = new BCB();
	bcb->set(page_id, frame_id);
	InsertBCB(bcb);
	UpdateFrame2Page(frame_id, page_id);

	InsertLRU(frame_id);
	AddNumUsePages();

	return page_id;
}

int LRU::FillFrame(int page_id, bool willUpdating) {
	LOG_DEBUG("execute LRU::FillFrame");

	int frame_id = GetFreeFrameId();
	if (frame_id == -1) {//�����ڿ���frame
		frame_id = SelectVictim();
		if (frame_id == -1) { FAIL; };
	} else {//���ڿ���frame
		;
	}

	if (!willUpdating) {//�����һ�����������£�Ӧ�ý���һ�δ���IO�������������϶�ȡdisk
		bFrame content = ReadPageFromDSMgr(page_id);
		memcpy(GetFramePtr(frame_id)->field, content.field, FRAMESIZE);
	}
	
	BCB* bcb = new BCB();
	bcb->set(page_id, frame_id);
	InsertBCB(bcb);
	UpdateFrame2Page(frame_id, page_id);

	InsertLRU(frame_id);

	return frame_id;
}

int LRU::InsertLRU(int frame_id) {
	LOG_DEBUG("execute LRU::InsertLRU");

	//head��frame_idԪ��������д����ʾ˫������ĵ�ǰ����
	while (BiList.head.frame_id >= capacity) {//����������
		SelectVictim();//��̭���δʹ�õ�ҳ
	}

	//����һ��Ԫ�أ���Ӧ����+1
	BiList.head.frame_id++;
	LRU_Node* tmp = new LRU_Node();
	tmp->frame_id = frame_id;
	tmp->pre = &(BiList.head);
	tmp->next = BiList.head.next;
	tmp->next->pre = tmp;
	BiList.head.next = tmp;
	frameId2nodePtr[frame_id] = tmp;

	return BiList.head.frame_id;
}

int LRU::SelectVictim() {
	LOG_DEBUG("execute LRU::SelectVictim");

	if (BiList.head.frame_id < capacity) {//�����滻��buffer����ʣ��ռ�
		return -1;
	}

	//ɾ��һ��Ԫ�أ���Ӧ����-1
	BiList.head.frame_id--;
	LRU_Node* tmp = BiList.tail.pre;
	int frame_id = tmp->frame_id;
	BiList.tail.pre = tmp->pre;
	tmp->pre->next = &(BiList.tail);
	if (!RemoveBCB(frame_id)) { FAIL; };
	delete tmp;
	frameId2nodePtr[frame_id] = nullptr;
	return frame_id;
}

void LRU::UpdateLRU(int frame_id) {
	LOG_DEBUG("execute LRU::UpdateLRU");

	LRU_Node* tmp = frameId2nodePtr[frame_id];
	if (!tmp) { FAIL; };
	tmp->pre->next = tmp->next;
	tmp->next->pre = tmp->pre;
	tmp->pre = &(BiList.head);
	tmp->next = BiList.head.next;
	tmp->next->pre = tmp;
	BiList.head.next = tmp;
}

int LRU::ReadPageFromBMgr(int page_id, bFrame* reader) {
	LOG_DEBUG("execute LRU::ReadPageFromBMgr");

	int frame_id = GetFrameId(page_id);
	if (frame_id == -1) {//δ����
		frame_id = FillFrame(page_id);
	} else {//����
		UpdateLRU(frame_id);
		countHit++;
	}

	memcpy(reader, GetFramePtr(frame_id), sizeof(bFrame));

	return frame_id;
}

int LRU::WritePageFromBMgr(int page_id, bFrame* writer) {
	LOG_DEBUG("execute LRU::WritePageFromBMgr");

	int frame_id = GetFrameId(page_id);
	if (frame_id == -1) {//δ����
		frame_id = FillFrame(page_id, true);
	} else {//����
		UpdateLRU(frame_id);
		countHit++;
	}

	
	bFrame* frame = GetFramePtr(frame_id);
	memcpy(frame->field, writer->field, sizeof(bFrame));

	//��ҳ��Ϊ��
	SetDirty(frame_id, true);

	return frame_id;
}