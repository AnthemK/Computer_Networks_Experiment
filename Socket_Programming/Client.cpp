#include "Client.h"

namespace TFTP {


	int Client_Class::SendPacket() {
		return sendto(Connection_Infor.Local_Socket,(char *)SendPkt.buf, SendPkt.Packetlen,0, Connection_Infor.getSktAddr(), SktAddrLen);
	}
	int Client_Class::RecivePacket() {
		int sizeofsockaddr, ans;
		Connection_Infor.Received_addr = Connection_Infor.addr; sizeofsockaddr = SktAddrLen;
		ans=recvfrom(Connection_Infor.Local_Socket, (char *)ReceivePkt.buf, DefBufSize, 0, (sockaddr* )&Connection_Infor.Received_addr, &sizeofsockaddr);
		if (sizeofsockaddr != SktAddrLen || Connection_Infor.Received_addr.sin_addr.S_un.S_addr != Connection_Infor.addr.sin_addr.S_un.S_addr) return -1;
		else return ans;
		return ans;
		//Connection_Infor.Received_addrΪ���յ������ݰ����׽��֣���һ����Ŀ��ͨ�ŷ��ġ�
		// ���Ƿ�ΪĿ�꽫�ڴ˺����ĸ��������ж�
		//���һ��������ʾ�Է��׽��ֵĴ�С������Ҫ�����ַ
		//��֪���Ƿ�sizeofsockaddr==sizeof(sockaddr)��ʱ��ų���
	}
	int Client_Class::AssemblyRQPacket() {
		return SendPkt.PackRQ(Connection_Infor.FunctionType, Connection_Infor.FilePath, File_DataMode);
	}
	int Client_Class::SendACK(int Now_BlkNo) {
		SendPkt.PackACK(Now_BlkNo);
		return SendPacket();

	}
	int Client_Class::BestEffort_Send() {       //�����
		Connection_Infor.ResendTimer = GetCurrentmsTime() + DefTimeOut;  //�����ʱ��֮���ش�
		Connection_Infor.RemainResendNum = DefRetries;     //�ش�����
		uint16 Now_OpCode = SendPkt.ExtractOpCode();
		uint16 Now_Blkno, Received_blk;
		Connection_Infor.TotPacketNum++;
		if (Now_OpCode == OpRRQ) Now_Blkno = 1;
		else if (Now_OpCode == OpWRQ) Now_Blkno = 0;
		else if (Now_OpCode == OpDATA) Now_Blkno = SendPkt.ExtractBlockNo();      //�յ����ACK֮��Ż�ֹͣ����
		else {
			Log_Output::OutputtoBoth(1, "Send a Packet which is not RRQ or WRQ or DATA with best-effort!");
			Now_OpCode = -1;
			//???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
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
				if (GetCurrentmsTime() >= Connection_Infor.ResendTimer && Connection_Infor.RemainResendNum <= 0) {  //��ʱ�����ش������ù���
					Log_Output::OutputtoBoth(1, "Can not Recieve Response for packet!");
					return -1;
				}
				else if (GetCurrentmsTime() >= Connection_Infor.ResendTimer) {   //��ʱ��
					err = SendPacket();
					if (err <= 0) {
						Log_Output::OutputtoBoth(1, "Send Packet Error!");
						return -1;
					}
					Log_Output::OutputtoBoth(4, "Resend Packet!");
					Connection_Infor.ResendTimer = GetCurrentmsTime() + DefTimeOut;
					Connection_Infor.RemainResendNum--;
					Connection_Infor.TotPacketNum++;     
				}
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
			ReceivePkt.setPacketLen(err);		//���ý��հ�����

			if (showInfo)
			{
				Log_Output::Log_Msg << " In Best-Effort Send ,Received :\n";
				Parse_Print_Packet(ReceivePkt);
			}

			uint16 op = ReceivePkt.CheckPacket();	//�����հ���ͬʱҲ��ȡ����opֵ
			if (op <= 0) {
				Log_Output::OutputtoBoth(3, "Recive a packet which is illegal!");
				Connection_Infor.ResendTimer = GetCurrentmsTime() + (DefTimeOut>>1);  //��Ϊ���ܵİ������⣬���Բ�����ȫ��ͬ��һ����ȷ��˳�����İ�����˲������ַ�ʽ
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			} 
			else if (op == OpERROR) {
				Parse_ErrorPackets(ReceivePkt);
				return -2;
			}
			else if ((Now_OpCode == OpDATA|| Now_OpCode == OpWRQ) && op == OpACK) {
				Received_blk = ReceivePkt.ExtractBlockNo();	//��ȡ���ݿ���
				if (Received_blk == Now_Blkno) {
					Connection_Infor.SuccessBytes += SendPkt.getDataLen(); //���Ӵ���ɹ����ֽ���
					if (Now_OpCode == OpDATA) {
						Log_Output::Log_Msg << "DATA Packet Blk " << Now_Blkno << " Send Success!"<<endl;
						Log_Output::OutputtoBoth(2, NULL);    //Ϊ�˲��Լ����ٶ�
					}
					else {
						Log_Output::OutputtoBoth(2, "WRQ Packet Send Success!");
					}
					return 0;		//��Ϊ��ǰ�µ����ݿ��򷵻�
				}
				//��Ϊ֮ǰ�����ݿ鶪�������ü�ʱ��       ��Ϊ�˴����ܵ��Ƿ�����֮ǰ���͵İ�������ζ����Ҫ�İ��ܿ������ϵ��� ����ֱ�����¼�ʱ
				Connection_Infor.ResendTimer = GetCurrentmsTime() + DefTimeOut;
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
				Connection_Infor.ResendTimer = GetCurrentmsTime() + DefTimeOut;
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
			else {
				Log_Output::OutputtoBoth(3, "The received packet is not what I want!");
				Connection_Infor.ResendTimer = GetCurrentmsTime() + (DefTimeOut >> 1);   //�ô�Ҳ���յ����ˣ����Լ���һ���ʱ��
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
			if (showInfo)
			{
				Log_Output::Log_Msg << " In WaitingForDATA :\n";
				Parse_Print_Packet(ReceivePkt);
			}

			uint16 op = ReceivePkt.CheckPacket();	//�����հ�
			uint16 Received_blk = ReceivePkt.ExtractBlockNo();	//��ȡ���ݿ���
			if (op <= 0) {
				Log_Output::OutputtoBoth(3, "Receive a packet which is illegal!");
				Sleep(DefSleepTime);   //˯��һ������ȴ�����
				continue;
			}
			else if (op == OpERROR) {
				Parse_ErrorPackets(ReceivePkt);
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
		Connection_Infor.Begin_Time = GetCurrentmsTime();
		AssemblyRQPacket();
		if (showInfo)
		{
			Log_Output::Log_Msg  << " Connection Request Packet:\n";
			Parse_Print_Packet(SendPkt);
		}
		if (BestEffort_Send()) {
			Log_Output::OutputtoBoth(1, "Connection Error!");
			return -1;
		}
		Connection_Infor.SuccessPacketNum++;
		Log_Output::OutputtoBoth(2, "Connection Success!!!");
		Connection_Infor.addr.sin_port = Connection_Infor.Received_addr.sin_port;
		Log_Output::Log_Msg << "Success Connection With Server:\nIP: " << inet_ntoa(Connection_Infor.addr.sin_addr) << "  Port:" << ntohs(Connection_Infor.addr.sin_port) << '\n';
		//FromsockaddrPrintIPandPort(Connection_Infor.addr);
		Log_Output::OutputtoBoth(4, NULL);
		return 0;
	}
	int Client_Class::Upload_File() {
		Now_BlkNO = 0;
		Connection_Infor.SuccessBytes = 0;
		Connection_Infor.SuccessPacketNum = 0;
		Connection_Infor.TotPacketNum = 0;
		if (Make_Connection()) {
			return -1;
		}
		Now_BlkNO++;
		char SendData[DefBufSize] = "";
		int lenofdata = 512;
		while (lenofdata == 512) {
			lenofdata = fread(SendData, 1, DataMaxSize, Connection_Infor.Local_FilePointer);
			SendPkt.PackDATA(Now_BlkNO,SendData,lenofdata);
			if (showInfo)
			{
				Log_Output::Log_Msg << "lenofdata :\n" << lenofdata<<"\n";
				Log_Output::Log_Msg << "Send DATA Packet :\n";
				Parse_Print_Packet(SendPkt);
			}
			if (BestEffort_Send()) {
				return -1;
			}

			Connection_Infor.SuccessPacketNum++;
			Now_BlkNO++;
		}
		Log_Output::Log_Msg << "Upload Mession Success!!!\n" << "File Name: " << Connection_Infor.FilePath << "\n" << "In " << std::dec << GetCurrentmsTime() - Connection_Infor.Begin_Time << " MillionSeconds\n" << "Totally transmitted " << Connection_Infor.SuccessBytes << " Bytes\n"  << "Average Speed:" << Connection_Infor.SuccessBytes * 1000 / (GetCurrentmsTime() - Connection_Infor.Begin_Time) << " Bytes per Second\n" << "Packet Loss Rate " << (1-sqrt((double)Connection_Infor.SuccessPacketNum / (double)Connection_Infor.TotPacketNum)) << "\n";
		Log_Output::OutputtoBoth(2, NULL);
	}
	int Client_Class::Download_File() {
		Now_BlkNO = 1;
		Connection_Infor.SuccessBytes = 0;
		Connection_Infor.SuccessPacketNum = 0;
		Connection_Infor.TotPacketNum = 0;
		if (Make_Connection()) {
			return -1;
		}
		int lenofdata;
		if (ReceivePkt.ExtractBlockNo() == Now_BlkNO) {
			lenofdata = fwrite((char*)ReceivePkt.GetByteAddr(4), 1, ReceivePkt.getDataLen(), Connection_Infor.Local_FilePointer);
			Connection_Infor.SuccessBytes += ReceivePkt.getDataLen(); //���Ӵ���ɹ����ֽ���
			Now_BlkNO++;
		}
		SendACK(ReceivePkt.ExtractBlockNo());
		while (ReceivePkt.getDataLen() == 512) {
			if (WaitingForDATA()) {
				return -1;
			}
			SendPkt.PackACK(ReceivePkt.ExtractBlockNo());
			if (ReceivePkt.ExtractBlockNo() == Now_BlkNO) {
				lenofdata = fwrite((char *)ReceivePkt.GetByteAddr(4),1, ReceivePkt.getDataLen(), Connection_Infor.Local_FilePointer);
				Connection_Infor.SuccessBytes += ReceivePkt.getDataLen(); //���Ӵ���ɹ����ֽ���
				Log_Output::Log_Msg << "Download Data Blk " << Now_BlkNO << " Success!!!\n";
				if(showInfo) Log_Output::Log_Msg <<"In " << std::dec << GetCurrentmsTime() - Connection_Infor.Begin_Time << " MillionSeconds\n" << "Totally transmitted " << Connection_Infor.SuccessBytes << " Bytes\n";
				Log_Output::OutputtoBoth(2, NULL);
				Now_BlkNO++;
				Connection_Infor.SuccessPacketNum++;
			}
			SendACK(ReceivePkt.ExtractBlockNo());
			Connection_Infor.TotPacketNum++;
		}
		Log_Output::Log_Msg << "Download Mession Success!!!\n" <<"File Name: "<< Connection_Infor.FilePath << "\n" << "In " << std::dec << GetCurrentmsTime() - Connection_Infor.Begin_Time << " MillionSeconds\n" << "Totally transmitted " << Connection_Infor.SuccessBytes << " Bytes\n" << "Average Speed:" << Connection_Infor.SuccessBytes * 1000 / (GetCurrentmsTime() - Connection_Infor.Begin_Time) << " Bytes per Second\n" << "Packet Loss Rate " <<(1- sqrt((double)Connection_Infor.SuccessPacketNum / (double )Connection_Infor.TotPacketNum))<<"\n";
		Log_Output::OutputtoBoth(2, NULL);

	}
}