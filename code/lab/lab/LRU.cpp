#include "LRU.h"

int LRU::FixNewPage(bFrame* tmp) {//写新页并加入缓存
	LOG_DEBUG("execute LRU::FixNewPage");

	int page_id = BMgr::FixNewPage(tmp);
	int frame_id = GetFreeFrameId();
	if (frame_id == -1) {//不存在空闲frame
		frame_id = SelectVictim();
		if (frame_id == -1) { FAIL; };
	}
	else {//存在空闲frame
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
	if (frame_id == -1) {//不存在空闲frame
		frame_id = SelectVictim();
		if (frame_id == -1) { FAIL; };
	} else {//存在空闲frame
		;
	}

	if (!willUpdating) {//如果下一步操作不更新，应该进行一次磁盘IO，即真正意义上读取disk
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

	//head的frame_id元素意义重写，表示双向链表的当前长度
	while (BiList.head.frame_id >= capacity) {//缓冲区已满
		SelectVictim();//淘汰最久未使用的页
	}

	//插入一个元素，对应长度+1
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

	if (BiList.head.frame_id < capacity) {//无需替换，buffer存在剩余空间
		return -1;
	}

	//删除一个元素，对应长度-1
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
	if (frame_id == -1) {//未命中
		frame_id = FillFrame(page_id);
	} else {//命中
		UpdateLRU(frame_id);
		countHit++;
	}

	memcpy(reader, GetFramePtr(frame_id), sizeof(bFrame));

	return frame_id;
}

int LRU::WritePageFromBMgr(int page_id, bFrame* writer) {
	LOG_DEBUG("execute LRU::WritePageFromBMgr");

	int frame_id = GetFrameId(page_id);
	if (frame_id == -1) {//未命中
		frame_id = FillFrame(page_id, true);
	} else {//命中
		UpdateLRU(frame_id);
		countHit++;
	}

	
	bFrame* frame = GetFramePtr(frame_id);
	memcpy(frame->field, writer->field, sizeof(bFrame));

	//该页设为脏
	SetDirty(frame_id, true);

	return frame_id;
}