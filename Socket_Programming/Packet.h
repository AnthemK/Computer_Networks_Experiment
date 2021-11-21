#ifndef _PACKET_H
#define _PACKET_H
//#pragma once   //Ҳ������ֻ����һ�ε����ã�����������Ҫ��һЩ
#include "Const.h"
#include "Log_Output.h"

namespace TFTP {
	class Packet {
	public:
		byte buf[DefBufSize];	//���ݰ�������  1024
		int Packetlen;	//���ݰ���Ч����  
		Packet() :buf(""), Packetlen(0) {}   //���캯������ʼ��

		void setPacketLen(const int l = 0);  //�������ݰ���Ч����
		int getDataLen() const;    //��ȡDATA�������ݳ���

		void addByte(byte add_val);         //���һ���ֽ�
		void addUint16(uint16 add_val);     //��������ֽ� ���������û�н����ֽڷ�ת��
		void addChars(const char* str);        //����ַ���
		void addBytes(const byte* str, int strLenth);        //����ֽڴ�����Ҫָ������
		void setOp(uint16 Opcode);    //�������ݰ��Ĳ�����


		byte* GetByteAddr(int Aimpls);  //���buf+Aimpls���ĵ�ַ
		const byte* getErrMsg() const;   //���ErrMsg��Ϣ 
		const byte* getData() const;  //��ȡDATA��������(����ַ)

		uint16 ExtractUint16(int base = 0) const;      //����baseλ�ÿ�ʼ�������ֽڳ�16λ�޷����������������Ѿ���תΪ����˳���ˣ�
		uint16 ExtractOpCode();     //����Opcode
		uint16 ExtractBlockNo();   //����BlockNo
		uint16 ExtractErrCode();   //����ErrCode


		bool PackRQ(uint16 OpRQ, const char* Filename, const int Modetype); //opRQ��ʾR/Q����   FileNameΪ������ļ���   typΪ0 ��������ƣ�����netascii��
		bool PackDATA(uint16 blockNo, const char* data); //��װDATA�� blockNo��ʾ���ı�ţ�dataΪ����
		void PackACK(uint16 blockNo);   //��װһ��ACK��
		void PackERROR(uint16 errCode, const char* msg = nullptr);   //��װһ��ERROR��  errCodeΪ1-7����������msg���ٷ���ظ�δ�������

		uint16 CheckPacket();		//�˲����ݰ������Լ������ĺϷ����ж�,��ҪԤ�Ȼ�ȡ���Լ�Packetlen
	};


}



#endif