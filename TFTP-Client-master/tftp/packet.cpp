#include "packet.h"

namespace tftp {
	/// <summary>
	/// 封装请求包
	/// </summary>
	/// <param name="opRQ">请求操作码RRQ/WRQ</param>
	/// <param name="filename">文件名</param>
	/// <param name="mode">传输模式</param>
	void tftp::Packet::packRQ(uint16 opRQ, const char* filename, const char* mode) {
		setLen(0);		//初始化包长度为0
		setOp(opRQ);	//设置操作码
		addChars(filename);	//添加文件名
		addChars(mode);	//添加传输模式字符串
	}

	/// <summary>
	/// 添加16位无符号数
	/// </summary>
	/// <param name="v">待添加数</param>
	void Packet::addUint16(uint16 v) {
		v = htons(v);	//转换为网络字节序
		memcpy(buf + len, &v, 2);
		len += 2;
	}

	/// <summary>
	/// 添加字符串
	/// </summary>
	/// <param name="str">待添加字符串</param>
	void Packet::addChars(const char* str) {
		strcpy(buf + len, str);
		len += strlen(str) + 1;		//长度要加上最后的'\0'
	}

	/// <summary>
	/// 解析成16位无符号数
	/// </summary>
	/// <param name="base">在数据包缓冲区的位置</param>
	/// <returns>解析后的无符号数</returns>
	uint16 Packet::parseUint16(int base) const {
		//注:必须先用byte类型转换不然buf[base]为0x80的时候会出错!!!
		uint16 v;
		memcpy(&v, buf + base, 2);
		return ntohs(v);	//转换为本机字节序
	}

	/// <summary>
	/// 封装DATA包
	/// </summary>
	/// <param name="blkNo">数据块编号</param>
	/// <param name="data">数据基址</param>
	/// <param name="l">数据长度</param>
	/// <returns>是否成功</returns>
	bool Packet::packDATA(uint16 blkNo, const char* data, int l) {
		if (l > DataMaxSize) {
			Logger::ost << "Data size exceeds upper limit.\n";
			Logger::logshow(LogERR);
			return false;
		}
		setLen(0);
		setOp(OpDATA);
		addUint16(blkNo);
		memcpy(buf + len, data, l);
		len += l;
		return true;
	}

	/// <summary>
	/// 封装ACK包
	/// </summary>
	/// <param name="blkNo">数据块编号</param>
	void Packet::packACK(uint16 blkNo) {
		setLen(0);
		setOp(OpACK);
		addUint16(blkNo);
	}

	/// <summary>
	/// 封装ERROR包
	/// </summary>
	/// <param name="errCode">错误码</param>
	/// <param name="msg">错误信息</param>
	void Packet::packERROR(uint16 errCode, const char* msg) {
		setLen(0);
		setOp(OpERROR);
		addUint16(errCode);
		//默认错误信息
		if (errCode > 0 && errCode < 8) addChars(ErrMsg[errCode]);
		//自定义错误信息
		else if (msg) addChars(msg);
		else if (errCode == 0 && !msg) addChars(ErrMsg[0]);
	}

	/// <summary>
	/// 核查数据包类型
	/// </summary>
	/// <returns>正数:数据包操作码, 0:数据包过短</returns>
	uint16 Packet::checkPacket() {
		if (len < 2) {
			cerr << "ERR: short packet\n";
			return 0;
		}
		uint16 op = getOp();
		switch (op) {
		case OpRRQ:
			if (len < 4) {
				cerr << "ERR: short RRQ packet\n";
				return 0;
			}
			return op;
		case OpWRQ:
			if (len < 4) {
				cerr << "ERR: short WRQ packet\n";
				return 0;
			}
			return op;
		case OpDATA:
			if (len < 4) {
				cerr << "ERR: short DATA packet\n";
				return 0;
			}
			return op;
		case OpACK:
			if (len < 4) {
				cerr << "ERR: short ACK packet\n";
				return 0;
			}
			return op;
		case OpERROR:
			if (len < 5) {
				cerr << "ERR: short ERROR packet\n";
				return 0;
			}
			return op;
		default:
			cerr << "ERR: unknown opcode\n";
			return 0;
		}
	}
}

