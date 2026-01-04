#!/usr/bin/env python3
"""
Parse PlantUML (.puml) files to extract state transitions and generate JSON mapping data.
"""

import re
import json
import os
import sys

def parse_transitions(puml_content):
    """
    Parse transitions from PlantUML content.
    Returns a list of dictionaries, each representing a transition.
    """
    transitions = []
    lines = puml_content.split('\n')
    
    # Patterns for transition lines
    # 1. Standard transition: STATE --> STATE : EVENT
    # 2. Self-transition: STATE --> STATE : EVENT
    # 3. Transition with condition: STATE --> STATE : EVENT (condition)
    # 4. Transition with description (no event): STATE --> STATE : Description
    # We'll capture source, target, and label.
    pattern = r'^\s*([A-Za-z0-9_]+)\s*-->\s*([A-Za-z0-9_]+)\s*:\s*(.+?)\s*(?:\((.+?)\))?\s*$'
    
    for line in lines:
        line = line.strip()
        # Skip comments and empty lines
        if line.startswith("'") or line.startswith("@") or not line:
            continue
        
        # Try to match transition pattern
        match = re.match(pattern, line)
        if match:
            source = match.group(1)
            target = match.group(2)
            label = match.group(3).strip()
            condition = match.group(4) if match.group(4) else ""
            
            # Determine if label is an event (contains SYS_EVT_ or uppercase with underscores)
            event = ""
            description = ""
            if re.match(r'^[A-Z][A-Z0-9_]+$', label) and '_' in label:
                event = label
                description = ""
            else:
                event = ""
                description = label
            
            transition = {
                "source": source,
                "target": target,
                "event": event,
                "description": description,
                "condition": condition,
                "label": label
            }
            transitions.append(transition)
    
    return transitions

def parse_states(puml_content):
    """
    Extract state names from PlantUML content.
    Returns a list of state names.
    """
    states = []
    # Pattern: state "STATE_NAME" as ALIAS
    pattern = r'state\s+"([^"]+)"\s+as\s+([A-Za-z0-9_]+)'
    matches = re.findall(pattern, puml_content, re.IGNORECASE)
    for state_name, alias in matches:
        states.append({
            "name": state_name,
            "alias": alias
        })
    return states

def parse_notes(puml_content):
    """
    Extract notes from PlantUML content, especially API functions.
    Returns a dictionary mapping state alias to note text.
    """
    notes = {}
    # Pattern: note right of STATE
    # We'll do a simple extraction for now.
    lines = puml_content.split('\n')
    current_note_state = None
    in_note = False
    note_lines = []
    for line in lines:
        if line.strip().startswith('note right of'):
            # Start of a note
            match = re.match(r'note right of\s+([A-Za-z0-9_]+)', line.strip())
            if match:
                current_note_state = match.group(1)
                in_note = True
                note_lines = []
        elif line.strip() == 'end note':
            if current_note_state and in_note:
                notes[current_note_state] = '\n'.join(note_lines)
                current_note_state = None
                in_note = False
        elif in_note:
            note_lines.append(line.strip())
    return notes

def generate_mapping_data(puml_file):
    """
    Generate mapping data from a PUML file.
    Returns a dictionary suitable for JSON export.
    """
    with open(puml_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    transitions = parse_transitions(content)
    states = parse_states(content)
    notes = parse_notes(content)
    
    # Build mapping data
    mapping = {
        "puml_file": os.path.basename(puml_file),
        "states": states,
        "transitions": [],
        "notes": notes
    }
    
    # Enrich transitions with additional info
    for i, trans in enumerate(transitions):
        # Determine API function from notes (simplistic)
        api_func = ""
        if trans["target"] in notes:
            note = notes[trans["target"]]
            # Look for API: pattern
            api_match = re.search(r'API:\s*([a-zA-Z0-9_]+\(\))', note)
            if api_match:
                api_func = api_match.group(1)
        
        # Determine subsystems from notes (simplistic)
        subsystems = []
        if trans["target"] in notes:
            note = notes[trans["target"]]
            # Look for Subsystems: pattern
            subsys_match = re.search(r'Subsystems:\s*(.+)', note)
            if subsys_match:
                subsys_text = subsys_match.group(1)
                # Extract links [[...]]
                links = re.findall(r'\[\[([^\]]+)\]\]', subsys_text)
                subsystems = links
        
        # Create enriched transition
        enriched = {
            "id": f"T{i+1:03d}",
            "source_state": trans["source"],
            "target_state": trans["target"],
            "event": trans["event"],
            "description": trans["description"],
            "condition": trans["condition"],
            "api_function": api_func,
            "subsystems": subsystems,
            "role_specific": {
                "developer": {
                    "entry_action": "",
                    "exit_action": "",
                    "parameters": "",
                    "return_value": ""
                },
                "tester": {
                    "test_case_id": "",
                    "expected_outcome": "",
                    "tested": False,
                    "error_codes": []
                },
                "architect": {
                    "event_type": "external",
                    "handling": "forwarded",
                    "performance_impact": "low",
                    "dependencies": []
                },
                "product": {
                    "user_scenario": "",
                    "feature": "",
                    "priority": ""
                },
                "support": {
                    "symptom": "",
                    "recovery_steps": [],
                    "log_keywords": []
                }
            }
        }
        mapping["transitions"].append(enriched)
    
    return mapping

def main():
    if len(sys.argv) < 2:
        print("Usage: python parse_puml.py <puml_file> [output_json]")
        sys.exit(1)
    
    puml_file = sys.argv[1]
    if not os.path.exists(puml_file):
        print(f"Error: File {puml_file} not found.")
        sys.exit(1)
    
    output_json = sys.argv[2] if len(sys.argv) > 2 else "mapping_data.json"
    
    mapping = generate_mapping_data(puml_file)
    
    with open(output_json, 'w', encoding='utf-8') as f:
        json.dump(mapping, f, indent=2, ensure_ascii=False)
    
    print(f"Generated {output_json} with {len(mapping['transitions'])} transitions.")

if __name__ == '__main__':
    main()
