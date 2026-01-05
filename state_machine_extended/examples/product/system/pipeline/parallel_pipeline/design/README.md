# Parallel Pipeline Decoupled Architecture Design Documents

## 📋 概述

本目录包含并行管道模块的解耦架构设计文档。这些PlantUML图表详细描述了模块化的并行工作流框架设计方案，用于指导代码实现和维护。

## 📚 设计文档列表

### 1. **并行管道架构类图** (`parallel_pipeline_architecture_class_diagram.puml`)
- **内容**: 完整的模块化架构类图
- **范围**: 所有模块的类结构、接口和关系
- **用途**: 理解整体架构和模块职责

### 2. **工作流执行时序图** (`parallel_pipeline_workflow_sequence_diagram.puml`)
- **内容**: 工作流完整执行流程
- **范围**: 初始化 → 配置 → 执行 → 监控 → 清理
- **用途**: 理解工作流生命周期和并发控制

### 3. **数据流混合模型时序图** (`parallel_pipeline_dataflow_sequence_diagram.puml`)
- **内容**: 混合依赖模型的工作原理
- **范围**: 任务类型、依赖检查、数据流处理
- **用途**: 理解三种任务类型的协作机制

### 4. **模块依赖关系图** (`parallel_pipeline_module_dependencies_diagram.puml`)
- **内容**: 模块间的依赖关系和分层架构
- **范围**: 依赖方向、耦合级别、使用场景
- **用途**: 理解模块解耦程度和编译依赖

### 5. **适配器框架设计图** (`parallel_pipeline_adapters_design_diagram.puml`)
- **内容**: 外部系统适配器框架详细设计
- **范围**: MQ、REST、数据库、文件系统适配器
- **用途**: 理解外部系统集成架构

## 🏗️ 架构设计原则

### 分层架构
```
应用层 (Application)     → parallel_pipeline.h
核心层 (Core)           → parallel_task.h, parallel_workflow.h
扩展层 (Extension)      → parallel_dataflow.h, data_stream_service.h, workflow_monitor.h
集成层 (Integration)    → parallel_adapters.h
```

### 设计模式应用
- **适配器模式**: 统一外部系统接口
- **工厂模式**: 模块化和配置管理
- **观察者模式**: 事件驱动和监控
- **策略模式**: 可插拔实现

### 依赖管理
- **单向依赖**: 避免循环依赖
- **可选依赖**: 运行时可配置的扩展
- **接口隔离**: 清晰的功能边界

## 🎯 任务类型说明

| 任务类型 | 依赖检查 | 适用场景 | 示例 |
|---------|---------|---------|------|
| `TASK_TYPE_LEGACY` | 仅任务依赖 | 简单工作流 | 传统批处理任务 |
| `TASK_TYPE_HYBRID` | 任务+数据依赖 | 混合工作流 | 传感器数据处理 |
| `TASK_TYPE_DATAFLOW` | 仅数据依赖 | 事件驱动 | 实时数据处理 |

## 📊 关键设计决策

### 1. 模块解耦策略
- **原因**: 原有单体头文件维护困难
- **方案**: 按功能职责划分模块
- **收益**: 独立开发、测试和维护

### 2. 依赖关系设计
- **核心模块**: 强依赖，编译时必需
- **扩展模块**: 弱依赖，运行时可选
- **适配器模块**: 零依赖，动态加载

### 3. 接口一致性
- **生命周期**: configure → connect → execute → disconnect → destroy
- **错误处理**: 统一的错误码和处理机制
- **配置管理**: 类型安全的分层配置

## 🔧 使用PlantUML

### 安装和使用
```bash
# 安装PlantUML (需要Java)
# 下载plantuml.jar并添加到PATH

# 生成PNG
plantuml parallel_pipeline_architecture_class_diagram.puml

# 生成SVG
plantuml -tsvg parallel_pipeline_architecture_class_diagram.puml

# 生成HTML
plantuml -thtml parallel_pipeline_architecture_class_diagram.puml
```

### 在线查看
- **GitHub**: 直接在仓库中查看
- **PlantUML Server**: https://www.plantuml.com/plantuml
- **VSCode插件**: PlantUML extension

## 📈 版本历史

| 版本 | 日期 | 主要变更 |
|-----|-----|---------|
| 1.0 | 2026-01-05 | 初始版本，包含完整解耦架构设计 |

## 🔗 相关文档

- [并行管道主文档](../../README.md)
- [数据流模块文档](../include/data_stream.h)
- [测试用例](../test/)
- [示例代码](../examples/)

## 🤝 贡献指南

1. **修改设计**: 先更新对应的PlantUML文件
2. **代码同步**: 确保代码实现与设计文档一致
3. **文档更新**: 及时更新此README和相关注释
4. **评审流程**: 重大变更需要架构评审

---

**文档维护**: 并行管道架构设计文档
**最后更新**: 2026-01-05
**维护者**: Design Patterns in C Team
