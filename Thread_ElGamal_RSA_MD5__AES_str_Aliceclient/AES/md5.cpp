#include "md5.h"

//填充函数
//m为原始明文，填充后的数据长度为512*n bit，填充后的数据在output中保存
void mExtend(vector<unsigned char>& output, int& n, string m) {
	n = (m.size() + 8) / 64 + 1;//8——"\0也被加上了"，1——一定填充
	output.resize(n * 64);
	int i = 0;
	//原始明文消息
	for (; i < m.size(); i++) {
		output[i] = m[i];
	}
	//填充10...0
	output[i++] = 0x80;
	while (i < output.size() - 8) {
		output[i] = 0;
		i++;
	}
	//填充长度，完成长度L的大小端转换。（变成小端模式——低位值存低地址）
	long long int len = m.size() * 8;
	for (i = output.size() - 8; i < output.size(); ++i) {
		output[i] = len % 256;//2^8=256
		len /= 256;
	}

}

void get_X(unsigned int X[], vector<unsigned char> m, int n) {
	n *= 64;
	for (int i = 0; i < 16; i++) {
		X[i] = (m[n + 4 * i + 3] << 24) + (m[n + 4 * i + 2] << 16)
			+ (m[n + 4 * i + 1] << 8) + m[n + 4 * i];
	}
}

