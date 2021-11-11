#pragma once
#include <string>
#include "config.h"
using namespace std;

//定义帧结构
struct bFrame {
    char* field;
    bFrame() {
        field = new char[FRAME_SIZE]();
    }
    ~bFrame() {
        delete[] field;
    }
};

//定义数据存储管理器
class DSMgr {
public:
    DSMgr() {
        pages = new int[MAX_PAGES];
    };

    //打开指定文件名的文件
    int OpenFile(string filename);

    //关闭数据文件，当且仅当数据修改或程序关闭调用
    int CloseFile();

    //从page_id对应的页读取数据，由缓冲管理器的FixPage函数调用
    bFrame ReadPage(int page_id);//bytes？？？

    //当frame从缓冲区取出时调用，返回编写的字节数，将数据保存至文件
    int WritePage(int frame_id, bFrame frm);

    //移动文件指针
    int Seek(int offset, int pos);

    //返回当前文件指针
    FILE* GetFile();

    //增加页面数，+1
    void IncNumPages();

    //返回页面数
    int GetNumPages();

    //设置page_id的use_bit
    void SetUse(int page_id, int use_bit);

    //返回page_id对应对应page的use_bit
    int GetUse(int page_id);

private:
    //当前文件的指针
    FILE* currFile;

    //当前页面数
    int numPages;

    //跟踪page的使用情况
    int* pages;
};