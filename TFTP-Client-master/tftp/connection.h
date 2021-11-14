#pragma once
#include "const.h"
#include "udpaddr.h"
#include "packet.h"
#include "logger.h"

namespace tftp {
	//tftp����
	class Conn {
		SOCKET skt;		//�׽���
		UDPAddr addr;	//��ַ
		char mode[9];	//����ģʽ
		uint16 blkNo = 0;	//���ݿ���
		Packet sendPkt, recvPkt;	//�������ݰ�,�������ݰ�
		uint64 totalRetries = 0;	//���ش�����
		uint64 totalBlks = 0;

		//�����׽���
		int sendTo() {
			return sendto(skt, sendPkt.buf, sendPkt.len,
				0, addr.getSktAddr(), SktAddrLen);
		}
		//�����׽���
		int recvFrom() {
			return recvfrom(skt, recvPkt.buf, DefBufSize
				, 0, addr.getSktAddr(), &SktAddrLen);
		}
		//�������ݰ�
		int sendPacket(bool isSender, int retry);

	public:
		Conn(SOCKET svrSkt, const UDPAddr& svrAddr,
			const char* _mode);
		//���ش��ķ������ݰ�
		bool sendWithRetry(bool isSender);
		//�������һ��ACK
		void sendLastACK();
		//���ʹ����
		void sendERROR(uint16 errCode, const char* msg = nullptr) {
			sendPkt.packERROR(errCode, msg);
			sendTo();
		}
		//��װWRQ��
		void packWRQ(const char* filename, const char* mode) {
			sendPkt.packRQ(OpWRQ, filename, mode);
		}
		//��װDATA��
		void packDATA(const char* data, int l) {
			sendPkt.packDATA(blkNo, data, l);
		}
		//��װRRQ��
		void packRRQ(const char* filename, const char* mode) {
			sendPkt.packRQ(OpRRQ, filename, mode);
		}
		//��װACK��
		void packACK() {
			//��װ�����ݿ���Ϊ�ϴ��յ������ݿ���
			sendPkt.packACK(recvPkt.getBlkNo());
		}
		//��DATA����ȡ����
		int getData(char* dest) const;
		//���ݿ���+1
		void incBlkNo() { ++blkNo, ++totalBlks; }
		//��ȡ��ǰ���ݿ���
		uint16 getBlkNo() { return blkNo; }
		//��ȡ�ش�����
		uint64 getTotalRetries() { return totalRetries; }
		//��ȡ�ܿ���
		uint64 getTotalBlks() { return totalBlks; }
	};

}
