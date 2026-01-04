import subprocess
import os
import glob
import tempfile
import re
import json

PLANTUML_JAR = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'plantuml.jar')

def generate_svg_content(puml_file):
    """
    Generate SVG content from PlantUML file using local plantuml.jar.
    Returns SVG string on success, None on failure.
    """
    if not os.path.exists(PLANTUML_JAR):
        print(f'Error: plantuml.jar not found at {PLANTUML_JAR}')
        return None
    
    # Ensure Java is available
    try:
        subprocess.run(['java', '-version'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print('Error: Java is not installed or not in PATH')
        return None
    
    # Create a temporary output directory
    with tempfile.TemporaryDirectory() as tmpdir:
        # Run plantuml.jar with -tsvg and -o to output to temp directory
        cmd = ['java', '-jar', PLANTUML_JAR, '-tsvg', '-o', tmpdir, puml_file]
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            if result.returncode != 0:
                print(f'PlantUML failed for {puml_file}: {result.stderr}')
                return None
        except subprocess.TimeoutExpired:
            print(f'Timeout while generating SVG for {puml_file}')
            return None
        except Exception as e:
            print(f'Error running PlantUML for {puml_file}: {e}')
            return None
        
        # Find generated SVG file
        base_name = os.path.basename(puml_file).replace('.puml', '.svg')
        svg_path = os.path.join(tmpdir, base_name)
        if not os.path.exists(svg_path):
            # Maybe plantuml outputs with same name but in current directory?
            svg_path = puml_file.replace('.puml', '.svg')
            if not os.path.exists(svg_path):
                print(f'Expected SVG file not created for {puml_file}')
                return None
        
        with open(svg_path, 'r', encoding='utf-8') as f:
            svg_content = f.read()
        
        return svg_content

def extract_states_and_events(puml_file):
    """
    Extract states and events from a PlantUML file.
    Returns a tuple (states, events) where states is a list of state names,
    and events is a list of event names.
    """
    states = []
    events = []
    try:
        with open(puml_file, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f'Error reading {puml_file}: {e}')
        return [], []
    
    # Extract state definitions: state "STATE_NAME" as ALIAS
    state_pattern = r'state\s+"([^"]+)"\s+as\s+\w+'
    matches = re.findall(state_pattern, content, re.IGNORECASE)
    for match in matches:
        if match not in states:
            states.append(match)
    
    # Extract transitions: --> : EVENT
    # Also handle self-transitions: --> : EVENT
    # We want to capture only event identifiers (uppercase with underscores)
    # and ignore descriptive text like "Auto transition after initialization"
    transition_pattern = r'-->.*?:\s*([A-Z][A-Z0-9_]+)'
    matches = re.findall(transition_pattern, content)
    for match in matches:
        # Filter out false positives: require at least one underscore or length > 5
        # This will keep SYS_EVT_* and similar but exclude "Auto", "If"
        if '_' in match or len(match) > 5:
            if match not in events:
                events.append(match)
    
    # Also extract events from note sections? (optional)
    return states, events

def extract_api_functions(puml_file):
    """
    Extract API functions mentioned in the PUML file (from notes).
    Returns a list of API function names.
    """
    try:
        with open(puml_file, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f'Error reading {puml_file}: {e}')
        return []
    
    apis = []
    # Look for note blocks that contain "API:" or "API function:"
    # Pattern: note ... end note
    note_pattern = r'note\s+[^\n]*\n(.*?)\n\s*end note'
    notes = re.findall(note_pattern, content, re.DOTALL | re.IGNORECASE)
    for note in notes:
        # Check if note contains "API" (case-insensitive)
        if re.search(r'API', note, re.IGNORECASE):
            # Extract function names like audio_fsm_init() or system_coordinator_init()
            # They may appear as bullet points: • audio_fsm_init()
            # or just plain text: audio_fsm_init()
            # Use regex to capture function names with parentheses
            matches = re.findall(r'([a-zA-Z_][a-zA-Z0-9_]*\(\))', note)
            apis.extend(matches)
    
    # Also look for lines containing "API:" directly (fallback)
    lines = content.split('\n')
    for line in lines:
        if 'API:' in line or 'API function:' in line:
            matches = re.findall(r'([a-zA-Z_][a-zA-Z0-9_]*\(\))', line)
            apis.extend(matches)
    
    return list(set(apis))

def generate_hierarchical_mapping_table(puml_file):
    """
    Generate hierarchical mapping table HTML from mapping data generated by parse_puml.
    Returns HTML string for the table, or empty string if no transitions.
    """
    try:
        # Import parse_puml module (same directory)
        import parse_puml
        mapping = parse_puml.generate_mapping_data(puml_file)
    except Exception as e:
        print(f'Error generating mapping data for {puml_file}: {e}')
        return ''
    
    transitions = mapping.get('transitions', [])
    if not transitions:
        return ''
    
    # Build hierarchical table
    html = '''
    <h2>Hierarchical Mapping Table (Role‑Specific)</h2>
    <p>This table shows each transition with role‑specific details.</p>
    <table class="hierarchical">
        <thead>
            <tr>
                <th>ID</th>
                <th>Source → Target</th>
                <th>Event</th>
                <th>Description</th>
                <th>Subsystems</th>
                <th>Developer</th>
                <th>Tester</th>
                <th>Architect</th>
                <th>Product</th>
                <th>Support</th>
            </tr>
        </thead>
        <tbody>'''
    
    for t in transitions:
        # Source and target (note: parse_puml uses 'source_state' and 'target_state' keys)
        src = t.get('source_state', '')
        tgt = t.get('target_state', '')
        event = t.get('event', '')
        desc = t.get('description', '')
        subsystems = '<br>'.join(t.get('subsystems', []))
        
        # Role-specific
        role = t.get('role_specific', {})
        dev = role.get('developer', {})
        dev_text = f"Entry: {dev.get('entry_action', '')}<br>Exit: {dev.get('exit_action', '')}"
        if len(dev_text) > 50:
            dev_text = dev_text[:50] + '...'
        
        tester = role.get('tester', {})
        tester_text = f"TC: {tester.get('test_case_id', '')}<br>Tested: {tester.get('tested', False)}"
        
        arch = role.get('architect', {})
        arch_text = f"Type: {arch.get('event_type', '')}<br>Impact: {arch.get('performance_impact', '')}"
        
        prod = role.get('product', {})
        prod_text = f"Scenario: {prod.get('user_scenario', '')}<br>Priority: {prod.get('priority', '')}"
        
        support = role.get('support', {})
        support_text = f"Symptom: {support.get('symptom', '')}<br>Recovery: {len(support.get('recovery_steps', []))} steps"
        
        html += f'''
            <tr>
                <td><strong>{t.get('id', '')}</strong></td>
                <td>{src} → {tgt}</td>
                <td><code>{event}</code></td>
                <td>{desc}</td>
                <td>{subsystems}</td>
                <td>{dev_text}</td>
                <td>{tester_text}</td>
                <td>{arch_text}</td>
                <td>{prod_text}</td>
                <td>{support_text}</td>
            </tr>'''
    
    html += '''
        </tbody>
    </table>
    <style>
        table.hierarchical {{
            font-size: 12px;
            border-collapse: collapse;
            width: 100%;
            overflow-x: auto;
            display: block;
        }}
        table.hierarchical th {{
            background-color: #e0f0ff;
            position: sticky;
            top: 0;
        }}
        table.hierarchical td, table.hierarchical th {{
            border: 1px solid #ccc;
            padding: 6px;
            vertical-align: top;
        }}
        table.hierarchical tr:nth-child(even) {{
            background-color: #f9f9f9;
        }}
    </style>'''
    
    return html

def generate_html(puml_file, html_file):
    svg_content = generate_svg_content(puml_file)
    if svg_content is None:
        return False
    
    # Extract metadata
    states, events = extract_states_and_events(puml_file)
    api_functions = extract_api_functions(puml_file)
    
    # Determine diagram type from filename
    puml_name = os.path.basename(puml_file)
    if 'system_coordinator' in puml_name:
        diagram_title = 'System Coordinator FSM'
        diagram_description = 'This diagram shows the system coordinator FSM with API calls and subsystem activation.'
    elif 'efsm_protocol' in puml_name:
        diagram_title = 'EFSM Protocol'
        diagram_description = 'Extended Finite State Machine (EFSM) protocol defines the communication between subsystems.'
    elif 'recording_fsm' in puml_name:
        diagram_title = 'Recording FSM'
        diagram_description = 'Recording FSM handles audio recording and processing.'
    elif 'audio_fsm' in puml_name:
        diagram_title = 'Audio FSM'
        diagram_description = 'Audio FSM manages audio input/output and processing.'
    elif 'comm_fsm' in puml_name:
        diagram_title = 'Communication FSM'
        diagram_description = 'Communication FSM handles network connectivity and data upload.'
    elif 'power_fsm' in puml_name:
        diagram_title = 'Power FSM'
        diagram_description = 'Power FSM manages battery, charging, and power states.'
    elif 'ota_fsm' in puml_name:
        diagram_title = 'OTA FSM'
        diagram_description = 'OTA FSM handles over‑the‑air firmware updates.'
    else:
        diagram_title = 'State Diagram'
        diagram_description = 'This diagram is generated from PlantUML.'
    
    # Generate mapping table rows
    mapping_rows = ''
    if states and events:
        # Determine appropriate API function for events
        def get_api_for_event(event, api_functions, puml_name):
            # Look for a dispatch function in api_functions
            for func in api_functions:
                if 'dispatch' in func.lower():
                    # Extract function name without parentheses
                    func_name = func.replace('()', '')
                    return f'{func_name}({event})'
            # Fallback: infer from puml_name
            if 'system_coordinator' in puml_name:
                return f'system_coordinator_dispatch_event({event})'
            else:
                # Extract fsm name from puml_name (e.g., recording_fsm_diagram_with_api.puml -> recording_fsm)
                import re
                match = re.search(r'([a-z]+_fsm)', puml_name)
                if match:
                    fsm_name = match.group(1)
                else:
                    fsm_name = 'unknown'
                return f'{fsm_name}_dispatch_event({event})'
        
        # Create a simple mapping: each event leads to some state (simplified)
        for i, event in enumerate(events[:5]):  # limit to 5 rows for brevity
            source = 'System Coordinator' if 'system_coordinator' in puml_name else 'Unknown'
            dest = 'Subsystem' if 'fsm' in puml_name else 'System Coordinator'
            condition = 'true'  # placeholder
            action = 'Transition'  # placeholder
            api_func = get_api_for_event(event, api_functions, puml_name)
            mapping_rows += f'''
                <tr>
                    <td>{event}</td>
                    <td>{source}</td>
                    <td>{dest}</td>
                    <td>{condition}</td>
                    <td>{action}</td>
                    <td><code>{api_func}</code></td>
                </tr>'''
    else:
        mapping_rows = '''
                <tr>
                    <td colspan="6">No mapping data extracted. Please check the PUML file.</td>
                </tr>'''
    
    # Generate API function list
    api_list = ''
    if api_functions:
        for func in api_functions:
            api_list += f'<li><code>{func}</code></li>\n'
    else:
        api_list = '<li>No API functions extracted.</li>'
    
    # Generate sequence diagram placeholder (optional)
    sequence_diagram = ''
    if 'system_coordinator' in puml_name:
        sequence_diagram = '''
        <h2>Sequence Diagram</h2>
        <p>Below is a sequence diagram illustrating the interaction between subsystems during a typical scenario.</p>
        <div class="diagram">
            <img src="system_coordinator_sequence.svg" alt="Sequence Diagram" style="max-width: 100%;">
        </div>
        <div class="note">
            <strong>Note:</strong> This sequence diagram is generated from <code>system_coordinator_sequence.puml</code>.
        </div>'''
    
    html = f'''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{diagram_title} - TWS Earphone System</title>
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
            border-bottom: 2px solid #0d6efd;
            padding-bottom: 10px;
        }}
        h2 {{
            color: #555;
        }}
        table {{
            border-collapse: collapse;
            width: 100%;
            margin-top: 20px;
        }}
        th, td {{
            border: 1px solid #ddd;
            padding: 8px;
            text-align: left;
        }}
        th {{
            background-color: #f2f2f2;
        }}
        code {{
            background-color: #f5f5f5;
            padding: 2px 4px;
            border-radius: 3px;
        }}
        pre {{
            background: #f0f0f0;
            padding: 10px;
            overflow: auto;
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
            background-color: #0d6efd;
            color: white;
            text-decoration: none;
            border-radius: 4px;
        }}
        .links a:hover {{
            background-color: #0b5ed7;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>{diagram_title} (with API Mapping)</h1>
        <p>{diagram_description}</p>
        
        <div class="diagram">
            {svg_content}
        </div>
        
        <div class="note">
            <strong>Note:</strong> This diagram is generated from <code>{puml_name}</code> using local PlantUML.
            No internet connectivity required.
        </div>
        
        <hr>
        
        <h2>Mapping Table</h2>
        <table>
            <thead>
                <tr>
                    <th>Event</th>
                    <th>Source Subsystem</th>
                    <th>Destination Subsystem</th>
                    <th>Condition</th>
                    <th>Action</th>
                    <th>API Function</th>
                </tr>
            </thead>
            <tbody>
                {mapping_rows}
        </tbody>
        </table>
        
        <!-- Hierarchical mapping table -->
        {generate_hierarchical_mapping_table(puml_file)}
        
        <hr>
        
        <h2>API Functions</h2>
        <ul>
            {api_list}
        </ul>
        
        {sequence_diagram}
        
        <hr>
        
        <div class="links">
            <a href="system_coordinator_state_diagram_with_api.html">System Coordinator FSM</a>
            <a href="recording_fsm_diagram_with_api.html">Recording FSM</a>
            <a href="audio_fsm_diagram_with_api.html">Audio FSM</a>
            <a href="comm_fsm_diagram_with_api.html">Communication FSM</a>
            <a href="power_fsm_diagram_with_api.html">Power FSM</a>
            <a href="ota_fsm_diagram_with_api.html">OTA FSM</a>
            <a href="efsm_protocol_diagram_with_api.html">EFSM Protocol</a>
            <a href="index.html">Index</a>
        </div>
    </div>
</body>
</html>'''
    
    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(html)
    
    print(f'Generated {html_file}')
    return True

def main():
    puml_files = glob.glob('*_diagram_with_api.puml')
    for puml in puml_files:
        html = puml.replace('.puml', '.html')
        print(f'Processing {puml} -> {html}')
        generate_html(puml, html)
    
    # Also generate for system_coordinator_with_all_subsystems.puml if exists
    if os.path.exists('system_coordinator_with_all_subsystems.puml'):
        generate_html('system_coordinator_with_all_subsystems.puml', 'system_coordinator_with_all_subsystems.html')
    
    print('All diagrams updated.')

if __name__ == '__main__':
    main()
