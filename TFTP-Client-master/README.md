# TFTP-Client
HUST, CSE, 2020 Computer Networking Experiment, Socket programming part
## 开发环境及依赖
1. 开发环境: Windows 10 x86
2. 开发语言: C++
3. IDE: Visual Stdio 2019
4. 项目生成工具: CMake 3.17.3
## 运行环境
Windows 10, **命令行/Powershell**
## 项目生成
1. 安装VS2019, CMake 3.17以上版本.
2. 运行脚本`BuildProject.bat`
3. 使用VS2019打开`./build/TFTPClient.sln`项目文件, **在"解决方案资源管理器"中选择"TFTPClient"右键"设为启动项目"**. 最后点击"本地Windows调试器"即可编译运行.
* PS:该TFTP客户端程序在文本模式(netascii)未实现CRLF与LF之间的转换. 该客户端程序未考虑性能等问题.