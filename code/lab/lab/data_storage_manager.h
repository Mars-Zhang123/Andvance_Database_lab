#pragma once
#include "config.h"
#include "header.h"
#include "logger.h"



//定义数据存储管理器
class DSMgr {
public:
    //在该类中重新定义off_t(编译器默认的off_t为int类型)
    typedef long long off_t;
    //一个bitmap能记录64个page使用情况
    typedef unsigned long long bit_map;

    DSMgr() {
        useBit = new bit_map[BIT_MAP_SZIE]();
        currFile = nullptr;
        numUsePages = 0;
        //默认初始预分配16个bit_map记录的page数
        numAllocatePages = NUM_PAGES_OF_BIT_MAP * 16;
    };

    ~DSMgr() {
        delete[]useBit;
        CloseFile();
    };

    //打开指定文件名的文件
    void OpenFile(string filename, bool create_file);

    //关闭数据文件
    void CloseFile();

    //从page_id对应的页读取数据，由缓冲管理器的FixPage函数调用
    bFrame ReadPage(int page_id);//bytes？？？

    //当frame从缓冲区取出时调用，返回编写的字节数，将数据保存至文件
    void WritePage(int page_id, bFrame *frm);

    //返回当前文件指针
    FILE* GetFile() { return currFile; };
    
    //返回已使用页面数
    int GetNumUsePages()const { return numUsePages; };

    //返回page_id对应对应page的use_bit
    bool GetUse(int page_id)const;

protected:
    //移动文件指针
    void Seek(off_t offset);

    //增加文件分配页,每次增加4*sizeof(bit_map)页
    void addAllocatePages();

    //设置page_id的use_bit
    void SetUse(int page_id, bool isUse);

private:
    //当前文件的指针
    FILE* currFile;
    
    //当前已申请页面数
    int numAllocatePages;

    //当前已使用页面数
    int numUsePages;

    //文件使用位图
    bit_map *useBit;

    //记录文件首部占用字节数
    const static off_t HEAD_BYTE_SIZE;

    //一个bit_map记录对应的page数
    const static int NUM_PAGES_OF_BIT_MAP;
    
    //位图尺寸
    const static int BIT_MAP_SZIE;

};