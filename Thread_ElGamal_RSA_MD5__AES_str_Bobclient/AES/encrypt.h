#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "keyExtend.h"

using namespace std;

// 设计G（2^8）有限域上的乘法函数 
Byte GF_Mul(Byte a, Byte b);


// 1 轮密钥加
void Cyc_Key_Add(Byte m[4 * 4], word w[4]);

// 2 字节代换
void SubBytes(Byte m[4 * 4]);

// 3 行移位
void ShiftRow(Byte m[4 * 4]);

// 4 列混合 
void MixColumns(Byte m[4 * 4], Byte s[4 * 4]);

// 5 加密函数
void encrypt(Byte m[4 * 4], word w[4 * (Nr + 1)]);

#endif