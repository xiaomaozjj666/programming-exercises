import contextlib
import io
import sys
import unittest
from pathlib import Path

# 把 src/ 加入导入路径，使测试能直接 import main 模块
SRC = Path(__file__).resolve().parents[1] / "src"
sys.path.insert(0, str(SRC))

import main  # noqa: E402


class MainTest(unittest.TestCase):
    def test_main_is_callable(self):
        """main 模块应导出可调用的 main() 入口。"""
        self.assertTrue(callable(main.main))

    def test_main_prints_greeting(self):
        """运行 main() 应输出项目问候语。"""
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            main.main()
        self.assertIn("Hello, Python project!", buf.getvalue())

    def test_main_output_not_empty(self):
        """main() 的输出不应为空。"""
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            main.main()
        self.assertTrue(buf.getvalue().strip())


if __name__ == "__main__":
    unittest.main()
