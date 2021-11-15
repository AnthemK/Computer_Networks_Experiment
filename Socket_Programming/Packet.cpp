#include "Packet.h"

namespace TFTP{

	void Packet::setPacketLen(const int val) { Packetlen = val; }
	int Packet::getDataLen() const { return Packetlen - 4; }


	void Packet::addByte(byte add_val) {
		memcpy(buf + Packetlen, &add_val, 1);
		Packetlen++;
	}
	void Packet::addUint16(uint16 add_val) {
		add_val = htons(add_val);	//一定要转换字节顺序
		memcpy(buf + Packetlen, &add_val, 2);
		Packetlen += 2;
	}
	void Packet::addChars(const char* AddStr) {
		int strLenth = strlen(AddStr);
		memcpy(buf + Packetlen, AddStr, strLenth+1);       //注意类型,返回0所在位置
		Packetlen += (strLenth + 1);		//因此长度要加上最后的'\0'
	}
	void Packet::addBytes(const byte* AddStr,int strLenth) {   //要指定长度
		memcpy(buf + Packetlen, AddStr, strLenth );      
		Packetlen += strLenth;		
	}
	void Packet::setOp(uint16 Opcode) { 
		Opcode = htons(Opcode);  //翻转
		memcpy(buf, &Opcode, 2);
	} 

	byte* Packet::GetByteAddr(int Aimpls) {return buf + Aimpls;}
	const byte* Packet::getErrMsg() const { return buf + 4;}
	//获取DATA包的数据(基地址)
	const byte* Packet::getData() const { return buf + 4; }


	uint16 Packet::ExtractUint16(int base) const {
		uint16 ext_val;
		memcpy(&ext_val, buf + base, 2);
		return ntohs(ext_val);  //记得转换顺序
	}
	uint16 Packet::ExtractOpCode() {return ExtractUint16(0);}
	uint16 Packet::ExtractBlockNo() { return ExtractUint16(2); }
	uint16 Packet::ExtractErrCode() { return ExtractUint16(2); }

	bool Packet::PackRQ(uint16 OpRQ, const char* Filename, const int Modetype) {
		if (OpRQ != 1 && OpRQ != 2) {
			//??????????????????????????????????????????????????????????????????????????
			return false;
		}
		if (Modetype != 0 && Modetype != 1) {
			//??????????????????????????????????????????????????????????????????????????
			return false;
		}
		setPacketLen();
		addUint16(OpRQ);
		addChars(Filename);
		addChars(RQMode[Modetype]);
		return true;
	}
	bool Packet::PackDATA(uint16 blockNo, const char* data) {
		if (strlen(data) > DataMaxSize) {
			//??????????????????????????????????????????????????????????????????????????
			return false;
		}
		setPacketLen();
		addUint16(OpDATA);
		addUint16(blockNo);
		addChars(data);
		return true;
	}
	void Packet::PackACK(uint16 blockNo) {
		setPacketLen();
		addUint16(OpACK);
		addUint16(blockNo);
	}
	void Packet::PackERROR(uint16 errCode, const char* msg) {
		setPacketLen();
		setOp(OpERROR);
		addUint16(errCode);
		if (errCode > 0 && errCode < 8) addChars(ErrMsg[errCode]);	//默认错误信息
		else if (msg) addChars(msg);		//自定义错误信息
		else if (errCode == 0) addChars(ErrMsg[0]);     //未定义错误
	}

	uint16 Packet::checkPacket() {
		//???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		if (Packetlen < 2) {
			cerr << "ERR: packet too short\n";
			return 0;
		}
		uint16 op = ExtractOpCode();
		switch (op) {
		case OpRRQ:
			if (Packetlen < 4) {
				cerr << "ERR: short RRQ packet\n";
				return 0;
			}
			return op;
		case OpWRQ:
			if (Packetlen < 4) {
				cerr << "ERR: short WRQ packet\n";
				return 0;
			}
			return op;
		case OpDATA:
			if (Packetlen < 4) {
				cerr << "ERR: short DATA packet\n";
				return 0;
			}
			return op;
		case OpACK:
			if (Packetlen < 4) {
				cerr << "ERR: short ACK packet\n";
				return 0;
			}
			return op;
		case OpERROR:
			if (Packetlen < 5) {
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