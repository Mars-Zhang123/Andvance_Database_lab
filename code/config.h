#pragma once
//帧的大小，frame与page保持同等大小，在硬盘为page，主存中为frame
const extern int FRAMESIZE;

//缓冲区大小（帧的个数）
const extern int DEFBUFSIZE;

//demo中数据库所有data存储在DATAFILE中，单文件存储
const extern string  DATAFILE;
