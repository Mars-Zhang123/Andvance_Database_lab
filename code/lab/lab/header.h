#pragma once
#include "config.h"
#include <mutex>

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

// FIRSTSIGN(x) �õ����λ1���ֵ�λ�ã�����ֵ��Χ[0,64],64����ȫΪ0,��csdnת��
#define FIRSTSIGN(x) (x ? CONDADD63((uint64_t)x) : 64)
/*ԭ�����ӣ�https ://blog.csdn.net/weixin_44327262/article/details/105903271*/

using std::mutex;

//����֡�ṹ
struct bFrame {
    char field[FRAMESIZE];
};

//Buffer Control Blocks��������ƿ�
struct BCB {
    int page_id;
    int frame_id;
    mutex latch;//�ڴ���,�������Ƴ���ʹ��
    int count;//�����ʼ���
    bool dirty;//���־
    BCB* next;//���ڹ��������

    BCB() :page_id(-1), frame_id(-1), count(0), dirty(false), next(nullptr) {};

    void set(int page_id, int frame_id) {
        this->frame_id = frame_id;
        this->page_id = page_id;
    }
};

struct LRU_Node {
    int frame_id;
    LRU_Node* next;
    LRU_Node* pre;
    LRU_Node() :next(nullptr), pre(nullptr) {};
};

struct LRU_List {
    LRU_Node head;
    LRU_Node tail;
    LRU_List() {
        head.frame_id = 0;//head��frame_id��¼��ǰ˫������ڵ���
        head.next = &tail;
        tail.pre = &head;
    }
    ~LRU_List() {
        LRU_Node* cur = head.next, * tmp;
        while (cur != &tail) {
            tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
};


