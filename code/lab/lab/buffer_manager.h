#pragma once
#include "config.h"
#include "data_storage_manager.h"

using std::mutex;

class BMgr :protected DSMgr {
public:
    BMgr(string filename, bool create_file) :DSMgr() {
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB * [DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
        OpenFile(filename, create_file);
    };

    ~BMgr() {
        delete[]frame2page;
        delete[]page2bcb;
        delete[]buffer;
    };

    //���������page��page_id
    int FixNewPage(bFrame *tmp);

    //���һ�ζ�page��count
    int UnfixPage(int page_id);

    //���ؿ��п��õ�frames������
    int NumFreeFrames();

    //��ӡ��Ӧframe������
    void PrintFrame(int frame_id);

protected:
    //key:page_id��value:BCB,H(K)=(page_id)%DEFBUFSIZE,��ͻ������������
    BCB* GetBCB(int page_id);

    //����GetBCB�õ�page_id��ӦBCB������frame_id
    int GetFrameId(int page_id);

    //��BCB����ɾ��ָ��BCB����SelectVictim()����
    void RemoveBCB(BCB* ptr, int page_id);

    //��frame_id��Ӧ��frame������λ�����ҽ�����frame���޸�ʱ����
    void SetDirty(int frame_id);

    //
    void UnsetDirty(int frame_id);

    //�ر�ϵͳ���û�ҳʱ���ã�����frameд��disk
    void WriteDirtys();

private:
    //key:frame_id, value:page_id
    int* frame2page;

    //key:page_id, value:ptr(BCB)
    BCB** page2bcb;

    //������
    bFrame* buffer;

};