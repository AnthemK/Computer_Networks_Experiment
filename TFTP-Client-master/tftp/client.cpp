#include "client.h"

namespace tftp {
	/// <summary>
	/// 建立tftp连接
	/// </summary>
	/// <returns>UDP的SOCKET</returns>
	SOCKET Client::openConn() {
		//创建套接字
		SOCKET skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (skt == INVALID_SOCKET) {
			Logger::ost << "Create socket fail.\n";
			Logger::logshow(LogERR);
			return INVALID_SOCKET;
		}
		//设置套接字为非阻塞模式
		u_long imode = 1;
		auto ret = ioctlsocket(skt, FIONBIO, &imode);
		if (ret == SOCKET_ERROR) {
			Logger::ost << "Set parameters with socket error.\n";
			Logger::logshow(LogERR);
			return INVALID_SOCKET;
		}
		return skt;
	}

	/// <summary>
	/// 处理文件路径
	/// </summary>
	/// <param name="filepath">待处理文件路径</param>
	/// <param name="filename">提取出的文件名</param>
	void Client::handleFilePath(const string& filepath,
		char* filename) const {
		auto f = filepath.find_last_of('\\');
		if (f == filepath.npos) strcpy(filename, filepath.c_str());
		else strcpy(filename, filepath.substr(f + 1).c_str());
	}

	/// <summary>
	/// 启动Winsock API
	/// </summary>
	/// <returns></returns>
	bool Client::startWinSkt() {
		WORD wVersion = MAKEWORD(2, 2);
		WSADATA wData;
		if (WSAStartup(wVersion, &wData)) {
			Logger::ost << "Initialize Winsock API fail.\n";
			Logger::logshow(LogERR);
			return false;
		}
		initWinSkt = true;
		return true;
	}

	/// <summary>
	/// 发送文件
	/// </summary>
	/// <param name="filepath">发送文件路径</param>
	/// <param name="mode">传输模式</param>
	/// <returns>是否成功</returns>
	bool Client::sendFile(const string& filepath, const char* mode) {
		//日志
		Logger::logNewLine();
		Logger::ost << "Write request for file:" << filepath
			<< "  mode:" << mode << "  server-addr:"
			<< svrAddr.getAddrStr() << endl;
		Logger::logshow(LogINFO);

		//判断是启动Winsock
		if (!initWinSkt) {
			Logger::ost << "Winsock API is not initialized\n";
			Logger::logshow(LogERR);
			Logger::ost<<"Please use startWinSkt() first\n";
			Logger::show(LogNULL);
			return false;
		}
		
		//设置文件打开模式:文本/二进制
		ifstream::open_mode fMode;
		if (strcmp(mode, ModeBin) == 0)
			fMode = ifstream::binary | ifstream::in;
		else if (strcmp(mode, ModeAscii) == 0)
			fMode = ifstream::in;
		else {
			Logger::ost << "Mode of transfer error.\n";
			Logger::logshow(LogERR);
			return false;
		}
		//打开文件
		ifstream fin(filepath, fMode);
		if (!fin) {
			Logger::ost << "Unable to open file:" 
				<< filepath << ".\n";
			Logger::logshow(LogERR);
			return false;
		}

		//建立tftp连接
		SOCKET svrSkt = openConn();
		if (svrSkt == INVALID_SOCKET) {
			return false;
		}
		Conn conn(svrSkt, svrAddr, mode);
		time_t timer = time(nullptr);
		uint64 fileSize = 0;
		char filename[256];

		//封装WRQ包并发送
		handleFilePath(filepath, filename);
		conn.packWRQ(filename, mode);
		if (!conn.sendWithRetry(true)) return false;
		conn.incBlkNo();	//数据块编号+1
		if (showInfo) 
			cerr << LogINFO << "already sent WRQ successfully\n";
		//封装DATA包并发送
		char sendBuf[DefBufSize];
		while (true) {
			fin.read(sendBuf, DataMaxSize);	//读取文件
			int len = fin.gcount();		//统计读取字节数
			conn.packDATA(sendBuf, len);	//封装DATA包
			if (!conn.sendWithRetry(true)) return false;	//发送数据包
			if (showInfo) {
				cerr << LogINFO << "already sent packet No." 
					<< conn.getTotalBlks() << " successfully\n";
			}
			conn.incBlkNo();	//数据块编号+1
			fileSize += len;
			if (len < DataMaxSize) break;	//最后一块终止读取
		}
		closeConn(svrSkt);	//关闭tftp连接

		//记录日志
		Logger::ost << "Send file successfully.  "
			<< "file:" << filepath << "  size:" << conn.getTotalBlks()
			<< "blks," << fileSize << "bytes"
			<< "  resent:"<<conn.getTotalRetries()<<" blks"
			<< "  throughput:" << fileSize / (time(nullptr) - timer + 1)
			<< "B/s\n";
		Logger::logshow(LogINFO);
		return true;
	}

