#pragma once
#include "config.h"
#include "data_storage_manager.h"

using std::mutex;

class BMgr :protected DSMgr {
public:
    BMgr(string filename, bool create_file) :DSMgr() {
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB * [DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
        OpenFile(filename, create_file);
    };

    ~BMgr() {
        delete[]frame2page;
        delete[]page2bcb;
        delete[]buffer;
    };

    //返回填充新page的page_id
    int FixNewPage(bFrame *tmp);

    //解除一次对page的count
    int UnfixPage(int page_id);

    //返回空闲可用的frames的总数
    int NumFreeFrames();

    //打印对应frame的内容
    void PrintFrame(int frame_id);

protected:
    //key:page_id，value:BCB,H(K)=(page_id)%DEFBUFSIZE,冲突则查找溢出队列
    BCB* GetBCB(int page_id);

    //调用GetBCB得到page_id对应BCB并返回frame_id
    int GetFrameId(int page_id);

    //从BCB队列删除指定BCB，由SelectVictim()调用
    void RemoveBCB(BCB* ptr, int page_id);

    //将frame_id对应的frame设置脏位，当且仅当该frame被修改时调用
    void SetDirty(int frame_id);

    //
    void UnsetDirty(int frame_id);

    //关闭系统或置换页时调用，将脏frame写回disk
    void WriteDirtys();

private:
    //key:frame_id, value:page_id
    int* frame2page;

    //key:page_id, value:ptr(BCB)
    BCB** page2bcb;

    //缓冲区
    bFrame* buffer;

};