#include "Const.h"

namespace TFPT {
	//来自学长的引用共享变量的方法
	//bool showInfo = false;	
	//int SktAddrLen = sizeof(sockaddr);	
}

long long TFTP::GetCurrentmsTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}