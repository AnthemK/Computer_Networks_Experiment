#include <WinSock2.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <cstdlib>
#include <time.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#define DATA_SIZE 512
#define BUFLEN 255
#define PKT_MAX_RXMT 3
#define PKT_RCV_TIMEOUT 3*1000
#define SEND_ERR 1
#define FIRST_RECV_ERR 2
#define RECV_ERR 3	
#define FILE_ERR 4
#define CONNECT_ERR 5
#define ERR_PACK 6
#define PACK_CORR 7
// TFTP�Ĳ�����
#define CMD_RRQ (short)1
#define CMD_WRQ (short)2
#define CMD_DATA (short)3
#define CMD_ACK (short)4
#define CMD_ERROR (short)5
#define CMD_LIST (short)6
#define CMD_HEAD (short)7
#define DEBUG
using namespace std;
// TFTP���ݰ��ṹ��
struct tftpPacket 
{
	unsigned short cmd;	// ǰ�����ֽڱ�ʾ������
	union	// �м����ֶ�
	{
		unsigned short code;
		unsigned short block;
		char filename[2];	// filenameֻ�������ֽڣ����ǿ����������д�룬����������
	};
	char data[DATA_SIZE];// ������������ֶ�
};
sockaddr_in serverAddr, clientAddr;
SOCKET sock;	// �ͻ���socket
unsigned int addr_len;	// ip��ַ����
//unsigned long Opt = 1;
double transByte, consumeTime;
FILE* logFp;
char logBuf[512];
time_t rawTime;	//����ʱ��
tm* info;

void setnonblocking(int sockfd) {
		unsigned long on = 1;
		ioctlsocket(sockfd, FIONBIO, &on);
}



