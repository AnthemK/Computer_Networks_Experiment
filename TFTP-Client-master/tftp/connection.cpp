#include "connection.h"

namespace tftp {
	/// <summary>
	/// 发送数据包
	/// </summary>
	/// <param name="isSender">是发送文件/接收文件</param>
	/// <param name="retry">重传次数</param>
	/// <returns>1:成功, -1/-2:失败不可重传, 0:超时可重传</returns>
	int Conn::sendPacket(bool isSender, int retry) {
		time_t deadLine = time(nullptr) + DefTimeOut;	//设置计时器
		int sl = sendTo();	//发送套接字
		//发送失败
		if (sl <= 0) {
			//sendto发送失败
			Logger::ost << "Send packet " << blkNo
				<< " fail with sendto().\n";
			Logger::logshow(LogERR);
			return -1;
		}

		while (true) {
			int rl = recvFrom();	//接收套接字
			//cout << blkNo << " " << rl << endl;
			//接收失败
			if (rl <= 0) {
				//超时则结束等待
				if (time(nullptr) >= deadLine) {
					Logger::ost << "Not recieve response for packet."
						<< blkNo << "  winsock-error-code:"
						<< WSAGetLastError() << endl; 
					Logger::logshow(LogWAIN);
					Logger::ost << "try " << retry + 1 << " times\n";
					Logger::show(LogNULL);
					return 0;
				}
				//未超时继续等待
				continue;
			}
			recvPkt.setLen(rl);		//设置接收包长度
			uint16 op = recvPkt.checkPacket();	//检测接收包
			//若为发送文件且受到ACK
			if (isSender && op == OpACK) {
				uint16 blk = recvPkt.getBlkNo();	//获取数据块编号
				if (blk == blkNo) return 1;		//若为当前新的数据块则返回
				//若为之前的数据块丢掉并重置计时器
				deadLine = time(nullptr) + DefTimeOut;
			}
			//若为接收文件且受到的为数据包则直接返回
			else if (!isSender && op == OpDATA) {
				return 1;
			}
			//受到错误包
			else if (op == OpERROR) {
				Logger::ost << "Send packet " << recvPkt.getBlkNo()
					<< " got error-packet  code:" << recvPkt.getErrCode();
				if (rl > 4)
					Logger::ost << " message:" << recvPkt.getErrMsg();
				Logger::ost << ".\n";
				Logger::logshow(LogERR);
				return -2;
			}
			//收到过短的数据包
			else if (op == 0) {
				Logger::ost << "Send packet " << recvPkt.getBlkNo()
					<< " got packet too short.\n";
				Logger::logshow(LogERR);
				sendERROR(4);	//发送错误包
				return -2;
			}
			//收到其他类型的数据包
			else {
				Logger::ost << "Send packet " << recvPkt.getBlkNo()
					<< " got mistake packet.\n";
				Logger::logshow(LogERR);
				sendERROR(4);	//发送错误包
				return -2;
			}
		}
		return -2;
	}

	Conn::Conn(SOCKET svrSkt, const UDPAddr& svrAddr,
		const char* _mode) {
		skt = svrSkt;
		addr = svrAddr;
		strcpy(mode, _mode);
	}

	/// <summary>
	/// 带重传的发送数据包
	/// </summary>
	/// <param name="isSender">是发送文件/接收文件</param>
	/// <returns>是否成功</returns>
	bool Conn::sendWithRetry(bool isSender) {
		int retry = 0;		//发送次数
		int ret;
		//未到重传上限则重传
		while (retry < DefRetries) {
			ret = sendPacket(isSender, retry);	//发送数据包
			if (ret == 1) {	//发送成功
				return true;
			}
			else if (ret == 0) {
				++retry;	//发送失败并重传
				++totalRetries;
			}
			else break;		//发送失败并退出
		}
		//重传到上限仍失败
		if (retry == DefRetries) {
			//一直未收到回复
			Logger::ost << "Not recieve response for packet No."
				<< blkNo << " with recvfrom().\n";
			Logger::logshow(LogERR);
			sendERROR(5);	//发送错误码
		}
		return false;
	}

	/// <summary>
	/// 发送接受文件的最后一个ACK
	/// </summary>
	void Conn::sendLastACK() {
		int sl = sendTo();
		if (sl == 4)  return;
		Logger::ost << "Send packet " << blkNo
			<< " fail with sendto()."
			<< "  winsock error code:"
			<< WSAGetLastError() << endl;
		Logger::logshow(LogERR);
	}

	/// <summary>
	/// 从DATA包获取数据
	/// </summary>
	/// <param name="dest">目标缓冲区</param>
	/// <returns>非负数表示数据长度, -1表示不是最新数据块, -2表示不为数据包</returns>
	int Conn::getData(char* dest) const {
		//不为DATA包
		if (recvPkt.getOp() != OpDATA) {
			return -2;
		}
		//不为最新的包
		else if (recvPkt.getBlkNo() != blkNo) {
			return -1;
		}
		//复制数据到缓冲区
		memcpy(dest, recvPkt.getData(), recvPkt.getDataLen());
		//返回数据长度
		return recvPkt.getDataLen();
	}
}


