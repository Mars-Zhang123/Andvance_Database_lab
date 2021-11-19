#include "buffer_manager.h"

int BMgr::FixNewPage(bFrame *tmp) {
	LOG_DEBUG("execute BMgr::FixNewPage");
	//ø…”≈ªØ...
	for (int page_id = 0; page_id < MAXPAGES; page_id++) {
		if (!GetUse(page_id)) {
			WritePage(page_id, tmp);
			return page_id;
		}
	}
	return -1;
}

int BMgr::UnfixPage(int page_id) {

}