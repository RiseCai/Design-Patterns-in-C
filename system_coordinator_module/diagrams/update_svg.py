import subprocess
import os
import glob
import sys

PLANTUML_JAR = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'plantuml.jar')

def generate_svg(puml_file, svg_file):
    """
    Generate SVG from PlantUML file using local plantuml.jar
    """
    if not os.path.exists(PLANTUML_JAR):
        print(f'Error: plantuml.jar not found at {PLANTUML_JAR}')
        return False
    
    # Ensure Java is available
    try:
        subprocess.run(['java', '-version'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print('Error: Java is not installed or not in PATH')
        return False
    
    # Run plantuml.jar with -tsvg option
    cmd = ['java', '-jar', PLANTUML_JAR, '-tsvg', puml_file]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        if result.returncode != 0:
            print(f'PlantUML failed for {puml_file}: {result.stderr}')
            return False
    except subprocess.TimeoutExpired:
        print(f'Timeout while generating SVG for {puml_file}')
        return False
    except Exception as e:
        print(f'Error running PlantUML for {puml_file}: {e}')
        return False
    
    # PlantUML generates SVG with same base name but .svg extension in same directory
    expected_svg = puml_file.replace('.puml', '.svg')
    if not os.path.exists(expected_svg):
        print(f'Expected SVG file not created: {expected_svg}')
        return False
    
    # If svg_file is different from expected_svg (maybe different directory), move it
    if os.path.abspath(expected_svg) != os.path.abspath(svg_file):
        os.replace(expected_svg, svg_file)
    
    print(f'Generated {svg_file}')
    return True

def main():
    puml_files = glob.glob('*.puml')
    for puml in puml_files:
        svg = puml.replace('.puml', '.svg')
        print(f'Processing {puml} -> {svg}')
        generate_svg(puml, svg)
    
    print('All SVG files updated.')

if __name__ == '__main__':
    main()
