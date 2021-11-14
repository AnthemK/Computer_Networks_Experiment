#pragma once
#include "const.h"
#include "logger.h"

namespace tftp {
	//数据包
	class Packet {
		//添加16位无符号数
		void addUint16(uint16 v);
		//添加字符串
		void addChars(const char* str);
		//解析成16位无符号数
		uint16 parseUint16(int base) const;

	public:
		char buf[DefBufSize];	//数据包缓冲区
		int len;	//数据包有效长度

		Packet() :len(0) {}
		//核查数据包类型
		uint16 checkPacket();
		//封装RQ请求包
		void packRQ(uint16 opRQ, const char* filename, const char* mode);
		//封装DATA包
		bool packDATA(uint16 blkNo, const char* data, int l);
		//封装ACK包
		void packACK(uint16 blkNo);
		//封装ERROR包
		void packERROR(uint16 errCode, const char* msg=nullptr);
		//设置数据包的操作码
		void setOp(uint16 op) { addUint16(uint16(op)); }
		//设置数据包有效长度
		void setLen(int l) { len = l; }
		//获取DATA包中数据长度
		int getDataLen() const { return len - 4; }
		//获取数据包的操作码
		uint16 getOp() const { return parseUint16(0); }
		//获取数据包的数据块编号
		uint16 getBlkNo()const { return parseUint16(2); }
		//获取ERROR包的错误码
		uint16 getErrCode() const { return parseUint16(2); }
		//获取ERROR包的错误信息
		const char* getErrMsg() const { return buf + 4; }
		//获取DATA包的数据(基地址)
		const char* getData() const { return buf + 4; }
	};
}
