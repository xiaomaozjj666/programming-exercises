# 评审指南

本文件定义本仓库的评审优先级与可跳过范围。

仓库由四个语言的入门示例、三语进制转换器、`benchmark/` 三语算法基准和 `starter-template/` 项目骨架组成。核心约束不是代码规模，而是"同一功能在三种语言里必须表现一致"：参数解析、退出码、数值精度、测试断言各自独立实现，任何一处单独改动都可能破坏这个约束。

## 最可能失效的地方

1. **三语进制参数解析的严格性被放松。** 最近一次修复把 `python/base_converter.py:133-141`（`isascii()` + `isdigit()`）、`js/base_converter.js:149-157`（`/^\d+$/`）与 `cpp/base_converter.cpp:184-193`（`strspn` 全数字校验）对齐为"只接受纯 ASCII 数字串，非法进制一律输出 `ERROR: Invalid base!` 并退出码 2"。`.github/workflows/ci.yml` 中 "CLI error semantics agree across three languages" 步骤用 12 个反例锁定该行为。任何一处换回 `int()`、`Number()` 或直接 `strtol`，三语就会重新分叉。
2. **小数与大整数精度三语不对称。** Python 用 `Fraction`（`python/base_converter.py:51-56`），JavaScript 用 BigInt 分子/分母（`js/base_converter.js:55-62`），C++ 用 `double` 并在 `1e-12` 处截断（`cpp/base_converter.cpp:81-87`、`109-113`）。CI 的一致性冒烟只比较整数 `255 10 16 -> FF`，循环小数与极小数的三语输出没有断言，末位差异不会变红。
3. **C++ 的 `long long` 边界窄于另外两版。** 溢出守卫在 `cpp/base_converter.cpp:77`，超过 2^63-1 直接判 `ERROR: Invalid input!`；Python 与 JavaScript 仍能正常转换。新增边界用例时若按同一期望值写三语断言，C++ 必失败。
4. **`benchmark/` 的转换语义与转换器不是同一套。** `benchmark/benchmark.cpp:105-116` 对十进制输入单独接受科学计数法（`1e5` 可解析），Python 与 JavaScript 会把 `e` 当非法数位；`benchmark/benchmark.py:9-11` 只允许整数转出，而 JavaScript 版可以输出小数。README 中"三语一致"只针对转换器，基准目录不要照搬这个承诺。
5. **CI 以输出文本计数判定成败。** `.github/workflows/ci.yml:106-119` 用 `grep -q FAIL` 与 `grep -c PASS >= 23` 判断基准结果：改一句输出措辞、改变 `PASS` 的大小写、或让程序打印完一部分后正常退出，都会造成假绿或假红。C++ 基准固定用 `-std=c++11`（`ci.yml:106`），与其它 C++ 文件的 `c++17` 不一致，写法升级时容易漏掉这一处。
6. **`c/char_counter.c` 的输入处理是教学级简化。** `fgets` 返回值未检查（第 5 行），空输入时后续循环读取的是未初始化缓冲区；固定 101 字节会静默截断长行；`isspace` 会把 CRLF 中的 `\r` 计入空格（第 16 行），Windows 本地与 Linux CI 对同一行文本可能得到相差 1 的统计。C++ 交互模式同样没有检查 `cin` 状态（`cpp/base_converter.cpp:205-219`），输入非数字时 `srcBase` 保持未初始化值。

## 重点看

- `python/base_converter.py`、`js/base_converter.js`、`cpp/base_converter.cpp` —— 同一语义改动必须三份同改，重点核对参数校验、退出码、负数与小数路径。
- `.github/workflows/ci.yml` —— 所有一致性承诺最终由这里的步骤兜底，改测试步骤等同于改可合并标准。
- `benchmark/benchmark.cpp`（552 行，口径最杂）、`benchmark/benchmark.py`、`benchmark/benchmark.js` —— 与转换器不是同一套语义，断言数量与输出文本是 CI 的直接输入。
- `c/char_counter.c` —— 固定缓冲区、字符分类与跨平台换行。
- `.gitignore` —— 第 10 行的 `benchmark` 规则依赖 `!benchmark/` 反选；新增同名产物（尤其是编译输出）会被静默忽略，改动后确认 `git status` 与 `git check-ignore -v` 的实际结果。

## 可以跳过

无自动生成目录，仍需看全量 diff。

以下真实存在的简单文件可略读：四个语言的 hello 入口 `c/hello.c`、`cpp/hello.cpp`、`js/hello.js`、`python/hello.py`，`starter-template/src/` 下四个仅打印问候语的入口，以及 `benchmark/benchmark.svg`（静态图表，仅当基准数据变化时需要重看）。

## 校验入口

仓库没有 `package.json`、`pyproject.toml`、`Makefile`，也没有独立的 lint/typecheck 配置；实际检查全部由 `.github/workflows/ci.yml` 执行。本地等价命令：

```bash
python python/base_converter.py --test
node js/base_converter.js --test
g++ -O2 -std=c++17 -Wall -Wextra cpp/base_converter.cpp -o base_converter && ./base_converter --test
python -m unittest discover -s starter-template/tests -v
```

## 评审规则

- 每条发现必须带 `file:line` 证据，不接受凭命名或猜测。
- 标严重度：Important / Nit / Pre-existing；区分本次引入与既有问题。
- 先报真实 bug，再报可读性；最多 3 条 nit。
- 已提过的发现不重复；第 3 轮起只审新增 diff，并请作者重新考虑方案。
- 每条发现必须被处理：修复，或在回复中说明为何不修。
- 安全类发现未解决前不得合并。
