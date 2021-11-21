#include "Client.h"

namespace TFTP {


	int Client_Class::SendPacket() {
		return sendto(Connection_Infor.Local_Socket,(char *)SendPkt.buf, SendPkt.Packetlen,0, Connection_Infor.getSktAddr(), SktAddrLen);
	}
	int Client_Class::RecivePacket() {
		return recvfrom(Connection_Infor.Local_Socket, (char *)RecivePkt.buf, DefBufSize, 0, Connection_Infor.getSktAddr(), &SktAddrLen);  
		//���һ����������ְ�����
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
		Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;  //�����ʱ��֮���ش�
		Connection_Infor.RemainResendNum = DefRetries;     //�ش�����
		uint16 Now_Blkno = SendPkt.ExtractBlockNo();      //�յ����ACK֮��Ż�ֹͣ����
		err=SendPacket();
		if (err <= 0) {
			Log_Output::OutputtoBoth(1, "Send Packet Error!");
			return -1;
		}
		while (1) {
			while (true) {
				err = RecivePacket();	//����һ�ν���
				if (err <= 0) {
					if (time(nullptr) >= Connection_Infor.ResendTimer && Connection_Infor.RemainResendNum <= 0) {  //��ʱ�����ش������ù���
						Log_Output::OutputtoBoth(1, "Can not Recieve Response for packet!");
						return -1;
					}
					else if (time(nullptr) >= Connection_Infor.ResendTimer) {   //��ʱ��
						err = SendPacket();
						if (err <= 0) {
							Log_Output::OutputtoBoth(1, "Send Packet Error!");
							return -1;
						}
						Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
						Connection_Infor.RemainResendNum--;
					}
					Sleep(DefSleepTime);   //˯��һ������ȴ�����
					continue;
				}
				RecivePkt.setPacketLen(err);		//���ý��հ�����
				uint16 op = RecivePkt.CheckPacket();	//�����հ�
				if (op == 0) {
					Log_Output::OutputtoBoth(3, "Recive a packet which is illegal!");
					Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
					Sleep(DefSleepTime);   //˯��һ������ȴ�����
					continue;
				} 
				else if (op == OpERROR) {
					Parse_ErrorPackets(Now_Blkno);
					return -2;
				}
				else if()
				else if (op == OpACK) {
					uint16 blk = recvPkt.getBlkNo();	//��ȡ���ݿ���
					if (blk == blkNo) return 1;		//��Ϊ��ǰ�µ����ݿ��򷵻�
					//��Ϊ֮ǰ�����ݿ鶪�������ü�ʱ��
					Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
					Sleep(DefSleepTime);   //˯��һ������ȴ�����
					continue;
				}
				//��Ϊ�����ļ����ܵ���Ϊ���ݰ���ֱ�ӷ���
				else if (!isSender && op == OpDATA) {
					return 1;
				}
				//�ܵ������
				else if (op == OpERROR) {
					Logger::ost << "Send packet " << recvPkt.getBlkNo()
						<< " got error-packet  code:" << recvPkt.getErrCode();
					if (rl > 4)
						Logger::ost << " message:" << recvPkt.getErrMsg();
					Logger::ost << ".\n";
					Logger::logshow(LogERR);
					return -2;
				}
				//�յ����̵����ݰ�
				else if (op == 0) {
					Logger::ost << "Send packet " << recvPkt.getBlkNo()
						<< " got packet too short.\n";
					Logger::logshow(LogERR);
					sendERROR(4);	//���ʹ����
					return -2;
				}
				//�յ��������͵����ݰ�
				else {
					Logger::ost << "Send packet " << recvPkt.getBlkNo()
						<< " got mistake packet.\n";
					Logger::logshow(LogERR);
					sendERROR(4);	//���ʹ����
					return -2;
				}
			}
			return -2;
		}

	}
	int Client_Class::Make_Connection() {
		AssemblyRQPacket();
		Connection_Infor.Begin_Time = time(NULL);
	}
}