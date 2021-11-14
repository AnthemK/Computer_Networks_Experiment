#include "udpaddr.h"

namespace tftp {
	UDPAddr::UDPAddr(byte a, byte b, byte c, byte d, uint16 port) {
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_un_b.s_b1 = a;
		addr.sin_addr.S_un.S_un_b.s_b1 = a;
		addr.sin_addr.S_un.S_un_b.s_b2 = b;
		addr.sin_addr.S_un.S_un_b.s_b3 = c;
		addr.sin_addr.S_un.S_un_b.s_b4 = d;
		addr.sin_port = htons(port);
	}


	UDPAddr::UDPAddr(const string& addrStr) {
		int col = addrStr.find(':');
		if (col == addrStr.npos) addr.sin_port = htons(DefPort);
		else addr.sin_port = htons((uint16)stoi(addrStr.substr(col + 1)));
		addr.sin_family = AF_INET;
		int ret = inet_pton(AF_INET, addrStr.substr(0, col).c_str(),
			&addr.sin_addr);
		if (ret != 1) addr.sin_addr.S_un.S_addr = DefIp;
	}


	UDPAddr::UDPAddr(const char* ip, uint16 port) {
		addr.sin_port = htons(port);
		addr.sin_family = AF_INET;
		int ret = inet_pton(AF_INET, ip, &addr.sin_addr);
		if (ret != 1) addr.sin_addr.S_un.S_addr = DefIp;
	}

	void UDPAddr::printAddrStr() const {
		cout << (int)addr.sin_addr.S_un.S_un_b.s_b1 << '.'
			<< (int)addr.sin_addr.S_un.S_un_b.s_b2 << '.'
			<< (int)addr.sin_addr.S_un.S_un_b.s_b3 << '.'
			<< (int)addr.sin_addr.S_un.S_un_b.s_b4
			<< ':' << ntohs(addr.sin_port) << endl;
	}

	string UDPAddr::getAddrStr() const {
		ostringstream ost;
		ost << inet_ntoa(addr.sin_addr) << ':' << ntohs(addr.sin_port);
		return ost.str();
	}

	

}