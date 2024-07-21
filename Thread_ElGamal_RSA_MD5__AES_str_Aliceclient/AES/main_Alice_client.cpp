#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define RSA_Public_Key "This is the public key of RSA"
#define AES_Key "This is the key of AES"
#define AES_Key_Successfully_Accepted "The AES key is accepted successfully. You can send messages"
#define PORT 55313
#define PORT_CA 55319
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<iostream>
#include<cstring>
#include<bitset>
#include<string>
#include<vector>
#include<thread>
#include<ctime>
#include<fstream>
#include<regex>
#include<windows.h>
#include<memory>

#define THREAD_NUM 10

#include"encrypt.h"
#include"decrypt.h"
#include"ThreadPool.h"
#include"md5.h"
#include"BigInt.h"
#include"Rsa.h"

using namespace std;

Rsa rsa;
bool isAESCommunication = false;
mutex mtx;
Byte key[16];
word w[4 * (Nr + 1)];
void myEncrypt(Byte m[], word w[]) {
	encrypt(m, w);//加密
}
void myDecrypt(Byte e[], int L, int num, string* m_v, word w[]) {
	decrypt(e, w);//解密
	int k = 0;
	int count = 16;
	if (num == (L - 1)) {
		count -= e[15].to_ulong();
	}
	while (k < count)
	{
		*(m_v) += ((char)e[k].to_ulong());
		k++;
	}
}
void init_createPQ(Rsa& rsa, int n)
{//初始化
	cout << "RSA密钥初始化(通过算法得到素数)...." << endl;
	long t1 = clock();
	rsa.init(n);
	long t2 = clock();
	cout << "初始化完成." << endl;
	cout << rsa << endl;
	cout << "RSA密钥初始化用时:" << (t2 - t1) / 1000 << "s." << endl << endl;
}
void init_inputPQ(Rsa& rsa)
{//初始化
	cout << "RSA密钥初始化(从文件获取素数)...." << endl;
	long t1 = clock();
	fstream file;
	file.open("twoPrime768.txt", ios::in);
	string buf_P, buf_Q;
	getline(file, buf_P);
	getline(file, buf_Q);
	file.close();
	rsa.init(buf_P, buf_Q);
	long t2 = clock();
	cout << "初始化完成." << endl;
	cout << rsa << endl;
	cout << "RSA密钥初始化用时:" << (t2 - t1) / 1000 << "s." << endl << endl;
}
bool islegal(const string& str)
{//判断输入是否合法
	for (string::const_iterator it = str.begin();it != str.end();++it)
		if (!isalnum(*it))//不是字母数字
			return false;
	return true;
}

void RSAdecode(Rsa& rsa, BigInt str, string& m)
{//RSA解密
	cout << "RSA解密AES的Key:" << endl;
	BigInt c(str);
	long t1 = clock();
	m = rsa.decodeByPr(c).toHexString();
	long t2 = clock();
	cout << "密文:" << c << endl
		<< "明文:" << m << endl;
	cout << "RSA解密用时:" << (t2 - t1) << "ms." << endl << endl;
}

void RSAencry(Rsa& rsa, BigInt& c, string str)
{//RSA加密
	cout << "RSA加密AES的Key:" << endl;
	BigInt m(str);
	long t1 = clock();
	c = rsa.encryptByPu(m);
	long t2 = clock();
	cout << "明文:" << m << endl
		<< "密文:" << c << endl;
	cout << "RSA加密用时:" << (t2 - t1) << "ms." << endl << endl;
}
string byte2string(Byte key[16]) {
	stringstream Hex_s;
	for (int i = 0;i < 16;i++) {
		Hex_s << setw(2) << setfill('0') << hex << key[i].to_ulong();
	}
	return Hex_s.str();
}
void string2byte(string key_str, Byte key_m[16]) {
	for (int i = 0; i < 16; ++i) {
		string byteString = key_str.substr(i * 2, 2);
		key_m[i] = std::bitset<8>(std::stoi(byteString, nullptr, 16));
	}
}

//线程函数
DWORD WINAPI clientReceiverThread(LPVOID lpParamter);
char target_username[100];
char username[100];
string p, g, y, r, s;

