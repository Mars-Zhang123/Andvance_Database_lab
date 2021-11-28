#pragma once
#include "config.h"
#include "logger.h"
#include <mutex>

using std::mutex;

#define HASLOWBITS(x, y) ((x) & ((1ULL << (y)) - 1))
#define CONDSHR(x, y) (HASLOWBITS(x, y) ? (x) : (x) >> (y))
#define CONDADD(x, y) (HASLOWBITS(x, y) ? 0 : (y))
#define CONDSHR32(x) CONDSHR(x, 32)
#define CONDSHR48(x) CONDSHR(CONDSHR32(x), 16)
#define CONDSHR56(x) CONDSHR(CONDSHR48(x), 8)
#define CONDSHR60(x) CONDSHR(CONDSHR56(x), 4)
#define CONDSHR62(x) CONDSHR(CONDSHR60(x), 2)
#define CONDADD32(x) CONDADD(x, 32)
#define CONDADD48(x) (CONDADD32(x) + CONDADD(CONDSHR32(x), 16))
#define CONDADD56(x) (CONDADD48(x) + CONDADD(CONDSHR48(x), 8))
#define CONDADD60(x) (CONDADD56(x) + CONDADD(CONDSHR56(x), 4))
#define CONDADD62(x) (CONDADD60(x) + CONDADD(CONDSHR60(x), 2))
#define CONDADD63(x) (CONDADD62(x) + CONDADD(CONDSHR62(x), 1))

// FIRSTSIGN(x) 得到最低位1出现的位置，返回值范围[0,64],64代表全为0,从csdn转载
#define FIRSTSIGN(x) (x ? CONDADD63((uint64_t)x) : 64)
/*原文链接：https ://blog.csdn.net/weixin_44327262/article/details/105903271*/

using std::mutex;

//定义帧结构
struct bFrame {
    char field[FRAMESIZE];
};

//Buffer Control Blocks，缓冲控制块
struct BCB {
    int page_id;
    int frame_id;
    mutex latch;//内存锁,并发控制程序使用
    int count;//被访问计数
    bool dirty;//脏标志
    BCB* next;//用于构造溢出链

    BCB() :page_id(-1), frame_id(-1), count(0), dirty(false), next(nullptr) {};

    void set(int page_id, int frame_id) {
        this->frame_id = frame_id;
        this->page_id = page_id;
    }
};

struct BiNode {
    int frame_id;
    BiNode* next;
    BiNode* pre;
    BiNode() :next(nullptr), pre(nullptr) {};
};

struct BiList {
    BiNode head;
    BiNode tail;
    BiList() {
        head.frame_id = 0;//head的frame_id记录当前双向链表节点数
        head.next = &tail;
        tail.pre = &head;
    }
    ~BiList() {
        LOG_DEBUG("exit BiList");
        BiNode* cur = head.next, * tmp;
        while (cur != &tail && cur) {
            tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
};


