#include<iostream>
#include<string>
#include"encrypt.h"

using namespace std;


//有限域上的乘法 GF(2^8)
//参考xtime()--x乘法
Byte GF_Mul(Byte a, Byte b) {
	Byte b_tmp = 0;
	for (int i = 0;i < 8;i++) {
		//b的低阶位为1就直接赋值\不为1就进行跳过这个赋值，直接成x
		if (b[0] == 1)
			b_tmp ^= a;
		//如果a的高阶位等于1，代表超出了x^8
		int h = a[7];
		a <<= 1;
		//所以用假如没有丢失数据的左移后a mod m(x)  等价于丢失数据后a ^= 0x1b
		if (h == 1)
			a ^= 0x1b;
		b >>= 1;

	}
	return b_tmp;
}


// 1 轮密钥加:将状态矩阵的一列的四个字节和轮密钥的对应字节进行异或   
void Cyc_Key_Add(Byte m[4 * 4], word w[4]) {
	for (int i = 0;i < 4;i++) {
		//从w字中获取四个字节
		word k0 = w[i] >> 24;
		word k1 = (w[i] << 8) >> 24;
		word k2 = (w[i] << 16) >> 24;
		word k3 = (w[i] << 24) >> 24;

		m[i] = m[i] ^ Byte(k0.to_ulong());
		m[i + 4] = m[i + 4] ^ Byte(k1.to_ulong());
		m[i + 8] = m[i + 8] ^ Byte(k2.to_ulong());
		m[i + 12] = m[i + 12] ^ Byte(k3.to_ulong());
	}

}

// 2 字节代换
void SubBytes(Byte m[4 * 4]) {
	// 将16个字节依次进行代换
	for (int i = 0;i < 16;i++) {
		int row = m[i][7] * 8 + m[i][6] * 4 + m[i][5] * 2 + m[i][4] * 1;
		int col = m[i][3] * 8 + m[i][2] * 4 + m[i][1] * 2 + m[i][0] * 1;
		m[i] = S[row * 16 + col];
	}
}

// 3 行移位 : 按行进行字节移位
void ShiftRow(Byte m[4 * 4]) {
	Byte b_tmp[4];
	for (int i = 0;i < 4;i++) {
		//把需要移动的字节存储下来
		for (int j = 0;j < i;j++) {
			b_tmp[j] = m[i * 4 + j];
		}
		//将需要向前移动的保存
		for (int j = 0;j < 4 - i;j++) {
			m[i * 4 + j] = m[i * 4 + j + i];
		}
		//暂存的数放回状态数组行中 
		for (int k = 4 - i;k < 4;k++) {
			m[i * 4 + k] = b_tmp[k + i - 4];
		}
	}
}

// 4 列混合
void MixColumns(Byte m[4 * 4], Byte C[4 * 4]) {
	Byte matr[4];
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 4;j++)
			matr[j] = m[i + j * 4];//把m中的列提取出来
		//逐个进行乘法运算，再存储到原来的列中
		m[i] = GF_Mul(C[0], matr[0]) ^ GF_Mul(C[1], matr[1]) ^ GF_Mul(C[2], matr[2]) ^ GF_Mul(C[3], matr[3]);
		m[i + 4] = GF_Mul(C[4], matr[0]) ^ GF_Mul(C[5], matr[1]) ^ GF_Mul(C[6], matr[2]) ^ GF_Mul(C[7], matr[3]);
		m[i + 8] = GF_Mul(C[8], matr[0]) ^ GF_Mul(C[9], matr[1]) ^ GF_Mul(C[10], matr[2]) ^ GF_Mul(C[11], matr[3]);
		m[i + 12] = GF_Mul(C[12], matr[0]) ^ GF_Mul(C[13], matr[1]) ^ GF_Mul(C[14], matr[2]) ^ GF_Mul(C[15], matr[3]);
	}
}

// 5 加密函数
void encrypt(Byte m[4 * 4], word w[4 * (Nr + 1)]) {
	word key[4];
	for (int i = 0; i < 4; i++)
		key[i] = w[i];
	//先进行一次轮密钥加 
	Cyc_Key_Add(m, key);

	//前九轮：   S盒  行移位  列混合  轮密钥加 
	for (int r = 1; r < Nr; r++)
	{
		SubBytes(m);
		ShiftRow(m);
		MixColumns(m, C);
		for (int i = 0; i < 4; i++)
			key[i] = w[4 * r + i];
		Cyc_Key_Add(m, key);
  
	}
	//第十轮   S盒  行移位  轮密钥加 
	SubBytes(m);
	ShiftRow(m);
	for (int i = 0; i < 4; ++i)
		key[i] = w[4 * Nr + i];
	Cyc_Key_Add(m, key);
}