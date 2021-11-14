#pragma once
#include "const.h"
#include "udpaddr.h"
#include "packet.h"
#include "logger.h"

namespace tftp {
	//tftp连接
	class Conn {
		SOCKET skt;		//套接字
		UDPAddr addr;	//地址
		char mode[9];	//传输模式
		uint16 blkNo = 0;	//数据块编号
		Packet sendPkt, recvPkt;	//发送数据包,接收数据包
		uint64 totalRetries = 0;	//总重传次数
		uint64 totalBlks = 0;

		//发送套接字
		int sendTo() {
			return sendto(skt, sendPkt.buf, sendPkt.len,
				0, addr.getSktAddr(), SktAddrLen);
		}
		//接收套接字
		int recvFrom() {
			return recvfrom(skt, recvPkt.buf, DefBufSize
				, 0, addr.getSktAddr(), &SktAddrLen);
		}
		//发送数据包
		int sendPacket(bool isSender, int retry);

	public:
		Conn(SOCKET svrSkt, const UDPAddr& svrAddr,
			const char* _mode);
		//带重传的发送数据包
		bool sendWithRetry(bool isSender);
		//发送最后一个ACK
		void sendLastACK();
		//发送错误包
		void sendERROR(uint16 errCode, const char* msg = nullptr) {
			sendPkt.packERROR(errCode, msg);
			sendTo();
		}
		//封装WRQ包
		void packWRQ(const char* filename, const char* mode) {
			sendPkt.packRQ(OpWRQ, filename, mode);
		}
		//封装DATA包
		void packDATA(const char* data, int l) {
			sendPkt.packDATA(blkNo, data, l);
		}
		//封装RRQ包
		void packRRQ(const char* filename, const char* mode) {
			sendPkt.packRQ(OpRRQ, filename, mode);
		}
		//封装ACK包
		void packACK() {
			//封装的数据块编号为上次收到的数据块编号
			sendPkt.packACK(recvPkt.getBlkNo());
		}
		//从DATA包获取数据
		int getData(char* dest) const;
		//数据块编号+1
		void incBlkNo() { ++blkNo, ++totalBlks; }
		//获取当前数据块编号
		uint16 getBlkNo() { return blkNo; }
		//获取重传次数
		uint64 getTotalRetries() { return totalRetries; }
		//获取总块数
		uint64 getTotalBlks() { return totalBlks; }
	};

}
