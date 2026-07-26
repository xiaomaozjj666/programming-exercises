// Any Base to Any Base Converter with Time Complexity Measurement
// Supports bases 2-36, case-insensitive input, large numbers

#include <iostream>
#include <string>
#include <cctype>
#include <chrono>

using namespace std;
using namespace chrono;

/**
 * Converts a decimal number to any base (2-36)
 * Time Complexity: O(log_b n)
 */
string decimalToBase(long long decimal, int base) {
    if (base < 2 || base > 36) return "ERROR: Invalid base!";
    if (decimal == 0) return "0";

    bool negative = false;
    if (decimal < 0) {
        negative = true;
        decimal = -decimal;
    }

    const string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string result = "";

    while (decimal > 0) {
        result = digits[decimal % base] + result;
        decimal /= base;
    }

    return negative ? "-" + result : result;
}

/**
 * Converts a number from any base (2-36) to decimal
 * Time Complexity: O(n)
 */
long long baseToDecimal(const string& number, int base) {
    if (base < 2 || base > 36) return -1;
    const string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    long long decimal = 0;

    for (char c : number) {
        char upperC = toupper(c);
        size_t pos = digits.find(upperC);

        if (pos == string::npos || pos >= base) return -1;
        decimal = decimal * base + pos;
    }
    return decimal;
}

/**
 * Converts ANY base to ANY base (2-36)
 * Time Complexity: O(n + log_b m)
 */
string baseToBase(const string& number, int srcBase, int dstBase) {
    long long dec = baseToDecimal(number, srcBase);
    if (dec == -1) return "ERROR: Invalid input!";
    return decimalToBase(dec, dstBase);
}

int main() {
    string num;
    int srcBase, dstBase;

    cout << "========== Any Base to Any Base Converter ==========" << endl;
    cout << "Enter number: ";
    cin >> num;
    cout << "Enter source base (2-36): ";
    cin >> srcBase;
    cout << "Enter target base (2-36): ";
    cin >> dstBase;

    // Start timing
    auto start = high_resolution_clock::now();

    // Run conversion
    string result = baseToBase(num, srcBase, dstBase);

    // End timing
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start);

    // Output
    cout << "\nResult: " << result << endl;
    cout << "Execution time: " << duration.count() << " nanoseconds" << endl;

    // Time Complexity Info
    cout << "\nTime Complexity Analysis:" << endl;
    cout << "- baseToDecimal: O(n)  (n = length of input string)" << endl;
    cout << "- decimalToBase: O(log_b m)  (m = decimal value, b = target base)" << endl;
    cout << "- Total: O(n + log_b m)" << endl;

    return 0;
}

