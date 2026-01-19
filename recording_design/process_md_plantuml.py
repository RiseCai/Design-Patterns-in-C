#!/usr/bin/env python3
"""
从 markdown 文件中提取 PlantUML 代码块并生成 SVG 文件。
使用现有的 update_svg.py 中的功能。
"""

import os
import re
import sys
import tempfile
import subprocess
from pathlib import Path

# 导入 update_svg_fixed.py 中的函数
# 由于 update_svg_fixed.py 在同一个目录，我们可以直接导入
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# 尝试导入 update_svg_fixed 模块
try:
    import update_svg_fixed
    PLANTUML_JAR = update_svg_fixed.PLANTUML_JAR
    generate_svg_local = update_svg_fixed.generate_svg
    HAS_LOCAL_GENERATOR = True
except ImportError:
    HAS_LOCAL_GENERATOR = False
    PLANTUML_JAR = None
    generate_svg_local = None

# 定义在线生成函数
def generate_svg_online(puml_file, svg_file):
    """
    Generate SVG from PlantUML file using online PlantUML server
    """
    import urllib.request
    import urllib.parse
    
    # Read PlantUML content
    with open(puml_file, 'r', encoding='utf-8') as f:
        plantuml_code = f.read()
    
    # Encode for PlantUML server
    # PlantUML uses a special encoding: https://plantuml.com/text-encoding
    import zlib
    import base64
    
    # Compress and encode
    compressed = zlib.compress(plantuml_code.encode('utf-8'))
    encoded = base64.b64encode(compressed).decode('ascii')
    
    # Remove trailing = characters
    encoded = encoded.rstrip('=')
    
    # Replace characters for URL
    encoded = encoded.replace('/', '_').replace('+', '-')
    
    # Add ~1 prefix for Huffman encoding (as per PlantUML error message)
    encoded = '~1' + encoded
    
    # Build URL
    url = f'https://www.plantuml.com/plantuml/svg/{encoded}'
    
    try:
        # Download SVG
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        response = urllib.request.urlopen(req, timeout=30)
        svg_content = response.read().decode('utf-8')
        
        # Check if it's an error
        if 'Syntax Error?' in svg_content or 'Error' in svg_content:
            print(f'PlantUML online server reported error for {puml_file}')
            return False
        
        # Save SVG
        with open(svg_file, 'w', encoding='utf-8') as f:
            f.write(svg_content)
        
        print(f'Generated {svg_file} (online)')
        return True
        
    except Exception as e:
        print(f'Error using online PlantUML server for {puml_file}: {e}')
        return False

# 定义通用生成函数
def generate_svg(puml_file, svg_file, use_online=False):
    """
    Generate SVG from PlantUML file using local plantuml.jar or online server
    """
    if use_online:
        return generate_svg_online(puml_file, svg_file)
    else:
        if not HAS_LOCAL_GENERATOR:
            print('Error: Local PlantUML generator not available. Falling back to online mode.')
            return generate_svg_online(puml_file, svg_file)
        return generate_svg_local(puml_file, svg_file)


def extract_plantuml_blocks(md_content):
    """
    从 markdown 内容中提取 PlantUML 代码块。
    返回一个列表，每个元素是一个字典，包含：
    - name: 代码块名称（从文件名或注释中提取）
    - content: PlantUML 代码内容
    - start_line: 代码块在文件中的起始行号
    """
    plantuml_blocks = []
    
    # 模式1: ```plantuml ... ```
    pattern1 = r'```plantuml\s*(.*?)```'
    # 模式2: @startuml ... @enduml
    pattern2 = r'(@startuml.*?@enduml)'
    
    # 使用正则表达式查找所有匹配
    matches1 = re.finditer(pattern1, md_content, re.DOTALL | re.IGNORECASE)
    matches2 = re.finditer(pattern2, md_content, re.DOTALL)
    
    # 处理第一种模式
    for match in matches1:
        content = match.group(1).strip()
        if content:
            # 尝试从内容中提取名称（查找文件名注释）
            name_match = re.search(r'文件名[：:]\s*`([^`]+)`', content)
            if name_match:
                name = name_match.group(1).replace('.puml', '')
            else:
                # 查找标题
                title_match = re.search(r'title\s+(.+?)\n', content)
                if title_match:
                    name = title_match.group(1).strip()
                else:
                    # 使用默认名称
                    name = f"plantuml_block_{len(plantuml_blocks)+1}"
            
            plantuml_blocks.append({
                'name': name,
                'content': content,
                'type': 'plantuml_code_block'
            })
    
    # 处理第二种模式
    for match in matches2:
        content = match.group(1).strip()
        if content:
            # 尝试从内容中提取名称
            title_match = re.search(r'title\s+(.+?)\n', content)
            if title_match:
                name = title_match.group(1).strip()
            else:
                name = f"plantuml_diagram_{len(plantuml_blocks)+1}"
            
            plantuml_blocks.append({
                'name': name,
                'content': content,
                'type': 'startuml_block'
            })
    
    return plantuml_blocks


