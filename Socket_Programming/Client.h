#ifndef _Client_H
#define _Client_H

#include "Const.h"
#include "Log_Output.h"
#include "General_Class.h"
#include "Packet.h"

namespace TFTP {

	class Client_Class {
		UDPInfor Connection_Infor;       //连接的通用信息
		Packet SendPkt, ReceivePkt;      //发送包和接受包
		int File_DataMode;           //文件的传输类型 0 -》octet 1-》 netascii
		int err;          //用来承接整型的错误信息数据
		uint16 Now_BlkNO;        //现在传输的包号 0-65536

		int SendPacket();       //发送一个数据包 存在SendPkt中 连接信息在Connection_Infor中
		int RecivePacket();     //接收一个数据包 存在RecivePkt中 连接信息在Connection_Infor中,套接字信息存在Recived_addr中
		int AssemblyRQPacket();    //组建一个R/Q型数据包      注意此处需要用到的变量都必须初始化
		int Parse_ErrorPackets();           //解析一个error包，并且输出
		int BestEffort_Send();         //提供一个尽力而为的发送（超时重传），只针对data包和RW请求包   把SendPkt发送到Connection_Infor.addr处
		int WaitingForDATA();         //不断收听，等待一个数据包
		int Make_Connection();         //建立一个连接
		int Upload_File();         
		//进行一次上载  
		//此时默认要上传的文件名存在Connection_Infor.FilePath中  本地的文件已经打开并且指针为Connection_Infor.Local_FilePointer
		//File_DataMode，FunctionType已被确定 
		int Download_File();       //进行一次下载 此时默认要下载的文件名存在Connection_Infor.FilePath中  本地的文件已经打开并且指针为Connection_Infor.Local_FilePointer
	};

}





#endif