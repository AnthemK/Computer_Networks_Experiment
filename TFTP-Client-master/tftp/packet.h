#pragma once
#include "const.h"
#include "logger.h"

namespace tftp {
	//���ݰ�
	class Packet {
		//���16λ�޷�����
		void addUint16(uint16 v);
		//����ַ���
		void addChars(const char* str);
		//������16λ�޷�����
		uint16 parseUint16(int base) const;

	public:
		char buf[DefBufSize];	//���ݰ�������
		int len;	//���ݰ���Ч����

		Packet() :len(0) {}
		//�˲����ݰ�����
		uint16 checkPacket();
		//��װRQ�����
		void packRQ(uint16 opRQ, const char* filename, const char* mode);
		//��װDATA��
		bool packDATA(uint16 blkNo, const char* data, int l);
		//��װACK��
		void packACK(uint16 blkNo);
		//��װERROR��
		void packERROR(uint16 errCode, const char* msg=nullptr);
		//�������ݰ��Ĳ�����
		void setOp(uint16 op) { addUint16(uint16(op)); }
		//�������ݰ���Ч����
		void setLen(int l) { len = l; }
		//��ȡDATA�������ݳ���
		int getDataLen() const { return len - 4; }
		//��ȡ���ݰ��Ĳ�����
		uint16 getOp() const { return parseUint16(0); }
		//��ȡ���ݰ������ݿ���
		uint16 getBlkNo()const { return parseUint16(2); }
		//��ȡERROR���Ĵ�����
		uint16 getErrCode() const { return parseUint16(2); }
		//��ȡERROR���Ĵ�����Ϣ
		const char* getErrMsg() const { return buf + 4; }
		//��ȡDATA��������(����ַ)
		const char* getData() const { return buf + 4; }
	};
}
