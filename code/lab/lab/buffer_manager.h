#pragma once
#include "config.h"
#include "data_storage_manager.h"


class BMgr :protected DSMgr {
public:
    BMgr(string filename, bool create_file) :DSMgr() {
        countHit = 0;
        LOG_DEBUG("enter BMgr");
        NumUseFrames = 0;
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB*[DEFBUFSIZE]();
        isUse = new bool[DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
        
        OpenFile(filename, create_file);
    };

    ~BMgr() {
        LOG_DEBUG("exit BMgr");
        delete[]frame2page;
        for (int idx = 0; idx < DEFBUFSIZE; idx++) {
            BCB *curPtr = page2bcb[idx], *prePtr;
            while (curPtr) {
                prePtr = curPtr;
                curPtr = curPtr->next;

                if (prePtr->dirty) {
                    WritePage(prePtr->page_id, GetFramePtr(prePtr->frame_id));
                }

                delete prePtr;
            }
        }
        delete[]page2bcb;
        delete[]buffer;
        delete[]isUse;
    };

    //返回空闲可用的frames的总数
    int NumFreeFrames()const { return DEFBUFSIZE - NumUseFrames; };

    //返回一个空闲的frame的frame_id，没有则返回-1
    int GetFreeFrameId();

    //打印对应frame的内容
    void PrintFrame(int frame_id)const { printf("The %d -th frame is: %s\n", frame_id, buffer[frame_id].field); };

    virtual void DeletePage(int page_id);

protected:
    //缓存命中计数
    count_n countHit;

    //返回填充新page的page_id
    virtual int FixNewPage(bFrame* tmp);

    //解除一次对page的count
    int UnfixPage(int page_id);

    //将page读入buffer，并返回frame_id,由调度策略决定如何选择填充的frame
    virtual int FillFrame(int page_id, bool willUpdating = false) = 0;

    //选择一个替换页，根据派生类的调度策略决定所替换的页
    virtual int SelectVictim() = 0;

    //从buffer中读取页，读取成功返回对应的frame_id，如果frame未加载至buffer，返回-1
    virtual int ReadPageFromBMgr(int page_id, bFrame* reader) = 0;

    //写页，若直接在buffer中写入成功则返回frame_id，若frame未加载，返回-1
    virtual int WritePageFromBMgr(int page_id, bFrame* writer) = 0;

    //key:page_id，value:BCB,H(K)=(page_id)%DEFBUFSIZE,冲突则查找溢出队列
    BCB* GetBCB(int page_id);

    //将bcb插入到静态hash中,时间复杂度O(1)
    void InsertBCB(BCB* bcb);

    //删除指定BCB，由SelectVictim()调用,时间复杂度O(n),删除成功返回true，否则false
    bool RemoveBCB(int frame_id, bool writeBack = true);

    //调用GetBCB得到page_id对应BCB并返回frame_id，不存在该page返回-1
    int GetFrameId(int page_id);

    //设置该frame的dirty位
    void SetDirty(int frame_id, bool isDirty);

    //返回对应帧的起始地址
    bFrame* GetFramePtr(int frame_id) { return buffer + frame_id; };

    //更新frame2page映射
    void UpdateFrame2Page(int fid, int pid) { frame2page[fid] = pid; };

    //
    int GetPageId(int frame_id)const { return frame2page[frame_id]; };

private:
    //将frame写回disk
    void WriteBack(int frame_id);

    //将所有脏frame写回disk
    void WriteDirtys();

    //设置对应帧的使用标志
    void SetUse(int frame_id, bool _isUse) { isUse[frame_id] = _isUse; };

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