	/// <summary>
	/// 接受文件
	/// </summary>
	/// <param name="filepath">文件存放路径</param>
	/// <param name="mode">传输模式</param>
	/// <returns>是否成功</returns>
	bool Client::recvFile(const string& filepath, const char* mode) {
		//日志
		Logger::logNewLine();
		Logger::ost << "Read request for file:" << filepath
			<< "  mode:" << mode << "  server-addr:"
			<< svrAddr.getAddrStr() << endl;
		Logger::logshow(LogINFO);

		//判断是启动Winsock
		if (!initWinSkt) {
			Logger::ost << "Winsock API is not initialized.\n";
			Logger::logshow(LogERR);
			Logger::ost << "Please use startWinSkt() first.\n";
			Logger::show(LogNULL);
			return false;
		}
		//建立tftp连接
		SOCKET svrSkt = openConn();
		if (svrSkt == INVALID_SOCKET) {
			return false;
		}
		Conn conn(svrSkt, svrAddr, mode);
		char filename[256];
		uint64 fileSize = 0;
		time_t timer = time(nullptr);

		//封装RRQ包并发送
		handleFilePath(filepath, filename);
		conn.packRRQ(filename, mode);
		if (!conn.sendWithRetry(false)) return false;
		conn.incBlkNo();	//数据块编号+1
		if (showInfo) 
			cerr << LogINFO << "Already sent RRQ successfully.\n";

		//设置文件打开模式:文本/二进制
		ofstream::open_mode fMode;
		if (strcmp(mode, ModeBin) == 0)
			fMode = ifstream::binary | ifstream::out;
		else if (strcmp(mode, ModeAscii) == 0)
			fMode = ifstream::out;
		else {
			Logger::ost << "Mode of transfer error.\n";
			Logger::logshow(LogERR);
			conn.sendERROR(0, "Read file failed.");
			return false;
		}
		//打开文件
		ofstream fout(filepath, fMode);
		if (!fout) {
			Logger::ost << "Unable to create file:" << filepath << ".\n";
			Logger::logshow(LogERR);
			conn.sendERROR(3);
			return false;
		}
		//接受DATA包并发送ACK
		char recvBuf[DefBufSize];
		while (true) {
			int len = conn.getData(recvBuf);	//将受到正确数据放入接收缓冲区
			//数据为下一个数据块
			if (len > 0) {
				fout.write(recvBuf, len);	//写入文件
				if (showInfo) {
					cerr << LogINFO << "Already receive packet No." 
						<< conn.getTotalBlks() << " successfully.\n";
				}
				conn.incBlkNo();	//数据块编号+1
				fileSize += len;
			}
			conn.packACK();		//封装ACK

			//最后一块数据块
			if (len >= 0 && len < DataMaxSize) {
				conn.sendLastACK();	//发送最后的ACK
				break;
			}
			//如果数据块不是最后一块且没有回应则失败
			else if (!conn.sendWithRetry(false)) {
				return false;
			}

		}
		closeConn(svrSkt);	//关闭tftp连接

		//记录日志
		Logger::ost << "Receive file successfully.  "
			<< "file:" << filepath << "  size:" << conn.getTotalBlks()
			<< "blks," << fileSize << "bytes"
			<< "  resent:" << conn.getTotalRetries() << " blks"
			<< "  throughput:" << fileSize / (time(nullptr) - timer + 1)
			<< "B/s\n";
		Logger::logshow(LogINFO);
		return true;
	}

}