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
		//Connection_Infor.Received_addrΪ���յ������ݰ����׽��֣���һ����Ŀ��ͨ�ŷ��ġ�
		// ���Ƿ�ΪĿ�꽫�ڴ˺����ĸ��������ж�
		//���һ��������ʾ�Է��׽��ֵĴ�С������Ҫ�����ַ
		//��������������������������������������������������������������������������������������������������������������
		//��֪���Ƿ�sizeofsockaddr==sizeof(sockaddr)��ʱ��ų���
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
	int Client_Class::BestEffort_Send() {       //�����
		Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;  //�����ʱ��֮���ش�
		Connection_Infor.RemainResendNum = DefRetries;     //�ش�����
		uint16 Now_OpCode = SendPkt.ExtractOpCode();
		uint16 Now_Blkno, Received_blk;
		if (Now_OpCode == OpRRQ) Now_Blkno = 1;
		else if (Now_OpCode == OpWRQ) Now_Blkno = 0;
		else if (Now_OpCode == OpDATA) Now_Blkno = SendPkt.ExtractBlockNo();      //�յ����ACK֮��Ż�ֹͣ����
		else {
			Log_Output::OutputtoBoth(1, "Send a Packet which is not RRQ or WRQ or DATA with best-effort!");
			Now_OpCode = -1;
			//?????????????????????????????????????????????????????????????????????????????????????????????????????????
			//�Ƿ���Ҫ��������
		}
		err=SendPacket();
		if (err <= 0) {
			Log_Output::OutputtoBoth(1, "Send Packet Error!");
			return -1;
		}
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
			ReceivePkt.setPacketLen(err);		//���ý��հ�����
			uint16 op = ReceivePkt.CheckPacket();	//�����հ���ͬʱҲ��ȡ����opֵ
			if (op <= 0) {
				Log_Output::OutputtoBoth(3, "Recive a packet which is illegal!");
				Connection_Infor.ResendTimer = time(NULL) + (DefTimeOut>>1);  //��Ϊ���ܵİ������⣬���Բ�����ȫ��ͬ��һ����ȷ��˳�����İ�����˲������ַ�ʽ
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			} 
			else if (op == OpERROR) {
				Parse_ErrorPackets();
				return -2;
			}
			else if ((Now_OpCode == OpDATA|| Now_OpCode == OpWRQ) && op == OpACK) {
				Received_blk = ReceivePkt.ExtractBlockNo();	//��ȡ���ݿ���
				if (Received_blk == Now_Blkno) {
					Connection_Infor.SuccessBytes += SendPkt.getDataLen(); //���Ӵ���ɹ����ֽ���
					if (Now_OpCode == OpDATA) {
						Log_Output::OutputtoBoth(2, "DATA Packet Send Success!");
					}
					else {
						Log_Output::OutputtoBoth(2, "WRQ Packet Send Success!");
					}
					return 0;		//��Ϊ��ǰ�µ����ݿ��򷵻�
				}
				//��Ϊ֮ǰ�����ݿ鶪�������ü�ʱ��       ��Ϊ�˴����ܵ��Ƿ�����֮ǰ���͵İ�������ζ����Ҫ�İ��ܿ������ϵ��� ����ֱ�����¼�ʱ
				Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
			else if (Now_OpCode == OpRRQ && op == OpDATA) {
				Received_blk = ReceivePkt.ExtractBlockNo();	//��ȡ���ݿ���
				if (Received_blk == Now_Blkno){
					Connection_Infor.SuccessBytes += SendPkt.getDataLen(); //���Ӵ���ɹ����ֽ���
					Log_Output::OutputtoBoth(2, "RRQ Packet Send Success!");
					return 0;		//��Ϊ��ǰ�µ����ݿ��򷵻�
				}
				//��Ϊ֮ǰ�����ݿ鶪�������ü�ʱ��
				Connection_Infor.ResendTimer = time(NULL) + DefTimeOut;
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
			else {
				Log_Output::OutputtoBoth(3, "The received packet is not what I want!");
				Connection_Infor.ResendTimer = time(NULL) + (DefTimeOut >> 1);   //�ô�Ҳ���յ����ˣ����Լ���һ���ʱ��
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
		}
	}
	int Client_Class::WaitingForDATA() {      
		while (true) {
			err = RecivePacket();	//����һ�ν���
			if (err <= 0) {
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
			ReceivePkt.setPacketLen(err);		//���ý��հ�����
			uint16 op = ReceivePkt.CheckPacket();	//�����հ�
			uint16 Received_blk = ReceivePkt.ExtractBlockNo();	//��ȡ���ݿ���
			if (op <= 0) {
				Log_Output::OutputtoBoth(3, "Receive a packet which is illegal!");
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
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
				Connection_Infor.SuccessBytes += ReceivePkt.getDataLen(); //���Ӵ���ɹ����ֽ���
				Now_BlkNO++;
			}
		}
	}
}