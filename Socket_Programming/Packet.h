#ifndef _PACKET_H
#define _PACKET_H
//#pragma once   //Ҳ������ֻ����һ�ε����ã�����������Ҫ��һЩ
#include "Client_Const.h"

namespace TFTP {
	class Packet {
	public:
		
		byte buf[DefBufSize];	//���ݰ�������  1024
		int Packetlen;	//���ݰ���Ч����  
		Packet() :buf(""), Packetlen(0) {}

		void setPacketLen(const int l = 0);  //�������ݰ���Ч����
		int getDataLen() const;    //��ȡDATA�������ݳ���

		void addByte(byte add_val);         //���һ���ֽ�
		void addUint16(uint16 add_val);     //��������ֽ� ���������û�н����ֽڷ�ת��
		void addChars(const char* str);        //����ַ���
		void addBytes(const byte* str, int strLenth);        //����ֽڴ�����Ҫָ������
		void setOp(uint16 Opcode);    //�������ݰ��Ĳ�����


		byte* GetByteAddr(int Aimpls);
		const byte* getErrMsg() const;
		//��ȡDATA��������(����ַ)
		const byte* getData() const;

		//������16λ�޷�����
		uint16 ExtractUint16(int base = 0) const;
		uint16 ExtractOpCode();
		uint16 ExtractBlockNo();
		uint16 ExtractErrCode();


		bool PackRQ(uint16 OpRQ, const char* Filename, const int Modetype); //opRQ��ʾR/Q����   FileNameΪ������ļ���   typΪ0 ��������ƣ�����netascii��
		bool PackDATA(uint16 blockNo, const char* data); //��װDATA�� blockNo��ʾ���ı�ţ�dataΪ����
		//��װACK��
		void PackACK(uint16 blockNo);
		//��װERROR��
		void PackERROR(uint16 errCode, const char* msg = nullptr);

		//�˲����ݰ�����
		uint16 checkPacket();
	};


}



#endif