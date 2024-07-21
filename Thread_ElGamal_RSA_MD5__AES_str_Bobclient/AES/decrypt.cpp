#include<iostream>
#include<string>
#include"decrypt.h"


using namespace std;

// 1 逆行变换  循环右移
void ShiftRow_(Byte m[4 * 4]) {
	Byte b_tmp[4];
	for (int i = 0;i < 4;i++) {

		//存数 防止被覆盖
		for (int j = 0;j < i;j++) {
			b_tmp[j] = m[i * 4 + 3 - j];
		}
		// 将不会发生下标溢出的进行赋值 
		for (int j = 0;j < 4 - i;j++)
		{
			m[i * 4 + 3 - j] = m[i * 4 + 3 - j - i];
		}
		// 将暂存的数放回状态数组 行中 
		for (int k = 0;k < i;k++) {
			m[i * 4 + k] = b_tmp[i - k - 1];
		}
	}

}
// 2 逆S盒变换
void SubBytes_(Byte m[4 * 4]) {
	// 将16个字节依次进行代换
	for (int i = 0;i < 16;i++) {
		int row = m[i][7] * 8 + m[i][6] * 4 + m[i][5] * 2 + m[i][4];
		int col = m[i][3] * 8 + m[i][2] * 4 + m[i][1] * 2 + m[i][0];
		m[i] = S_[row * 16 + col];
	}
}

// 3 逆列变换   没变化没问题 
void MixColumns_(Byte m[4 * 4], Byte C_[4 * 4]) {
	Byte matr[4];
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 4;j++)
			matr[j] = m[i + j * 4];

		m[i] = GF_Mul(C_[0], matr[0]) ^ GF_Mul(C_[1], matr[1]) ^ GF_Mul(C_[2], matr[2]) ^ GF_Mul(C_[3], matr[3]);
		m[i + 4] = GF_Mul(C_[4], matr[0]) ^ GF_Mul(C_[5], matr[1]) ^ GF_Mul(C_[6], matr[2]) ^ GF_Mul(C_[7], matr[3]);
		m[i + 8] = GF_Mul(C_[8], matr[0]) ^ GF_Mul(C_[9], matr[1]) ^ GF_Mul(C_[10], matr[2]) ^ GF_Mul(C_[11], matr[3]);
		m[i + 12] = GF_Mul(C_[12], matr[0]) ^ GF_Mul(C_[13], matr[1]) ^ GF_Mul(C_[14], matr[2]) ^ GF_Mul(C_[15], matr[3]);
	}
}

// 4 解密函数 
void decrypt(Byte m[4 * 4], word w[4 * (Nr + 1)]) {
	word key[4];
	for (int i = 0; i < 4; i++)
		key[i] = w[4 * Nr + i];
	//先进行一次轮密钥加 
	Cyc_Key_Add(m, key);

	//前九轮操作  逆行移位   逆S盒   轮密钥加  逆列混合 
	for (int r = Nr - 1; r > 0; r--)
	{
		ShiftRow_(m);
		SubBytes_(m);
		for (int i = 0; i < 4; i++)
			key[i] = w[4 * r + i];
		Cyc_Key_Add(m, key);
		MixColumns_(m, C_);
	}
	//第十轮   逆行移位   逆S盒 轮密钥加 
	ShiftRow_(m);
	SubBytes_(m);
	for (int i = 0; i < 4; i++)
		key[i] = w[i];
	Cyc_Key_Add(m, key);
}
