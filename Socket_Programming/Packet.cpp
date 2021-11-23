#include "Packet.h"

namespace TFTP{

	void Packet::setPacketLen(const int val) { Packetlen = val; }
	int Packet::getDataLen() const { return Packetlen - 4; }


	void Packet::addByte(byte add_val) {
		memcpy(buf + Packetlen, &add_val, 1);
		Packetlen++;
	}
	void Packet::addUint16(uint16 add_val) {
		add_val = htons(add_val);	//һ��Ҫת���ֽ�˳��
		memcpy(buf + Packetlen, &add_val, 2);
		Packetlen += 2;
	}
	void Packet::addChars(const char* AddStr) {
		int strLenth = strlen(AddStr);
		memcpy(buf + Packetlen, AddStr, strLenth+1);       //ע������,����0����λ��
		Packetlen += (strLenth + 1);		//��˳���Ҫ��������'\0'
	}
	void Packet::addBytes(const byte* AddStr,int strLenth) {   //Ҫָ������
		memcpy(buf + Packetlen, AddStr, strLenth );      
		Packetlen += strLenth;		
	}
	void Packet::setOp(uint16 Opcode) { 
		Opcode = htons(Opcode);  //��ת
		memcpy(buf, &Opcode, 2);
	} 

	byte* Packet::GetByteAddr(int Aimpls) {return buf + Aimpls;}
	const byte* Packet::getErrMsg() const { return buf + 4;}
	//��ȡDATA��������(����ַ)
	const byte* Packet::getData() const { return buf + 4; }


	uint16 Packet::ExtractUint16(int base) const {
		uint16 ext_val;
		memcpy(&ext_val, buf + base, 2);
		return ntohs(ext_val);       //ת��˳��
	}
	uint16 Packet::ExtractOpCode() {return ExtractUint16(0);}
	uint16 Packet::ExtractBlockNo() { return ExtractUint16(2); }
	uint16 Packet::ExtractErrCode() { return ExtractUint16(2); }

	bool Packet::PackRQ(uint16 OpRQ, const char* Filename, const int Modetype) {
		if (OpRQ != 1 && OpRQ != 2) {
			Log_Output::OutputtoBoth(1,"OpCode is not 1 or 2!");
			return false;
		}
		if (Modetype != 0 && Modetype != 1) {
			Log_Output::OutputtoBoth(1, "ModeType Illegal!");
			return false;
		}
		setPacketLen();
		addUint16(OpRQ);
		if (Filename != NULL) addChars(Filename);
		else addChars(""),assert(getDataLen() == 3);
		addChars(RQMode[Modetype]);
		return true;
	}
	bool Packet::PackDATA(uint16 blockNo, const char* data, const int Lenth) {
		if (Lenth > DataMaxSize) {
			Log_Output::OutputtoBoth(1, "Data too Large!");
			return false;
		}
		setPacketLen();
		addUint16(OpDATA);
		addUint16(blockNo);
		addBytes((byte* )data,Lenth);
		return true;
	}
	void Packet::PackACK(uint16 blockNo) {
		setPacketLen();
		addUint16(OpACK);
		addUint16(blockNo);
	}
	void Packet::PackERROR(uint16 errCode, const char* msg) {
		setPacketLen();
		addUint16(OpERROR);
		addUint16(errCode);
		if (errCode > 0 && errCode < 8) addChars(ErrMsg[errCode]);	//Ĭ�ϴ�����Ϣ
		else if (msg) addChars(msg);		//�Զ��������Ϣ
		else if (errCode == 0) addChars(ErrMsg[0]);     //δ�������
	}

	uint16 Packet::CheckPacket() {
		if (Packetlen < 2) {
			Log_Output::OutputtoBoth(1,"Packet Too Short!");
			return -1;  
		}
		if (Packetlen > DefBufSize) {
			Log_Output::OutputtoBoth(1, "Packet Too Long!");
			return -2;
		}
		uint16 op = ExtractOpCode();
		int lenth;
		char* TestStr;
		switch (op) {
		case OpRRQ:
			if (Packetlen < 4) {
				Log_Output::OutputtoBoth(1, "RRQ Packet Too Short!");
				return -1;
			}
			lenth = strlen((char*)GetByteAddr(2));
			//���ļ���ַ�������
			TestStr = (char *)GetByteAddr(lenth+4);
			if (strcmp(TestStr, RQMode[0]) != 0 && strcmp(TestStr, RQMode[1]) != 0)
			{
				Log_Output::OutputtoBoth(1, "RRQ Packet Mode Illegal!");
				return -3;
			}
			return op;
		case OpWRQ:
			if (Packetlen < 4) {
				Log_Output::OutputtoBoth(1, "WRQ Packet Too Short!");
				return -1;
			}
			lenth = strlen((char*)GetByteAddr(2));
			//���ļ���ַ�������
			TestStr = (char*)GetByteAddr(lenth + 4);
			if (strcmp(TestStr, RQMode[0]) != 0 && strcmp(TestStr, RQMode[1]) != 0)
			{
				Log_Output::OutputtoBoth(1, "WRQ Packet Mode Illegal!");
				return -3;
			}
			return op;
		case OpDATA:
			if (Packetlen < 4) {
				Log_Output::OutputtoBoth(1, "DATA Packet Too Short!");
				return -1;
			}
			return op;
		case OpACK:
			if (Packetlen < 4) {
				Log_Output::OutputtoBoth(1, "ACK Packet Too Short!");
				return -1;
			}
			if (Packetlen > 4) {
				Log_Output::OutputtoBoth(1, "ACK Packet Too Long!");
				return -2;
			}
			return op;
		case OpERROR:
			if (Packetlen < 5) {
				Log_Output::OutputtoBoth(1, "ERR Packet Too Short!");
				return -1;
			}
			if (ExtractErrCode() >= 8) {
				Log_Output::OutputtoBoth(1, "Illegal Error Code!");
				return -4;
			}
			return op;
		default:
			Log_Output::OutputtoBoth(1, "Unknown Opcode!");
			return 0;
		}
	}

}