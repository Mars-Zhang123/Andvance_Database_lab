#pragma once
#include "config.h"
#include <mutex>
#include <unordered_map>

using std::string;
using std::mutex;
using std::unordered_map;

//Buffer Control Blocks��������ƿ�
struct BCB {
    int page_id;
    int frame_id;
    mutex latch;//�ڴ���,�������Ƴ���ʹ��
    int count;//�����ʼ���
    bool dirty;//���־
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
    //���һ�ζ�page��count
    int UnfixPage(int page_id);
    //���ؿ��п��õ�frames������
    int NumFreeFrames();
    //�ͷ�hash table
    ~BMgr() {
        delete[]ftop;
        delete[]bcbs;
        delete[]ptof;
    };

protected:
    //ѡ��һ�������滻��frame�����dirty��Ч��д��disk
    int SelectVictim();

    //key:page_id��value:frame_id,H(K)=(page_id)%BUF_SIZE,��ͻ������������
    int Hash(int page_id);

    //��BCB����ɾ��ָ��BCB����SelectVictim()����
    void RemoveBCB(BCB* ptr, int page_id);

    //ɾ��LRU�б���ɾ��frid��ӦԪ��
    void RemoveLRUEle(int frid);

    //��frame_id��Ӧ��frame������λ�����ҽ�����frame���޸�ʱ����
    void SetDirty(int frame_id);

    //
    void UnsetDirty(int frame_id);

    //�ر�ϵͳʱ���ã�����frameд��disk
    void WriteDirtys();

    //��ӡ��Ӧframe������
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