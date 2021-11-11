#pragma once
#include "config.h"
#include <mutex>
#include <unordered_map>

using std::string;
using std::mutex;
using std::unordered_map;

//Buffer Control Blocks，缓冲控制块
struct BCB {
    int page_id;
    int frame_id;
    mutex latch;//内存锁,并发控制程序使用
    int count;//被访问计数
    bool dirty;//脏标志
    BCB* next;
    BCB* pre;
    BCB() {
        dirty = true;
        page_id = frame_id = -1;
        count = 0;
        next = pre = nullptr;
    };
};

class BMgr {
public:
    BMgr() {
        ftop = new int[BUF_SIZE]();
        bcbs = new BCB * [BUF_SIZE]();
        ptof = new int[BUF_SIZE]();
    };
    //
    int FixPage(int page_id, int prot);
    //
    void FixNewPage();
    //解除一次对page的count
    int UnfixPage(int page_id);
    //返回空闲可用的frames的总数
    int NumFreeFrames();
    //释放hash table
    ~BMgr() {
        delete[]ftop;
        delete[]bcbs;
        delete[]ptof;
    };

protected:
    //选择一个将被替换的frame，如果dirty生效，写回disk
    int SelectVictim();

    //key:page_id，value:frame_id,H(K)=(page_id)%BUF_SIZE,冲突则查找溢出队列
    int Hash(int page_id);

    //从BCB队列删除指定BCB，由SelectVictim()调用
    void RemoveBCB(BCB* ptr, int page_id);

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
    //key:frame_id, value:page_id
    int* ftop;

    //key:page_id, value:ptr(BCB)
    int* ptof;

    //
    BCB** bcbs;
};