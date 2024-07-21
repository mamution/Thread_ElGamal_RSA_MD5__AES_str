import socket
import time
from GetBigPrime import get_prime
import random
import hashlib
from tkinter import messagebox
from Utils import gcd, getFirstPrimitiveRoot, getAllPrimitiveRoot, get_host_ip

"""数字签名Server——模拟证书颁布机构(CA), 只会和请求签名者进行通信, 不可以作为通信中转Server"""
# Alice and Bob must link to this server to get the signature of their message at the same time.

port = 55319
buff_size = 4096
ip_addr = socket.gethostbyname(get_host_ip())  # 获取本机局域网IP地
userlist = []

# 创建socket对象，指定TCP协议和端口
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((ip_addr, port))
server_socket.listen(5)

while True:
    """处理Bob"""
    client_socket, addr = server_socket.accept()
    userlist.append([client_socket, addr])
    now_time = time.asctime()
    print('Connected by address: ', addr, "  ", now_time)

    # 先接收用户名，用户名作为id，与明文key进行拼接，确保不同用户的签名不同（默认用户使用的id是真实身份）
    id = client_socket.recv(buff_size)
    prompt_message = f"{now_time}. Connected! Trent (CA) is ready to sign your message.\nYour UserID is: {id.decode('gbk')}\n"
    prompt_message += "Send your message to be signed."
    client_socket.send(bytes(prompt_message, encoding='gbk'))
    # id = client_socket.recv(1024)
    # print('UserID: ', id.decode('gbk'))

    # 提示client发送待签名的消息
    # prompt_message = "Send your message to be signed."
    # client_socket.send(bytes(prompt_message, encoding='gbk'))

    # 接收明文消息（RSA公钥）
    message = client_socket.recv(buff_size)
    print('Plaintext: ', message.decode('gbk'))

    # 数字签名
    bit_len = 2048  # 密钥长度 bit
    now_time = time.asctime()
    print("\n\n", now_time)
    print("==========================================生成密钥==============================================")
    start_time = time.time()
    p = get_prime(bit_len)
    print(f"p ({bit_len} bit):\n{hex(p)[2:]}")
    # firstp = getFirstPrimitiveRoot(p)  # 第一个本原根, 用于生成所有本原根
    # pR = getAllPrimitiveRoot(p, firstp)  # 所有的本原根
    ## 随机选择一个本原根g
    # g = pR[random.randint(0, len(pR) - 1)]
    g = 2
    x = random.randint(1, p - 2)
    y = pow(g, x, p)
    
    public_key = f"------------------------------------------数字证书 ({time.asctime()})------------------------------------------\
\n颁证机构: Trent (CA)\n持有者: {id.decode('gbk')}\n哈希函数: MD5\n在此之前无效: Sun Jun   9 17:22:17 2024\n在此之后无效: Wed Jul  10 17:22:17 2024\
\n\nElGamal公钥 (p {bit_len}bit, g, y):\n"
    
    pgy_value = f"{hex(p)[2:], hex(g)[2:], hex(y)[2:]}\n"  # 公钥单独发送 便于处理
    print("\n" + public_key + pgy_value)  # 发回给client
    client_socket.send(bytes(public_key, encoding='gbk'))
    # 系统暂停，等待client接收公钥
    time.sleep(0.5)
    client_socket.send(bytes(pgy_value, encoding='gbk'))
    print(f"私钥 x:\n{hex(x)[2:]}")  # 只有server知道，用私钥签名，用公钥验证

    end_time = time.time()
    print(f"生成密钥用时: {end_time - start_time} s")

    # print("\n==========================================数字签名==============================================")
    M = message.decode('gbk')
    massage_M = repr(M)[1:-1]  # 处理转义符(会自动加''，要去掉，不然md5码就错了)

    massage_M += id.decode('gbk')  # 用户id作为盐值

    md5_hash = hashlib.md5(massage_M.encode('utf8')).hexdigest()  # 获取MD5哈希值(16进制)
    print(f"\nMD5 Hash of '{massage_M}' is : {md5_hash}")
    m = int(md5_hash, 16)  # 待签名的消息的哈希值

    # 随机选择 k 使得 gcd(k, p-1)=1
    while True:
        k = random.randint(0, p - 1)
        if gcd(k, p - 1) == 1:
            break

    r = pow(g, k, p)
    # 求 k 的逆元
    # 扩展欧几里得算法求逆, ki 即为最终需要的逆元
    ai, bi = k, p - 1
    ki, ti, xi, yi = 1, 0, 0, 1  # 初始化s,t,x2,y2
    while bi:
        qi, ri = divmod(ai, bi)
        ai, bi = bi, ri  # 求最大公约数
        ki, ti, xi, yi = xi, yi, ki - qi * xi, ti - qi * yi  # 辗转相除

    s = ki * (m - x * r) % (p - 1)
    signature = f"\n数字签名 (r, s):\n{hex(r)[2:], hex(s)[2:]}\n\nRSA公钥 || UserID: {massage_M}\n------------------------------------------\
-----------------------------------------------------------------------------\n\nSignature completed!\n"
    print(signature)
    client_socket.send(bytes(signature, encoding='gbk'))
    time.sleep(1)

    # 关闭当前客户端socket
    client_socket.close()


    """ 处理Alice """
    alice_socket, alice_addr = server_socket.accept()
    now_time = time.asctime()
    print('Connected by address: ', alice_addr, "  ", now_time)

    # 先接收用户名，用户名作为id，与明文key进行拼接，确保不同用户的签名不同（默认用户使用的id是真实身份）
    id = alice_socket.recv(buff_size)
    prompt_message = f"{now_time}. Connected! Trent (CA) is ready to give you the cert.\nYour UserID is: {id.decode('gbk')}\n"
    alice_socket.send(bytes(prompt_message, encoding='gbk'))
    time.sleep(1.5)

    # 发送数字证书signature
    alice_socket.send(bytes(public_key + pgy_value + signature, encoding='gbk'))
    time.sleep(1.5)

    # 发送ElGamal公钥
    alice_socket.send(bytes(hex(p)[2:], encoding='gbk'))
    time.sleep(1.5)
    alice_socket.send(bytes(hex(g)[2:], encoding='gbk'))
    time.sleep(1.5)
    alice_socket.send(bytes(hex(y)[2:], encoding='gbk'))
    time.sleep(1.5)

    # 发送签名
    alice_socket.send(bytes(hex(r)[2:], encoding='gbk'))
    time.sleep(1.5)
    alice_socket.send(bytes(hex(s)[2:], encoding='gbk'))
    time.sleep(1.5)

    print("Cert sent!\n")
    alice_socket.close()

    print("Press '否(N)' to continue or '是(Y)' to close the server...")
    res = messagebox.askokcancel('提示', '是否关闭服务器？')
    if res == True :
        print("Server closed!\n")
        server_socket.close()
        exit(0)
    else:
        print("Server still running!")
        break

