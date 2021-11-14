#include "connection.h"

namespace tftp {
	/// <summary>
	/// �������ݰ�
	/// </summary>
	/// <param name="isSender">�Ƿ����ļ�/�����ļ�</param>
	/// <param name="retry">�ش�����</param>
	/// <returns>1:�ɹ�, -1/-2:ʧ�ܲ����ش�, 0:��ʱ���ش�</returns>
	int Conn::sendPacket(bool isSender, int retry) {
		time_t deadLine = time(nullptr) + DefTimeOut;	//���ü�ʱ��
		int sl = sendTo();	//�����׽���
		//����ʧ��
		if (sl <= 0) {
			//sendto����ʧ��
			Logger::ost << "Send packet " << blkNo
				<< " fail with sendto().\n";
			Logger::logshow(LogERR);
			return -1;
		}

		while (true) {
			int rl = recvFrom();	//�����׽���
			//cout << blkNo << " " << rl << endl;
			//����ʧ��
			if (rl <= 0) {
				//��ʱ������ȴ�
				if (time(nullptr) >= deadLine) {
					Logger::ost << "Not recieve response for packet."
						<< blkNo << "  winsock-error-code:"
						<< WSAGetLastError() << endl; 
					Logger::logshow(LogWAIN);
					Logger::ost << "try " << retry + 1 << " times\n";
					Logger::show(LogNULL);
					return 0;
				}
				//δ��ʱ�����ȴ�
				continue;
			}
			recvPkt.setLen(rl);		//���ý��հ�����
			uint16 op = recvPkt.checkPacket();	//�����հ�
			//��Ϊ�����ļ����ܵ�ACK
			if (isSender && op == OpACK) {
				uint16 blk = recvPkt.getBlkNo();	//��ȡ���ݿ���
				if (blk == blkNo) return 1;		//��Ϊ��ǰ�µ����ݿ��򷵻�
				//��Ϊ֮ǰ�����ݿ鶪�������ü�ʱ��
				deadLine = time(nullptr) + DefTimeOut;
			}
			//��Ϊ�����ļ����ܵ���Ϊ���ݰ���ֱ�ӷ���
			else if (!isSender && op == OpDATA) {
				return 1;
			}
			//�ܵ������
			else if (op == OpERROR) {
				Logger::ost << "Send packet " << recvPkt.getBlkNo()
					<< " got error-packet  code:" << recvPkt.getErrCode();
				if (rl > 4)
					Logger::ost << " message:" << recvPkt.getErrMsg();
				Logger::ost << ".\n";
				Logger::logshow(LogERR);
				return -2;
			}
			//�յ����̵����ݰ�
			else if (op == 0) {
				Logger::ost << "Send packet " << recvPkt.getBlkNo()
					<< " got packet too short.\n";
				Logger::logshow(LogERR);
				sendERROR(4);	//���ʹ����
				return -2;
			}
			//�յ��������͵����ݰ�
			else {
				Logger::ost << "Send packet " << recvPkt.getBlkNo()
					<< " got mistake packet.\n";
				Logger::logshow(LogERR);
				sendERROR(4);	//���ʹ����
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
	/// ���ش��ķ������ݰ�
	/// </summary>
	/// <param name="isSender">�Ƿ����ļ�/�����ļ�</param>
	/// <returns>�Ƿ�ɹ�</returns>
	bool Conn::sendWithRetry(bool isSender) {
		int retry = 0;		//���ʹ���
		int ret;
		//δ���ش��������ش�
		while (retry < DefRetries) {
			ret = sendPacket(isSender, retry);	//�������ݰ�
			if (ret == 1) {	//���ͳɹ�
				return true;
			}
			else if (ret == 0) {
				++retry;	//����ʧ�ܲ��ش�
				++totalRetries;
			}
			else break;		//����ʧ�ܲ��˳�
		}
		//�ش���������ʧ��
		if (retry == DefRetries) {
			//һֱδ�յ��ظ�
			Logger::ost << "Not recieve response for packet No."
				<< blkNo << " with recvfrom().\n";
			Logger::logshow(LogERR);
			sendERROR(5);	//���ʹ�����
		}
		return false;
	}

	/// <summary>
	/// ���ͽ����ļ������һ��ACK
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
	/// ��DATA����ȡ����
	/// </summary>
	/// <param name="dest">Ŀ�껺����</param>
	/// <returns>�Ǹ�����ʾ���ݳ���, -1��ʾ�����������ݿ�, -2��ʾ��Ϊ���ݰ�</returns>
	int Conn::getData(char* dest) const {
		//��ΪDATA��
		if (recvPkt.getOp() != OpDATA) {
			return -2;
		}
		//��Ϊ���µİ�
		else if (recvPkt.getBlkNo() != blkNo) {
			return -1;
		}
		//�������ݵ�������
		memcpy(dest, recvPkt.getData(), recvPkt.getDataLen());
		//�������ݳ���
		return recvPkt.getDataLen();
	}
}