string md5(string text) {
	vector<unsigned char> message;//填充后的消息
	int n;//按512比特一块，message可以分为n块
	mExtend(message, n, text);
	unsigned int A, B, C, D;
	unsigned int AA, BB, CC, DD;
	unsigned int X[16];
	A = 0x67452301;
	B = 0xEFCDAB89;
	C = 0x98BADCFE;
	D = 0x10325476;
	for (int i = 0; i < n; ++i) {
		get_X(X, message, i);
		AA = A, BB = B, CC = C, DD = D;

		FF(A, B, C, D, X[0], 7, 0xd76aa478);
		FF(D, A, B, C, X[1], 12, 0xe8c7b756);
		FF(C, D, A, B, X[2], 17, 0x242070db);
		FF(B, C, D, A, X[3], 22, 0xc1bdceee);
		FF(A, B, C, D, X[4], 7, 0xf57c0faf);
		FF(D, A, B, C, X[5], 12, 0x4787c62a);
		FF(C, D, A, B, X[6], 17, 0xa8304613);
		FF(B, C, D, A, X[7], 22, 0xfd469501);
		FF(A, B, C, D, X[8], 7, 0x698098d8);
		FF(D, A, B, C, X[9], 12, 0x8b44f7af);
		FF(C, D, A, B, X[10], 17, 0xffff5bb1);
		FF(B, C, D, A, X[11], 22, 0x895cd7be);
		FF(A, B, C, D, X[12], 7, 0x6b901122);
		FF(D, A, B, C, X[13], 12, 0xfd987193);
		FF(C, D, A, B, X[14], 17, 0xa679438e);
		FF(B, C, D, A, X[15], 22, 0x49b40821);

		GG(A, B, C, D, X[1], 5, 0xf61e2562);
		GG(D, A, B, C, X[6], 9, 0xc040b340);
		GG(C, D, A, B, X[11], 14, 0x265e5a51);
		GG(B, C, D, A, X[0], 20, 0xe9b6c7aa);
		GG(A, B, C, D, X[5], 5, 0xd62f105d);
		GG(D, A, B, C, X[10], 9, 0x2441453);
		GG(C, D, A, B, X[15], 14, 0xd8a1e681);
		GG(B, C, D, A, X[4], 20, 0xe7d3fbc8);
		GG(A, B, C, D, X[9], 5, 0x21e1cde6);
		GG(D, A, B, C, X[14], 9, 0xc33707d6);
		GG(C, D, A, B, X[3], 14, 0xf4d50d87);
		GG(B, C, D, A, X[8], 20, 0x455a14ed);
		GG(A, B, C, D, X[13], 5, 0xa9e3e905);
		GG(D, A, B, C, X[2], 9, 0xfcefa3f8);
		GG(C, D, A, B, X[7], 14, 0x676f02d9);
		GG(B, C, D, A, X[12], 20, 0x8d2a4c8a);

		HH(A, B, C, D, X[5], 4, 0xfffa3942);
		HH(D, A, B, C, X[8], 11, 0x8771f681);
		HH(C, D, A, B, X[11], 16, 0x6d9d6122);
		HH(B, C, D, A, X[14], 23, 0xfde5380c);
		HH(A, B, C, D, X[1], 4, 0xa4beea44);
		HH(D, A, B, C, X[4], 11, 0x4bdecfa9);
		HH(C, D, A, B, X[7], 16, 0xf6bb4b60);
		HH(B, C, D, A, X[10], 23, 0xbebfbc70);
		HH(A, B, C, D, X[13], 4, 0x289b7ec6);
		HH(D, A, B, C, X[0], 11, 0xeaa127fa);
		HH(C, D, A, B, X[3], 16, 0xd4ef3085);
		HH(B, C, D, A, X[6], 23, 0x4881d05);
		HH(A, B, C, D, X[9], 4, 0xd9d4d039);
		HH(D, A, B, C, X[12], 11, 0xe6db99e5);
		HH(C, D, A, B, X[15], 16, 0x1fa27cf8);
		HH(B, C, D, A, X[2], 23, 0xc4ac5665);

		II(A, B, C, D, X[0], 6, 0xf4292244);
		II(D, A, B, C, X[7], 10, 0x432aff97);
		II(C, D, A, B, X[14], 15, 0xab9423a7);
		II(B, C, D, A, X[5], 21, 0xfc93a039);
		II(A, B, C, D, X[12], 6, 0x655b59c3);
		II(D, A, B, C, X[3], 10, 0x8f0ccc92);
		II(C, D, A, B, X[10], 15, 0xffeff47d);
		II(B, C, D, A, X[1], 21, 0x85845dd1);
		II(A, B, C, D, X[8], 6, 0x6fa87e4f);
		II(D, A, B, C, X[15], 10, 0xfe2ce6e0);
		II(C, D, A, B, X[6], 15, 0xa3014314);
		II(B, C, D, A, X[13], 21, 0x4e0811a1);
		II(A, B, C, D, X[4], 6, 0xf7537e82);
		II(D, A, B, C, X[11], 10, 0xbd3af235);
		II(C, D, A, B, X[2], 15, 0x2ad7d2bb);
		II(B, C, D, A, X[9], 21, 0xeb86d391);

		A = A + AA, B = B + BB, C = C + CC, D = D + DD;
	}
	stringstream ss;
	//将A,B,C,D由大端格式变为小端格式
	hex2hexstring(ss, (A & 0xffU));
	hex2hexstring(ss, ((A >> 8) & 0xffU));
	hex2hexstring(ss, ((A >> 16) & 0xffU));
	hex2hexstring(ss, ((A >> 24) & 0xffU));
	hex2hexstring(ss, (B & 0xffU));
	hex2hexstring(ss, ((B >> 8) & 0xffU));
	hex2hexstring(ss, ((B >> 16) & 0xffU));
	hex2hexstring(ss, ((B >> 24) & 0xffU));
	hex2hexstring(ss, (C & 0xffU));
	hex2hexstring(ss, ((C >> 8) & 0xffU));
	hex2hexstring(ss, ((C >> 16) & 0xffU));
	hex2hexstring(ss, ((C >> 24) & 0xffU));
	hex2hexstring(ss, (D & 0xffU));
	hex2hexstring(ss, ((D >> 8) & 0xffU));
	hex2hexstring(ss, ((D >> 16) & 0xffU));
	hex2hexstring(ss, ((D >> 24) & 0xffU));
	return ss.str();
}
void hex2hexstring(stringstream& ss,unsigned int tmp) {
	ss << setw(2) << setfill('0') << hex << tmp;
}