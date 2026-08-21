# Starter Template（多语言练习项目骨架）

本目录是 **code-practice** 仓库合并进来的多语言练习项目骨架：内置 Python、JavaScript（Node.js）、C、C++ 四种语言的入门代码，并配好标准化的项目目录结构，帮助你练习的同时养成规范的项目习惯。

> 此前的独立仓库 code-practice 已合并至本目录，后续更新都在这里进行。

## 目录结构

```text
starter-template/
|-- src/              # 核心代码与各语言入口
|   |-- main.py       # Python 入口
|   |-- index.js      # JavaScript 入口
|   |-- main.c        # C 入口
|   `-- main.cpp      # C++ 入口
|-- tests/            # 测试代码
|   `-- test_main.py  # Python 测试（真实断言，CI 会执行）
|-- utils/            # 通用工具
|-- config/           # 配置相关
|-- docs/             # 项目文档
|-- scripts/          # 辅助脚本
|-- .env.example      # 环境变量示例
|-- CHANGELOG.md      # 变更记录
|-- CONTRIBUTING.md   # 协作规范
`-- README.md         # 本说明
```

## 快速开始

| 语言 | 命令 |
| --- | --- |
| Python | `python src/main.py` |
| JavaScript | `node src/index.js` |
| C | `gcc src/main.c -o build/main-c && ./build/main-c` |
| C++ | `g++ src/main.cpp -o build/main-cpp && ./build/main-cpp` |
| 测试 | `python -m unittest discover -s tests -v` |

环境要求：Python 3、Node.js、GCC/G++（Windows 建议 MinGW-w64）。

## 约定

- 目录与文件名小写 kebab-case（`user-guide.md`）
- 私密配置放进 `.env`（已被 `.gitignore` 忽略），不要提交
- 提交信息使用 conventional commit 格式（`feat:` / `fix:` / `docs:` / `chore:`）
- 修改代码后运行测试：`python -m unittest discover -s tests -v`

## 变更记录

见 [CHANGELOG.md](CHANGELOG.md)；协作规范见 [CONTRIBUTING.md](CONTRIBUTING.md)。
