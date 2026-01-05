#!/usr/bin/env python3
"""
Parallel Pipeline Design Review Documentation Generator

This script generates a comprehensive HTML design review document that includes:
- Standard Operating Procedure (SOP) for design reviews
- Four major review categories with graphical evaluation criteria
- Embedded design diagrams within relevant review sections
- Requirements mapping and validation guidelines

Usage:
    python generate_design_html.py

Output:
    parallel_pipeline_design_review.html - Complete design review documentation
"""

import os
import glob
import re
from pathlib import Path
from typing import Dict, List, Tuple
from datetime import datetime

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

def find_diagrams(design_dir: str) -> Dict[str, DesignDiagram]:
    """Find all SVG files and their corresponding PUML files, organized by review category"""
    diagrams = {}

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

        # Categorize diagrams by their content
        svg_filename = os.path.basename(svg_file).lower().replace(" ", "_")
        print(f"Processing: {svg_filename}")  # Debug output

        if "architecture_class" in svg_filename or "class_diagram" in svg_filename or "decoupled_architecture" in svg_filename:
            diagrams["architecture_class"] = diagram
            print(f"  -> Classified as: architecture_class")
        elif "component_interaction" in svg_filename or "interaction" in svg_filename:
            diagrams["component_interaction"] = diagram
            print(f"  -> Classified as: component_interaction")
        elif "module_dependencies" in svg_filename or "dependencies" in svg_filename:
            diagrams["module_dependencies"] = diagram
            print(f"  -> Classified as: module_dependencies")
        elif "adapters_framework" in svg_filename or "adapters_design" in svg_filename:
            diagrams["adapters"] = diagram
            print(f"  -> Classified as: adapters")
        elif "data_flow" in svg_filename and "hybrid" not in svg_filename:
            diagrams["data_flow"] = diagram
            print(f"  -> Classified as: data_flow")
        elif "data_flow_hybrid" in svg_filename or "hybrid_model" in svg_filename:
            diagrams["data_flow_hybrid"] = diagram
            print(f"  -> Classified as: data_flow_hybrid")
        elif "performance" in svg_filename or "performance_model" in svg_filename:
            diagrams["performance"] = diagram
            print(f"  -> Classified as: performance")
        elif "state_machine" in svg_filename:
            diagrams["state_machine"] = diagram
            print(f"  -> Classified as: state_machine")
        elif "workflow_execution" in svg_filename or "execution_sequence" in svg_filename:
            diagrams["workflow_execution"] = diagram
            print(f"  -> Classified as: workflow_execution")
        elif "deployment" in svg_filename:
            diagrams["deployment"] = diagram
            print(f"  -> Classified as: deployment")
        else:
            # Other diagrams
            other_key = f"other_{len([k for k in diagrams.keys() if k.startswith('other_')])+1}"
            diagrams[other_key] = diagram
            print(f"  -> Classified as: {other_key}")

    return diagrams

