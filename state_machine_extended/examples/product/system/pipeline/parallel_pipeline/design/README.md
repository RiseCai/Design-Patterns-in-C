# Parallel Pipeline Design Diagrams

This directory contains PlantUML diagrams documenting the architecture and design of the parallel pipeline framework.

## Available Diagrams

1. **parallel_pipeline_architecture_class_diagram.puml** - Overall architecture class diagram showing the decoupled modules
2. **parallel_pipeline_workflow_sequence_diagram.puml** - Sequence diagram of workflow execution
3. **parallel_pipeline_dataflow_sequence_diagram.puml** - Data flow sequence diagram
4. **parallel_pipeline_module_dependencies_diagram.puml** - Module dependency relationships
5. **parallel_pipeline_adapters_design_diagram.puml** - External system adapters design

## Generating SVG Diagrams

### Prerequisites

1. **Java Runtime Environment (JRE)** - Required to run PlantUML
2. **PlantUML JAR** - Download from https://plantuml.com/download

### Setup PlantUML

1. Download `plantuml.jar` from the official PlantUML website
2. Place `plantuml.jar` in the project root directory: `D:\david\work\temp\Design-Patterns-in-C\plantuml.jar`

### Generate Diagrams

Run the update script from this directory:

```bash
cd state_machine_extended/examples/product/system/pipeline/parallel_pipeline/design
python update_svg.py
```

This will generate SVG files for all PUML diagrams in the directory.

### Manual Generation

If you prefer to generate diagrams manually:

```bash
java -jar ../../../../../../plantuml.jar -tsvg diagram_name.puml
```

## Diagram Descriptions

### Architecture Class Diagram
Shows the modular architecture after decoupling:
- Main entry point (`parallel_pipeline.h`)
- Core modules (Task, Workflow)
- Extension modules (DataFlow, DataStream, Monitor)
- Integration modules (Adapters)

### Workflow Sequence Diagram
Illustrates the execution flow of parallel workflows:
- Task initialization and dependency checking
- Concurrent execution with concurrency control
- Completion handling and cleanup

### Data Flow Sequence Diagram
Demonstrates data flow between tasks:
- Producer-consumer relationships
- Data stream buffering and synchronization
- Hybrid task execution patterns

### Module Dependencies Diagram
Visualizes the dependency relationships between modules:
- Clear separation of concerns
- Optional vs. required dependencies
- Extension points for future enhancements

### Adapters Design Diagram
Shows the pluggable adapter architecture:
- MQ, REST, Database, and File System adapters
- Standardized interfaces
- Extensible adapter framework

## Architecture Principles Reflected

1. **Single Responsibility** - Each module has a focused purpose
2. **Dependency Inversion** - Clear dependency directions
3. **Interface Segregation** - Separate interfaces for different concerns
4. **Open/Closed Principle** - Easy extension without modification
5. **Backward Compatibility** - Existing code continues to work

## File Organization

```
design/
├── *.puml                 # PlantUML source files
├── *.svg                  # Generated SVG diagrams
├── update_svg.py          # Diagram generation script
└── README.md             # This documentation
```

## Maintenance

When updating diagrams:
1. Edit the corresponding `.puml` file
2. Run `python update_svg.py` to regenerate SVGs
3. Commit both `.puml` and `.svg` files

The SVG files are included in version control for convenience, allowing others to view diagrams without setting up PlantUML.
