// 任意进制转换器：支持负数与小数，2-36 进制，仅用 C++ 标准库。
//
// 设计要点：
// - 整数部分用 long long 承载；小数部分用 double（教学示例，注意浮点精度；
//   需要任意精度时可参照 python/ 与 js/ 版本的有理数实现）；
// - 错误处理使用 bool& 输出参数，不再用 -1 同时充当"值"与"错误码"。
//
// 编译运行：
//   g++ -O2 -std=c++17 base_converter.cpp -o base_converter
//   ./base_converter              # 交互模式
//   ./base_converter 255 10 16    # 命令行模式
//   ./base_converter --test       # 内置测试
// （Windows / MSVC 需额外加 /utf-8，否则中文注释会被按本地代码页误读）
//
// 示例：
//   ./base_converter 255 10 16     -> FF
//   ./base_converter -123 10 2      -> -1111011
//   ./base_converter 10.5 10 16     -> A.8
//   ./base_converter 0.5 10 2       -> 0.1

#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static const string DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * 解析任意进制的字符串。
 * @param number  输入字符串（可带正负号与小数点）
 * @param base    源进制（2-36）
 * @param ok      输出参数：解析是否成功
 * @param sign    输出参数：1 或 -1
 * @param intPart 输出参数：整数部分（long long）
 * @param fracPart 输出参数：小数部分（double，0 表示没有小数）
 */
void parseNumber(const string& number, int base, bool& ok, int& sign,
                 long long& intPart, double& fracPart) {
    ok = false;
    sign = 1;
    intPart = 0;
    fracPart = 0.0;
    if (base < 2 || base > 36) return;

    string text = number;
    // 去掉首尾空白
    size_t b = text.find_first_not_of(" \t\r\n");
    if (b == string::npos) return;
    size_t e = text.find_last_not_of(" \t\r\n");
    text = text.substr(b, e - b + 1);
    if (text.empty()) return;

    if (text[0] == '-' || text[0] == '+') {
        sign = (text[0] == '-') ? -1 : 1;
        text = text.substr(1);
    }

    string intStr = text, fracStr;
    size_t dot = text.find('.');
    if (dot != string::npos) {
        intStr = text.substr(0, dot);
        fracStr = text.substr(dot + 1);
    }
    if (intStr.empty()) intStr = "0";

    // 整数部分：Horner 求值（超出 long long 范围视为非法输入，避免有符号溢出 UB）
    for (char c : intStr) {
        size_t pos = DIGITS.find((char)toupper((unsigned char)c));
        if (pos == string::npos || (int)pos >= base) return;
        if (intPart > (LLONG_MAX - (long long)pos) / base) return;
        intPart = intPart * base + (long long)pos;
    }
    // 小数部分：累加 digit * base^-k
    double scale = 1.0;
    for (char c : fracStr) {
        size_t pos = DIGITS.find((char)toupper((unsigned char)c));
        if (pos == string::npos || (int)pos >= base) return;
        scale /= base;
        fracPart += (double)pos * scale;
    }
    ok = true;
}

/**
 * 把 (符号, 整数部分, 小数部分) 格式化为目标进制字符串。
 */
string formatNumber(int sign, long long intPart, double fracPart, int base) {
    if (base < 2 || base > 36) return "ERROR: Invalid base!";

    string digits;
    long long n = intPart;
    if (n == 0) {
        digits = "0";
    } else {
        while (n > 0) {
            digits += DIGITS[n % base];
            n /= base;
        }
        reverse(digits.begin(), digits.end());
    }

    if (fracPart > 1e-12) {
        digits += '.';
        double f = fracPart;
        int precision = 15;  // 小数位输出上限
        while (f > 1e-12 && precision > 0) {
            f *= base;
            int digit = (int)f;
            digits += DIGITS[digit];
            f -= digit;
            precision--;
        }
    }
    if (digits == "0") return "0"; // -0 规范化为 0
    return (sign < 0 ? "-" : "") + digits;
}

/**
 * 任意进制互转（2-36），支持负数与小数。
 */
