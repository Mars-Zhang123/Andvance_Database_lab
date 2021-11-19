#include "data_storage_manager.h"

const int DSMgr::NUM_PAGES_OF_BIT_MAP = sizeof(bit_map) * 8;
const int DSMgr::BIT_MAP_SIZE = MAXPAGES / NUM_PAGES_OF_BIT_MAP + 1;
const DSMgr::off_t  DSMgr::HEAD_BYTE_SIZE = 2 * sizeof(int) + sizeof(bit_map) * off_t(DSMgr::BIT_MAP_SIZE);

void DSMgr::OpenFile(string filename, bool create_file) {
	LOG_DEBUG("execute DSMgr::OpenFile");
	if (fopen_s(&currFile, (filename).c_str(), "wb+"))
		FAIL;
	if (create_file) {
		if (fwrite(&numAllocatePages, sizeof(int), 1, currFile) != 1)
			FAILARG("Error writing element of \"numAllocatePages\" in file");
		if(fwrite(&numUsePages, sizeof(int), 1, currFile) != 1)
			FAILARG("Error writing element of \"numUsePages\" in file");
		if (fwrite(useBit, sizeof(bit_map), BIT_MAP_SIZE, currFile) != BIT_MAP_SIZE)
			FAILARG("Error writing element of \"useBit\" in file");

		bFrame* tmp = new bFrame[numAllocatePages]();
		if (fwrite(&tmp, sizeof(bFrame), numAllocatePages, currFile) != numAllocatePages)
			FAILARG("Error allocating pages");
		delete[]tmp;
	} else {
		if (fread(&numAllocatePages, sizeof(int), 1, currFile) != 1)
			FAILARG("Error reading element of \"numAllocatePages\" in file");
		if (fread(&numUsePages, sizeof(int), 1, currFile) != 1)
			FAILARG("Error reading element of \"numUsePages\" in file");
		if (fread(useBit, sizeof(bit_map), BIT_MAP_SIZE, currFile) != BIT_MAP_SIZE)
			FAILARG("Error reading element of \"useBit\" in file");
	}
}

void DSMgr::CloseFile() {
	LOG_DEBUG("execute DSMgr::CloseFile");

	//将首部写回文件
	if (fwrite(&numAllocatePages, sizeof(int), 1, currFile) != 1)
		FAILARG("Error writing element of \"numAllocatePages\" in file");
	if (fwrite(&numUsePages, sizeof(int), 1, currFile) != 1)
		FAILARG("Error writing element of \"numUsePages\" in file");
	if (fwrite(useBit, sizeof(bit_map), BIT_MAP_SIZE, currFile) != BIT_MAP_SIZE)
		FAILARG("Error writing element of \"useBit\" in file");

	if (fclose(currFile))
		FAIL;
}

void DSMgr::Seek(off_t offset) {
	LOG_DEBUG("execute DSMgr::Seek");
	if (_fseeki64(currFile, offset, SEEK_SET) || ferror(currFile))
		FAIL;
}

void DSMgr::addAllocatePages() {
	LOG_DEBUG("execute DSMgr::addAllcatePages");

	Seek(HEAD_BYTE_SIZE + numAllocatePages * off_t(FRAMESIZE));
	int addSize = NUM_PAGES_OF_BIT_MAP * 4;
	numAllocatePages += addSize;
	
	bFrame* tmp = new bFrame[addSize]();
	if (fwrite(&tmp, sizeof(bFrame), addSize, currFile) != addSize)
		FAILARG("Error adding allocated pages");
	delete[]tmp;
}

void DSMgr::SetUse(int page_id, bool isUse) {
	LOG_DEBUG("execute DSMgr::SetUse");
	if (page_id >= numAllocatePages) {
		FAILARG("Prohibit operation on unassigned pages");
	}

	bit_map& use_bit = useBit[page_id / NUM_PAGES_OF_BIT_MAP];
	if (isUse) {//定义为已使用
		use_bit |= (1ULL << (page_id % NUM_PAGES_OF_BIT_MAP));
	} else { //定义为未使用
		use_bit &= ~(1ULL << (page_id % NUM_PAGES_OF_BIT_MAP));
	}
}

bool DSMgr::GetUse(int page_id) const {
	LOG_DEBUG("execute DSMgr::GetUse");

	bit_map& use_bit = useBit[page_id / NUM_PAGES_OF_BIT_MAP];
	if (use_bit & (1ULL << (page_id % NUM_PAGES_OF_BIT_MAP))) {return true;}
	return false;
}

bFrame DSMgr::ReadPage(int page_id) {
	LOG_DEBUG("execute DSMgr::ReadPage");
	if (!GetUse(page_id)) {
		FAILARG("Prohibit reading unassigned pages");
	}

	off_t off_set = HEAD_BYTE_SIZE + page_id * off_t(FRAMESIZE);
	Seek(off_set);
	bFrame ret;
	if (fread(&ret, sizeof(bFrame), 1, currFile) != 1)
		FAILARG("Error reading the page in file");
	return ret;
}

void DSMgr::WritePage(int page_id, bFrame* frm) {
	LOG_DEBUG("execute DSMgr::WritePage");
	if (page_id >= MAXPAGES) {
		FAILARG("The page visited exceeds the maximum file size");
	}

	//判断该页是否分配，若未分配，则分配
	while (page_id >= numAllocatePages) {addAllocatePages();}

	off_t off_set = HEAD_BYTE_SIZE + page_id * off_t(FRAMESIZE);
	Seek(off_set);
	if (fwrite(frm, sizeof(bFrame), 1, currFile) != 1)
		FAILARG("Error writing the page in file");
	SetUse(page_id, true);
}

void DSMgr::DeletePage(int page_id) {
	LOG_DEBUG("execute DSMgr::DeletePage");
	//标记为未使用，逻辑上删除，并非物理上删除
	SetUse(page_id, false);
}