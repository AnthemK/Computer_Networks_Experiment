#include "Const.h"

namespace TFPT {
	//����ѧ�������ù�������ķ���
	//bool showInfo = false;	
	//int SktAddrLen = sizeof(sockaddr);	
}

long long TFTP::GetCurrentmsTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}