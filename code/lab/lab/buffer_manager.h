#pragma once
#include "config.h"
#include "data_storage_manager.h"

using std::mutex;

class BMgr :protected DSMgr {
public:
    BMgr(string filename, bool create_file) :DSMgr() {
        NumUseFrames = 0;
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB*[DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
        isUse = new bool[DEFBUFSIZE]();
        OpenFile(filename, create_file);
    };

    ~BMgr() {
        delete[]frame2page;
        for (int idx = 0; idx < DEFBUFSIZE; idx++) {
            BCB *curPtr = page2bcb[idx], *prePtr;
            while (curPtr) {
                prePtr = curPtr;
                curPtr = curPtr->next;

                if (prePtr->dirty)
                    WritePage(prePtr->page_id, buffer + prePtr->frame_id);

                delete prePtr;
            }
        }
        delete[]page2bcb;
        delete[]buffer;
        delete[]isUse;
    };

    //�麯������������������Ȳ�����ʵ��
    //int FixPage(int page_id);

    //���������page��page_id
    int FixNewPage(bFrame *tmp);

    //���һ�ζ�page��count
    int UnfixPage(int page_id);

    //���ؿ��п��õ�frames������
    int NumFreeFrames()const { return DEFBUFSIZE - NumUseFrames; };

    //����һ�����е�frame��frame_id��û���򷵻�-1
    int GetFreeFrameId();

    //��ӡ��Ӧframe������
    void PrintFrame(int frame_id)const { printf("The %d-th frame is: %s\n", &frame_id, buffer + frame_id); };

protected:
    //key:page_id��value:BCB,H(K)=(page_id)%DEFBUFSIZE,��ͻ������������
    BCB* GetBCB(int page_id);

    //��bcb���뵽��̬hash��
    void InsertBCB(BCB* bcb);

    //����GetBCB�õ�page_id��ӦBCB������frame_id
    int GetFrameId(int page_id);

    //ɾ��ָ��BCB����SelectVictim()����
    void RemoveBCB(int frame_id);

    //���ø�frame��dirtyλ
    void SetDirty(int frame_id, bool isDirty);

    //��������frameд��disk
    void WriteDirtys();

    //���ض�Ӧ֡����ʼ��ַ
    bFrame* GetFramePtr(int frame_id)const { return buffer + frame_id; };

    //���ö�Ӧ֡��ʹ�ñ�־
    void SetUse(int frame_id, bool _isUse) { isUse[frame_id] = _isUse; };

private:
    //key:frame_id, value:page_id
    int* frame2page;

    //key:page_id, value:ptr(BCB)
    BCB** page2bcb;

    //������
    bFrame* buffer;

    //������ʹ�ñ�־
    bool* isUse;

    //��ʹ�õ�frame��
    int NumUseFrames;
};