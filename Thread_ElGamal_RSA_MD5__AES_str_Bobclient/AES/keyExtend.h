#ifndef KEYEXTEND_H
#define KEYEXTEND_H
//密钥扩展
#include<bitset>

using namespace std;

typedef bitset<8> Byte;//字节（8bit）
typedef bitset<32> word;//字（32bit）=4字节

extern Byte S[256];//S盒
extern Byte S_[256];//S'盒
extern word Rcon[10];
extern Byte C[4 * 4];
extern Byte C_[4 * 4];

//定义种子密钥长度以及扩展轮数
const int Nr = 10;//轮数 
const int Nk = 4; //种子密钥有四个字 

//密钥扩展 相关函数
// 1 四个字节转换成一个字 
word GetWord(Byte b0, Byte b1, Byte b2, Byte b3);
// 2 字移位 
word WordMove(word rw);
// 3 S盒替换 
word SubWord(word sw);
// 4 密钥扩展 
void KeyExpansion(Byte key[4 * Nk], word w[4 * (Nr + 1)]);
// 加密使用的列混合数组


#endif