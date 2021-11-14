#include "./tftp/client.h"
#include "./anyoption/anyoption.h"
#include <iostream>
#include <cstring>

using namespace std;
using namespace tftp;


int main(int argc, char* argv[]) {

	bool isSender;	//�����ļ�������ļ�
	char ip[20];	//������IP��ַ
	char filepath[256];		//�ļ�·��
	char mode[40];	//����ģʽ

	AnyOption* opt = new AnyOption;
	//�����������ʾ��Ϣ
	opt->addUsage("Usage: tftpClient -i <serverip> -u|d <filepath> [-m <mode>] [-s]");
	opt->addUsage("");
	opt->addUsage("    -h  --help      Print usage information and exit");
	opt->addUsage("    -i  --ip        Server ip address xxx.xxx.xxx.xxx");
	opt->addUsage("    -u  --upload    Upload file to server");
	opt->addUsage("    -d  --download  Download file from server");
	opt->addUsage("    -m  --mode      TFTP modes of transfer: octet(b) or netascii(t)");
	opt->addUsage("    -s  --show      Show detail about packet");
	//���������ѡ��
	opt->setFlag("help", 'h');
	opt->setOption("ip", 'i');
	opt->setOption("upload", 'u');
	opt->setOption("download", 'd');
	opt->setOption("mode", 'm');
	opt->setFlag("show", 's');
	//���������в���
	opt->processCommandArgs(argc, argv);

	//������Ϣ
	if (opt->getFlag('h') || opt->getFlag("help")) {
		opt->printUsage();
	}
	//��ȡIP��ַ
	if (opt->getValue('i') || opt->getValue("ip")) {
		if (opt->getValue('i')) strcpy(ip, opt->getValue('i'));
		else strcpy(ip, opt->getValue("ip"));
	}
	else {
		opt->printUsage();
		return 0;
	}
	//��ȡ�ϴ��������ļ�·��
	if ((opt->getValue('u') || opt->getValue("upload"))
		&& (opt->getValue('d') || opt->getValue("download"))) {
		opt->printUsage();
		return 0;
	}
	else if (opt->getValue('u') || opt->getValue("upload")) {
		isSender = true;
		if (opt->getValue('u')) strcpy(filepath, opt->getValue('u'));
		else strcpy(filepath, opt->getValue("upload"));
	}
	else if (opt->getValue('d') || opt->getValue("download")) {
		isSender = false;
		if (opt->getValue('d')) strcpy(filepath, opt->getValue('d'));
		else strcpy(filepath, opt->getValue("download"));
	}
	else {
		opt->printUsage();
		return 0;
	}
	//��ȡ����ģʽ(Ĭ��Ϊ������)
	if (opt->getValue('m') || opt->getValue("mode")) {
		if (opt->getValue('m')) {
			strcpy(mode, opt->getValue('m'));
		}
		else strcpy(mode, opt->getValue("mode"));
		if (!strcmp(mode, "t")) strcpy(mode, ModeAscii);
		else if (!strcmp(mode, "b")) strcpy(mode, ModeBin);
		else if (strcmp(mode, ModeAscii) && strcmp(mode, ModeBin)) {
			opt->printUsage();
			return 0;
		}
	}
	else strcpy(mode, ModeBin);
	//�Ƿ����ϸ��
	if (opt->getFlag('s') || opt->getFlag("show"))
		tftp::showInfo = true;

	delete opt;

	Client c(ip);	//�����ͻ���
	c.startWinSkt();	//��ʼ���׽���API
	//�����ļ�
	if (isSender) {
		if (c.sendFile(filepath, mode)) {
			cout << "\nSend file " << filepath << " successfully!\n";
		}
		else {
			cout << "\nSend file " << filepath << " failed!\n";
		}
	}
	//�����ļ�
	else {
		if (c.recvFile(filepath, mode)) {
			cout << "\nReceive file " << filepath << " successfully!\n";
		}
		else {
			cout << "\nReceive file " << filepath << " failed!\n";
		}
	}
	c.cleanWinSkt();	//�ر��׽���
}