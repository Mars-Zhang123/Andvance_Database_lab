#include "two_queues.h"

void TwoQueues::Update2Q(int frame_id) {
	LOG_DEBUG("TwoQueues::Update2Q");

	BiNode* node = frameId2coldPtr[frame_id];
	if (!node) { //在hot表中,更新即可(将其移至表首)
		node = frameId2hotPtr[frame_id];
		if (!node) { FAIL; }
		node->pre->next = node->next;
		node->next->pre = node->pre;
		node->pre = &(hot.head);
		node->next = hot.head.next;
		node->next->pre = node;
		hot.head.next = node;
	} else {//在cold表中，在cold表删除节点，并且在hot表创立对应新节点
		//在cold删除
		node->pre->next = node->next;
		node->next->pre = node->pre;
		cold.head.frame_id--;
		frameId2coldPtr[node->frame_id] = nullptr;

		Insert2Q(node->frame_id, false);
		delete node;
	}
}

int TwoQueues::Insert2Q(int frame_id, bool willInsertCold) {
	LOG_DEBUG("TwoQueues::Insert2Q");
	
	BiList* _list;
	willDeleteColdFrame = willInsertCold;
	BiNode* node = new BiNode();
	node->frame_id = frame_id;

	if (willInsertCold) {//插入cold
		_list = &cold;
		while (_list->head.frame_id >= _list->tail.frame_id) {//缓冲区已满
			SelectVictim();//淘汰最久未使用的页
		}
		frameId2coldPtr[frame_id] = node;
	} else {//插入hot
		_list = &hot;
		while (_list->head.frame_id >= _list->tail.frame_id) {
			SelectVictim();
		}
		frameId2hotPtr[frame_id] = node;
	}

	//插入，对应长度+1
	_list->head.frame_id++;
	node->pre = &(_list->head);
	node->next = _list->head.next;
	node->next->pre = node;
	_list->head.next = node;

	return hot.head.frame_id + cold.head.frame_id;
}

int TwoQueues::SelectVictim() {
	LOG_DEBUG("TwoQueues::Insert2Q");

	BiNode* tail;
	BiNode* node;
	int frame_id;
	if (willDeleteColdFrame) {//删除冷页
		tail = &(cold.tail);
		//无需替换
		if (cold.head.frame_id < tail->frame_id) { return -1; }
		cold.head.frame_id--;
		node = tail->pre;
		frame_id = node->frame_id;
		frameId2coldPtr[frame_id] = nullptr;
	} else {//删除热页
		tail = &(hot.tail);
		if (hot.head.frame_id < tail->frame_id) { return -1; }
		hot.head.frame_id--;
		node = tail->pre;
		frame_id = node->frame_id;
		frameId2hotPtr[frame_id] = nullptr;
	}

	tail->pre = node->pre;
	node->pre->next = tail;
	if (!RemoveBCB(frame_id)) { FAIL; };
	delete node;
	
	return frame_id;
}

int TwoQueues::FillFrame(int page_id, bool willUpdating) {
	LOG_DEBUG("TwoQueues::FillFrame");

	int frame_id = GetFreeFrameId();
	if (frame_id == -1) {//不存在空闲frame
		willDeleteColdFrame = true;
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

	//第一次填充，所以应该插入冷到冷链
	Insert2Q(frame_id, true);

	return frame_id;
}

int TwoQueues::FixNewPage(bFrame* tmp) {
	LOG_DEBUG("TwoQueues::FixNewPage");

	willDeleteColdFrame = true;
	int page_id = BMgr::FixNewPage(tmp);
	int frame_id = GetFreeFrameId();
	if (frame_id == -1) {//不存在空闲frame
		frame_id = SelectVictim();
		if (frame_id == -1) { FAIL; };
	} else {//存在空闲frame
		;
	}

	memcpy(GetFramePtr(frame_id)->field, tmp->field, FRAMESIZE);
	BCB* bcb = new BCB();
	bcb->set(page_id, frame_id);
	InsertBCB(bcb);
	UpdateFrame2Page(frame_id, page_id);

	Insert2Q(frame_id, true);
	AddNumUsePages();

	return page_id;
}

int TwoQueues::ReadPageFromBMgr(int page_id, bFrame* reader) {
	LOG_DEBUG("TwoQueues::ReadPageFromBMgr");

	int frame_id = GetFrameId(page_id);
	if (frame_id == -1) {//未命中
		frame_id = FillFrame(page_id);
	} else {//命中
		Update2Q(frame_id);
		countHit++;
	}

	memcpy(reader, GetFramePtr(frame_id), sizeof(bFrame));

	return frame_id;
}

int TwoQueues::WritePageFromBMgr(int page_id, bFrame* writer) {
	LOG_DEBUG("TwoQueues::WritePageFromBMgr");

	int frame_id = GetFrameId(page_id);
	if (frame_id == -1) {//未命中
		frame_id = FillFrame(page_id, true);
	} else {//命中
		Update2Q(frame_id);
		countHit++;
	}


	bFrame* frame = GetFramePtr(frame_id);
	memcpy(frame->field, writer->field, sizeof(bFrame));

	//该页设为脏
	SetDirty(frame_id, true);

	return frame_id;
}