string baseToBase(const string& number, int srcBase, int dstBase) {
    if (srcBase < 2 || srcBase > 36 || dstBase < 2 || dstBase > 36) {
        return "ERROR: Invalid base!";
    }
    bool ok;
    int sign;
    long long intPart;
    double fracPart;
    parseNumber(number, srcBase, ok, sign, intPart, fracPart);
    if (!ok) return "ERROR: Invalid input!";
    return formatNumber(sign, intPart, fracPart, dstBase);
}

/** 内置测试：返回是否全部通过。 */
bool runTests() {
    struct Case { string number; int src; int dst; string expected; };
    vector<Case> cases = {
        {"0", 10, 16, "0"},
        {"255", 10, 16, "FF"},
        {"FF", 16, 10, "255"},
        {"100110", 3, 16, "FF"},
        {"-123", 10, 2, "-1111011"},
        {"-FF", 16, 10, "-255"},
        {"0.5", 10, 2, "0.1"},
        {"10.5", 10, 16, "A.8"},
        {"-0.5", 10, 2, "-0.1"},
        // 超过 2^53 的大整数（long long 仍精确；py/js 版用 int/BigInt）
        {"9007199254740993", 10, 16, "20000000000001"},
        // 超出 long long 范围（2^63）→ 非法输入
        {"9223372036854775808", 10, 10, "ERROR: Invalid input!"},
        {"2", 2, 10, "ERROR: Invalid input!"},
        {"12G", 16, 10, "ERROR: Invalid input!"},
        // 进制越界
        {"255", 10, 99, "ERROR: Invalid base!"},
        {"FF", 99, 10, "ERROR: Invalid base!"},
    };
    int failed = 0;
    for (const auto& c : cases) {
        string got = baseToBase(c.number, c.src, c.dst);
        bool ok = got == c.expected;
        if (!ok) ++failed;
        cout << "  [" << (ok ? "PASS" : "FAIL") << "] " << c.number
             << " (" << c.src << "->" << c.dst << "): " << got
             << (ok ? "" : " (expected " + c.expected + ")") << endl;
    }
    cout << "共 " << cases.size() << " 个用例，失败 " << failed << " 个。" << endl;
    return failed == 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2 && string(argv[1]) == "--test") {
        return runTests() ? 0 : 1;
    }
    if (argc == 4) {
        // strtol 溢出时钳位而非未定义行为，随后的 base 范围校验会拒绝非法值
        int srcBase = (int)strtol(argv[2], nullptr, 10);
        int dstBase = (int)strtol(argv[3], nullptr, 10);
        if (srcBase < 2 || srcBase > 36 || dstBase < 2 || dstBase > 36) {
            cout << "ERROR: Invalid base!" << endl;
            return 2;
        }
        string result = baseToBase(argv[1], srcBase, dstBase);
        cout << result << endl;
        return result.rfind("ERROR", 0) == 0 ? 1 : 0;
    }

    // 交互模式
    string num;
    int srcBase, dstBase;
    cout << "========== Any Base to Any Base Converter ==========" << endl;
    cout << "Enter number: ";
    cin >> num;
    cout << "Enter source base (2-36): ";
    cin >> srcBase;
    if (srcBase < 2 || srcBase > 36) {
        cout << "ERROR: Invalid base!" << endl;
        return 2;
    }
    cout << "Enter target base (2-36): ";
    cin >> dstBase;
    if (dstBase < 2 || dstBase > 36) {
        cout << "ERROR: Invalid base!" << endl;
        return 2;
    }

    auto start = chrono::high_resolution_clock::now();
    string result = baseToBase(num, srcBase, dstBase);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);

    cout << "\nResult: " << result << endl;
    cout << "Execution time: " << duration.count() << " nanoseconds" << endl;

    cout << "\nTime Complexity Analysis:" << endl;
    cout << "- parseNumber: O(n)  (n = length of input string)" << endl;
    cout << "- formatNumber: O(log_b m)  (m = decimal value, b = target base)" << endl;
    cout << "- Total: O(n + log_b m)" << endl;
    return result.rfind("ERROR", 0) == 0 ? 1 : 0;
}
