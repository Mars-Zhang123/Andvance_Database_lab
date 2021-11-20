#pragma once
#include "config.h"
#include "data_storage_manager.h"

using std::mutex;

class BMgr :protected DSMgr {
public:
    BMgr(string filename, bool create_file) :DSMgr() {
        NumUseFrames = 0;
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB*[DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
        isUse = new bool[DEFBUFSIZE]();
        OpenFile(filename, create_file);
    };

    ~BMgr() {
        delete[]frame2page;
        for (int idx = 0; idx < DEFBUFSIZE; idx++) {
            BCB *curPtr = page2bcb[idx], *prePtr;
            while (curPtr) {
                prePtr = curPtr;
                curPtr = curPtr->next;

                if (prePtr->dirty)
                    WritePage(prePtr->page_id, buffer + prePtr->frame_id);

                delete prePtr;
            }
        }
        delete[]page2bcb;
        delete[]buffer;
        delete[]isUse;
    };

    //虚函数，具体由派生类调度策略类实现
    //int FixPage(int page_id);

    //返回填充新page的page_id
    int FixNewPage(bFrame *tmp);

    //解除一次对page的count
    int UnfixPage(int page_id);

    //返回空闲可用的frames的总数
    int NumFreeFrames()const { return DEFBUFSIZE - NumUseFrames; };

    //返回一个空闲的frame的frame_id，没有则返回-1
    int GetFreeFrameId();

    //打印对应frame的内容
    void PrintFrame(int frame_id)const { printf("The %d-th frame is: %s\n", &frame_id, buffer + frame_id); };

protected:
    //key:page_id，value:BCB,H(K)=(page_id)%DEFBUFSIZE,冲突则查找溢出队列
    BCB* GetBCB(int page_id);

    //将bcb插入到静态hash中
    void InsertBCB(BCB* bcb);

    //调用GetBCB得到page_id对应BCB并返回frame_id
    int GetFrameId(int page_id);

    //删除指定BCB，由SelectVictim()调用
    void RemoveBCB(int frame_id);

    //设置该frame的dirty位
    void SetDirty(int frame_id, bool isDirty);

    //将所有脏frame写回disk
    void WriteDirtys();

    //返回对应帧的起始地址
    bFrame* GetFramePtr(int frame_id)const { return buffer + frame_id; };

    //设置对应帧的使用标志
    void SetUse(int frame_id, bool _isUse) { isUse[frame_id] = _isUse; };

private:
    //key:frame_id, value:page_id
    int* frame2page;

    //key:page_id, value:ptr(BCB)
    BCB** page2bcb;

    //缓冲区
    bFrame* buffer;

    //缓冲区使用标志
    bool* isUse;

    //已使用的frame数
    int NumUseFrames;
};