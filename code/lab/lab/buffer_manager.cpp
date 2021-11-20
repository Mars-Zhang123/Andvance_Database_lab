#include "buffer_manager.h"

int BMgr::FixNewPage(bFrame *tmp) {
	LOG_DEBUG("execute BMgr::FixNewPage");
	int page_id = GetFreePageId();
	WritePage(page_id, tmp);
	SetUse(page_id, true);
	return page_id;
}

int BMgr::UnfixPage(int page_id) {
	LOG_DEBUG("execute BMgr::UnfixPage");
	int &count = GetBCB(page_id)->count;
	count ? count-- : 0/*latch解锁*/;
	
	return 0;
}

BCB* BMgr::GetBCB(int page_id) {
	LOG_DEBUG("execute BMgr::GetBCB");

	int idx = page_id % DEFBUFSIZE;
	BCB* ret = page2bcb[idx];

	while (ret && ret->page_id != page_id) { ret = ret->next; };

	return ret;
}

int BMgr::GetFrameId(int page_id) {
	LOG_DEBUG("execute BMgr::GetFrameId");
	BCB* bcb = GetBCB(page_id);
	//如果返回的bcb为null，返回-1代表该page不在buffer中
	return bcb ? GetBCB(page_id)->frame_id : -1;
}

void BMgr::SetDirty(int frame_id, bool isDirty) {
	LOG_DEBUG("execute BMgr::SetDirty");

	GetBCB(frame2page[frame_id])->dirty = isDirty;
}

void BMgr::WriteDirtys() {
	LOG_DEBUG("execute BMgr::WriteDirtys");

	for (int idx = 0; idx < DEFBUFSIZE; idx++) {
		BCB* ptr = page2bcb[idx];
		while (ptr) {
			if (ptr->dirty) {//该页是脏，需写回
				WritePage(ptr->page_id, buffer + ptr->frame_id);
				//清除脏位
				SetDirty(ptr->frame_id, false);
				ptr = ptr->next;
			}
		}
	}
}

int BMgr::GetFrameId(int page_id) {
	LOG_DEBUG("execute BMgr::GetFrameId");

	if (NumUseFrames == DEFBUFSIZE) { return -1; };

	for (int id = 0; id < DEFBUFSIZE; id++) {
		if (!isUse[id]) { return id; };
	}

	FAIL;
	return -1;
}

void BMgr::InsertBCB(BCB* bcb) {
	LOG_DEBUG("execute BMgr::InsertBCB");

	int page_id = bcb->page_id;
	int idx = page_id % DEFBUFSIZE;

	//头插法
	bcb->next = page2bcb[idx];
	page2bcb[idx] = bcb;
}