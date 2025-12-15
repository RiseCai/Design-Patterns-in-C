import requests
import base64
import zlib
import os

def encode_plantuml(text):
    """Encode PlantUML text to the format used in URLs."""
    compressor = zlib.compressobj(level=9, wbits=-15)
    compressed = compressor.compress(text.encode('utf-8')) + compressor.flush()
    encoded = base64.b64encode(compressed).decode('ascii')
    # Translate to PlantUML's custom base64
    translated = encoded.translate(str.maketrans(
        'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/',
        '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/'
    ))
    return translated

def generate_html(puml_file, html_file):
    with open(puml_file, 'r', encoding='utf-8') as f:
        puml_content = f.read()
    
    encoded = encode_plantuml(puml_content)
    url = f'http://www.plantuml.com/plantuml/svg/{encoded}'
    
    try:
        response = requests.get(url, timeout=30)
        if response.status_code != 200:
            print(f"Failed to fetch SVG: {response.status_code}")
            return False
        svg_content = response.text
    except Exception as e:
        print(f"Error fetching SVG: {e}")
        return False
    
    # Create HTML wrapper
    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PlantUML Diagram: {os.path.basename(puml_file)}</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f9f9f9;
        }}
        .container {{
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        h1 {{
            color: #333;
            border-bottom: 2px solid #4CAF50;
            padding-bottom: 10px;
        }}
        .diagram {{
            text-align: center;
            margin: 20px 0;
            overflow: auto;
        }}
        .note {{
            margin-top: 20px;
            padding: 15px;
            background-color: #f0f8ff;
            border-left: 4px solid #2196F3;
            font-size: 14px;
        }}
        .links {{
            margin-top: 20px;
        }}
        .links a {{
            display: inline-block;
            margin-right: 15px;
            padding: 8px 15px;
            background-color: #4CAF50;
            color: white;
            text-decoration: none;
            border-radius: 4px;
        }}
        .links a:hover {{
            background-color: #45a049;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>System Coordinator with All Subsystems</h1>
        <p>This diagram shows the activation relationships between system coordinator states and subsystem FSMs.</p>
        <div class="diagram">
            {svg_content}
        </div>
        <div class="note">
            <strong>Note:</strong> This diagram is generated from <code>{os.path.basename(puml_file)}</code> using the PlantUML server.
            If the diagram does not display correctly, ensure you have internet connectivity.
        </div>
        <div class="links">
            <a href="system_coordinator_state_diagram_with_api.html">Main Coordinator Diagram</a>
            <a href="recording_fsm_diagram_with_api.html">Recording FSM</a>
            <a href="audio_fsm_diagram_with_api.html">Audio FSM</a>
            <a href="comm_fsm_diagram_with_api.html">Communication FSM</a>
            <a href="power_fsm_diagram_with_api.html">Power FSM</a>
            <a href="ota_fsm_diagram_with_api.html">OTA FSM</a>
        </div>
    </div>
</body>
</html>"""
    
    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(html)
    
    print(f"Generated {html_file}")
    return True

if __name__ == '__main__':
    puml = 'system_coordinator_with_all_subsystems.puml'
    html = 'system_coordinator_with_all_subsystems.html'
    if os.path.exists(puml):
        generate_html(puml, html)
    else:
        print(f"File {puml} not found.")