bool upload(char* filename) 
{
	int rxmt;
	double spack=0, epack=0;
	clock_t start, end;	// ��¼ʱ��
	transByte = 0;	// �����ֽ���
	int time_wait_ack, r_size, choose;	// �ȴ�ʱ��,���հ���С,�����ʽ
	tftpPacket sendPacket, rcv_packet;	// ���Ͱ�,���հ�
	// д�������ݰ�
	sendPacket.cmd = htons(CMD_WRQ);	// д�������
	cout << "please choose the file format(1.netascii  2.octet)" << endl;
	cin >> choose;
	if (choose == 1)
		sprintf(sendPacket.filename, "%s%c%s%c", filename, 0, "netascii", 0);
	else
		sprintf(sendPacket.filename, "%s%c%s%c", filename, 0, "octet", 0);
	// �����������ݰ�
	for (rxmt = 0; rxmt < PKT_MAX_RXMT; rxmt++)
	{
		if (sendto(sock, (char*)&sendPacket, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, addr_len) == SOCKET_ERROR)
		{
			continue;
			epack++;
		}
		else spack++;
		
		// �ȴ����ջ�Ӧ,���ȴ�3s,ÿ��20msˢ��һ��
		for (time_wait_ack = 0; time_wait_ack < PKT_RCV_TIMEOUT; time_wait_ack += 20)
		{
			// ���Խ���
			r_size = recvfrom(sock, (char*)&rcv_packet, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, (int*)&addr_len);
			if (r_size > 0 && r_size < 4)
				printf("Bad packet: r_size=%d\n", r_size);	// ���հ��쳣
			if (r_size >= 4 && rcv_packet.cmd == htons(CMD_ACK) && rcv_packet.block == htons(0))
			{ 
				break;	// �ɹ��յ�ACK
				spack++;
			}
			if (r_size == -1)
				Sleep(20);
		}
		if (time_wait_ack < PKT_RCV_TIMEOUT)
			break;	// ���ͳɹ�
		else // δ�յ�ACK���ش�
		{
			cout << "ACK drop.." << endl;
			epack++;
			time(&rawTime);
			info = localtime(&rawTime);
			sprintf(logBuf, "%s WARNING: upload %s, mode: %s, %s\n", asctime(info), filename, choose == 1 ? ("netascii") : ("octet"), "Can't receive ACK, Retransmission");
			fwrite(logBuf, 1, strlen(logBuf), logFp);
			continue;
		}
	}
	if (rxmt >= PKT_MAX_RXMT)
	{
		// ��ʱ,δ���ܵ�ACK
		printf("Could not receive from server.\n");
		time(&rawTime);
		info = localtime(&rawTime);
		sprintf(logBuf, "%s ERROR: upload %s, mode: %s, %s\n", asctime(info), filename, choose == 1 ? ("netascii") : ("octet"), "Could not receive from server.");
		fwrite(logBuf, 1, strlen(logBuf), logFp);
		return false;
	}
	
	FILE* fp = NULL;	// ���ļ�
	if (choose == 1)
		fp = fopen(filename, "r");
	else
		fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("File not exists!\n");
		time(&rawTime);
		info = localtime(&rawTime);
		sprintf(logBuf, "%s ERROR: upload %s, mode: %s, %s\n",asctime(info), filename, choose == 1 ? ("netascii") : ("octet"),"File not exists!");
		fwrite(logBuf, 1, strlen(logBuf), logFp);
		return false;
	}
	int s_size = 0;
	unsigned short block = 1;
	sendPacket.cmd = htons(CMD_DATA);	// ���ݰ�������
	// �������ݰ�
	start = clock();
	do 
	{
		memset(sendPacket.data, 0, sizeof(sendPacket.data));
		sendPacket.block = htons(block);	// д�����
		s_size = fread(sendPacket.data, 1, DATA_SIZE, fp);	// ��������
		transByte += s_size;
		// ����ش�3��
		for (rxmt = 0; rxmt < PKT_MAX_RXMT; rxmt++) 
		{
			// �������ݰ�
			if (sendto(sock, (char*)&sendPacket, s_size + 4, 0, (struct sockaddr*)&serverAddr, addr_len) == SOCKET_ERROR)
			{
				continue;
				epack++;
			}
			else spack++;
			printf("Send the %d block\n", block);
			// �ȴ�ACK,���ȴ�3s,ÿ��20msˢ��һ��
			for (time_wait_ack = 0; time_wait_ack < PKT_RCV_TIMEOUT; time_wait_ack += 20) 
			{
				r_size = recvfrom(sock, (char*)&rcv_packet, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, (int*)&addr_len);
				if (r_size > 0 && r_size < 4) 
					printf("Bad packet: r_size=%d\n", r_size);
				if (r_size >= 4 && rcv_packet.cmd == htons(CMD_ACK) && rcv_packet.block == htons(block))
				{
					break;
					spack++;
				}
				if (r_size == -1)
					Sleep(20);
			}
			if (time_wait_ack < PKT_RCV_TIMEOUT) 
				break;	// ���ͳɹ�
			else // δ�յ�ACK���ش�
			{
				cout << "ACK drop.." << endl;
				epack++;
				time(&rawTime);
				info = localtime(&rawTime);
				sprintf(logBuf, "%s WARNING: upload %s, mode: %s, %s\n",asctime(info), filename, choose == 1 ? ("netascii") : ("octet"),"Can't receive ACK, Retransmission");
				fwrite(logBuf, 1, strlen(logBuf), logFp);
				continue;
			}
		}
		if (rxmt >= PKT_MAX_RXMT) 
		{
			// 3���ش�ʧ��
			printf("Could not receive ACK from server.\n");
			fclose(fp);
			time(&rawTime);
			info = localtime(&rawTime);
			sprintf(logBuf, "%s ERROR: upload %s, mode: %s, %s\n",asctime(info), filename, choose == 1 ? ("netascii") : ("octet"),"Could not receive ACK from server.");
			fwrite(logBuf, 1, strlen(logBuf), logFp);
			return false;
		}
		block++;	// ������һ�����ݿ�
	} while (s_size == DATA_SIZE);	// �����ݿ�δװ��ʱ��Ϊʱ���һ�����ݣ�����ѭ��
	end = clock();
	printf("Send file end.\n");
	fclose(fp);
	consumeTime = ((double)(end - start)) / CLK_TCK;
	printf("upload file size: %.2f kB consuming time: %.3f s\n", transByte / 1024, consumeTime);
	printf("upload speed: %.2f kB/s\n", transByte / (1024 * consumeTime));
	printf("drop percent: %.1f%%\n", epack / (epack + spack) * 100);
	time(&rawTime);
	info = localtime(&rawTime);
	sprintf(logBuf, "%s\n INFO: upload %s, mode: %s, size: %.1f kB, consuming time: %.4f s\n",asctime(info), filename, choose == 1 ? ("netascii") : ("octet"), transByte / 1024, consumeTime);
	fwrite(logBuf, 1, strlen(logBuf), logFp);
	return true;
}
bool download(char* remoteFile, char* localFile) 
{
	FILE* fp = NULL;
	int cnt=0;
	int rxmt;
	double epack = 0, spack = 0;
	clock_t start, end;	// ��¼ʱ��
	transByte = 0;	// �����ֽ���
	int time_wait_ack, r_size, choose;	// �ȴ�ʱ��,���հ���С��ѡ��ģʽ
	tftpPacket sendPacket, rcv_packet, ack;// ���Ͱ�,���հ�
	int time_wait_data;
	unsigned short block = 1;
	// ���������ݰ�
	start = clock();
	for (rxmt = 0; rxmt < PKT_MAX_RXMT; rxmt++)
	{
		sendPacket.cmd = htons(CMD_RRQ);	// ��ȡ������
		cout << "please choose the file format(1.netascii  2.octet)" << endl;
		cin >> choose;
		if (choose == 1)
			sprintf(sendPacket.filename, "%s%c%s%c", remoteFile, 0, "netascii", 0);
		else
			sprintf(sendPacket.filename, "%s%c%s%c", remoteFile, 0, "octet", 0);
		// �����������ݰ�
		cout << "1" << endl;
		if (sendto(sock, (char*)&sendPacket, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, addr_len) == SOCKET_ERROR)
		{
			epack++;
			continue;
		}
		else spack++;
		// �½�����д���ļ�
		if (choose == 1)
			fp = fopen(localFile, "w");
		else
			fp = fopen(localFile, "wb");
		if (fp == NULL)
		{
			printf("Create file \"%s\" error.\n", localFile);
			time(&rawTime);
			info = localtime(&rawTime);
			sprintf(logBuf, "%s ERROR: download %s as %s, mode: %s, Create file \"%s\" error.\n", asctime(info), remoteFile, localFile, choose == 1 ? ("netascii") : ("octet"), localFile);
			fwrite(logBuf, 1, strlen(logBuf), logFp);
			return false;
		}
		// ��������

		sendPacket.cmd = htons(CMD_ACK);
		do {
			for (time_wait_data = 0; time_wait_data < PKT_RCV_TIMEOUT; time_wait_data += 20)
			{
				//���շ��������͵����ݰ�
				r_size = recvfrom(sock, (char*)&rcv_packet, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, (int*)&addr_len);
				if (r_size > 0 && r_size < 4)
					printf("Bad packet: r_size=%d\n", r_size);
				if (r_size >= 4 && rcv_packet.cmd == htons(CMD_DATA) && rcv_packet.block == htons(block))
				{
					cnt = 0;
					spack++;
					printf("DATA: block=%d, data_size=%d\n", ntohs(rcv_packet.block), r_size - 4);
					// ����ack����
					sendPacket.block = rcv_packet.block;	//ack�����������
					sendto(sock, (char*)&sendPacket, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, addr_len);
					//д������
					fwrite(rcv_packet.data, 1, r_size - 4, fp);
					break;
				}
				if (r_size == -1)
					Sleep(20);
			}
			if (time_wait_data >= PKT_RCV_TIMEOUT && block == 1)
			{
				epack++;
				break;
			}
			if (time_wait_data < PKT_RCV_TIMEOUT && block == 1)	rxmt = 4;
			if (time_wait_data >= PKT_RCV_TIMEOUT && block > 1 && cnt<3)
			{	
				printf("Datapack drop..\n");
				cnt++;
				epack++;
				sendPacket.block = htons(block - 1);
				sendto(sock, (char*)&sendPacket, sizeof(tftpPacket), 0, (struct sockaddr*)&serverAddr, addr_len);
				r_size = DATA_SIZE + 4;
				continue;
			}
			
			if (time_wait_data >= PKT_RCV_TIMEOUT && cnt==3)
			{
				printf("Wait for DATA #%d timeout.\n", block);
				fclose(fp);
				time(&rawTime);
				info = localtime(&rawTime);
				sprintf(logBuf, "%s ERROR: download %s as %s, mode: %s, Wait for DATA #%d timeout.\n", asctime(info), remoteFile, localFile, choose == 1 ? ("netascii") : ("octet"), block);
				fwrite(logBuf, 1, strlen(logBuf), logFp);
				return false;
			}
			transByte += (r_size - 4);
			block++;
		} while (r_size == DATA_SIZE + 4);
		if (rxmt == PKT_MAX_RXMT)
		{
			printf("Could not receive data from server!\n");
			return false;
		}
	}
			end = clock();
			consumeTime = ((double)(end - start)) / CLK_TCK;
			printf("download file size: %.2f kB consuming time: %.3f s\n", transByte / 1024, consumeTime);
			printf("download speed: %.2f kB/s\n", transByte / (1024 * consumeTime));
			printf("drop percent :%.1f%%\n", epack / (epack + spack) * 100);
			fclose(fp);
			time(&rawTime);
			info = localtime(&rawTime);
			sprintf(logBuf, "%s INFO: download %s as %s, mode: %s, size: %.1f kB, consuming time: %.2f s\n", asctime(info), remoteFile, localFile, choose == 1 ? ("netascii") : ("octet"), transByte / 1024, consumeTime);
			fwrite(logBuf, 1, strlen(logBuf), logFp);
			return true;
}
void main() {

	time(&rawTime);
	info = localtime(&rawTime);
	// ����־�ļ�
	logFp = fopen("tftp.log", "a");
	if (logFp == NULL) 
		cout << "open log file failed" << endl;
	printf("��ǰ�ı���ʱ������ڣ�%s", asctime(info));
	char serverIP[20];
	char buf[BUFLEN];
	char token1[BUFLEN];
	char token2[BUFLEN];
	WSADATA wsaData;
	addr_len = sizeof(struct sockaddr_in);
	
	//��ʼ�� winsock
	int nRC = WSAStartup(0x0101, &wsaData);
	if (nRC)
	{
		printf("Client initialize winsock error!\n");
		return;
	}
	if (wsaData.wVersion != 0x0101)
	{
		printf("Client's winsock version error!\n");
		WSACleanup();
		return;
	}
	printf("Client's winsock initialized !\n");
	cout << "input server IP:" ;
	
#ifdef DEBUG
	strcpy(serverIP, "127.0.0.1");
	cout << "127.0.0.1" << endl;
#else
	cin >> serverIP;
#endif
	
	// �����ͻ���socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)// ����ʧ��
	{
		cout << "Client create socket error!" << endl;
		WSACleanup();
		return;
	}
	printf("Client socket create OK!\n");
	setnonblocking(sock);
	while (1)
	{
		// ���÷����� ��ַ��(ipv4) �˿� ip
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(69);
		serverAddr.sin_addr.S_un.S_addr = inet_addr(serverIP);
		cout << "Please input upload or download: ";
		cin >> buf;
		if (buf == NULL)
			continue;
		if (strcmp(buf, "upload") == 0)
		{
			cout << "Please input filename: ";
			cin >> token1;
			if (token1 != NULL)
				upload(token1);
			else
				cout << "please input filename correctly!!!" << endl;
			cout << "****upload****" << endl << endl;
		}		
		else if (strcmp(buf, "download") == 0)
		{
			cout << "Please input remote filename: ";
			cin >> token1;
			cout << "Please input local file name: ";
			cin >> token2;
			if (token1 != NULL && token2 != NULL) 
				download(token1, token2);
			else 
				cout << "please input remote filename or local filename correctly!!!\n" << endl;
			cout << "****download****" << endl << endl;
		}
		else if (strcmp(buf, "exit") == 0) 
		{
			cout << "****exit****" << endl;
			break;
		}
		else
			cout << "Please input upload or download correctly!!!\n";
		//getchar();
	}
	fclose(logFp);
}

/*
download
123.txt
11.txt
1
*/