def generate_html(diagrams: Dict[str, DesignDiagram], output_file: str):
    """Generate comprehensive HTML design review document"""

    # Get current timestamp
    current_time = datetime.now().strftime("%Y-%m-%d")

    # Calculate statistics
    total_diagrams = len(diagrams)
    diagrams_with_titles = sum(1 for d in diagrams.values() if d.title)
    total_notes = sum(len(d.notes) for d in diagrams.values())
    diagrams_with_descriptions = sum(1 for d in diagrams.values() if d.description)

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

        .review-process {{
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            margin-bottom: 40px;
        }}

        .review-process h2 {{
            color: #2c3e50;
            border-bottom: 3px solid #3498db;
            padding-bottom: 10px;
            margin-bottom: 30px;
            font-size: 1.8em;
        }}

        .review-sop {{
            background: white;
            padding: 25px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            margin-bottom: 30px;
        }}

        .sop-steps {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
        }}

        .sop-step {{
            background: #f8f9fa;
            padding: 20px;
            border-radius: 8px;
            border-left: 4px solid #3498db;
            position: relative;
        }}

        .sop-step-number {{
            position: absolute;
            top: -10px;
            left: -10px;
            background: #3498db;
            color: white;
            width: 30px;
            height: 30px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: bold;
            font-size: 14px;
        }}

        .review-points {{
            margin-bottom: 15px;
        }}

        .review-point {{
            margin-bottom: 10px;
            padding: 8px;
            background: white;
            border-radius: 5px;
            border: 1px solid #e1e1e1;
        }}

        .review-point strong {{
            color: #3498db;
        }}

        .evaluation-table {{
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
            background: white;
            border-radius: 5px;
            overflow: hidden;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }}

        .evaluation-table th,
        .evaluation-table td {{
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #eee;
        }}

        .evaluation-table th {{
            background: #f8f9fa;
            font-weight: bold;
            color: #2c3e50;
        }}

        .evaluation-table .good-column {{
            background: linear-gradient(135deg, #d4edda 0%, #c3e6cb 100%);
            border-left: 4px solid #28a745;
        }}

        .evaluation-table .bad-column {{
            background: linear-gradient(135deg, #f8d7da 0%, #f5c6cb 100%);
            border-left: 4px solid #dc3545;
        }}

        .evaluation-table .neutral-column {{
            background: #fff3cd;
            border-left: 4px solid #ffc107;
        }}

        .judgment-criteria {{
            background: #fff3cd;
            border: 1px solid #ffeaa7;
            border-radius: 5px;
            padding: 15px;
            margin-top: 10px;
        }}

        .criteria-pass {{
            color: #27ae60;
            font-weight: bold;
        }}

        .criteria-fail {{
            color: #e74c3c;
            font-weight: bold;
        }}

        .criteria-conditional {{
            color: #f39c12;
            font-weight: bold;
        }}

        .inline-diagram {{
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            margin: 20px 0;
            overflow: hidden;
        }}

        .inline-diagram-header {{
            background: #2c3e50;
            color: white;
            padding: 15px;
            font-size: 1.1em;
            font-weight: bold;
        }}

        .inline-diagram-content {{
            padding: 15px;
        }}

        .diagram-image {{
            width: 100%;
            height: auto;
            border: 1px solid #e1e1e1;
            border-radius: 5px;
            margin-bottom: 15px;
        }}

        .diagram-review-notes {{
            background: #e8f5e8;
            border: 1px solid #c8e6c9;
            border-radius: 5px;
            padding: 15px;
            margin-top: 15px;
        }}

        .diagram-review-notes h5 {{
            color: #27ae60;
            margin-bottom: 10px;
            font-size: 1em;
        }}

        .requirements-mapping {{
            background: white;
            padding: 25px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            margin-bottom: 30px;
        }}

        .mapping-section h3 {{
            color: #2c3e50;
            margin-bottom: 15px;
            font-size: 1.3em;
        }}

        .mapping-grid {{
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
        }}

        .mapping-item {{
            background: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            border-left: 3px solid #3498db;
        }}

        .mapping-item h4 {{
            color: #3498db;
            margin-bottom: 8px;
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

            .mapping-grid {{
                grid-template-columns: 1fr;
            }}

            .review-categories {{
                grid-template-columns: 1fr;
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

        <!-- 评审SOP -->
        <div class="review-sop">
            <h2>📋 设计评审标准操作程序 (SOP)</h2>
            <div class="sop-steps">
                <div class="sop-step">
                    <div class="sop-step-number">1</div>
                    <h4>📋 评审准备阶段</h4>
                    <p><strong>目标：</strong> 确保评审人员充分理解设计背景和需求</p>
                    <ul>
                        <li>分发设计文档和相关图表给所有评审人员</li>
                        <li>设计人员简要介绍设计目标和关键变更</li>
                        <li>评审人员提前阅读文档，准备问题清单</li>
                        <li>确认评审会议时间和参会人员</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">2</div>
                    <h4>🔍 架构完整性评审</h4>
                    <p><strong>目标：</strong> 验证系统架构设计的合理性和完整性</p>
                    <ul>
                        <li>按照评价指标表逐项检查</li>
                        <li>查看对应的设计图表内容</li>
                        <li>记录发现的问题和改进建议</li>
                        <li>给出明确的评审结论</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">3</div>
                    <h4>⚡ 性能架构评审</h4>
                    <p><strong>目标：</strong> 评估性能设计和优化策略的有效性</p>
                    <ul>
                        <li>按照评价指标表逐项检查</li>
                        <li>查看对应的设计图表内容</li>
                        <li>验证性能基准测试计划</li>
                        <li>评估优化策略的合理性</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">4</div>
                    <h4>🛡️ 可靠性设计评审</h4>
                    <p><strong>目标：</strong> 检查系统的容错能力和错误处理机制</p>
                    <ul>
                        <li>按照评价指标表逐项检查</li>
                        <li>查看对应的设计图表内容</li>
                        <li>验证状态机设计的完整性</li>
                        <li>检查监控和告警机制</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">5</div>
                    <h4>🚀 部署运维评审</h4>
                    <p><strong>目标：</strong> 验证部署方案和运维能力的可行性</p>
                    <ul>
                        <li>按照评价指标表逐项检查</li>
                        <li>查看对应的设计图表内容</li>
                        <li>评估部署架构的灵活性</li>
                        <li>确认运维工具链的完整性</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">6</div>
                    <h4>🎯 需求映射验证</h4>
                    <p><strong>目标：</strong> 确保设计与需求和代码实现的对齐</p>
                    <ul>
                        <li>验证功能需求的完整覆盖</li>
                        <li>检查性能指标的具体实现路径</li>
                        <li>确认可靠性保障机制</li>
                        <li>评估运维需求的实现程度</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">7</div>
                    <h4>📝 评审总结与行动项</h4>
                    <p><strong>目标：</strong> 整理评审结果并制定改进计划</p>
                    <ul>
                        <li>汇总所有评审问题和发现</li>
                        <li>确定问题优先级和修复时间表</li>
                        <li>指定责任人和跟进机制</li>
                        <li>记录评审结论和建议</li>
                    </ul>
                </div>

                <div class="sop-step">
                    <div class="sop-step-number">8</div>
                    <h4>🔄 跟进验证</h4>
                    <p><strong>目标：</strong> 确认问题修复和设计完善</p>
                    <ul>
                        <li>验证问题修复的完整性</li>
                        <li>确认设计文档的更新</li>
                        <li>必要时进行二次评审</li>
                        <li>归档评审记录和决策</li>
                    </ul>
                </div>
            </div>
        </div>

        <!-- 架构完整性评审 -->
        <div class="review-process">
            <h2>🔍 架构完整性评审</h2>

            <div class="review-points">
                <div class="review-point">
                    <strong>模块依赖关系：</strong>检查各模块间的耦合度，验证解耦设计是否合理
                </div>
                <div class="review-point">
                    <strong>接口契约：</strong>验证API接口定义的完整性和一致性
                </div>
                <div class="review-point">
                    <strong>扩展机制：</strong>评估插件架构和适配器框架的可扩展性
                </div>
            </div>

            <table class="evaluation-table">
                <thead>
                    <tr>
                        <th>检查内容</th>
                        <th class="good-column">✅ 好的评价指标</th>
                        <th class="bad-column">❌ 坏的评价指标</th>
                        <th class="neutral-column">⚠️ 一般评价指标</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>分层架构</strong></td>
                        <td class="good-column">清晰的四层架构：<br>• 应用层→核心层→扩展层→集成层<br>• 每层职责明确，边界清晰</td>
                        <td class="bad-column">分层混乱：<br>• 层次不清，职责混杂<br>• 跨层调用频繁</td>
                        <td class="neutral-column">基本分层：<br>• 层次可识别但边界模糊<br>• 存在少量跨层依赖</td>
                    </tr>
                    <tr>
                        <td><strong>依赖方向</strong></td>
                        <td class="good-column">严格的自上而下：<br>• 高层不依赖低层<br>• 依赖倒置原则正确应用</td>
                        <td class="bad-column">存在逆向依赖：<br>• 低层依赖高层<br>• 循环依赖关系</td>
                        <td class="neutral-column">部分逆向：<br>• 个别逆向依赖<br>• 有合理的技术理由</td>
                    </tr>
                    <tr>
                        <td><strong>接口定义</strong></td>
                        <td class="good-column">完整规范：<br>• 方法签名、参数、返回值<br>• 异常处理、线程安全<br>• 版本兼容性</td>
                        <td class="bad-column">定义缺失：<br>• 接口不完整<br>• 参数类型不一致<br>• 缺少错误处理</td>
                        <td class="neutral-column">基本完整：<br>• 主要接口定义<br>• 部分细节待完善</td>
                    </tr>
                </tbody>
            </table>

            <div class="judgment-criteria">
                <strong>评审结论标准：</strong><br>
                <span class="criteria-pass">✅ 通过：</span> 三个检查内容均为好的评价指标<br>
                <span class="criteria-conditional">⚠️ 条件通过：</span> 允许一个一般评价指标，或有明确的改进计划<br>
                <span class="criteria-fail">❌ 不通过：</span> 存在坏的评价指标或多个一般评价指标
            </div>

            <!-- 相关设计图表 -->
"""

    # Add architecture diagrams
    for key, diagram in diagrams.items():
        if key == "architecture_class":
            svg_name = os.path.basename(diagram.svg_file)
            html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表2: 架构类图</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Architecture Class Diagram" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证四层架构是否清晰标识</li>
                            <li>✅ 检查依赖箭头方向是否自上而下</li>
                            <li>✅ 确认接口定义的完整性</li>
                        </ul>
                    </div>
                </div>
            </div>"""
        elif key == "component_interaction":
            svg_name = os.path.basename(diagram.svg_file)
            html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表3: 组件交互架构</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Component Interaction Architecture" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证交互模式的多样性</li>
                            <li>✅ 检查契约规范的明确性</li>
                            <li>✅ 确认扩展点的预留情况</li>
                        </ul>
                    </div>
                </div>
            </div>"""
        elif key == "module_dependencies":
            svg_name = os.path.basename(diagram.svg_file)
            html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表7: 模块依赖关系</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Module Dependencies Diagram" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证耦合级别的合理性</li>
                            <li>✅ 检查依赖层次的清晰度</li>
                            <li>✅ 确认构建场景的多样性</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    # Add workflow_execution diagram to architecture review section
    if "workflow_execution" in diagrams:
        diagram = diagrams["workflow_execution"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表4: 工作流执行序列图</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Workflow Execution Sequence Diagram" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证工作流的启动和初始化序列</li>
                            <li>✅ 检查条件分支和多阶段处理的执行逻辑</li>
                            <li>✅ 确认任务状态转换和调度机制</li>
                            <li>✅ 对齐动态决策和错误重试的执行流程</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    html_content += """
        </div>

        <!-- 性能架构评审 -->
        <div class="review-process">
            <h2>⚡ 性能架构评审</h2>

            <div class="review-points">
                <div class="review-point">
                    <strong>并发模型：</strong>评估并行处理能力和资源利用效率
                </div>
                <div class="review-point">
                    <strong>数据流控制：</strong>检查背压处理和流控机制
                </div>
                <div class="review-point">
                    <strong>优化策略：</strong>验证性能优化模式的有效性
                </div>
            </div>

            <table class="evaluation-table">
                <thead>
                    <tr>
                        <th>检查内容</th>
                        <th class="good-column">✅ 好的评价指标</th>
                        <th class="bad-column">❌ 坏的评价指标</th>
                        <th class="neutral-column">⚠️ 一般评价指标</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>流控机制</strong></td>
                        <td class="good-column">完善的背压处理：<br>• 缓冲区管理<br>• 流量整形<br>• 拥塞控制</td>
                        <td class="bad-column">缺少流控：<br>• 无背压机制<br>• 缓冲区溢出风险<br>• 资源竞争</td>
                        <td class="neutral-column">基本流控：<br>• 部分背压处理<br>• 缓冲区有限保护</td>
                    </tr>
                    <tr>
                        <td><strong>并发效率</strong></td>
                        <td class="good-column">高效的并发模型：<br>• 锁竞争最小化<br>• 资源利用最大化<br>• 可扩展性强</td>
                        <td class="bad-column">并发问题：<br>• 锁竞争严重<br>• 死锁风险<br>• 资源利用低</td>
                        <td class="neutral-column">一般并发：<br>• 基本无锁竞争<br>• 资源利用一般</td>
                    </tr>
                    <tr>
                        <td><strong>优化策略</strong></td>
                        <td class="good-column">多维度优化：<br>• 计算、内存、I/O优化<br>• 性能指标量化<br>• 基准测试验证</td>
                        <td class="bad-column">优化缺失：<br>• 无明确优化策略<br>• 性能指标模糊<br>• 缺少验证</td>
                        <td class="neutral-column">部分优化：<br>• 部分维度优化<br>• 指标基本明确</td>
                    </tr>
                </tbody>
            </table>

            <div class="judgment-criteria">
                <strong>评审结论标准：</strong><br>
                <span class="criteria-pass">✅ 通过：</span> 三个检查内容均为好的评价指标<br>
                <span class="criteria-conditional">⚠️ 条件通过：</span> 允许一个一般评价指标，需补充性能测试<br>
                <span class="criteria-fail">❌ 不通过：</span> 存在坏的评价指标或性能瓶颈明显
            </div>

            <!-- 相关设计图表 -->
"""

    if "data_flow" in diagrams:
        diagram = diagrams["data_flow"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表4: 数据流架构</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Data Flow Architecture" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证背压处理机制</li>
                            <li>✅ 检查缓冲区管理</li>
                            <li>✅ 确认数据流控制的完整性</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    if "performance" in diagrams:
        diagram = diagrams["performance"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表8: 性能优化模型</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Performance Architecture Model" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证优化策略的全面性</li>
                            <li>✅ 检查性能指标的量化</li>
                            <li>✅ 确认优化效果的可验证性</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    html_content += """
        </div>

        <!-- 可靠性设计评审 -->
        <div class="review-process">
            <h2>🛡️ 可靠性设计评审</h2>

            <div class="review-points">
                <div class="review-point">
                    <strong>错误处理：</strong>检查异常处理和恢复机制
                </div>
                <div class="review-point">
                    <strong>状态管理：</strong>验证状态机设计和状态一致性
                </div>
                <div class="review-point">
                    <strong>容错能力：</strong>评估故障检测和恢复能力
                </div>
            </div>

            <table class="evaluation-table">
                <thead>
                    <tr>
                        <th>检查内容</th>
                        <th class="good-column">✅ 好的评价指标</th>
                        <th class="bad-column">❌ 坏的评价指标</th>
                        <th class="neutral-column">⚠️ 一般评价指标</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>错误处理</strong></td>
                        <td class="good-column">完善的异常处理：<br>• 错误分类明确<br>• 恢复机制完整<br>• 错误传播清晰</td>
                        <td class="bad-column">错误处理缺失：<br>• 无异常处理<br>• 错误被忽略<br>• 系统崩溃风险</td>
                        <td class="neutral-column">基本错误处理：<br>• 部分异常处理<br>• 错误恢复不完整</td>
                    </tr>
                    <tr>
                        <td><strong>状态管理</strong></td>
                        <td class="good-column">可靠的状态机：<br>• 状态转换明确<br>• 一致性保证<br>• 并发安全</td>
                        <td class="bad-column">状态管理混乱：<br>• 状态转换不清晰<br>• 一致性问题<br>• 竞态条件</td>
                        <td class="neutral-column">一般状态管理：<br>• 状态基本清晰<br>• 一致性有保证</td>
                    </tr>
                    <tr>
                        <td><strong>容错能力</strong></td>
                        <td class="good-column">强大的容错：<br>• 故障检测及时<br>• 自动恢复机制<br>• 降级处理完善</td>
                        <td class="bad-column">容错能力弱：<br>• 故障检测滞后<br>• 无恢复机制<br>• 单点故障</td>
                        <td class="neutral-column">基本容错：<br>• 部分故障检测<br>• 手动恢复为主</td>
                    </tr>
                </tbody>
            </table>

            <div class="judgment-criteria">
                <strong>评审结论标准：</strong><br>
                <span class="criteria-pass">✅ 通过：</span> 三个检查内容均为好的评价指标<br>
                <span class="criteria-conditional">⚠️ 条件通过：</span> 允许一个一般评价指标，需补充监控告警<br>
                <span class="criteria-fail">❌ 不通过：</span> 存在坏的评价指标或可靠性风险明显
            </div>

            <!-- 相关设计图表 -->
"""

    if "state_machine" in diagrams:
        diagram = diagrams["state_machine"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表9: 状态机架构</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline State Machine Architecture" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证状态转换的完整性</li>
                            <li>✅ 检查状态一致性保证</li>
                            <li>✅ 确认故障检测和恢复机制</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    html_content += """
        </div>

        <!-- 部署运维评审 -->
        <div class="review-process">
            <h2>🚀 部署运维评审</h2>

            <div class="review-points">
                <div class="review-point">
                    <strong>部署架构：</strong>检查部署方案的灵活性和可扩展性
                </div>
                <div class="review-point">
                    <strong>可观测性：</strong>验证监控、日志和度量收集机制
                </div>
                <div class="review-point">
                    <strong>配置管理：</strong>评估配置驱动的行为修改能力
                </div>
            </div>

            <table class="evaluation-table">
                <thead>
                    <tr>
                        <th>检查内容</th>
                        <th class="good-column">✅ 好的评价指标</th>
                        <th class="bad-column">❌ 坏的评价指标</th>
                        <th class="neutral-column">⚠️ 一般评价指标</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>部署架构</strong></td>
                        <td class="good-column">高度灵活：<br>• 多场景支持<br>• 自动化部署<br>• 快速回滚</td>
                        <td class="bad-column">部署复杂：<br>• 部署困难<br>• 手动操作多<br>• 回滚困难</td>
                        <td class="neutral-column">基本部署：<br>• 部分自动化<br>• 回滚有保障</td>
                    </tr>
                    <tr>
                        <td><strong>可观测性</strong></td>
                        <td class="good-column">完善监控：<br>• 全方位监控<br>• 实时告警<br>• 问题定位快速</td>
                        <td class="bad-column">监控缺失：<br>• 监控覆盖低<br>• 无告警机制<br>• 问题排查难</td>
                        <td class="neutral-column">基本监控：<br>• 主要指标监控<br>• 告警机制存在</td>
                    </tr>
                    <tr>
                        <td><strong>配置管理</strong></td>
                        <td class="good-column">动态配置：<br>• 运行时修改<br>• 配置版本控制<br>• 配置验证</td>
                        <td class="bad-column">配置僵化：<br>• 需要重启<br>• 无版本控制<br>• 配置错误风险</td>
                        <td class="neutral-column">静态配置：<br>• 重启后生效<br>• 基本版本控制</td>
                    </tr>
                </tbody>
            </table>

            <div class="judgment-criteria">
                <strong>评审结论标准：</strong><br>
                <span class="criteria-pass">✅ 通过：</span> 三个检查内容均为好的评价指标<br>
                <span class="criteria-conditional">⚠️ 条件通过：</span> 允许一个一般评价指标，需完善运维工具<br>
                <span class="criteria-fail">❌ 不通过：</span> 存在坏的评价指标或运维困难明显
            </div>

            <!-- 相关设计图表 -->
"""

    if "deployment" in diagrams:
        diagram = diagrams["deployment"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表6: 部署架构</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Deployment Architecture" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证部署场景的多样性</li>
                            <li>✅ 检查监控栈的完整性</li>
                            <li>✅ 确认配置管理机制</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    html_content += """
        </div>

        <div class="requirements-mapping">
            <h3>🎯 用户场景与设计实现映射验证</h3>
            <div class="mapping-grid">
                <div class="mapping-item">
                    <h4>📊 数据处理流水线场景</h4>
                    <p><strong>典型场景：</strong> 大数据批量处理、实时数据流处理</p>
                    <p><strong>架构映射：</strong> 验证数据流架构是否支持场景需求</p>
                    <p><strong>实现验证：</strong> 工作流引擎和数据管道的集成测试</p>
                    <p><strong>性能保障：</strong> 吞吐量和延迟指标满足场景要求</p>
                    <p><strong>流程对齐：</strong> <a href="#data_flow_hybrid">数据流混合模型序列图</a> 展示完整的启动和执行流程</p>
                </div>

                <div class="mapping-item">
                    <h4>🔄 动态任务编排场景</h4>
                    <p><strong>典型场景：</strong> 条件分支、多阶段处理、错误重试</p>
                    <p><strong>架构映射：</strong> 状态机和任务调度器的协作机制</p>
                    <p><strong>实现验证：</strong> 工作流定义和执行引擎的功能测试</p>
                    <p><strong>可靠性保障：</strong> 任务状态一致性和故障恢复能力</p>
                    <p><strong>流程对齐：</strong> <a href="#workflow_execution">工作流执行序列图</a> 展示完整的任务启动和执行流程</p>
                </div>

                <div class="mapping-item">
                    <h4>🔌 外部系统集成场景</h4>
                    <p><strong>典型场景：</strong> 数据库连接、消息队列、API调用</p>
                    <p><strong>架构映射：</strong> 适配器框架和连接器的设计模式</p>
                    <p><strong>实现验证：</strong> 第三方库集成和协议适配的测试</p>
                    <p><strong>扩展性保障：</strong> 新集成能力的快速接入机制</p>
                </div>

                <div class="mapping-item">
                    <h4>📈 监控与运维场景</h4>
                    <p><strong>典型场景：</strong> 系统监控、性能调优、故障排查</p>
                    <p><strong>架构映射：</strong> 可观测性和配置管理的实现框架</p>
                    <p><strong>实现验证：</strong> 监控指标收集和告警机制的测试</p>
                    <p><strong>运维保障：</strong> 运行时配置和部署管理的便利性</p>
                </div>
            </div>

            <div class="use-case-validation">
                <h3>✅ 用户场景覆盖验证清单</h3>
                <div class="validation-checklist">
                    <div class="validation-item">
                        <h4>🔍 场景完整性检查</h4>
                        <ul>
                            <li>✅ 核心业务场景是否全部覆盖</li>
                            <li>✅ 异常处理场景是否充分考虑</li>
                            <li>✅ 边界条件和极限情况是否测试</li>
                            <li>✅ 用户交互流程是否顺畅</li>
                        </ul>
                    </div>

                    <div class="validation-item">
                        <h4>🎯 设计实现一致性</h4>
                        <ul>
                            <li>✅ 架构设计是否支持所有场景需求</li>
                            <li>✅ 接口设计是否满足场景调用要求</li>
                            <li>✅ 数据流设计是否保障场景数据处理</li>
                            <li>✅ 状态管理是否覆盖场景状态变化</li>
                        </ul>
                    </div>

                    <div class="validation-item">
                        <h4>⚡ 性能可靠性保障</h4>
                        <ul>
                            <li>✅ 场景性能指标是否达到预期</li>
                            <li>✅ 高并发场景的稳定性保障</li>
                            <li>✅ 长时间运行的资源管理</li>
                            <li>✅ 故障场景的快速恢复能力</li>
                        </ul>
                    </div>

                    <div class="validation-item">
                        <h4>🔧 运维部署便利性</h4>
                        <ul>
                            <li>✅ 场景部署配置是否简单直观</li>
                            <li>✅ 运行时监控是否全面有效</li>
                            <li>✅ 问题定位和故障排查是否便捷</li>
                            <li>✅ 版本升级和回滚是否安全可靠</li>
                        </ul>
                    </div>
                </div>

                <div class="scenario-mapping-table">
                    <h3>📋 典型用户场景映射表</h3>
                    <table class="evaluation-table">
                        <thead>
                            <tr>
                                <th>用户场景</th>
                                <th>核心需求</th>
                                <th>架构组件</th>
                                <th>实现验证</th>
                                <th>测试状态</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td><strong>批量数据处理</strong></td>
                                <td>高吞吐量数据转换</td>
                                <td>DataFlow + ParallelTask</td>
                                <td>集成测试覆盖率 > 90%</td>
                                <td class="good-column">✅ 通过</td>
                            </tr>
                            <tr>
                                <td><strong>实时流处理</strong></td>
                                <td>低延迟数据流转</td>
                                <td>DataStream + Workflow</td>
                                <td>性能基准测试验证</td>
                                <td class="good-column">✅ 通过</td>
                            </tr>
                            <tr>
                                <td><strong>条件任务编排</strong></td>
                                <td>动态决策和分支</td>
                                <td>StateMachine + Adapters</td>
                                <td>状态转换测试完整</td>
                                <td class="neutral-column">⚠️ 待完善</td>
                            </tr>
                            <tr>
                                <td><strong>系统集成接入</strong></td>
                                <td>多协议外部连接</td>
                                <td>Adapters + Connectors</td>
                                <td>兼容性测试通过</td>
                                <td class="good-column">✅ 通过</td>
                            </tr>
                            <tr>
                                <td><strong>运维监控管理</strong></td>
                                <td>实时状态观测</td>
                                <td>Monitor + Dashboard</td>
                                <td>监控指标完整收集</td>
                                <td class="neutral-column">⚠️ 开发中</td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>

        <div class="navigation">
            <h3>🧭 快速导航</h3>
            <div class="nav-links">
"""

    # Add navigation links for remaining diagrams (excluding data_flow_hybrid and workflow_execution)
    other_diagrams = {k: v for k, v in diagrams.items() if k.startswith("other_")}
    remaining_diagrams = {k: v for k, v in other_diagrams.items() if k not in ["data_flow_hybrid", "workflow_execution"]}
    for key, diagram in remaining_diagrams.items():
        svg_name = os.path.basename(diagram.svg_file)
        title = diagram.title or svg_name.replace('.svg', '').replace('Parallel Pipeline ', '')
        html_content += f'                <a href="#{key}" class="nav-link">{title}</a>\n'

    html_content += """            </div>
        </div>

        <!-- 其他设计图表 -->
        <div class="diagram-grid">
"""

    # Add data_flow_hybrid diagram to performance review section
    if "data_flow_hybrid" in diagrams:
        diagram = diagrams["data_flow_hybrid"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram">
                <div class="inline-diagram-header">📊 图表5: 数据流混合模型序列图</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Data Flow Hybrid Model Sequence Diagram" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证数据流处理的启动序列</li>
                            <li>✅ 检查混合模型的切换逻辑</li>
                            <li>✅ 确认数据管道的初始化流程</li>
                            <li>✅ 对齐大数据处理和实时流处理的启动需求</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    html_content += """
        </div>

        <!-- Add workflow_execution diagram to requirements mapping section -->
        <div class="requirements-mapping">
            <h3>🔄 工作流执行序列图 - 用户场景流程对齐</h3>
"""

    if "workflow_execution" in diagrams:
        diagram = diagrams["workflow_execution"]
        svg_name = os.path.basename(diagram.svg_file)
        html_content += f"""
            <div class="inline-diagram" id="workflow_execution">
                <div class="inline-diagram-header">📊 图表10: 工作流执行序列图</div>
                <div class="inline-diagram-content">
                    <img src="{svg_name}" alt="Parallel Pipeline Workflow Execution Sequence Diagram" class="diagram-image" loading="lazy">
                    <div class="diagram-review-notes">
                        <h5>🔍 评审要点检查：</h5>
                        <ul>
                            <li>✅ 验证工作流的启动和初始化序列</li>
                            <li>✅ 检查条件分支和多阶段处理的执行逻辑</li>
                            <li>✅ 确认任务状态转换和调度机制</li>
                            <li>✅ 对齐动态决策和错误重试的执行流程</li>
                        </ul>
                    </div>
                </div>
            </div>"""

    # Add data_flow_hybrid diagram reference for requirements mapping
    if "data_flow_hybrid" in diagrams:
        html_content += """
            <div class="diagram-reference" id="data_flow_hybrid">
                <h4>📊 数据流混合模型序列图参考</h4>
                <p><strong>适用场景：</strong> 数据处理流水线场景的流程对齐验证</p>
                <p><strong>查看位置：</strong> 性能架构评审部分</p>
                <p><strong>验证内容：</strong> 启动流程、混合模型切换、数据管道初始化</p>
            </div>"""

    html_content += """
        </div>

        <!-- 其他设计图表 -->
        <div class="diagram-grid">
"""

    # Add remaining diagram cards (excluding data_flow_hybrid and workflow_execution)
    remaining_diagrams = {k: v for k, v in other_diagrams.items() if k not in ["data_flow_hybrid", "workflow_execution"]}
    for key, diagram in remaining_diagrams.items():
        svg_name = os.path.basename(diagram.svg_file)
        title = diagram.title or svg_name.replace('.svg', '').replace('Parallel Pipeline ', '')
        description = diagram.description or "暂无详细描述"

        html_content += f"""
            <div class="diagram-card" id="{key}">
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

    html_content += f"""
        </div>

        <div class="footer">
            <p>📅 生成时间: {current_time} | 🔧 自动生成脚本 | 🎯 并行管道架构设计评审</p>
            <p>💡 本文档包含完整的架构设计图表，用于设计评审和技术评估</p>
        </div>
    </div>

    <script>
        // Add smooth scrolling for navigation links
        document.querySelectorAll('.nav-link').forEach(link => {{
            link.addEventListener('click', function(e) {{
                e.preventDefault();
                const targetId = this.getAttribute('href');
                const targetElement = document.querySelector(targetId);
                if (targetElement) {{
                    targetElement.scrollIntoView({{ behavior: 'smooth' }});
                }}
            }});
        }});

        // Add lazy loading for images
        const images = document.querySelectorAll('.diagram-image');
        const imageObserver = new IntersectionObserver((entries, observer) => {{
            entries.forEach(entry => {{
                if (entry.isIntersecting) {{
                    const img = entry.target;
                    img.classList.add('loaded');
                    observer.unobserve(img);
                }}
            }});
        }});

        images.forEach(img => imageObserver.observe(img));
    </script>
</body>
</html>"""

    # Write HTML file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(html_content)

    print(f"✅ Generated comprehensive design review documentation: {output_file}")
    print(f"📊 Total diagrams: {total_diagrams}")
    print(f"📝 Diagrams with titles: {diagrams_with_titles}")
    print(f"📋 Total notes: {total_notes}")
    print(f"📋 Diagrams with descriptions: {diagrams_with_descriptions}")

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
