#!/usr/bin/env python3
"""任意进制转换器：支持负数与小数，2-36 进制，仅用 Python 标准库。

设计要点：
- 整数部分使用任意精度 int（无 2^53 精度丢失问题）；
- 小数部分使用 fractions.Fraction 精确表示，输出时乘基取整，最多 15 位小数。

用法：
    python3 base_converter.py <数字> <源进制> <目标进制>   # 转换并打印结果
    python3 base_converter.py --test                      # 运行内置测试

示例：
    python3 base_converter.py 255 10 16     -> FF
    python3 base_converter.py -123 10 2      -> -1111011
    python3 base_converter.py 10.5 10 16     -> A.8
    python3 base_converter.py 0.5 10 2       -> 0.1
"""

import sys
from fractions import Fraction

DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
FRAC_PRECISION = 15  # 小数位输出上限


def parse_number(number: str, base: int):
    """把任意进制的字符串解析为 (ok, 符号, 整数部分, 小数部分)。

    整数部分返回 int（任意精度），小数部分返回 Fraction（精确）。
    """
    if base < 2 or base > 36:
        return False, 1, 0, Fraction(0)
    text = number.strip()
    if not text:
        return False, 1, 0, Fraction(0)
    sign = -1 if text.startswith("-") else 1
    if text[0] in "+-":
        text = text[1:]
    if "." in text:
        int_str, frac_str = text.split(".", 1)
    else:
        int_str, frac_str = text, ""
    if not int_str:
        int_str = "0"
    int_part = 0
    for ch in int_str:
        pos = DIGITS.find(ch.upper())
        if pos == -1 or pos >= base:
            return False, 1, 0, Fraction(0)
        int_part = int_part * base + pos
    frac_part = Fraction(0)
    for i, ch in enumerate(frac_str, start=1):
        pos = DIGITS.find(ch.upper())
        if pos == -1 or pos >= base:
            return False, 1, 0, Fraction(0)
        frac_part += Fraction(pos, base**i)
    return True, sign, int_part, frac_part


def format_number(sign: int, int_part: int, frac_part: Fraction, base: int) -> str:
    """把 (符号, 整数部分, 小数部分) 格式化为目标进制字符串。"""
    digits = []
    n = int_part
    if n == 0:
        digits.append("0")
    else:
        while n > 0:
            digits.append(DIGITS[n % base])
            n //= base
        digits.reverse()
    out = "".join(digits)
    if frac_part:
        out += "."
        f = frac_part
        precision = FRAC_PRECISION
        while f and precision > 0:
            f *= base
            digit = int(f)
            out += DIGITS[digit]
            f -= digit
            precision -= 1
    return ("-" if sign < 0 else "") + out


def base_to_base(number: str, src_base: int, dst_base: int) -> str:
    """任意进制互转（2-36），支持负数与小数。"""
    ok, sign, int_part, frac_part = parse_number(number, src_base)
    if not ok:
        return "ERROR: Invalid input!"
    return format_number(sign, int_part, frac_part, dst_base)


def run_tests() -> bool:
    cases = [
        # (输入, 源进制, 目标进制, 期望)
        ("0", 10, 16, "0"),
        ("255", 10, 16, "FF"),
        ("FF", 16, 10, "255"),
        ("100110", 3, 16, "FF"),
        ("-123", 10, 2, "-1111011"),
        ("-FF", 16, 10, "-255"),
        ("0.5", 10, 2, "0.1"),
        ("10.5", 10, 16, "A.8"),
        ("-0.5", 10, 2, "-0.1"),
        # 超过 2^53 的大整数（Python int 任意精度，无精度丢失）
        ("9007199254740993", 10, 16, "20000000000001"),
        ("20000000000001", 16, 10, "9007199254740993"),
        # 非法输入
        ("2", 2, 10, "ERROR: Invalid input!"),
        ("12G", 16, 10, "ERROR: Invalid input!"),
    ]
    failed = 0
    for number, src, dst, expected in cases:
        got = base_to_base(number, src, dst)
        ok = got == expected
        if not ok:
            failed += 1
        print(f"  [{'PASS' if ok else 'FAIL'}] {number!r} ({src}->{dst}): {got!r}"
              + ("" if ok else f" (expected {expected!r})"))
    print(f"共 {len(cases)} 个用例，失败 {failed} 个。")
    return failed == 0


def main() -> int:
    if len(sys.argv) == 2 and sys.argv[1] == "--test":
        return 0 if run_tests() else 1
    if len(sys.argv) != 4:
        print("用法: python3 base_converter.py <数字> <源进制> <目标进制>  或  --test")
        return 2
    try:
        src, dst = int(sys.argv[2]), int(sys.argv[3])
    except ValueError:
        print("ERROR: Invalid base!")
        return 2
    print(base_to_base(sys.argv[1], src, dst))
    return 0


if __name__ == "__main__":
    sys.exit(main())
