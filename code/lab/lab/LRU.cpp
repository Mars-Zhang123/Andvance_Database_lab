#include "LRU.h"

int LRU::FixPage(int page_id) {
	LOG_DEBUG("execute LRU::FixPage");

	bFrame content = ReadPage(page_id);
	int frame_id = GetFreeFrameId();
	if (frame_id != -1) {//´æÔÚ¿ÕÏĞframe
		memcpy(GetFramePtr(frame_id), &content, FRAMESIZE);
		SetUse(frame_id, true);
		BCB* bcb = new BCB();
		bcb->set(page_id, frame_id);
		InsertBCB(bcb);
	} else {//²»´æÔÚ¿ÕÏĞframe
		int frame_id = SelectVictim();
	}
}