from GetBigPrime import is_prime
from multiprocessing import Pool
import socket


def gcd(a: int, b: int):
    """欧几里得算法求最大公约数"""
    while a != 0:
        a, b = b % a, a
    return b

def euler(n):
    """欧拉函数"""
    # 如果n是质数直接返回n-1, 因为n一定是素数, 这里直接返回n-1就是n的欧拉函数值了
    return n - 1
    # if is_prime(n):
    #     return n - 1
    # m = 0
    # for i in range(n):
    #     if gcd(i, n) == 1:
    #         m += 1
    # return m

def prime_factors(n):
    """返回n的所有素数因子"""
    i = 2
    factors = set()
    while i * i <= n:
        if n % i:
            i += 1
        else:
            n //= i
            factors.add(i)
    if n > 1:
        factors.add(n)
    return factors

def is_primitive_root(a, p, euler_n, factors):
    """检查 a 是否是本原根"""
    return all(pow(a, euler_n // factor, p) != 1 for factor in factors)


def getFirstPrimitiveRoot(p):
    """算出第一个本原根"""
    euler_n = euler(p)
    factors = prime_factors(euler_n)
    with Pool() as pool:
        results = pool.starmap(is_primitive_root, [(a, p, euler_n, factors) for a in range(2, p)])
        for a, result in enumerate(results, start=2):
            if result:
                return a
    return False


def compute_primitive_root(i, p, first):
    """计算本原根的辅助函数"""
    if gcd(i, p - 1) == 1:
        return pow(first, i, p)
    return None

def getAllPrimitiveRoot(p, first):
    """求得所有的本原根"""
    primitiveRoot = set()
    with Pool() as pool:
        results = pool.starmap(compute_primitive_root, [(i, p, first) for i in range(1, p)])
        for result in results:
            if result is not None:
                primitiveRoot.add(result)
    return list(primitiveRoot)

def get_host_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))  # 114.114.114.114也是dns地址
        ip = s.getsockname()[0]
    finally:
        s.close()
    return ip


