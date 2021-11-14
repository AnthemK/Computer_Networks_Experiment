#pragma once
#include "const.h"

namespace tftp {
	//UDP��ַ
	class UDPAddr {
		static const uint16 DefPort = DefSvrPort;	//Ĭ�϶˿�
		static const uint DefIp = 0x0100007f;	//Ĭ��IP:127.0.0.1
		sockaddr_in addr;	//IP��ַ
	public:
		UDPAddr(){ 
			addr.sin_port = htons(DefPort);
			addr.sin_addr.S_un.S_addr = DefIp;
		}
		UDPAddr(const string& addrStr);
		UDPAddr(const char* ip, uint16 port);
		UDPAddr(byte a, byte b, byte c, byte d, uint16 port);
		//�����ַ�ַ���
		void printAddrStr() const;
		//��ȡ��ַ�ַ���
		string getAddrStr() const;	
		//��ȡIP��ַָ��
		sockaddr* getSktAddr() { return (sockaddr*)&addr; }
	};
}
