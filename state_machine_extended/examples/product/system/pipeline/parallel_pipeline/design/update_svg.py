import subprocess
import os
import glob
import sys

# Path to plantuml.jar relative to this script (7 levels up to project root)
PLANTUML_JAR = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', '..', '..', '..', 'plantuml.jar')

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

    # Wait a bit for file to be written (PlantUML might need time to complete)
    import time
    time.sleep(0.5)  # Wait 500ms

    # PlantUML generates SVG files based on diagram titles, not filenames
    # Look for any SVG files that match common patterns
    all_svg_files = glob.glob('*.svg')

    if all_svg_files:
        # Find the most recently modified SVG file (likely the one we just generated)
        svg_files_with_mtime = [(f, os.path.getmtime(f)) for f in all_svg_files]
        svg_files_with_mtime.sort(key=lambda x: x[1], reverse=True)  # Sort by modification time, newest first

        latest_svg = svg_files_with_mtime[0][0]

        # Verify file has content (not empty)
        if os.path.getsize(latest_svg) > 0:
            print(f'Generated {latest_svg}')
            return True
        else:
            print(f'Generated SVG file is empty: {latest_svg}')
            return False

    print(f'No SVG file found for {puml_file}')
    return False

def main():
    """
    Generate SVG files for all PUML files in the design directory
    """
    puml_files = glob.glob('*.puml')
    if not puml_files:
        print('No PUML files found in the design directory')
        return

    success_count = 0
    total_count = len(puml_files)

    print(f'Found {total_count} PUML files. Generating SVG diagrams...')
    print('=' * 50)

    for puml in puml_files:
        svg = puml.replace('.puml', '.svg')
        print(f'Processing {puml} -> {svg}')

        if generate_svg(puml, svg):
            success_count += 1
        else:
            print(f'Failed to generate SVG for {puml}')

    print('=' * 50)
    print(f'Generation complete: {success_count}/{total_count} files processed successfully')

    if success_count == total_count:
        print('All SVG files updated successfully!')
    else:
        print(f'Warning: {total_count - success_count} files failed to generate')
        sys.exit(1)

if __name__ == '__main__':
    main()
