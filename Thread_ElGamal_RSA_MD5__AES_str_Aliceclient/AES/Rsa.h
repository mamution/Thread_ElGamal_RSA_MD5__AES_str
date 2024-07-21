#pragma once
#include"BigInt.h"
class Rsa
{
public:
	Rsa();
	~Rsa();
	void init(unsigned int n);//初始化，产生公私钥对
	void init(BigInt p, BigInt q);
	void init_Public(BigInt e, BigInt N)//公钥初始化
	{
		this->e = e;
		this->N = N;
	}
	void init_Private(BigInt _d, BigInt N)//私约钥初始化
	{
		this->_d = _d;
		this->N = N;
	}
public:
	BigInt encryptByPu(const BigInt& m);//私钥加密
	BigInt decodeByPr(const BigInt& c);//公钥解密

	BigInt encryptByPr(const BigInt& m);//公钥加密
	BigInt decodeByPu(const BigInt& m);//私钥解密
private:
	BigInt createOddNum(unsigned int n);//生成长度为n的奇数
	bool isPrime(const BigInt& a, const unsigned int k);//判断素数
	BigInt createPrime(unsigned int n, int it_cout);//生成长度为n的素数
	void createExp(const BigInt& ou);//从一个欧拉数中生成公钥、私钥指数
	BigInt createRandomSmallThan(const BigInt& a);//创建小数
	friend ostream& operator <<(ostream& out, const Rsa& rsa)//输出
	{
		out << "N:" << rsa.N << "\n";
		out << "p:" << rsa._p << "\n";
		out << "q:" << rsa._q << "\n";
		out << "e:" << rsa.e << "\n";
		out << "d:" << rsa._d;
		return out;
	}
public:
	BigInt e, N;//公钥
private:
	BigInt _d;//私钥
	BigInt _p, _q;//
	BigInt _ol;//欧拉数
};