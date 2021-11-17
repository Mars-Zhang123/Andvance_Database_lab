#include "data_storage_manager.h"

void DSMgr::OpenFile(string filename) {
	LOG_DEBUG("execute DSMgr::OpenFile");
	if (fopen_s(&currFile, (filename).c_str(), "wb+"))
		FAIL;
}

void DSMgr::CloseFile() {
	LOG_DEBUG("execute DSMgr::CloseFile");
	if (fclose(currFile))
		FAIL;
}

void DSMgr::Seek(off_t offset) {
	LOG_DEBUG("execute DSMgr::Seek");
	if (_fseeki64(currFile, offset, SEEK_SET) || ferror(currFile))
		FAIL;
}

bFrame DSMgr::ReadPage(int page_id) {

}