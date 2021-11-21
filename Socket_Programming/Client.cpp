#include "Client.h"

namespace TFTP {


	int Client_Class::SendPacket() {
		return sendto(Connection_Infor.Local_Socket,(char *)SendPkt.buf, SendPkt.Packetlen,0, Connection_Infor.getSktAddr(), SktAddrLen);
	}
	int Client_Class::RecivePacket() {
		return recvfrom(Connection_Infor.Local_Socket, (char *)RecivePkt.buf, DefBufSize, 0, Connection_Infor.getSktAddr(), &SktAddrLen);  
		//最后一个参数好奇怪啊（（
	}
	int Client_Class::AssemblyRQPacket() {
		SendPkt.setOp(Connection_Infor.FunctionType);
		SendPkt.setPacketLen(2);
		SendPkt.addChars(Connection_Infor.FilePath);
		SendPkt.addChars(RQMode[File_DataMode]);
	}
	int Client_Class::Parse_ErrorPackets(int Now_Blkno) {
		Log_Output::Log_Msg << "When Send packet " << Now_Blkno
			<< " Recived a error-packet  code:" << RecivePkt.ExtractErrCode();
		if (RecivePkt.Packetlen > 4)
			Log_Output::Log_Msg<<'\n' << " With message:" << RecivePkt.getErrMsg();
		Log_Output::Log_Msg << "\n";
		Log_Output::OutputtoBoth(1, NULL);
	}
	int Client_Class::BestEffort_Send() {
		Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;  //在这个时间之后重传
		Connection_Infor.RemainResendNum = DefRetries;     //重传次数
		uint16 Now_Blkno = SendPkt.ExtractBlockNo();      //收到这个ACK之后才会停止发送
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
			RecivePkt.setPacketLen(err);		//设置接收包长度
			uint16 op = RecivePkt.CheckPacket();	//检测接收包
			if (op == 0) {
				Log_Output::OutputtoBoth(3, "Recive a packet which is illegal!");
				Connection_Infor.ResendTimer = time(NULL) + (DefTimeOut>>1);  //因为接受的包有问题，所以不能完全等同于一个正确的顺序错误的包，因此采用这种方式
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			} 
			else if (op == OpERROR) {
				Parse_ErrorPackets(Now_Blkno);
				return -2;
			}
			else if (op == OpACK) {
				uint16 Recived_blk = RecivePkt.ExtractBlockNo();	//获取数据块编号
				if (Recived_blk == Now_Blkno) {
					Connection_Infor.SuccessBytes += SendPkt.getDataLen() - 4; //增加传输成功的字节数
					return 1;		//若为当前新的数据块则返回
				}
				//若为之前的数据块丢掉并重置计时器
				Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
			else {
				Log_Output::OutputtoBoth(3, "Recive a packet which is wrong!");
				Connection_Infor.ResendTimer = time(NULL) + (DefTimeOut >> 1);
				Sleep(DefSleepTime);   //睡眠一会儿，等待接收
				continue;
			}
		}
	}
	int Client_Class::Make_Connection() {
		AssemblyRQPacket();
		Connection_Infor.Begin_Time = time(NULL);
	}
}