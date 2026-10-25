// Base Conversion and Time Complexity Test Cases
// This file demonstrates base conversion functions and time complexity analysis

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>
#include <stdexcept>

// ==================== Base Conversion Functions ====================

/**
 * Converts a decimal number to any base
 * Time Complexity: O(log_b n) - where b is the target base, each iteration divides n by b
 * @param decimal The decimal number to convert
 * @param base The target base (2-36)
 * @return The representation of the number in the target base as a string
 */
std::string decimalToBase(double decimal, int base) {
    if (base < 2 || base > 36) {
        throw std::invalid_argument("Invalid base");
    }
    if (decimal == 0) return "0";

    // Values beyond the long long range (or non-finite) would make the
    // static_cast below undefined behavior; fall back to decimal scientific notation.
    // max_digits10 保证 double 的往返精度（默认 6 位会把 12345678901 打成 1.23457e+10）。
    double absDecimal = std::abs(decimal);
    if (!(absDecimal < 9.2233720368547758e18) ||
        (base == 10 && (absDecimal > 1e10 || absDecimal < 1e-6))) {
        std::stringstream ss;
        ss.precision(std::numeric_limits<double>::max_digits10);
        ss << decimal;
        return ss.str();
    }

    static const std::string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result;
    result.reserve(64); // Preallocate space to avoid reallocations
    
    // Handle negative numbers
    bool isNegative = false;
    if (decimal < 0) {
        isNegative = true;
        decimal = -decimal;
    }
    
    // Convert integer part
    long long intPart = static_cast<long long>(decimal);
    if (intPart == 0) {
        result = "0";
    } else {
        std::string intResult;
        intResult.reserve(32);
        while (intPart > 0) {
            intResult += digits[intPart % base];
            intPart = intPart / base;
        }
        // Reverse the integer part
        std::reverse(intResult.begin(), intResult.end());
        result = intResult;
    }
    
    // Convert fractional part
    double fracPart = decimal - static_cast<long long>(decimal);
    if (fracPart > 1e-15) { // Use smaller epsilon for better precision
        result += ".";
        int precision = 15; // Increase precision to 15 decimal places
        while (fracPart > 1e-15 && precision > 0) {
            fracPart *= base;
            int digit = static_cast<int>(fracPart);
            result += digits[digit];
            fracPart -= digit;
            precision--;
        }
    }
    
    // Add negative sign if needed
    if (isNegative) {
        result = "-" + result;
    }
    
    return result;
}

/**
 * Converts a number from any base to decimal
 * Time Complexity: O(n) - where n is the length of the input string
 * @param number The number in the source base as a string
 * @param base The source base (2-36)
 * @return The decimal representation of the number
 */
