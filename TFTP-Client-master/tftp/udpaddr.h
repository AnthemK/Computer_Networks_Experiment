#pragma once
#include "const.h"

namespace tftp {
	//UDP地址
	class UDPAddr {
		static const uint16 DefPort = DefSvrPort;	//默认端口
		static const uint DefIp = 0x0100007f;	//默认IP:127.0.0.1
		sockaddr_in addr;	//IP地址
	public:
		UDPAddr(){ 
			addr.sin_port = htons(DefPort);
			addr.sin_addr.S_un.S_addr = DefIp;
		}
		UDPAddr(const string& addrStr);
		UDPAddr(const char* ip, uint16 port);
		UDPAddr(byte a, byte b, byte c, byte d, uint16 port);
		//输出地址字符串
		void printAddrStr() const;
		//获取地址字符串
		string getAddrStr() const;	
		//获取IP地址指针
		sockaddr* getSktAddr() { return (sockaddr*)&addr; }
	};
}
