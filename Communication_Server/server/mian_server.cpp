#define _WINSOCK_DEPRECATED_NO_WARNINGS//vs环境下必须定义，否则无法使用inet_addr函数
#define _CRT_SECURE_NO_WARNINGS
#define PORT 55313
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<iostream>
#include<cstring>
using namespace std;

//保存两个用户的信息
struct clientInfo
{
	char userName[100];
	SOCKET connfd;
	struct clientInfo* next;
};
//全局变量
struct clientInfo* userlist;

//线程函数
DWORD WINAPI threadfunc(LPVOID lpParamter);

int main() {
	//初始化用户列表
	userlist = (struct clientInfo*)malloc(sizeof(struct clientInfo));
	userlist->next = NULL;
	//初始化网络环境
	WSADATA data;
	int res = WSAStartup(MAKEWORD(2, 2), &data);
	if (res) {
		cout << "初始化网络错误" << endl;
		return -1;
	}
	//创建套接字
	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);//unix下是用int型
	if (socketserver == -1) {
		cout << "创建套接字错误";
		return -1;
	}
	//绑定
	sockaddr_in addr;//创建套接字结构体
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.S_un.S_addr = inet_addr("172.31.118.155");
	if (bind(socketserver, (sockaddr*)&addr, sizeof(addr)) == -1) {
		cout << "绑定地址端口失败";
		return -1;
	}
	//监听
	if (listen(socketserver, 5) == -1) {
		cout << "监听套接字失败";
		return -1;
	}
	cout << "服务端已初始化成功，正在等待连接..." << endl;
	//接收
	sockaddr clientAddr;
	int len = sizeof(clientAddr);
	while (1) {
		SOCKET connfd = accept(socketserver, &clientAddr, &len);
		if (connfd == -1) {
			cout << "连接失败" << endl;
			return -1;
		}
		//cout << "一个新用户正在连接服务端（"<<connfd<<"）" << endl;
		HANDLE thread = CreateThread(NULL, 0, threadfunc, reinterpret_cast<LPVOID>(&connfd), 0, NULL);
		//CloseHandle(thread);
	}
	closesocket(socketserver);
	//free(userlist);
	WSACleanup();
	return 0;
}


DWORD WINAPI threadfunc(LPVOID lpParamter) {
	SOCKET* connfd0 = reinterpret_cast<SOCKET*>(lpParamter);
	SOCKET connfd = *(connfd0);

	char buff[32768];
	char message[32768];
	char username[100];

	//创建用户结点
	struct clientInfo* cn = (struct clientInfo*)malloc(sizeof(struct clientInfo));

	//插入用户列表
	struct clientInfo* p = userlist;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = cn;

	//初始化用户信息
	cn->connfd = connfd;
	cn->next = NULL;
	memset(buff, 0, sizeof(buff));
	int ret = recv(connfd, buff, sizeof(buff), 0);
	if (ret > 0) {
		buff[ret] = 0x00;
	}
	strcpy(cn->userName, buff);
	cout << "一个新用户：connfd（" << cn->connfd << "），username（" << cn->userName << "）" << endl;

	memset(buff, 0, sizeof(buff));
	ret = recv(connfd, buff, sizeof(buff), 0);
	if (ret > 0) {
		buff[ret] = '\0';
	}
	memset(username, 0, sizeof(username));
	strcpy(username, buff);
	cout << "接收用户（" << cn->userName << "）发送的目标用户成功" << endl;
	SOCKET tmp;
	p = userlist->next;
	while (p != NULL) {
		if (strcmp(p->userName, username) == 0) {
			tmp = p->connfd;
			break;
		}
		p = p->next;
	}
	//循环接受
	while (1) {
		//接收用户需要发送的信息
		//接收目标用户名
		
		memset(buff, 0, sizeof(buff));
		ret = recv(connfd, buff, sizeof(buff), 0);
		if (ret > 0) {
			buff[ret] = '\0';
		}
		strcpy(message, buff);
		cout << "接收用户（" << cn->userName << "）发送的信息成功" << endl;
		//转发
		send(tmp, message, strlen(message), 0);
		cout << "服务端已实现消息从" << cn->userName << "到" << p->userName << "的转发" << endl;
		cout << message << endl;
	}

	closesocket(connfd);
	//	free(cn);
	return 0;
}