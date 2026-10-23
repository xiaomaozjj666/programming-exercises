# 安全策略

`programming-exercises` 是面向初学者的多语言编程练习仓库，包含 C / C++ / JavaScript / Python 入门示例、`starter-template/` 项目骨架与 `benchmark/` 三语算法基准。仓库不处理用户数据、不提供网络服务，安全问题主要出在示例代码与 CI 流程本身。

## 支持的版本

本仓库不发布版本号、tag 或 release，以 `main` 分支最新提交为准。

| 版本 | 支持状态 |
|---|---|
| `main` 最新提交 | ✅ 接收安全报告并在默认分支修复 |
| 历史提交 / 任意快照 | ❌ 不单独维护，请先更新到 `main` |

## 私密报告漏洞

**请勿在公开 Issue 中粘贴漏洞细节或可直接利用的代码。**

请通过 GitHub 的私密漏洞报告渠道提交：

1. 打开 [Security Advisories](https://github.com/xiaomaozjj666/programming-exercises/security/advisories) 页面；
2. 点击 **Report a vulnerability**；
3. 按表单填写复现步骤与影响范围。

也可直接访问：<https://github.com/xiaomaozjj666/programming-exercises/security/advisories/new>

若页面上看不到该入口（仓库尚未开启私密报告），请先开一个**不含漏洞细节**的 Issue 说明需要私密渠道，维护者开启后会与你联系。

报告中建议包含：

- 受影响的文件与 `main` 上的提交号；
- 复现步骤：编译器 / 解释器版本、完整命令、输入数据，以及实际结果与预期结果；
- 影响说明，例如越界读写、未定义行为、拒绝服务或 CI 流程问题；
- 如有可能，附上修复建议。

在修复发布前，请勿公开细节。

## 范围说明

- 覆盖本仓库的示例源码、`starter-template/`、`benchmark/` 与 GitHub Actions 工作流。
- 示例以教学可读性优先，未按生产标准做完整输入校验、资源限制或内存安全强化；请勿把示例原样用于特权或生产环境。针对示例加固的建议按普通 Issue 处理，不属于本仓库漏洞。
- 不覆盖：第三方编译器 / 运行时自身的缺陷，以及示例算法在特定输入下的性能差异。
