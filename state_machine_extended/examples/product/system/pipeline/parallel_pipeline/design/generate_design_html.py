#!/usr/bin/env python3
"""
Parallel Pipeline Design Documentation Generator

This script generates an HTML page that displays all SVG design diagrams
with extracted notes and titles from the corresponding PUML files.

Usage:
    python generate_design_html.py

Output:
    parallel_pipeline_design_review.html - Complete design documentation
"""

import os
import glob
import re
from pathlib import Path
from typing import Dict, List, Tuple

class DesignDiagram:
    def __init__(self, svg_file: str, puml_file: str):
        self.svg_file = svg_file
        self.puml_file = puml_file
        self.title = ""
        self.description = ""
        self.notes = []

    def extract_info(self):
        """Extract title and notes from PUML file"""
        if not os.path.exists(self.puml_file):
            return

        try:
            with open(self.puml_file, 'r', encoding='utf-8') as f:
                content = f.read()

            # Extract title
            title_match = re.search(r'^title\s+(.+)$', content, re.MULTILINE)
            if title_match:
                self.title = title_match.group(1).strip()

            # Extract description from comments at the top
            lines = content.split('\n')
            in_description = False
            description_lines = []

            for line in lines[:50]:  # Check first 50 lines
                line = line.strip()
                if line.startswith("/'"):
                    in_description = True
                    continue
                elif line.startswith("'/"):
                    break
                elif in_description and line:
                    description_lines.append(line)

            if description_lines:
                self.description = '\n'.join(description_lines)

            # Extract note blocks
            note_pattern = r'note\s+(?:right|left|top|bottom|as\s+\w+)\s*\n(.*?)\n\s*end note'
            note_matches = re.findall(note_pattern, content, re.DOTALL | re.IGNORECASE)

            for note in note_matches:
                # Clean up the note content
                note_lines = [line.strip() for line in note.split('\n') if line.strip()]
                if note_lines:
                    self.notes.append('\n'.join(note_lines))

        except Exception as e:
            print(f"Warning: Could not extract info from {self.puml_file}: {e}")

def find_diagrams(design_dir: str) -> List[DesignDiagram]:
    """Find all SVG files and their corresponding PUML files"""
    diagrams = []

    # Find all SVG files
    svg_files = glob.glob(os.path.join(design_dir, "*.svg"))

    for svg_file in sorted(svg_files):
        # Try to find corresponding PUML file
        svg_name = Path(svg_file).stem

        # Try different PUML naming patterns
        possible_puml_names = [
            svg_name.replace(" ", "_").lower() + ".puml",
            svg_name.replace(" ", "_") + ".puml",
            svg_name.lower() + ".puml",
            svg_name + ".puml"
        ]

        puml_file = None
        for puml_name in possible_puml_names:
            candidate = os.path.join(design_dir, puml_name)
            if os.path.exists(candidate):
                puml_file = candidate
                break

        diagram = DesignDiagram(svg_file, puml_file or "")
        diagram.extract_info()
        diagrams.append(diagram)

    return diagrams

