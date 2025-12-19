#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path

def main():
    # 支持的文件扩展名
    extensions = {'.cpp', '.h', '.hpp'}
    
    # 获取当前工作目录
    root_dir = Path.cwd()
    
    # 查找所有匹配的文件（递归）
    files_to_format = []
    for ext in extensions:
        files_to_format.extend(root_dir.rglob(f'*{ext}'))
    
    if not files_to_format:
        print("未找到任何 .cpp, .h, 或 .hpp 文件。")
        return

    print(f"找到 {len(files_to_format)} 个文件，开始使用 clang-format 格式化...\n")

    # 检查 clang-format 是否可用
    try:
        subprocess.run(['clang-format', '--version'], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("错误：未找到 clang-format。请运行 'sudo apt install clang-format' 安装。", file=sys.stderr)
        sys.exit(1)

    # 格式化每个文件（原地修改）
    for filepath in sorted(files_to_format):
        try:
            print(f"格式化: {filepath}")
            subprocess.run(['clang-format', '-i', str(filepath)], check=True)
        except subprocess.CalledProcessError as e:
            print(f"格式化失败: {filepath} - 错误: {e}", file=sys.stderr)
        except Exception as e:
            print(f"未知错误: {filepath} - {e}", file=sys.stderr)

    print("\n✅ 所有文件格式化完成！")

if __name__ == '__main__':
    main()