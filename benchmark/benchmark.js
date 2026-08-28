#!/usr/bin/env node
/* 进制转换与算法复杂度基准测试 */
const DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

function decimalToBase(n, base) {
  if (!Number.isInteger(base) || base < 2 || base > 36) throw new Error("base must be 2-36");
  if (!Number.isInteger(n)) throw new Error("only integers are supported");
  if (n === 0) return "0";
  const sign = n < 0 ? "-" : "";
  n = Math.abs(n);
  const res = [];
  while (n > 0) {
    res.push(DIGITS[n % base]);
    n = Math.floor(n / base);
  }
  return sign + res.reverse().join("");
}

function baseToDecimal(s, base) {
  if (!Number.isInteger(base) || base < 2 || base > 36) throw new Error("base must be 2-36");
  s = String(s).toUpperCase();
  let sign = 1;
  if (s.startsWith("-")) { sign = -1; s = s.slice(1); }
  else if (s.startsWith("+")) { s = s.slice(1); }
  const digitVal = (ch) => {
    const v = DIGITS.indexOf(ch);
    if (v === -1 || v >= base) throw new Error(`invalid digit '${ch}' for base ${base}`);
    return v;
  };
  const parts = s.split(".");
  if (parts.length > 2) throw new Error(`invalid digit '.' for base ${base}`);
  let n = 0;
  for (const ch of parts[0]) n = n * base + digitVal(ch);
  if (parts.length === 2) {
    let factor = 1 / base;
    for (const ch of parts[1]) { n += digitVal(ch) * factor; factor /= base; }
  }
  return n === 0 ? 0 : sign * n;
}

function decimalToBinary(n) { return decimalToBase(n, 2); }
function binaryToDecimal(s) { return baseToDecimal(s, 2); }
function decimalToHex(n) { return decimalToBase(n, 16); }
function hexToDecimal(s) { return baseToDecimal(s, 16); }

function linearSearch(arr, target) {
  for (let i = 0; i < arr.length; i++) if (arr[i] === target) return i;
  return -1;
}

function binarySearch(arr, target) {
  let lo = 0, hi = arr.length - 1;
  while (lo <= hi) {
    const mid = Math.floor((lo + hi) / 2);
    if (arr[mid] === target) return mid;
    if (arr[mid] < target) lo = mid + 1;
    else hi = mid - 1;
  }
  return -1;
}

function bubbleSort(arr) {
  const a = [...arr];
  for (let i = 0; i < a.length - 1; i++)
    for (let j = 0; j < a.length - i - 1; j++)
      if (a[j] > a[j + 1]) [a[j], a[j + 1]] = [a[j + 1], a[j]];
  return a;
}

function test(name, got, expected) {
  const ok = got === expected;
  console.log(`  ${name}: ${got} (expected ${expected}) ${ok ? "PASS" : "FAIL"}`);
}

function runTests() {
  console.log("=== 进制转换测试 ===\n");
  console.log("Decimal to Binary:");
  for (const [n, e] of [[0,"0"],[1,"1"],[10,"1010"],[255,"11111111"]]) test(`dec ${n} -> bin`, decimalToBinary(n), e);
  console.log("\nBinary to Decimal:");
  for (const [n, e] of [["0",0],["1",1],["1010",10],["11111111",255]]) test(`bin ${n} -> dec`, binaryToDecimal(n), e);
  console.log("\nDecimal to Hex:");
  for (const [n, e] of [[0,"0"],[1,"1"],[10,"A"],[255,"FF"],[1000,"3E8"]]) test(`dec ${n} -> hex`, decimalToHex(n), e);
  console.log("\nHex to Decimal:");
  for (const [n, e] of [["0",0],["1",1],["A",10],["FF",255],["3E8",1000]]) test(`hex ${n} -> dec`, hexToDecimal(n), e);

  console.log("\n=== 算法测试 ===\n");
  const arr = [1,3,5,7,9,11,13,15,17,19];
  console.log(`LinearSearch: find 11 in [${arr}] -> index ${linearSearch(arr,11)} (expected 5)`);
  console.log(`LinearSearch: find 20 in [${arr}] -> index ${linearSearch(arr,20)} (expected -1)`);
  console.log(`BinarySearch: find 11 in [${arr}] -> index ${binarySearch(arr,11)} (expected 5)`);
  console.log(`BinarySearch: find 20 in [${arr}] -> index ${binarySearch(arr,20)} (expected -1)`);
  const unsorted = [9,3,7,1,5,11,13,2,4,8];
  console.log(`BubbleSort: [${unsorted}] -> [${bubbleSort(unsorted)}]`);

  console.log("\n=== 时间复杂度分析 ===\n");
  const big = Array.from({length:10000}, () => Math.floor(Math.random()*10000));
  const sorted = [...big].sort((a,b)=>a-b);

  let t0 = performance.now(); linearSearch(big, -1); let t1 = performance.now();
  console.log(`LinearSearch O(n):  ${(t1-t0).toFixed(4)}ms (n=10000)`);

  t0 = performance.now(); binarySearch(sorted, -1); t1 = performance.now();
  console.log(`BinarySearch O(logn): ${(t1-t0).toFixed(4)}ms (n=10000)`);

  const small = Array.from({length:1000}, () => Math.floor(Math.random()*1000));
  t0 = performance.now(); bubbleSort(small); t1 = performance.now();
  console.log(`BubbleSort O(n^2):   ${(t1-t0).toFixed(4)}ms (n=1000)`);
}

const mode = process.argv[2];
if (mode === "--convert") {
  const num = process.argv[3];
  const fromB = parseInt(process.argv[4] || "10", 10);
  const toB = parseInt(process.argv[5] || "10", 10);
  try {
    const dec = baseToDecimal(num, fromB);
    console.log(toB === 10 ? dec : decimalToBase(dec, toB));
  } catch(e) { console.error("Error:", e.message); process.exit(1); }
} else {
  runTests();
}