def save_plantuml_to_file(plantuml_content, output_dir, base_name):
    """
    将 PlantUML 内容保存到文件。
    返回保存的文件路径。
    """
    # 清理文件名
    safe_name = re.sub(r'[^\w\-_\. ]', '_', base_name)
    safe_name = safe_name.replace(' ', '_')
    
    # 确保文件名以 .puml 结尾
    if not safe_name.endswith('.puml'):
        safe_name += '.puml'
    
    output_path = os.path.join(output_dir, safe_name)
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(plantuml_content)
    
    print(f'Saved PlantUML to: {output_path}')
    return output_path


def process_markdown_file(md_file, output_dir=None, use_online=False):
    """
    处理 markdown 文件，提取 PlantUML 代码块并生成 SVG。
    """
    if not os.path.exists(md_file):
        print(f'Error: Markdown file not found: {md_file}')
        return False
    
    # 读取 markdown 文件内容
    with open(md_file, 'r', encoding='utf-8') as f:
        md_content = f.read()
    
    # 提取 PlantUML 代码块
    plantuml_blocks = extract_plantuml_blocks(md_content)
    
    if not plantuml_blocks:
        print(f'No PlantUML blocks found in {md_file}')
        return False
    
    print(f'Found {len(plantuml_blocks)} PlantUML block(s) in {md_file}')
    
    # 确定输出目录
    if output_dir is None:
        output_dir = os.path.dirname(md_file)
        # 如果目录名为空，使用当前目录
        if not output_dir:
            output_dir = '.'
    
    # 创建输出目录（如果不存在）
    os.makedirs(output_dir, exist_ok=True)
    
    # 处理每个 PlantUML 代码块
    results = []
    for i, block in enumerate(plantuml_blocks):
        print(f'\nProcessing block {i+1}: {block["name"]}')
        
        # 保存 PlantUML 内容到临时文件
        puml_file = save_plantuml_to_file(
            block['content'], 
            output_dir, 
            f"{os.path.splitext(os.path.basename(md_file))[0]}_{block['name']}"
        )
        
        # 生成 SVG 文件
        svg_file = puml_file.replace('.puml', '.svg')
        
        if generate_svg(puml_file, svg_file, use_online=use_online):
            results.append({
                'name': block['name'],
                'puml_file': puml_file,
                'svg_file': svg_file,
                'success': True
            })
        else:
            results.append({
                'name': block['name'],
                'puml_file': puml_file,
                'svg_file': svg_file,
                'success': False
            })
    
    # 打印结果摘要
    print('\n' + '='*60)
    print('Processing Summary:')
    print('='*60)
    success_count = sum(1 for r in results if r['success'])
    print(f'Successfully generated: {success_count}/{len(results)} SVG files')
    
    for result in results:
        status = '✓ SUCCESS' if result['success'] else '✗ FAILED'
        print(f'{status}: {result["name"]}')
        if result['success']:
            print(f'  SVG: {result["svg_file"]}')
    
    return success_count > 0


def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Extract PlantUML code from markdown files and generate SVG diagrams.'
    )
    parser.add_argument(
        'md_file',
        help='Path to the markdown file containing PlantUML code blocks'
    )
    parser.add_argument(
        '-o', '--output-dir',
        help='Output directory for generated .puml and .svg files (default: same as markdown file)'
    )
    parser.add_argument(
        '--online',
        action='store_true',
        help='Use online PlantUML server instead of local plantuml.jar'
    )
    
    args = parser.parse_args()
    
    success = process_markdown_file(args.md_file, args.output_dir, use_online=args.online)
    
    if success:
        print('\nProcessing completed successfully!')
        sys.exit(0)
    else:
        print('\nProcessing completed with errors.')
        sys.exit(1)


if __name__ == '__main__':
    main()
