#ifndef DECRYPT_H
#define DECRYPT_H
#include"encrypt.h"


using namespace std;

// 1 逆行变换
void ShiftRow_(Byte m[4 * 4]);

// 2 逆S盒变换
void SubBytes_(Byte m[4 * 4]);

// 3 逆列变换
void MixColumns_(Byte m[4 * 4]);

// 4 解密函数
void decrypt(Byte in[4 * 4], word w[4 * (Nr + 1)]);

#endif