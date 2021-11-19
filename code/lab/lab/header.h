#pragma once
#include "config.h"
#include <mutex>

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
};


