#pragma once
#include "config.h"
#include <mutex>

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
};