int main() {
	char buff[32768];
	char message[32768];

	//初始化环境
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		cout << "初始化网络错误！" << endl;
		WSACleanup();
		return -1;
	}

	//创建套接字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << "创建套接字失败！" << endl;
		WSACleanup();
		return -1;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("172.31.118.155");
	addr.sin_port = htons(PORT);


	//连接
	ret = connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		WSACleanup();
		cout << "连接服务器失败" << endl;
		return -1;
	}

	//发送用户名给server
	memset(username, 0, sizeof(username));
	cout << "1、输入用户名：";
	cin >> username;
	getchar();
	memset(buff, 0, sizeof(buff));
	strcpy(buff, username);
	send(sock, buff, strlen(buff), 0);
	//发送目标用户名
	memset(target_username, 0, sizeof(target_username));
	cout << "3、输入目标用户名：";
	cin >> target_username;
	getchar();
	memset(buff, 0, sizeof(buff));
	strcpy(buff, target_username);
	send(sock, buff, strlen(buff), 0);


	//创建一个线程用于不停接收服务端发来的信息
	HANDLE thread = CreateThread(NULL, 0, clientReceiverThread, reinterpret_cast<LPVOID>(&sock), 0, NULL);
	Byte** m = NULL;
	int num = 0;
	while (1) {
		unique_lock<mutex> lock(mtx);
		if (isAESCommunication == false)
			continue;
		//输入的明文
		cout << endl << "-------------------------------发送AES加密的消息-----------------------------------" << endl;
		string M;
		cout << "--请输入发送的明文（非中文）:" << endl;//获取一行
		getline(cin, M);
		cout << endl;
		string md5Str = md5(M);
		cout << "--明文的MD5为:" << endl << md5Str << endl << endl;//获取一行
		M += md5Str;
		//pkcs7补齐法
		int is = M.size() % 16;
		{
			string tmp;
			tmp.assign((16 - M.size() % 16), (char)(16 - M.size() % 16));
			M += tmp;
		}
		int L = M.size() / 16;

		num = 0;
		m = new Byte * [L];
		string E = "";
		{
			ThreadPool myEncryptPool(THREAD_NUM);
			while (num < L) {
				m[num] = new Byte[16];
				for (size_t i = 0; i < 16; i++)
				{
					m[num][i] = M[num * 16 + i];
				}
				myEncryptPool.enqueue(myEncrypt, m[num], w);
				num++;
			}
		}
		for (size_t i = 0; i < L; i++)
		{
			E += (byte2string(m[i]));
		}
		cout << "--明文通过AES-128加密后的密文是:" << endl;
		cout << E << endl;
		cout << endl;
		for (size_t i = 0; i < L; i++)
		{
			delete[] m[i];
		}
		delete[] m;
		m = NULL;
		//发送信息
		memset(buff, 0, sizeof(buff));
		strcpy(buff, E.data());
		send(sock, buff, strlen(buff), 0);
		cout << "发送消息成功" << endl;
	}
	closesocket(sock);
	WSACleanup();
}