double baseToDecimal(const std::string& number, int base) {
    if (base < 2 || base > 36) {
        throw std::invalid_argument("Invalid base");
    }
    
    // Special case for base 10 with scientific notation
    if (base == 10) {
        size_t ePos = number.find('e');
        size_t EPos = number.find('E');
        if (ePos != std::string::npos || EPos != std::string::npos) {
            std::stringstream ss(number);
            double value = 0.0;
            if (!(ss >> value)) {
                throw std::invalid_argument("Invalid scientific notation");
            }
            return value;
        }
    }
    
    static const std::string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    double decimal = 0.0;
    size_t i = 0;
    size_t n = number.size();
    
    // Handle negative numbers
    bool isNegative = false;
    if (n > 0 && number[0] == '-') {
        isNegative = true;
        i = 1;
    }
    
    // Find decimal point
    size_t decimalPoint = number.find('.');
    if (decimalPoint == std::string::npos) {
        decimalPoint = n;
    }
    
    // Convert integer part
    for (; i < decimalPoint; i++) {
        char c = number[i];
        char upperC = (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c;
        size_t digitValue = digits.find(upperC);
        if (digitValue == std::string::npos || digitValue >= static_cast<size_t>(base)) {
            throw std::invalid_argument("Invalid digit for the given base");
        }
        decimal = decimal * base + digitValue;
    }
    
    // Convert fractional part
    if (decimalPoint < n) {
        double factor = 1.0 / base;
        for (i = decimalPoint + 1; i < n; i++) {
            char c = number[i];
            char upperC = (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c;
            size_t digitValue = digits.find(upperC);
            if (digitValue == std::string::npos || digitValue >= static_cast<size_t>(base)) {
                throw std::invalid_argument("Invalid digit for the given base");
            }
            decimal += digitValue * factor;
            factor /= base;
        }
    }
    
    // Apply negative sign if needed
    if (isNegative) {
        decimal = -decimal;
    }
    
    return decimal;
}

/**
 * Converts a number from one base to another
 * Time Complexity: O(n + log_b m) - where n is the length of the input string, b is the target base, and m is the decimal value
 * @param number The number in the source base as a string
 * @param sourceBase The source base (2-36)
 * @param targetBase The target base (2-36)
 * @return The representation of the number in the target base as a string
 */
std::string baseToBase(const std::string& number, int sourceBase, int targetBase) {
    try {
        double decimal = baseToDecimal(number, sourceBase);
        return decimalToBase(decimal, targetBase);
    } catch (const std::exception& e) {
        return std::string("Invalid input: ") + e.what();
    }
}

/**
 * Converts a decimal number to binary (for backward compatibility)
 * Time Complexity: O(log_2 n)
 * @param decimal The decimal number to convert
 * @return The binary representation as a string
 */
std::string decimalToBinary(int decimal) {
    return decimalToBase(static_cast<double>(decimal), 2);
}

/**
 * Converts a binary string to decimal (for backward compatibility)
 * Time Complexity: O(n)
 * @param binary The binary string to convert
 * @return The decimal representation
 */
double binaryToDecimal(const std::string& binary) {
    return baseToDecimal(binary, 2);
}

/**
 * Converts a decimal number to hexadecimal (for backward compatibility)
 * Time Complexity: O(log_16 n)
 * @param decimal The decimal number to convert
 * @return The hexadecimal representation as a string
 */
std::string decimalToHexadecimal(int decimal) {
    return decimalToBase(static_cast<double>(decimal), 16);
}

/**
 * Converts a hexadecimal string to decimal (for backward compatibility)
 * Time Complexity: O(n)
 * @param hex The hexadecimal string to convert
 * @return The decimal representation
 */
double hexadecimalToDecimal(const std::string& hex) {
    return baseToDecimal(hex, 16);
}

// ==================== Time Complexity Demonstration ====================

/**
 * Linear search algorithm
 * Time Complexity: O(n) - worst case requires checking all elements
 * @param arr The array to search
 * @param target The value to search for
 * @return The index of the target, or -1 if not found
 */
int linearSearch(const std::vector<int>& arr, int target) {
    for (int i = 0; i < static_cast<int>(arr.size()); i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

/**
 * Binary search algorithm (requires sorted array)
 * Time Complexity: O(log n) - each iteration halves the search space
 * @param arr The sorted array to search
 * @param target The value to search for
 * @return The index of the target, or -1 if not found
 */
int binarySearch(const std::vector<int>& arr, int target) {
    int left = 0;
    int right = static_cast<int>(arr.size()) - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/**
 * Bubble sort algorithm
 * Time Complexity: O(n^2) - nested loops
 * @param arr The array to sort
 * @return The sorted array
 */
std::vector<int> bubbleSort(std::vector<int> arr) {
    int n = static_cast<int>(arr.size());
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
    return arr;
}

// ==================== Test Cases ====================

// 失败计数：与 Python/JS 版一致，任何断言失败都让进程以非零码退出
static int g_failures = 0;

void checkResult(const std::string& label, const std::string& got, const std::string& expected) {
    bool passed = (got == expected);
    if (!passed) ++g_failures;
    std::cout << "  " << label << ": " << got << " (expected " << expected << ") "
              << (passed ? "PASS" : "FAIL") << std::endl;
}

void checkResult(const std::string& label, double got, double expected) {
    bool passed = (got == expected);
    if (!passed) ++g_failures;
    std::cout << "  " << label << ": " << got << " (expected " << expected << ") "
              << (passed ? "PASS" : "FAIL") << std::endl;
}

/**
 * Test decimal to binary conversion
 * @param input The decimal input
 * @param expected The expected binary output
 */
void testDecimalToBinary(int input, const std::string& expected) {
    checkResult("dec " + std::to_string(input) + " -> bin", decimalToBinary(input), expected);
}

/**
 * Test binary to decimal conversion
 * @param input The binary input
 * @param expected The expected decimal output
 */
void testBinaryToDecimal(const std::string& input, int expected) {
    checkResult("bin " + input + " -> dec", binaryToDecimal(input), expected);
}

/**
 * Test decimal to hexadecimal conversion
 * @param input The decimal input
 * @param expected The expected hexadecimal output
 */
void testDecimalToHexadecimal(int input, const std::string& expected) {
    checkResult("dec " + std::to_string(input) + " -> hex", decimalToHexadecimal(input), expected);
}

/**
 * Test hexadecimal to decimal conversion
 * @param input The hexadecimal input
 * @param expected The expected decimal output
 */
void testHexadecimalToDecimal(const std::string& input, int expected) {
    checkResult("hex " + input + " -> dec", hexadecimalToDecimal(input), expected);
}

/**
 * Demonstrate time complexity by measuring execution time
 */
void demonstrateTimeComplexity() {
    // Generate a large array for testing
    std::vector<int> largeArray;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 9999);
    
    for (int i = 0; i < 10000; i++) {
        largeArray.push_back(distr(gen));
    }
    
    std::vector<int> sortedArray = largeArray;
    std::sort(sortedArray.begin(), sortedArray.end());
    
    // Test linear search time
    std::cout << "Linear Search Time:" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    linearSearch(largeArray, 9999);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << "ms" << std::endl;
    
    // Test binary search time
    std::cout << "\nBinary Search Time:" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    binarySearch(sortedArray, 9999);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << "ms" << std::endl;
    
    // Test bubble sort time (on a smaller array to avoid long execution)
    std::cout << "\nBubble Sort Time (on 1000 elements):" << std::endl;
    std::vector<int> smallArray;
    for (int i = 0; i < 1000; i++) {
        smallArray.push_back(distr(gen));
    }
    start = std::chrono::high_resolution_clock::now();
    bubbleSort(smallArray);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << "ms" << std::endl;
    
    std::cout << "\nNote: Binary search is significantly faster than linear search for large sorted arrays." << std::endl;
    std::cout << "Bubble sort has O(n^2) time complexity and becomes slow for larger arrays." << std::endl;
}

/**
 * Test arbitrary base conversion
 */
void testArbitraryBaseConversion();

/**
 * Run all test cases
 */
void runTests() {
    std::cout << "=== Base Conversion Tests ===" << std::endl;
    
    // Test decimal to binary conversion
    std::cout << "Decimal to Binary:" << std::endl;
    testDecimalToBinary(0, "0");
    testDecimalToBinary(1, "1");
    testDecimalToBinary(10, "1010");
    testDecimalToBinary(255, "11111111");
    
    // Test binary to decimal conversion
    std::cout << "\nBinary to Decimal:" << std::endl;
    testBinaryToDecimal("0", 0);
    testBinaryToDecimal("1", 1);
    testBinaryToDecimal("1010", 10);
    testBinaryToDecimal("11111111", 255);
    
    // Test decimal to hexadecimal conversion
    std::cout << "\nDecimal to Hexadecimal:" << std::endl;
    testDecimalToHexadecimal(0, "0");
    testDecimalToHexadecimal(1, "1");
    testDecimalToHexadecimal(10, "A");
    testDecimalToHexadecimal(255, "FF");
    testDecimalToHexadecimal(1000, "3E8");
    
    // Test hexadecimal to decimal conversion
    std::cout << "\nHexadecimal to Decimal:" << std::endl;
    testHexadecimalToDecimal("0", 0);
    testHexadecimalToDecimal("1", 1);
    testHexadecimalToDecimal("A", 10);
    testHexadecimalToDecimal("FF", 255);
    testHexadecimalToDecimal("3E8", 1000);
    
    std::cout << "\n=== Time Complexity Algorithm Tests ===" << std::endl;

    // 断言式自检（与 Python/JS 版一致）：查找/排序结果错了 CI 必须红
    std::vector<int> testArray = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    checkResult("LinearSearch: find 11", static_cast<double>(linearSearch(testArray, 11)), 5.0);
    checkResult("LinearSearch: find 20", static_cast<double>(linearSearch(testArray, 20)), -1.0);
    checkResult("BinarySearch: find 11", static_cast<double>(binarySearch(testArray, 11)), 5.0);
    checkResult("BinarySearch: find 20", static_cast<double>(binarySearch(testArray, 20)), -1.0);

    std::vector<int> unsortedArray = {9, 3, 7, 1, 5, 11, 13, 2, 4, 8};
    std::vector<int> sortedArray = bubbleSort(unsortedArray);
    std::vector<int> expectedSorted = unsortedArray;
    std::sort(expectedSorted.begin(), expectedSorted.end());
    checkResult("BubbleSort",
                sortedArray == expectedSorted ? "sorted" : "wrong order",
                "sorted");
    
    // Time complexity demonstration
    std::cout << "\n=== Time Complexity Analysis ===" << std::endl;
    demonstrateTimeComplexity();
    
    // Test arbitrary base conversion
    std::cout << "\n=== Arbitrary Base Conversion Tests ===" << std::endl;
    testArbitraryBaseConversion();

    if (g_failures > 0) {
        std::cout << "\n" << g_failures << " check(s) FAIL" << std::endl;
    }
}

/**
 * Test arbitrary base conversion
 */
void testArbitraryBaseConversion() {
    std::cout << "Decimal to Various Bases:" << std::endl;
    int testNumber = 255;
    std::cout << "Decimal: " << testNumber << std::endl;
    std::cout << "Binary: " << decimalToBase(testNumber, 2) << std::endl;
    std::cout << "Octal: " << decimalToBase(testNumber, 8) << std::endl;
    std::cout << "Hexadecimal: " << decimalToBase(testNumber, 16) << std::endl;
    std::cout << "Base 3: " << decimalToBase(testNumber, 3) << std::endl;
    std::cout << "Base 7: " << decimalToBase(testNumber, 7) << std::endl;
    std::cout << "Base 36: " << decimalToBase(testNumber, 36) << std::endl;
    
    std::cout << "\nNegative Numbers:" << std::endl;
    std::cout << "Decimal: -255" << std::endl;
    std::cout << "Binary: " << decimalToBase(-255, 2) << std::endl;
    std::cout << "Hexadecimal: " << decimalToBase(-255, 16) << std::endl;
    
    std::cout << "\nDecimal Fractions:" << std::endl;
    std::cout << "Decimal: 0.5" << std::endl;
    std::cout << "Binary: " << decimalToBase(0.5, 2) << std::endl;
    std::cout << "Decimal: 0.125" << std::endl;
    std::cout << "Binary: " << decimalToBase(0.125, 2) << std::endl;
    std::cout << "Decimal: 0.625" << std::endl;
    std::cout << "Binary: " << decimalToBase(0.625, 2) << std::endl;
    
    std::cout << "\nMixed Numbers:" << std::endl;
    std::cout << "Decimal: 10.5" << std::endl;
    std::cout << "Binary: " << decimalToBase(10.5, 2) << std::endl;
    std::cout << "Hexadecimal: " << decimalToBase(10.5, 16) << std::endl;
    
    std::cout << "\nLarge Numbers:" << std::endl;
    std::cout << "Decimal: 1000000" << std::endl;
    std::cout << "Binary: " << decimalToBase(1000000, 2) << std::endl;
    std::cout << "Hexadecimal: " << decimalToBase(1000000, 16) << std::endl;
    
    std::cout << "\nVery Small Numbers (Scientific Notation):" << std::endl;
    std::cout << "Decimal: 0.000001" << std::endl;
    std::cout << "Scientific: " << decimalToBase(0.000001, 10) << std::endl;
    
    std::cout << "\nEdge Cases:" << std::endl;
    std::cout << "Decimal: 0" << std::endl;
    std::cout << "Binary: " << decimalToBase(0, 2) << std::endl;
    std::cout << "Base 36: " << decimalToBase(0, 36) << std::endl;
    
    std::cout << "\nVarious Bases to Decimal:" << std::endl;
    std::cout << "Binary '11111111' to decimal: " << baseToDecimal("11111111", 2) << std::endl;
    std::cout << "Octal '377' to decimal: " << baseToDecimal("377", 8) << std::endl;
    std::cout << "Hexadecimal 'FF' to decimal: " << baseToDecimal("FF", 16) << std::endl;
    std::cout << "Base 3 '100110' to decimal: " << baseToDecimal("100110", 3) << std::endl;
    std::cout << "Base 7 '513' to decimal: " << baseToDecimal("513", 7) << std::endl;
    std::cout << "Base 36 '73' to decimal: " << baseToDecimal("73", 36) << std::endl;
    
    std::cout << "\nNegative Numbers from Other Bases:" << std::endl;
    std::cout << "Binary '-11111111' to decimal: " << baseToDecimal("-11111111", 2) << std::endl;
    std::cout << "Hexadecimal '-FF' to decimal: " << baseToDecimal("-FF", 16) << std::endl;
    
    std::cout << "\nFractions from Other Bases:" << std::endl;
    std::cout << "Binary '0.1' to decimal: " << baseToDecimal("0.1", 2) << std::endl;
    std::cout << "Hexadecimal '0.A' to decimal: " << baseToDecimal("0.A", 16) << std::endl;
    
    std::cout << "\nBase to Base Conversion:" << std::endl;
    std::cout << "Binary '11111111' to octal: " << baseToBase("11111111", 2, 8) << std::endl;
    std::cout << "Octal '377' to binary: " << baseToBase("377", 8, 2) << std::endl;
    std::cout << "Hexadecimal 'FF' to base 7: " << baseToBase("FF", 16, 7) << std::endl;
    std::cout << "Base 3 '100110' to hexadecimal: " << baseToBase("100110", 3, 16) << std::endl;
    
    std::cout << "\nTime Complexity Analysis for Arbitrary Base Conversion:" << std::endl;
    std::cout << "1. decimalToBase(int decimal, int base): O(log_b n)" << std::endl;
    std::cout << "   - Where b is the target base" << std::endl;
    std::cout << "   - Each iteration divides the number by the base" << std::endl;
    std::cout << "   - Number of iterations is log_b(decimal)" << std::endl;
    
    std::cout << "\n2. baseToDecimal(const std::string& number, int base): O(n)" << std::endl;
    std::cout << "   - Where n is the length of the input string" << std::endl;
    std::cout << "   - Single pass through the string" << std::endl;
    std::cout << "   - Each character processed once" << std::endl;
    
    std::cout << "\n3. baseToBase(const std::string& number, int sourceBase, int targetBase): O(n + log_b m)" << std::endl;
    std::cout << "   - Where n is the length of the input string" << std::endl;
    std::cout << "   - b is the target base" << std::endl;
    std::cout << "   - m is the decimal value of the input" << std::endl;
    std::cout << "   - Combines the time complexity of baseToDecimal and decimalToBase" << std::endl;
}

int main() {
    runTests();
    // 与 Python/JS 版一致：任何断言失败都以非零码退出（CI 依赖退出码与 FAIL 计数）
    return g_failures > 0 ? 1 : 0;
}