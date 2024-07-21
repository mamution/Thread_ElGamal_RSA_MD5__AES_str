#ifndef MD5_H
#define MD5_H
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<iomanip>
using namespace std;

//定义一些操作
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define CLS(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac) { a += F(b, c, d) + x + ac; a = CLS(a, s); a += b; }

#define GG(a,b,c,d,x,s,ac) { a += G(b, c, d) + x + ac; a = CLS(a, s); a += b; }

#define HH(a,b,c,d,x,s,ac) { a += H(b, c, d) + x + ac; a = CLS(a, s); a += b; }

#define II(a,b,c,d,x,s,ac) { a += I(b, c, d) + x + ac; a = CLS(a, s); a += b; }

//填充函数
//m为原始明文，填充后的数据长度为512*n bit，填充后的数据在output中保存
void mExtend(vector<unsigned char>& date, int& n, string m);

//生成第n块的X数组，（小端->大端）大小端转换
void get_X(unsigned int X[], vector<unsigned char> m, int n);

void hex2hexstring(stringstream& ss, unsigned int tmp);

string md5(string inputStr);

#endif