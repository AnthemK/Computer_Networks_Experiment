#ifndef _Client_H
#define _Client_H

#include "Const.h"
#include "Log_Output.h"
#include "General_Class.h"
#include "Packet.h"

namespace TFTP {

	class Client_Class {
		UDPInfor Connection_Infor;       //连接的通用信息
		Packet SendPkt, RecivePkt;      //发送包和接受包
		int File_DataMode;           //文件的传输类型 0 -》octet 1-》 netascii
		int err;          //用来承接整型的错误信息数据

		int SendPacket();       //发送一个数据包 存在SendPkt中 连接信息在Connection_Infor中
		int RecivePacket();     //接收一个数据包 存在RecivePkt中 连接信息在Connection_Infor中
		int AssemblyRQPacket();    //组建一个R/Q型数据包   注意此处需要用到的变量
		int Parse_ErrorPackets(int Now_Blkno);           //解析一个error包，并且输出
		int BestEffort_Send();         //提供一个尽力而为的发送（超时重传），只针对data包
		int Make_Connection();         //建立一个连接
	};


}





#endif