#pragma once
#include "config.h"
#include <mutex>

using std::mutex;

//Buffer Control Blocks��������ƿ�
struct BCB {
    int page_id;
    int frame_id;
    mutex latch;//�ڴ���,�������Ƴ���ʹ��
    int count;//�����ʼ���
    bool dirty;//���־
    BCB* next;
    BCB() {

    };
};

class BMgr {
public:
    BMgr() {
        ftop = new int[DEFBUFSIZE];
        ptof = new BCB * [DEFBUFSIZE];
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
        delete[]ptof;
    };

protected:
    //ѡ��һ�������滻��frame�����dirty��Ч��д��disk
    int SelectVictim();
    //key:page_id��value:frame_id,H(K)=(page_id)%DEFBUFSIZE,��ͻ������������
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
    BCB** ptof;
};