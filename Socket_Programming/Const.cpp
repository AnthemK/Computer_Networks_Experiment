#include "Const.h"

namespace TFPT {
	//����ѧ�������ù�������ķ���
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

//���Ǻ�������GetCurrentmsTimeΪʲôҪд����������������루��