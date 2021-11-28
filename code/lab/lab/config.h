#pragma once
#include <string>

using std::string;

//帧的大小(在硬盘为page，主存中为frame)
#define FRAMESIZE 4096

//缓冲区大小（帧的个数）
const extern int DEFBUFSIZE;

//demo中数据库所有data存储在DATAFILE中，单文件存储
const extern string  DATAFILE;

//demo中默认单个文件最大页数
const extern int MAXPAGES;

//日志开关
extern bool LOG_ON_OFF;


