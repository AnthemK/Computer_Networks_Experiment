#include "client.h"

namespace tftp {
	/// <summary>
	/// ����tftp����
	/// </summary>
	/// <returns>UDP��SOCKET</returns>
	SOCKET Client::openConn() {
		//�����׽���
		SOCKET skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (skt == INVALID_SOCKET) {
			Logger::ost << "Create socket fail.\n";
			Logger::logshow(LogERR);
			return INVALID_SOCKET;
		}
		//�����׽���Ϊ������ģʽ
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
	/// �����ļ�·��
	/// </summary>
	/// <param name="filepath">�������ļ�·��</param>
	/// <param name="filename">��ȡ�����ļ���</param>
	void Client::handleFilePath(const string& filepath,
		char* filename) const {
		auto f = filepath.find_last_of('\\');
		if (f == filepath.npos) strcpy(filename, filepath.c_str());
		else strcpy(filename, filepath.substr(f + 1).c_str());
	}

	/// <summary>
	/// ����Winsock API
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
	/// �����ļ�
	/// </summary>
	/// <param name="filepath">�����ļ�·��</param>
	/// <param name="mode">����ģʽ</param>
	/// <returns>�Ƿ�ɹ�</returns>
	bool Client::sendFile(const string& filepath, const char* mode) {
		//��־
		Logger::logNewLine();
		Logger::ost << "Write request for file:" << filepath
			<< "  mode:" << mode << "  server-addr:"
			<< svrAddr.getAddrStr() << endl;
		Logger::logshow(LogINFO);

		//�ж�������Winsock
		if (!initWinSkt) {
			Logger::ost << "Winsock API is not initialized\n";
			Logger::logshow(LogERR);
			Logger::ost<<"Please use startWinSkt() first\n";
			Logger::show(LogNULL);
			return false;
		}
		
		//�����ļ���ģʽ:�ı�/������
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
		//���ļ�
		ifstream fin(filepath, fMode);
		if (!fin) {
			Logger::ost << "Unable to open file:" 
				<< filepath << ".\n";
			Logger::logshow(LogERR);
			return false;
		}

		//����tftp����
		SOCKET svrSkt = openConn();
		if (svrSkt == INVALID_SOCKET) {
			return false;
		}
		Conn conn(svrSkt, svrAddr, mode);
		time_t timer = time(nullptr);
		uint64 fileSize = 0;
		char filename[256];

		//��װWRQ��������
		handleFilePath(filepath, filename);
		conn.packWRQ(filename, mode);
		if (!conn.sendWithRetry(true)) return false;
		conn.incBlkNo();	//���ݿ���+1
		if (showInfo) 
			cerr << LogINFO << "already sent WRQ successfully\n";
		//��װDATA��������
		char sendBuf[DefBufSize];
		while (true) {
			fin.read(sendBuf, DataMaxSize);	//��ȡ�ļ�
			int len = fin.gcount();		//ͳ�ƶ�ȡ�ֽ���
			conn.packDATA(sendBuf, len);	//��װDATA��
			if (!conn.sendWithRetry(true)) return false;	//�������ݰ�
			if (showInfo) {
				cerr << LogINFO << "already sent packet No." 
					<< conn.getTotalBlks() << " successfully\n";
			}
			conn.incBlkNo();	//���ݿ���+1
			fileSize += len;
			if (len < DataMaxSize) break;	//���һ����ֹ��ȡ
		}
		closeConn(svrSkt);	//�ر�tftp����

		//��¼��־
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
	/// �����ļ�
	/// </summary>
	/// <param name="filepath">�ļ����·��</param>
	/// <param name="mode">����ģʽ</param>
	/// <returns>�Ƿ�ɹ�</returns>
	bool Client::recvFile(const string& filepath, const char* mode) {
		//��־
		Logger::logNewLine();
		Logger::ost << "Read request for file:" << filepath
			<< "  mode:" << mode << "  server-addr:"
			<< svrAddr.getAddrStr() << endl;
		Logger::logshow(LogINFO);

		//�ж�������Winsock
		if (!initWinSkt) {
			Logger::ost << "Winsock API is not initialized.\n";
			Logger::logshow(LogERR);
			Logger::ost << "Please use startWinSkt() first.\n";
			Logger::show(LogNULL);
			return false;
		}
		//����tftp����
		SOCKET svrSkt = openConn();
		if (svrSkt == INVALID_SOCKET) {
			return false;
		}
		Conn conn(svrSkt, svrAddr, mode);
		char filename[256];
		uint64 fileSize = 0;
		time_t timer = time(nullptr);

		//��װRRQ��������
		handleFilePath(filepath, filename);
		conn.packRRQ(filename, mode);
		if (!conn.sendWithRetry(false)) return false;
		conn.incBlkNo();	//���ݿ���+1
		if (showInfo) 
			cerr << LogINFO << "Already sent RRQ successfully.\n";

		//�����ļ���ģʽ:�ı�/������
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
		//���ļ�
		ofstream fout(filepath, fMode);
		if (!fout) {
			Logger::ost << "Unable to create file:" << filepath << ".\n";
			Logger::logshow(LogERR);
			conn.sendERROR(3);
			return false;
		}
		//����DATA��������ACK
		char recvBuf[DefBufSize];
		while (true) {
			int len = conn.getData(recvBuf);	//���ܵ���ȷ���ݷ�����ջ�����
			//����Ϊ��һ�����ݿ�
			if (len > 0) {
				fout.write(recvBuf, len);	//д���ļ�
				if (showInfo) {
					cerr << LogINFO << "Already receive packet No." 
						<< conn.getTotalBlks() << " successfully.\n";
				}
				conn.incBlkNo();	//���ݿ���+1
				fileSize += len;
			}
			conn.packACK();		//��װACK

			//���һ�����ݿ�
			if (len >= 0 && len < DataMaxSize) {
				conn.sendLastACK();	//��������ACK
				break;
			}
			//������ݿ鲻�����һ����û�л�Ӧ��ʧ��
			else if (!conn.sendWithRetry(false)) {
				return false;
			}

		}
		closeConn(svrSkt);	//�ر�tftp����

		//��¼��־
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