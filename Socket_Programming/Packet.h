#ifndef _PACKET_H
#define _PACKET_H
//#pragma once   //也可以起到只编译一次的作用，但是适用性要差一些
#include "Client_Const.h"

namespace TFTP {
	class Packet {
	public:
		
		byte buf[DefBufSize];	//数据包缓冲区  1024
		int Packetlen;	//数据包有效长度  
		Packet() :buf(""), Packetlen(0) {}

		void setPacketLen(const int l = 0);  //设置数据包有效长度
		int getDataLen() const;    //获取DATA包中数据长度

		void addByte(byte add_val);         //添加一个字节
		void addUint16(uint16 add_val);     //添加两个字节 （传入变量没有进行字节翻转）
		void addChars(const char* str);        //添加字符串
		void addBytes(const byte* str, int strLenth);        //添加字节串，需要指定长度
		void setOp(uint16 Opcode);    //设置数据包的操作码


		byte* GetByteAddr(int Aimpls);
		const byte* getErrMsg() const;
		//获取DATA包的数据(基地址)
		const byte* getData() const;

		//解析成16位无符号数
		uint16 ExtractUint16(int base = 0) const;
		uint16 ExtractOpCode();
		uint16 ExtractBlockNo();
		uint16 ExtractErrCode();


		bool PackRQ(uint16 OpRQ, const char* Filename, const int Modetype); //opRQ表示R/Q种类   FileName为请求的文件名   typ为0 传输二进制，否则netascii码
		bool PackDATA(uint16 blockNo, const char* data); //封装DATA包 blockNo表示包的编号，data为数据
		//封装ACK包
		void PackACK(uint16 blockNo);
		//封装ERROR包
		void PackERROR(uint16 errCode, const char* msg = nullptr);

		//核查数据包类型
		uint16 checkPacket();
	};


}



#endif