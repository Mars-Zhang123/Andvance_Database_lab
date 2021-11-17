#pragma once
#include "config.h"
#include "header.h"
#include "logger.h"



//�������ݴ洢������
class DSMgr {
public:
    typedef long long off_t;

    DSMgr() {
        pages = new off_t[MAXPAGES];
        currFile = nullptr;
        useBit = 0;
    };

    ~DSMgr() {
        delete[]pages;
    };

    //��ָ���ļ������ļ�
    void OpenFile(string filename);

    //�ر������ļ�
    void CloseFile();

    //��page_id��Ӧ��ҳ��ȡ���ݣ��ɻ����������FixPage��������
    bFrame ReadPage(int page_id);//bytes������

    //��frame�ӻ�����ȡ��ʱ���ã����ر�д���ֽ����������ݱ������ļ�
    int WritePage(int frame_id, bFrame frm);

    //�ƶ��ļ�ָ��
    void Seek(off_t offset);
    
    //���ص�ǰ�ļ�ָ��
    FILE* GetFile();
    
    //����ҳ������+1
    void IncNumPages();
    
    //����ҳ����
    int GetNumPages();
    
    //����page_id��use_bit
    void SetUse(int page_id, int use_bit);
    
    //����page_id��Ӧ��Ӧpage��use_bit
    int GetUse(int page_id);

private:
    //��ǰ�ļ���ָ��
    FILE* currFile;
    
    //��ǰҳ����
    int numPages;

    //��¼pa'ge
    off_t* pages;

    //�ļ�ʹ��λͼ
    size_t useBit;
};