#include "./tftp/client.h"
#include "./anyoption/anyoption.h"
#include <iostream>
#include <cstring>

using namespace std;
using namespace tftp;


int main(int argc, char* argv[]) {

	bool isSender;	//发送文件或接受文件
	char ip[20];	//服务器IP地址
	char filepath[256];		//文件路径
	char mode[40];	//传输模式

	AnyOption* opt = new AnyOption;
	//添加命令行提示信息
	opt->addUsage("Usage: tftpClient -i <serverip> -u|d <filepath> [-m <mode>] [-s]");
	opt->addUsage("");
	opt->addUsage("    -h  --help      Print usage information and exit");
	opt->addUsage("    -i  --ip        Server ip address xxx.xxx.xxx.xxx");
	opt->addUsage("    -u  --upload    Upload file to server");
	opt->addUsage("    -d  --download  Download file from server");
	opt->addUsage("    -m  --mode      TFTP modes of transfer: octet(b) or netascii(t)");
	opt->addUsage("    -s  --show      Show detail about packet");
	//添加命令行选项
	opt->setFlag("help", 'h');
	opt->setOption("ip", 'i');
	opt->setOption("upload", 'u');
	opt->setOption("download", 'd');
	opt->setOption("mode", 'm');
	opt->setFlag("show", 's');
	//解析命令行参数
	opt->processCommandArgs(argc, argv);

	//帮助信息
	if (opt->getFlag('h') || opt->getFlag("help")) {
		opt->printUsage();
	}
	//提取IP地址
	if (opt->getValue('i') || opt->getValue("ip")) {
		if (opt->getValue('i')) strcpy(ip, opt->getValue('i'));
		else strcpy(ip, opt->getValue("ip"));
	}
	else {
		opt->printUsage();
		return 0;
	}
	//提取上传或下载文件路径
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
	//提取传输模式(默认为二进制)
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
	//是否输出细节
	if (opt->getFlag('s') || opt->getFlag("show"))
		tftp::showInfo = true;

	delete opt;

	Client c(ip);	//创建客户端
	c.startWinSkt();	//初始化套接字API
	//发送文件
	if (isSender) {
		if (c.sendFile(filepath, mode)) {
			cout << "\nSend file " << filepath << " successfully!\n";
		}
		else {
			cout << "\nSend file " << filepath << " failed!\n";
		}
	}
	//接受文件
	else {
		if (c.recvFile(filepath, mode)) {
			cout << "\nReceive file " << filepath << " successfully!\n";
		}
		else {
			cout << "\nReceive file " << filepath << " failed!\n";
		}
	}
	c.cleanWinSkt();	//关闭套接字
}