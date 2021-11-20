#pragma once
#include "config.h"
#include "header.h"
#include "logger.h"



//�������ݴ洢������
class DSMgr {
public:
    //�ڸ��������¶���off_t(������Ĭ�ϵ�off_tΪint����)
    typedef long long off_t;
    //һ��bitmap�ܼ�¼64��pageʹ�����
    typedef unsigned long long bit_map;

    DSMgr() {
        useBit = new bit_map[BIT_MAP_SIZE]();
        currFile = nullptr;
        numUsePages = 0;
        //Ĭ�ϳ�ʼԤ����16��bit_map��¼��page��
        numAllocatePages = NUM_PAGES_OF_BIT_MAP * 16;
    };

    ~DSMgr() {
        delete[]useBit;
        CloseFile();
    };

    //��ָ���ļ������ļ�
    void OpenFile(string filename, bool create_file);

    //�ر������ļ�
    void CloseFile();

    //��page_id��Ӧ��ҳ��ȡ���ݣ��ɻ����������FixPage��������
    bFrame ReadPage(int page_id);

    //��frame�ӻ�����ȡ��ʱ���ã������ݱ������ļ�
    void WritePage(int page_id, bFrame *frm);

    //���ص�ǰ�ļ�ָ��
    FILE* GetFile() { return currFile; };

    //��ʹ��ҳ����+1
    void AddNumUsePages() { numUsePages++; };
    
    //������ʹ��ҳ����
    int GetNumUsePages()const { return numUsePages; };

    //���ط����ҳ����
    int GetNumAllocatedPages()const { return numAllocatePages; };

    //����page_id��Ӧ��Ӧpage��use_bit
    bool GetUse(int page_id)const;

    //�߼�����ɾ��ָ����ҳ
    void DeletePage(int page_id);

    //�����ļ���ʹ�洢��������������������ɾ������ռ䣨��64*2Ԥ��ҳ������Ϊ��ʱ������Ŀδʵ�ָú���
    void TidyAndClean() {};

    //�õ�һ����Ϊ��ǰ��δʹ�õ�page_id���ú������ܸı�numAllocatedPagesֵ
    int GetFreePageId();

protected:
    //�ƶ��ļ�ָ��
    void Seek(off_t offset);

    //�����ļ�����ҳ,ÿ������4*sizeof(bit_map)ҳ
    void AddAllocatePages();

    //����page_id��use_bit,true:��ʹ��,false:δʹ��
    void SetUse(int page_id, bool isUse);

private:
    //��ǰ�ļ���ָ��
    FILE* currFile;
    
    //��ǰ������ҳ����
    int numAllocatePages;

    //��ǰ��ʹ��ҳ����
    int numUsePages;

    //�ļ�ʹ��λͼ
    bit_map *useBit;

    //��¼�ļ��ײ�ռ���ֽ���
    const static off_t HEAD_BYTE_SIZE;

    //һ��bit_map��¼��Ӧ��page��
    const static int NUM_PAGES_OF_BIT_MAP;
    
    //λͼ����
    const static int BIT_MAP_SIZE;

};