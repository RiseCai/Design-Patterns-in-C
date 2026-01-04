import os
import glob

def needs_update(puml_path, html_path, svg_path):
    """Return True if HTML or SVG is older than PUML, or if missing."""
    if not os.path.exists(puml_path):
        # PUML missing, cannot generate
        return False, "PUML missing"
    puml_mtime = os.path.getmtime(puml_path)
    html_exists = os.path.exists(html_path)
    svg_exists = os.path.exists(svg_path)
    if not html_exists or not svg_exists:
        return True, "HTML or SVG missing"
    html_mtime = os.path.getmtime(html_path)
    svg_mtime = os.path.getmtime(svg_path)
    if html_mtime < puml_mtime or svg_mtime < puml_mtime:
        return True, "HTML/SVG older than PUML"
    return False, "up-to-date"

def main():
    diagrams_dir = "."
    puml_files = glob.glob("*_diagram_with_api.puml")
    puml_files.extend(glob.glob("system_coordinator_sequence*.puml"))
    puml_files.extend(glob.glob("system_coordinator_with_all_subsystems.puml"))
    
    print("Checking diagrams for updates...")
    print("=" * 80)
    for puml in puml_files:
        base = puml.replace('.puml', '')
        html = base + '.html'
        svg = base + '.svg'
        need, reason = needs_update(puml, html, svg)
        status = "NEEDS UPDATE" if need else "OK"
        print(f"{puml:50} -> {status:15} ({reason})")
    
    # Also check for HTML/SVG without PUML
    html_files = glob.glob("*.html")
    for html in html_files:
        puml = html.replace('.html', '.puml')
        if not os.path.exists(puml) and not html.startswith("system_coordinator_with_subsystems"):
            print(f"{html:50} -> ORPHAN HTML (no PUML)")
    
    svg_files = glob.glob("*.svg")
    for svg in svg_files:
        puml = svg.replace('.svg', '.puml')
        if not os.path.exists(puml):
            print(f"{svg:50} -> ORPHAN SVG (no PUML)")

if __name__ == "__main__":
    main()
