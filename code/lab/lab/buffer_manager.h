#pragma once
#include "config.h"
#include "data_storage_manager.h"


class BMgr :protected DSMgr {
public:
    BMgr(string filename, bool create_file) :DSMgr() {
        countHit = 0;
        LOG_DEBUG("enter BMgr");
        NumUseFrames = 0;
        frame2page = new int[DEFBUFSIZE];
        page2bcb = new BCB*[DEFBUFSIZE]();
        isUse = new bool[DEFBUFSIZE]();
        buffer = new bFrame[DEFBUFSIZE]();
        
        OpenFile(filename, create_file);
    };

    ~BMgr() {
        LOG_DEBUG("exit BMgr");
        delete[]frame2page;
        for (int idx = 0; idx < DEFBUFSIZE; idx++) {
            BCB *curPtr = page2bcb[idx], *prePtr;
            while (curPtr) {
                prePtr = curPtr;
                curPtr = curPtr->next;

                if (prePtr->dirty) {
                    WritePage(prePtr->page_id, GetFramePtr(prePtr->frame_id));
                }

                delete prePtr;
            }
        }
        delete[]page2bcb;
        delete[]buffer;
        delete[]isUse;
    };

    //���ؿ��п��õ�frames������
    int NumFreeFrames()const { return DEFBUFSIZE - NumUseFrames; };

    //����һ�����е�frame��frame_id��û���򷵻�-1
    int GetFreeFrameId();

    //��ӡ��Ӧframe������
    void PrintFrame(int frame_id)const { printf("The %d -th frame is: %s\n", frame_id, buffer[frame_id].field); };

    virtual void DeletePage(int page_id);

protected:
    //�������м���
    count_n countHit;

    //���������page��page_id
    virtual int FixNewPage(bFrame* tmp);

    //���һ�ζ�page��count
    int UnfixPage(int page_id);

    //��page����buffer��������frame_id,�ɵ��Ȳ��Ծ������ѡ������frame
    virtual int FillFrame(int page_id, bool willUpdating = false) = 0;

    //ѡ��һ���滻ҳ������������ĵ��Ȳ��Ծ������滻��ҳ
    virtual int SelectVictim() = 0;

    //��buffer�ж�ȡҳ����ȡ�ɹ����ض�Ӧ��frame_id�����frameδ������buffer������-1
    virtual int ReadPageFromBMgr(int page_id, bFrame* reader) = 0;

    //дҳ����ֱ����buffer��д��ɹ��򷵻�frame_id����frameδ���أ�����-1
    virtual int WritePageFromBMgr(int page_id, bFrame* writer) = 0;

    //key:page_id��value:BCB,H(K)=(page_id)%DEFBUFSIZE,��ͻ������������
    BCB* GetBCB(int page_id);

    //��bcb���뵽��̬hash��,ʱ�临�Ӷ�O(1)
    void InsertBCB(BCB* bcb);

    //ɾ��ָ��BCB����SelectVictim()����,ʱ�临�Ӷ�O(n),ɾ���ɹ�����true������false
    bool RemoveBCB(int frame_id, bool writeBack = true);

    //����GetBCB�õ�page_id��ӦBCB������frame_id�������ڸ�page����-1
    int GetFrameId(int page_id);

    //���ø�frame��dirtyλ
    void SetDirty(int frame_id, bool isDirty);

    //���ض�Ӧ֡����ʼ��ַ
    bFrame* GetFramePtr(int frame_id) { return buffer + frame_id; };

    //����frame2pageӳ��
    void UpdateFrame2Page(int fid, int pid) { frame2page[fid] = pid; };

    //
    int GetPageId(int frame_id)const { return frame2page[frame_id]; };

private:
    //��frameд��disk
    void WriteBack(int frame_id);

    //��������frameд��disk
    void WriteDirtys();

    //���ö�Ӧ֡��ʹ�ñ�־
    void SetUse(int frame_id, bool _isUse) { isUse[frame_id] = _isUse; };

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