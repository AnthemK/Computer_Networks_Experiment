#include "Client.h"

namespace TFTP {


	int Client_Class::SendPacket() {
		return sendto(Connection_Infor.Local_Socket,(char *)SendPkt.buf, SendPkt.Packetlen,0, Connection_Infor.getSktAddr(), SktAddrLen);
	}
	int Client_Class::RecivePacket() {
		int sizeofsockaddr, ans;
		ans=recvfrom(Connection_Infor.Local_Socket, (char *)ReceivePkt.buf, DefBufSize, 0, (sockaddr* )&Connection_Infor.Received_addr, &sizeofsockaddr);
		if (sizeofsockaddr != SktAddrLen) return -1;
		else return ans;
		//Connection_Infor.Received_addr为接收到的数据包的套接字，不一定是目标通信方的。
		// 其是否为目标将在此函数的父函数里判断
		//最后一个参数表示对方套接字的大小，所以要传入地址
		//？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
		//不知道是否sizeofsockaddr==sizeof(sockaddr)的时候才成立
	}
	int Client_Class::AssemblyRQPacket() {
		SendPkt.PackRQ(Connection_Infor.FunctionType, Connection_Infor.FilePath, File_DataMode);
	}
	int Client_Class::Parse_ErrorPackets() {
		Log_Output::Log_Msg << " Received a error-packet Code:" << ReceivePkt.ExtractErrCode();
		if (ReceivePkt.Packetlen > 4)
			Log_Output::Log_Msg<<'\n' << " With message:" << ReceivePkt.getErrMsg();
		Log_Output::Log_Msg << "\n";
		Log_Output::OutputtoBoth(1, NULL);
	}
	int Client_Class::BestEffort_Send() {       //待检查
		Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;  //在这个时间之后重传
		Connection_Infor.RemainResendNum = DefRetries;     //重传次数
		uint16 Now_OpCode = SendPkt.ExtractOpCode();
		uint16 Now_Blkno, Received_blk;
		if (Now_OpCode == OpRRQ) Now_Blkno = 1;
		else if (Now_OpCode == OpWRQ) Now_Blkno = 0;
		else if (Now_OpCode == OpDATA) Now_Blkno = SendPkt.ExtractBlockNo();      //收到这个ACK之后才会停止发送
		else {
			Log_Output::OutputtoBoth(1, "Send a Packet which is not RRQ or WRQ or DATA with best-effort!");
			Now_OpCode = -1;
			//?????????????????????????????????????????????????????????????????????????????????????????????????????????
			//是否需要继续运行
		}
		err=SendPacket();
		if (err <= 0) {
			Log_Output::OutputtoBoth(1, "Send Packet Error!");
			return -1;
		}
		while (true) {
			err = RecivePacket();	//进行一次接收
			if (err <= 0) {
				if (time(nullptr) >= Connection_Infor.ResendTimer && Connection_Infor.RemainResendNum <= 0) {  //超时并且重传次数用光了
					Log_Output::OutputtoBoth(1, "Can not Recieve Response for packet!");
					return -1;
				}
				else if (time(nullptr) >= Connection_Infor.ResendTimer) {   //超时了
					err = SendPacket();
					if (err <= 0) {
						Log_Output::OutputtoBoth(1, "Send Packet Error!");
						return -1;
					}
					Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
					Connection_Infor.RemainResendNum--;
				}
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
			ReceivePkt.setPacketLen(err);		//设置接收包长度
			uint16 op = ReceivePkt.CheckPacket();	//检测接收包，同时也获取包的op值
			if (op <= 0) {
				Log_Output::OutputtoBoth(3, "Recive a packet which is illegal!");
				Connection_Infor.ResendTimer = time(NULL) + (DefTimeOut>>1);  //因为接受的包有问题，所以不能完全等同于一个正确的顺序错误的包，因此采用这种方式
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			} 
			else if (op == OpERROR) {
				Parse_ErrorPackets();
				return -2;
			}
			else if ((Now_OpCode == OpDATA|| Now_OpCode == OpWRQ) && op == OpACK) {
				Received_blk = ReceivePkt.ExtractBlockNo();	//获取数据块编号
				if (Received_blk == Now_Blkno) {
					Connection_Infor.SuccessBytes += SendPkt.getDataLen(); //增加传输成功的字节数
					if (Now_OpCode == OpDATA) {
						Log_Output::OutputtoBoth(2, "DATA Packet Send Success!");
					}
					else {
						Log_Output::OutputtoBoth(2, "WRQ Packet Send Success!");
					}
					return 0;		//若为当前新的数据块则返回
				}
				//若为之前的数据块丢掉并重置计时器       因为此处接受的是服务器之前发送的包，这意味着想要的包很可能马上到达 所以直接重新计时
				Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
			else if (Now_OpCode == OpRRQ && op == OpDATA) {
				Received_blk = ReceivePkt.ExtractBlockNo();	//获取数据块编号
				if (Received_blk == Now_Blkno){
					Connection_Infor.SuccessBytes += SendPkt.getDataLen(); //增加传输成功的字节数
					Log_Output::OutputtoBoth(2, "RRQ Packet Send Success!");
					return 0;		//若为当前新的数据块则返回
				}
				//若为之前的数据块丢掉并重置计时器
				Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
			else {
				Log_Output::OutputtoBoth(3, "The received packet is not what I want!");
				Connection_Infor.ResendTimer = time(NULL) + (DefTimeOut >> 1);   //好歹也算收到包了，所以加上一半的时间
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
		}
	}
	int Client_Class::WaitingForDATA() {      
		while (true) {
			err = RecivePacket();	//进行一次接收
			if (err <= 0) {
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
			ReceivePkt.setPacketLen(err);		//设置接收包长度
			uint16 op = ReceivePkt.CheckPacket();	//检测接收包
			uint16 Received_blk = ReceivePkt.ExtractBlockNo();	//获取数据块编号
			if (op <= 0) {
				Log_Output::OutputtoBoth(3, "Receive a packet which is illegal!");
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
			else if (op == OpERROR) {
				Parse_ErrorPackets();
				return -2;
			}
			else if (op == OpDATA) {
				return 0;
			}
			else {
				Log_Output::OutputtoBoth(3, "Receive a packet which is wrong!");
				continue;
			}
		}
	}
	int Client_Class::Make_Connection() {
		Connection_Infor.Begin_Time = time(NULL);
		AssemblyRQPacket();
		if (BestEffort_Send()) {
			Log_Output::OutputtoBoth(1, "Make Connection Error!");
			return -1;
		}
		Connection_Infor.addr.sin_port = Connection_Infor.Received_addr.sin_port;
		return 0;
	}
	int Client_Class::Upload_File() {
		Now_BlkNO = 0;
		Connection_Infor.SuccessBytes = 0;
		if (Make_Connection()) {
			return -1;
		}
		Now_BlkNO++;
		char SendData[DefBufSize] = "";
		int lenofdata = 0;
		while (lenofdata == 512) {
			lenofdata = fread(SendData, 1, DataMaxSize, Connection_Infor.Local_FilePointer);
			SendPkt.PackDATA(Now_BlkNO,SendData,lenofdata);
			if (BestEffort_Send()) {
				return -1;
			}
			Now_BlkNO++;
		}
	}
	int Client_Class::Download_File() {
		Now_BlkNO = 1;
		Connection_Infor.SuccessBytes = 0;
		if (Make_Connection()) {
			return -1;
		}
		int lenofdata;
		while (ReceivePkt.Packetlen == 512) {
			if (WaitingForDATA()) {
				return -1;
			}
			SendPkt.PackACK(ReceivePkt.ExtractBlockNo());
			if (ReceivePkt.ExtractBlockNo() == Now_BlkNO) {
				lenofdata = fwrite((char *)ReceivePkt.GetByteAddr(4),1, ReceivePkt.getDataLen(), Connection_Infor.Local_FilePointer);
				Connection_Infor.SuccessBytes += ReceivePkt.getDataLen(); //增加传输成功的字节数
				Now_BlkNO++;
			}
		}
	}
}