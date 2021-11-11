#pragma once
#include <string>

using std::string;

//帧的大小，frame与page保持同等大小，在硬盘为page，主存中为frame
const extern int FRAME_SIZE;

//缓冲区大小（帧的个数）
const extern int BUF_SIZE;

//demo中数据库所有data存储在DATAFILE中，单文件存储
const extern string  DATA_FILE;

//demo中默认单个文件最大页数
const extern int MAX_PAGES;