def generate_html(diagrams: List[DesignDiagram], output_file: str):
    """Generate HTML page with all diagrams"""

    html_content = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>并行管道架构设计评审文档</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.6;
            color: #333;
            background-color: #f5f5f5;
        }}

        .container {{
            max-width: 1400px;
            margin: 0 auto;
            padding: 20px;
        }}

        .header {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 40px 0;
            text-align: center;
            margin-bottom: 40px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }}

        .header h1 {{
            font-size: 2.5em;
            margin-bottom: 10px;
            font-weight: 300;
        }}

        .header p {{
            font-size: 1.2em;
            opacity: 0.9;
        }}

        .diagram-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
            gap: 30px;
            margin-bottom: 40px;
        }}

        .diagram-card {{
            background: white;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            overflow: hidden;
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }}

        .diagram-card:hover {{
            transform: translateY(-5px);
            box-shadow: 0 8px 15px rgba(0, 0, 0, 0.2);
        }}

        .diagram-header {{
            background: #2c3e50;
            color: white;
            padding: 20px;
            border-bottom: 3px solid #3498db;
        }}

        .diagram-title {{
            font-size: 1.3em;
            font-weight: 600;
            margin-bottom: 5px;
        }}

        .diagram-description {{
            font-size: 0.9em;
            opacity: 0.9;
            line-height: 1.4;
        }}

        .diagram-content {{
            padding: 20px;
        }}

        .diagram-image {{
            width: 100%;
            height: auto;
            border: 1px solid #e1e1e1;
            border-radius: 5px;
            margin-bottom: 15px;
        }}

        .diagram-notes {{
            background: #f8f9fa;
            border-left: 4px solid #3498db;
            padding: 15px;
            margin-top: 15px;
            border-radius: 0 5px 5px 0;
        }}

        .diagram-notes h4 {{
            color: #2c3e50;
            margin-bottom: 10px;
            font-size: 1em;
        }}

        .diagram-notes p {{
            margin-bottom: 8px;
            font-size: 0.9em;
            line-height: 1.5;
        }}

        .navigation {{
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            margin-bottom: 30px;
        }}

        .nav-links {{
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
        }}

        .nav-link {{
            display: inline-block;
            padding: 8px 16px;
            background: #3498db;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            font-size: 0.9em;
            transition: background 0.3s ease;
        }}

        .nav-link:hover {{
            background: #2980b9;
        }}

        .stats {{
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            margin-bottom: 30px;
            text-align: center;
        }}

        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }}

        .stat-item {{
            padding: 15px;
            background: #f8f9fa;
            border-radius: 8px;
        }}

        .stat-number {{
            font-size: 2em;
            font-weight: bold;
            color: #3498db;
            display: block;
        }}

        .stat-label {{
            font-size: 0.9em;
            color: #666;
            margin-top: 5px;
        }}

        .footer {{
            text-align: center;
            padding: 20px;
            color: #666;
            font-size: 0.9em;
        }}

        .badge {{
            display: inline-block;
            padding: 4px 8px;
            background: #27ae60;
            color: white;
            border-radius: 12px;
            font-size: 0.8em;
            margin-left: 10px;
        }}

        @media (max-width: 768px) {{
            .header h1 {{
                font-size: 2em;
            }}

            .diagram-grid {{
                grid-template-columns: 1fr;
            }}

            .nav-links {{
                justify-content: center;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔧 并行管道架构设计评审文档</h1>
            <p>Parallel Pipeline Architecture Design Review Documentation</p>
        </div>

        <div class="stats">
            <h2>📊 设计图表统计</h2>
            <div class="stats-grid">
                <div class="stat-item">
                    <span class="stat-number">{len(diagrams)}</span>
                    <span class="stat-label">设计图表总数</span>
                </div>
                <div class="stat-item">
                    <span class="stat-number">{sum(1 for d in diagrams if d.title)}</span>
                    <span class="stat-label">带标题图表</span>
                </div>
                <div class="stat-item">
                    <span class="stat-number">{sum(len(d.notes) for d in diagrams)}</span>
                    <span class="stat-label">注释块数量</span>
                </div>
                <div class="stat-item">
                    <span class="stat-number">{len([d for d in diagrams if d.description])}</span>
                    <span class="stat-label">详细描述图表</span>
                </div>
            </div>
        </div>

        <div class="navigation">
            <h3>🧭 快速导航</h3>
            <div class="nav-links">
"""

    # Add navigation links
    for i, diagram in enumerate(diagrams):
        svg_name = os.path.basename(diagram.svg_file)
        title = diagram.title or svg_name.replace('.svg', '').replace('Parallel Pipeline ', '')
        html_content += f'                <a href="#diagram-{i}" class="nav-link">{title}</a>\n'

    html_content += """            </div>
        </div>

        <div class="diagram-grid">
"""

    # Add diagram cards
    for i, diagram in enumerate(diagrams):
        svg_name = os.path.basename(diagram.svg_file)
        title = diagram.title or svg_name.replace('.svg', '').replace('Parallel Pipeline ', '')
        description = diagram.description or "暂无详细描述"

        html_content += f"""
            <div class="diagram-card" id="diagram-{i}">
                <div class="diagram-header">
                    <div class="diagram-title">{title}</div>
                    <div class="diagram-description">{description[:100]}{"..." if len(description) > 100 else ""}</div>
                </div>
                <div class="diagram-content">
                    <img src="{svg_name}" alt="{title}" class="diagram-image" loading="lazy">
"""

        # Add notes if available
        if diagram.notes:
            html_content += """                    <div class="diagram-notes">
                        <h4>📝 设计说明</h4>
"""
            for note in diagram.notes:
                # Convert markdown-style formatting to HTML
                note_html = note.replace('**', '<strong>').replace('*', '<em>')
                # Handle line breaks
                note_html = note_html.replace('\n- ', '<br>• ')
                note_html = note_html.replace('\n', '<br>')
                html_content += f"""                        <p>{note_html}</p>
"""
            html_content += """                    </div>
"""

        html_content += """                </div>
            </div>
"""

    html_content += """
        </div>

        <div class="footer">
            <p>📅 生成时间: 2026-01-05 | 🔧 自动生成脚本 | 🎯 并行管道架构设计评审</p>
            <p>💡 本文档包含完整的架构设计图表，用于设计评审和技术评估</p>
        </div>
    </div>

    <script>
        // Add smooth scrolling for navigation links
        document.querySelectorAll('.nav-link').forEach(link => {
            link.addEventListener('click', function(e) {
                e.preventDefault();
                const targetId = this.getAttribute('href');
                const targetElement = document.querySelector(targetId);
                if (targetElement) {
                    targetElement.scrollIntoView({ behavior: 'smooth' });
                }
            });
        });

        // Add lazy loading for images
        const images = document.querySelectorAll('.diagram-image');
        const imageObserver = new IntersectionObserver((entries, observer) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    const img = entry.target;
                    img.classList.add('loaded');
                    observer.unobserve(img);
                }
            });
        });

        images.forEach(img => imageObserver.observe(img));
    </script>
</body>
</html>"""

    # Write HTML file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(html_content)

    print(f"✅ Generated HTML documentation: {output_file}")
    print(f"📊 Total diagrams: {len(diagrams)}")
    print(f"📝 Diagrams with titles: {sum(1 for d in diagrams if d.title)}")
    print(f"📋 Total notes: {sum(len(d.notes) for d in diagrams)}")

def main():
    """Main function"""
    design_dir = os.path.dirname(os.path.abspath(__file__))
    output_file = os.path.join(design_dir, "parallel_pipeline_design_review.html")

    print("🔍 Scanning design diagrams...")
    diagrams = find_diagrams(design_dir)

    print(f"📊 Found {len(diagrams)} SVG diagrams")
    print("📝 Extracting diagram information...")

    print("🎨 Generating HTML documentation...")
    generate_html(diagrams, output_file)

    print("\n✅ Design review documentation generated successfully!")
    print(f"🌐 Open {output_file} in your browser to view the documentation")

if __name__ == "__main__":
    main()
