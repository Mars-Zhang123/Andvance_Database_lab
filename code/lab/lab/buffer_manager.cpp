#include "buffer_manager.h"

int BMgr::FixNewPage(bFrame *tmp) {
	LOG_DEBUG("execute BMgr::FixNewPage");
	int page_id = GetFreePageId();
	WritePage(page_id, tmp);
	return page_id;
}

int BMgr::UnfixPage(int page_id) {
	LOG_DEBUG("execute BMgr::UnfixPage");
	int &count = GetBCB(page_id)->count;
	count ? count-- : 0/*latch����*/;
	
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
	//������ص�bcbΪnull������-1�����page����buffer��
	return bcb ? bcb->frame_id : -1;
}

void BMgr::SetDirty(int frame_id, bool isDirty) {
	LOG_DEBUG("execute BMgr::SetDirty");

	GetBCB(frame2page[frame_id])->dirty = isDirty;
}

void BMgr::WriteBack(int frame_id) {
	LOG_DEBUG("execute BMgr::WriteBack");

	int page_id = frame2page[frame_id];
	BCB* bcb = GetBCB(page_id);
	if (bcb->dirty) {
		WritePage(page_id, GetFramePtr(frame_id));
		SetDirty(frame_id, false);
	}
}

void BMgr::WriteDirtys() {
	LOG_DEBUG("execute BMgr::WriteDirtys");

	for (int idx = 0; idx < DEFBUFSIZE; idx++) {
		BCB* ptr = page2bcb[idx];
		while (ptr) {
			if (ptr->dirty) {//��ҳ���࣬��д��
				WritePage(ptr->page_id, GetFramePtr(ptr->frame_id));
				//�����λ
				SetDirty(ptr->frame_id, false);
				ptr = ptr->next;
			}
		}
	}
}

int BMgr::GetFreeFrameId() {
	LOG_DEBUG("execute BMgr::GetFreeFrameId");

	if (NumUseFrames == DEFBUFSIZE) { return -1; };

	for (int id = 0; id < DEFBUFSIZE; id++) {
		if (!isUse[id]) { return id; };
	}

	return -1;
}

void BMgr::InsertBCB(BCB* bcb) {
	LOG_DEBUG("execute BMgr::InsertBCB");

	int page_id = bcb->page_id;
	int idx = page_id % DEFBUFSIZE;

	//ͷ�巨
	bcb->next = page2bcb[idx];
	page2bcb[idx] = bcb;

	SetUse(bcb->frame_id, true);
	NumUseFrames++;
}

bool BMgr::RemoveBCB(int frame_id, bool writeBack) {
	LOG_DEBUG("execute BMgr::RemoveBCB");

	int idx = frame2page[frame_id] % DEFBUFSIZE;

	BCB* cur = page2bcb[idx], * pre = cur;
	while (cur) {
		if (cur->frame_id == frame_id) {
			if (pre == cur) {//��ɾ����BCB������
				page2bcb[idx] = cur->next;
			} else {//��������
				pre->next = cur->next;
			}
			if (cur->dirty && writeBack) {//��������Ҫд��
				WritePage(cur->page_id, GetFramePtr(frame_id));
			}
			delete cur;
			//��frame�ı��Ϊδʹ��
			SetUse(frame_id, false);
			frame2page[frame_id] = -1;
			NumUseFrames--;
			return true;
		}
		pre = cur;
		cur = cur->next;
	}

	return false;
}

void BMgr::DeletePage(int page_id) {
	LOG_DEBUG("execute BMgr::DeletePage");

	int frame_id = GetFrameId(page_id);
	if (frame_id != -1) {//��ҳ��buffer��
		RemoveBCB(frame_id, false);
	}

	DSMgr::DeletePage(page_id);
}