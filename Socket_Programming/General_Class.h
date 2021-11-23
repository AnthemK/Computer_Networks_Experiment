#ifndef _GENERAL_CLASS_H
#define _GENERAL_CLASS_H

#include "Const.h"
#include "Log_Output.h"

namespace TFTP {

	int getUDPSocket();  //初始化获取本地的SOCKET
	bool PrintIP(u_long ipval);      //输出一个IP地址
	bool PrintPort(uint16 port);    //输出一个端口号，此端口号应当为网络字节序格式
	bool FromsockaddrPrintIPandPort(const sockaddr_in printed_addr);          //从sockaddr输出IP地址以及端口号
	char* GetFileName(char* FilePath);       //从文件的路径中获得文件的名字
	int CreateFilePointer(char* FilePath,int Open_Type,FILE* &Aimfp);        //创建文件指针 通过typ指定是读还是写
	//UDP地址
	class UDPInfor {
	public:
		sockaddr_in addr;	   //对方的socketaddr
		SOCKET Local_Socket;    //本地的socet
		FILE* Local_FilePointer;   //本地打开文件的指针
		int FunctionType;          //表示现在的功能类型 0表示未知  1表示下载文件  2表示上载文件
		char FilePath[100];         //文件路径字符串，当为客户端时，存储目标文件的名字 党委
		time_t Begin_Time;         //连接开始的时间


		//???????????????????????????????????????
		//自此一下没有初始化
		time_t ResendTimer;         //重传计时器
		int RemainResendNum;       //剩余的重传次数
		int SuccessBytes;          //已经传输成功的字节数，ACK后才会被计算
		sockaddr_in Received_addr;     //接收包的sockaddr  初始时悬空？


		UDPInfor();
		UDPInfor(const char* ip, uint16 port);   //从一个字符串初始化IP地址以及端口号
		UDPInfor(byte a, byte b, byte c, byte d, uint16 port);  //从四个byte初始化IP 地址 以及端口号
		
		uint16 ChangePort(uint16 NewPort);     //改变ip地址 输入的应当为正常字节序
		uint64 ChangeIP(const char* ip);      //改变端口号  正常字节序输入
		sockaddr* getSktAddr();   //获取sockaddr型指针
	};
}





#endif 