DWORD WINAPI clientReceiverThread(LPVOID lpParamter) {
	SOCKET* connfd0 = reinterpret_cast<SOCKET*>(lpParamter);
	SOCKET connfd = *(connfd0);
	char buff[32768];
	string** m_v = NULL;
	int num = 0;
	Byte** e = NULL;


	while (1) {
		memset(buff, 0, sizeof(buff));
		int ret = recv(connfd, buff, sizeof(buff) - 1, 0);
		Sleep(500);
		if (ret > 0) {
			buff[ret] = '\0';
			string substr(buff);
			if (strcmp(buff, RSA_Public_Key) == 0) {
				memset(buff, 0, sizeof(buff));
				int ret = recv(connfd, buff, sizeof(buff) - 1, 0);
				buff[ret] = '\0';
				string str(buff);
				cout <<endl<< "-----------------------------------收到RSA公钥消息-------------------------------------" << endl;
				regex reg(",");
				sregex_token_iterator iter(str.begin(), str.end(), reg, -1);
				rsa.e = iter->str();
				cout << "RSA.e：" << rsa.e << endl;
				iter++;
				rsa.N = iter->str();
				cout << "RSA.N：" << rsa.N << endl;
				{
					cout << endl << "------------------------------从Trent(CA)获取对方的数字签名--------------------------" << endl;
					//初始化环境
					WSADATA data_ca;
					int ret_ca = WSAStartup(MAKEWORD(2, 2), &data_ca);
					if (ret_ca) {
						cout << "初始化网络错误！" << endl;
						WSACleanup();
						return -1;
					}

					//创建与数字签名server通信的套接字
					SOCKET sock_ca = socket(AF_INET, SOCK_STREAM, 0);
					if (sock_ca == INVALID_SOCKET) {
						cerr << "创建套接字失败！" << endl;
						WSACleanup();
						return -1;
					}
					sockaddr_in addr_ca;
					addr_ca.sin_family = AF_INET;
					addr_ca.sin_addr.S_un.S_addr = inet_addr("172.31.118.155");
					addr_ca.sin_port = htons(PORT_CA);

					ret_ca = connect(sock_ca, (sockaddr*)&addr_ca, sizeof(addr_ca));
					if (ret_ca == -1) {
						WSACleanup();
						cout << "连接服务器失败" << endl;
						return -1;
					}
					cout << "--连接数字签名Server Trent(CA)成功" << endl;
					// 发送用户名给Trent
					// 连接后立刻发送用户名——UserID(表明身份)
					memset(buff, 0, sizeof(buff));
					strcpy(buff, username);
					send(sock_ca, buff, strlen(buff), 0);
					Sleep(500);

					// 接收提示消息
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						cout << buff << "\n\n";
					}

					// 接收数字证书
					cout << "--获取RSA公钥数字证书:\n";
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						cout << buff << "\n\n";
					}

					// 接收ElGamal公钥
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						p = buff;
					}
					Sleep(100);
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						g = buff;
					}
					Sleep(100);
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						y = buff;
					}
					Sleep(100);
					// 接收签名
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						r = buff;
					}
					Sleep(100);
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						s = buff;
					}
					cout << "--数字签名Server Trent(CA)与Bob的连接已关闭连接\n";
					closesocket(sock_ca);
				}
				// 验证签名
				cout <<endl<< "-----------------------------------进行数字签名验证-------------------------------------" << endl;
				// v1 = pow(y, r, p) * pow(r, s, p) % p
				// v2 = pow(g, m, p)
				string M = str + target_username;  // 用户id作为盐值
				string md5_M = md5(M);
				// cout << p << endl << g << endl << y << endl << r << endl << s << endl << md5_M<< endl;
				BigInt pp(p), gg(g), yy(y), rr(r), ss(s), MM(md5_M);
				BigInt v1 = (yy.moden(rr, pp) * rr.moden(ss, pp)) % pp;  //moden(exp, mod)模幂运算
				BigInt v2 = gg.moden(MM, pp);
				cout << "--计算得到V1、V2，判断是否相等" << endl;
				cout << "v1: \n" << v1 << endl;
				cout << "v2: \n" << v2 << endl;
				if (v1 == v2)
					cout << "\n数字签名验证成功\n";
				else
					cout << "\n数字签名验证失败\n";
				cout << endl << "------------------------------------AES密钥初始化-----------------------------------" << endl;
				cout << "--请输入AES_128的种子密钥的16进制（如:139A81AC71296CE688112233AACDFF10）" << endl;
				string KEY_STRING;
				cin >> KEY_STRING;
				getchar();
				string2byte(KEY_STRING, key);
				cout << endl << "--------------------------------发送RSA加密的AES密钥--------------------------------" << endl;
				cout << "--RSA加密前的AES种子密钥是:" << endl;
				for (int i = 0;i < 16;i++) {
					cout << hex << key[i].to_ulong() << "  ";
					if ((i + 1) % 4 == 0)
						cout << endl;
				}
				cout << endl;
				BigInt key_c_str;
				KeyExpansion(key, w); // 密钥扩展
				RSAencry(rsa, key_c_str, byte2string(key));
				memset(buff, 0, sizeof(buff));
				strcpy(buff, AES_Key);
				send(connfd, buff, strlen(buff), 0);
				Sleep(1000);
				memset(buff, 0, sizeof(buff));
				strcpy(buff, key_c_str.toHexString().data());
				send(connfd, buff, strlen(buff), 0);
				Sleep(1000);
			}
			else if (strcmp(buff, AES_Key_Successfully_Accepted) == 0) {
				cout << endl << "----------------------------收到AES密钥被确认接受的消息----------------------------" << endl;
				memset(buff, 0, sizeof(buff));
				strcpy(buff, AES_Key_Successfully_Accepted);
				send(connfd, buff, strlen(buff), 0);
				cout << "--发送此消息的确认消息" << endl;
				unique_lock<mutex> lock(mtx);
				isAESCommunication = true;
				cout << "--可以进行AES加密通信了" << endl;
			}
			else {
				cout << endl << "---------------------------------收到AES加密的消息-------------------------------------" << endl;
				string str_E(buff);
				cout << "--收到的被AES加密后的密文为:" << endl << str_E << endl << endl;
				int L = str_E.size() / 32;
				string M_Tmp = "";
				m_v = new string * [L];
				num = 0;
				e = new Byte * [L];
				{
					ThreadPool myDecryptPool(THREAD_NUM);
					while (num < L)
					{
						e[num] = new Byte[16];
						string2byte(str_E.substr(num * 32, 32), e[num]);
						m_v[num] = new string("");
						myDecryptPool.enqueue(myDecrypt, e[num], L, num, m_v[num], w);
						num++;
					}
				}
				for (size_t i = 0; i < L; i++)
				{
					M_Tmp += *(m_v[i]);
				}
				int m_len = M_Tmp.length() - 32;
				cout << "--进行AES解密后的明文是:" << endl;
				cout << M_Tmp << endl << endl;
				string M_Tmp_Del_Md5 = M_Tmp.substr(0, m_len);
				string New_Md5 = M_Tmp.substr(m_len, 32);
				cout << "--明文(不包含MD5)是:" << endl;
				cout << M_Tmp_Del_Md5 << endl << endl;
				cout << "--明文计算得到的MD5是:" << endl;
				string MD5 = md5(M_Tmp_Del_Md5);
				cout << MD5 << endl << endl;
				cout << "--明文消息验证" << endl;
				if (MD5.compare(New_Md5) == 0)
					cout << "--md5验证成功\n\n";
				else
					cout << "--md5验证失败\n\n";

				for (size_t i = 0; i < L; i++)
				{
					delete[] e[i];
				}
				delete[] m_v;
				delete[] e;
				m_v = NULL;
				e = NULL;
			}
		}
		else if (ret == 0) {
			cout << "服务器已关闭连接" << endl;
			closesocket(connfd);
			return 0;
		}
		else {
			cout << "服务器已关闭连接" << endl;
			closesocket(connfd);
			WSACleanup();
			return 0;
		}
	}

	closesocket(connfd);
	return 0;
}
