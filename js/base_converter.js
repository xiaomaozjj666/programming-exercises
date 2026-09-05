#!/usr/bin/env node
/**
 * 任意进制转换器：支持负数与小数，2-36 进制，无第三方依赖。
 *
 * 设计要点：
 * - 整数部分使用 BigInt，避免 Number 超过 2^53 时的精度丢失；
 * - 小数部分用 BigInt 分子/分母的有理数表示，输出时乘基取整，最多 15 位小数。
 *
 * 用法：
 *   node base_converter.js <数字> <源进制> <目标进制>   # 转换并打印结果
 *   node base_converter.js --test                      # 运行内置测试
 *
 * 示例：
 *   node base_converter.js 255 10 16    -> FF
 *   node base_converter.js -123 10 2     -> -1111011
 *   node base_converter.js 10.5 10 16    -> A.8
 *   node base_converter.js 0.5 10 2      -> 0.1
 */
"use strict";

const DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const FRAC_PRECISION = 15; // 小数位输出上限

function digitValue(ch) {
  return DIGITS.indexOf(ch.toUpperCase());
}

/**
 * 把任意进制的字符串解析为 {negative, intPart, fracNum, fracDen}，
 * 非法输入返回 null。整数部分为 BigInt，小数部分为分数 fracNum/fracDen。
 */
function parseNumber(number, base) {
  if (base < 2 || base > 36) return null;
  let text = String(number).trim();
  if (!text) return null;
  let negative = false;
  if (text[0] === "-" || text[0] === "+") {
    negative = text[0] === "-";
    text = text.slice(1);
  }
  let intStr = text;
  let fracStr = "";
  const dot = text.indexOf(".");
  if (dot !== -1) {
    intStr = text.slice(0, dot);
    fracStr = text.slice(dot + 1);
  }
  if (!intStr) intStr = "0";
  let intPart = 0n;
  for (const ch of intStr) {
    const pos = digitValue(ch);
    if (pos === -1 || pos >= base) return null;
    intPart = intPart * BigInt(base) + BigInt(pos);
  }
  let fracNum = 0n;
  let fracDen = 1n;
  for (const ch of fracStr) {
    const pos = digitValue(ch);
    if (pos === -1 || pos >= base) return null;
    fracNum = fracNum * BigInt(base) + BigInt(pos);
    fracDen = fracDen * BigInt(base);
  }
  return { negative, intPart, fracNum, fracDen };
}

/** 把解析结果格式化为目标进制字符串。 */
function formatNumber(parsed, base) {
  if (base < 2 || base > 36) return "ERROR: Invalid base!";
  const digits = [];
  let n = parsed.intPart;
  if (n === 0n) {
    digits.push("0");
  } else {
    while (n > 0n) {
      digits.push(DIGITS[Number(n % BigInt(base))]);
      n = n / BigInt(base);
    }
    digits.reverse();
  }
  let out = digits.join("");
  if (parsed.fracNum !== 0n) {
    out += ".";
    let num = parsed.fracNum;
    let den = parsed.fracDen;
    let precision = FRAC_PRECISION;
    while (num !== 0n && precision > 0) {
      num *= BigInt(base);
      out += DIGITS[Number(num / den)];
      num %= den;
      precision--;
    }
  }
  if (out === "0") return "0"; // -0 规范化为 0
  return (parsed.negative ? "-" : "") + out;
}

/** 任意进制互转（2-36），支持负数与小数。 */
function baseToBase(number, srcBase, dstBase) {
  if (srcBase < 2 || srcBase > 36 || dstBase < 2 || dstBase > 36) return "ERROR: Invalid base!";
  const parsed = parseNumber(number, srcBase);
  if (!parsed) return "ERROR: Invalid input!";
  return formatNumber(parsed, dstBase);
}

/** 内置测试：返回是否全部通过。 */
function runTests() {
  const cases = [
    ["0", 10, 16, "0"],
    ["255", 10, 16, "FF"],
    ["FF", 16, 10, "255"],
    ["100110", 3, 16, "FF"],
    ["-123", 10, 2, "-1111011"],
    ["-FF", 16, 10, "-255"],
    ["0.5", 10, 2, "0.1"],
    ["10.5", 10, 16, "A.8"],
    ["-0.5", 10, 2, "-0.1"],
    // 超过 2^53 的大整数（BigInt，无精度丢失）
    ["9007199254740993", 10, 16, "20000000000001"],
    ["20000000000001", 16, 10, "9007199254740993"],
    // 非法输入
    ["2", 2, 10, "ERROR: Invalid input!"],
    ["12G", 16, 10, "ERROR: Invalid input!"],
    // 进制越界
    ["255", 10, 99, "ERROR: Invalid base!"],
    ["FF", 99, 10, "ERROR: Invalid base!"],
  ];
  let failed = 0;
  for (const [number, src, dst, expected] of cases) {
    const got = baseToBase(number, src, dst);
    const ok = got === expected;
    if (!ok) failed += 1;
    console.log(`  [${ok ? "PASS" : "FAIL"}] ${JSON.stringify(number)} (${src}->${dst}): ${JSON.stringify(got)}${ok ? "" : ` (expected ${JSON.stringify(expected)})`}`);
  }
  console.log(`共 ${cases.length} 个用例，失败 ${failed} 个。`);
  return failed === 0;
}

function main() {
  const argv = process.argv.slice(2);
  if (argv.length === 1 && argv[0] === "--test") {
    process.exit(runTests() ? 0 : 1);
  }
  if (argv.length !== 3) {
    console.log("用法: node base_converter.js <数字> <源进制> <目标进制>  或  --test");
    process.exit(2);
  }
  const src = Number(argv[1]);
  const dst = Number(argv[2]);
  if (!Number.isInteger(src) || !Number.isInteger(dst)) {
    console.log("ERROR: Invalid base!");
    process.exit(2);
  }
  if (src < 2 || src > 36 || dst < 2 || dst > 36) {
    console.log("ERROR: Invalid base!");
    process.exit(2);
  }
  const result = baseToBase(argv[0], src, dst);
  console.log(result);
  process.exitCode = result.startsWith("ERROR") ? 1 : 0;
}

main();
