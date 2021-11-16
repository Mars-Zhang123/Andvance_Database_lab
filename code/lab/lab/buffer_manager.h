#pragma once
#include "config.h"
#include "data_storage_manager.h"
#include "header.h"

using std::mutex;

class BMgr {
public:
    BMgr(DSMgr* _dms_gr) :dms_gr(_dms_gr) {
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB * [DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
    };

    ~BMgr() {
        delete[]frame2page;
        delete[]page2bcb;
        delete[]buffer;
    };

    //
    int FixPage(int page_id, int prot);

    //
    void FixNewPage();

    //���һ�ζ�page��count
    int UnfixPage(int page_id);

    //���ؿ��п��õ�frames������
    int NumFreeFrames();

protected:
    //ѡ��һ�������滻��frame�����dirty��Ч��д��disk
    int SelectVictim();

    //key:page_id��value:BCB,H(K)=(page_id)%DEFBUFSIZE,��ͻ������������
    BCB* GetBCB(int page_id);
    
    //����GetBCB�õ�page_id��ӦBCB������frame_id
    int GetFrameId(int page_id);

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
    //key:frame_id, value:page_id
    int* frame2page;
    
    //key:page_id, value:ptr(BCB)
    BCB** page2bcb;

    //���ݴ洢������
    DSMgr* dms_gr;

    //������
    bFrame *buffer;
};