#include "Const.h"

namespace TFPT {
	//来自学长的引用共享变量的方法
	//bool showInfo = false;	
	//int SktAddrLen = sizeof(sockaddr);	
}
bool TFTP::ReadInforFromConfiguration = false;
bool TFTP::showInfo = false;
int TFTP::SktAddrLen = sizeof(sockaddr);
bool TFTP::EchoInputPara = true;
long long TFTP::GetCurrentmsTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//不是很理解这个GetCurrentmsTime为什么要写到外面才能正常编译（（