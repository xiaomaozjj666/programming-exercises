#!/usr/bin/env python3
"""进制转换与算法复杂度基准测试"""
import time, random, sys, argparse

DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

def decimal_to_base(n, base: int) -> str:
    if base < 2 or base > 36: raise ValueError("base must be 2-36")
    if isinstance(n, float):
        if not n.is_integer(): raise ValueError("only integers are supported")
        n = int(n)
    if n == 0: return "0"
    sign = "-" if n < 0 else ""
    n = abs(n)
    res = []
    while n > 0:
        res.append(DIGITS[n % base])
        n //= base
    return sign + "".join(reversed(res))

def base_to_decimal(s: str, base: int):
    if base < 2 or base > 36: raise ValueError("base must be 2-36")
    s = s.upper()
    sign = 1
    if s.startswith("-"): sign, s = -1, s[1:]
    elif s.startswith("+"): s = s[1:]
    def digit_val(ch):
        v = DIGITS.find(ch)
        if v == -1 or v >= base: raise ValueError(f"invalid digit '{ch}' for base {base}")
        return v
    if "." in s:
        int_part, frac_part = s.split(".", 1)
        value = 0.0
        for ch in int_part: value = value * base + digit_val(ch)
        factor = 1.0 / base
        for ch in frac_part:
            value += digit_val(ch) * factor
            factor /= base
        return sign * value
    n = 0
    for ch in s:
        n = n * base + digit_val(ch)
    return sign * n

def decimal_to_binary(n: int) -> str: return decimal_to_base(n, 2)
def binary_to_decimal(s: str): return base_to_decimal(s, 2)
def decimal_to_hex(n: int) -> str: return decimal_to_base(n, 16)
def hex_to_decimal(s: str): return base_to_decimal(s, 16)

def linear_search(arr, target):
    for i, v in enumerate(arr):
        if v == target: return i
    return -1

def binary_search(arr, target):
    lo, hi = 0, len(arr) - 1
    while lo <= hi:
        mid = (lo + hi) // 2
        if arr[mid] == target: return mid
        if arr[mid] < target: lo = mid + 1
        else: hi = mid - 1
    return -1

def bubble_sort(arr):
    a = arr[:]
    n = len(a)
    for i in range(n - 1):
        for j in range(n - i - 1):
            if a[j] > a[j + 1]:
                a[j], a[j + 1] = a[j + 1], a[j]
    return a

def test_conv(name, got, expected):
    ok = got == expected
    print(f"  {name}: {got} (expected {expected}) {'PASS' if ok else 'FAIL'}")

def run_tests():
    print("=== 进制转换测试 ===\n")
    print("Decimal to Binary:")
    for n, e in [(0,"0"),(1,"1"),(10,"1010"),(255,"11111111")]:
        test_conv(f"dec {n} -> bin", decimal_to_binary(n), e)
    print("\nBinary to Decimal:")
    for n, e in [("0",0),("1",1),("1010",10),("11111111",255)]:
        test_conv(f"bin {n} -> dec", binary_to_decimal(n), e)
    print("\nDecimal to Hex:")
    for n, e in [(0,"0"),(1,"1"),(10,"A"),(255,"FF"),(1000,"3E8")]:
        test_conv(f"dec {n} -> hex", decimal_to_hex(n), e)
    print("\nHex to Decimal:")
    for n, e in [("0",0),("1",1),("A",10),("FF",255),("3E8",1000)]:
        test_conv(f"hex {n} -> dec", hex_to_decimal(n), e)

    print("\n=== 算法测试 ===\n")
    arr = [1,3,5,7,9,11,13,15,17,19]
    print(f"LinearSearch: find 11 in {arr} -> index {linear_search(arr,11)} (expected 5)")
    print(f"LinearSearch: find 20 in {arr} -> index {linear_search(arr,20)} (expected -1)")
    print(f"BinarySearch: find 11 in {arr} -> index {binary_search(arr,11)} (expected 5)")
    print(f"BinarySearch: find 20 in {arr} -> index {binary_search(arr,20)} (expected -1)")
    unsorted = [9,3,7,1,5,11,13,2,4,8]
    print(f"BubbleSort: {unsorted} -> {bubble_sort(unsorted)}")

    print("\n=== 时间复杂度分析 ===\n")
    big = [random.randint(0,9999) for _ in range(10000)]
    sorted_big = sorted(big)

    t0 = time.perf_counter(); linear_search(big, -1); t1 = time.perf_counter()
    print(f"LinearSearch O(n):  {(t1-t0)*1000:.4f}ms (n=10000)")

    t0 = time.perf_counter(); binary_search(sorted_big, -1); t1 = time.perf_counter()
    print(f"BinarySearch O(logn): {(t1-t0)*1000:.4f}ms (n=10000)")

    small = [random.randint(0,999) for _ in range(1000)]
    t0 = time.perf_counter(); bubble_sort(small); t1 = time.perf_counter()
    print(f"BubbleSort O(n^2):   {(t1-t0)*1000:.4f}ms (n=1000)")

    print("\nBinarySearch > LinearSearch > BubbleSort 的时间复杂度对比.")

def run_convert(args):
    try:
        if args.from_base and args.to_base:
            dec = base_to_decimal(args.number, args.from_base)
            if args.to_base == 10:
                res = dec
            else:
                res = decimal_to_base(dec, args.to_base)
        elif args.from_base:
            print(base_to_decimal(args.number, args.from_base))
            return
        else:
            res = decimal_to_base(int(args.number, 10), args.to_base or 10)
        print(res)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr); sys.exit(1)

if __name__ == "__main__":
    p = argparse.ArgumentParser(description="进制转换与算法复杂度基准测试")
    p.add_argument("--test", action="store_true", help="运行全部测试")
    p.add_argument("--convert", action="store_true", help="转换模式")
    p.add_argument("--number", help="输入数字")
    p.add_argument("--from-base", type=int, help="源进制 (2-36)")
    p.add_argument("--to-base", type=int, help="目标进制 (2-36)")
    args = p.parse_args()
    if args.convert:
        run_convert(args)
    else:
        run_tests()
