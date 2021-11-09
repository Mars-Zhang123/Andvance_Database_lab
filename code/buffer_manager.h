#pragma once
#include "config.h"
#include <mutex>
using namespace std;

//Buffer Control Blocks，缓冲控制块
struct BCB {
    int page_id;
    int frame_id;
    mutex latch;//内存锁
    int count;//被访问计数
    bool dirty;//脏标志
    BCB * next;
    BCB() {
        
    };
};

class BMgr {
public:
    BMgr();
    int FixPage(int page_id, int prot);
    void FixNewPage();
    //解除一次对page的count
    int UnfixPage(int page_id);
    //返回空闲可用的frames的总数
    int NumFreeFrames();
    
    //选择一个将被替换的frame，如果dirty生效，写回disk
    int SelectVictim();
    //key:page_id，value:frame_id
    int Hash(int page_id);
    //从BCB队列删除指定BCB，由SelectVictim()调用
    void RemoveBCB(BCB * ptr, int page_id);
    //删除LRU列表中删除frid对应元素
    void RemoveLRUEle(int frid);
    //将frame_id对应的frame设置脏位，当且仅当该frame被修改时调用
    void SetDirty(int frame_id);
    //
    void UnsetDirty(int frame_id);
    //关闭系统时调用，将脏frame写回disk
    void WriteDirtys();
    //打印对应frame的内容
    void PrintFrame(int frame_id);
private:
    // Hash Table
    int ftop[DEFBUFSIZE];
    BCB* ptof[DEFBUFSIZE];
};