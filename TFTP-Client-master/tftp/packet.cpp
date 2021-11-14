#include "packet.h"

namespace tftp {
	/// <summary>
	/// ��װ�����
	/// </summary>
	/// <param name="opRQ">���������RRQ/WRQ</param>
	/// <param name="filename">�ļ���</param>
	/// <param name="mode">����ģʽ</param>
	void tftp::Packet::packRQ(uint16 opRQ, const char* filename, const char* mode) {
		setLen(0);		//��ʼ��������Ϊ0
		setOp(opRQ);	//���ò�����
		addChars(filename);	//����ļ���
		addChars(mode);	//��Ӵ���ģʽ�ַ���
	}

	/// <summary>
	/// ���16λ�޷�����
	/// </summary>
	/// <param name="v">�������</param>
	void Packet::addUint16(uint16 v) {
		v = htons(v);	//ת��Ϊ�����ֽ���
		memcpy(buf + len, &v, 2);
		len += 2;
	}

	/// <summary>
	/// ����ַ���
	/// </summary>
	/// <param name="str">������ַ���</param>
	void Packet::addChars(const char* str) {
		strcpy(buf + len, str);
		len += strlen(str) + 1;		//����Ҫ��������'\0'
	}

	/// <summary>
	/// ������16λ�޷�����
	/// </summary>
	/// <param name="base">�����ݰ���������λ��</param>
	/// <returns>��������޷�����</returns>
	uint16 Packet::parseUint16(int base) const {
		//ע:��������byte����ת����Ȼbuf[base]Ϊ0x80��ʱ������!!!
		uint16 v;
		memcpy(&v, buf + base, 2);
		return ntohs(v);	//ת��Ϊ�����ֽ���
	}

	/// <summary>
	/// ��װDATA��
	/// </summary>
	/// <param name="blkNo">���ݿ���</param>
	/// <param name="data">���ݻ�ַ</param>
	/// <param name="l">���ݳ���</param>
	/// <returns>�Ƿ�ɹ�</returns>
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
	/// ��װACK��
	/// </summary>
	/// <param name="blkNo">���ݿ���</param>
	void Packet::packACK(uint16 blkNo) {
		setLen(0);
		setOp(OpACK);
		addUint16(blkNo);
	}

	/// <summary>
	/// ��װERROR��
	/// </summary>
	/// <param name="errCode">������</param>
	/// <param name="msg">������Ϣ</param>
	void Packet::packERROR(uint16 errCode, const char* msg) {
		setLen(0);
		setOp(OpERROR);
		addUint16(errCode);
		//Ĭ�ϴ�����Ϣ
		if (errCode > 0 && errCode < 8) addChars(ErrMsg[errCode]);
		//�Զ��������Ϣ
		else if (msg) addChars(msg);
		else if (errCode == 0 && !msg) addChars(ErrMsg[0]);
	}

	/// <summary>
	/// �˲����ݰ�����
	/// </summary>
	/// <returns>����:���ݰ�������, 0:���ݰ�����</returns>
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

