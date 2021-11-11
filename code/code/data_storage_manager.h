#pragma once
#include <string>
#include "config.h"
using namespace std;

//����֡�ṹ
struct bFrame {
    char* field;
    bFrame() {
        field = new char[FRAME_SIZE]();
    }
    ~bFrame() {
        delete[] field;
    }
};

//�������ݴ洢������
class DSMgr {
public:
    DSMgr() {
        pages = new int[MAX_PAGES];
    };

    //��ָ���ļ������ļ�
    int OpenFile(string filename);

    //�ر������ļ������ҽ��������޸Ļ����رյ���
    int CloseFile();

    //��page_id��Ӧ��ҳ��ȡ���ݣ��ɻ����������FixPage��������
    bFrame ReadPage(int page_id);//bytes������

    //��frame�ӻ�����ȡ��ʱ���ã����ر�д���ֽ����������ݱ������ļ�
    int WritePage(int frame_id, bFrame frm);

    //�ƶ��ļ�ָ��
    int Seek(int offset, int pos);

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

    //����page��ʹ�����
    int* pages;
};