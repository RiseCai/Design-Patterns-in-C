#!/usr/bin/env python3
"""
Generate Excel workbook with state machine mapping tables.
Creates a single .xlsx file with multiple sheets for bidirectional mapping.
"""
import pandas as pd
import os

def main():
    base_dir = os.path.dirname(__file__)
    output_path = os.path.join(base_dir, "state_machine_mapping_workbook.xlsx")
    
    # Load CSV files
    mapping_csv = os.path.join(base_dir, "state_machine_mapping.csv")
    requirements_csv = os.path.join(base_dir, "state_machine_mapping_requirements.csv")
    bidirectional_csv = os.path.join(base_dir, "state_machine_bidirectional_mapping.csv")
    config_json = os.path.join(base_dir, "fsm_config_mapping.json")
    
    # Create Excel writer
    with pd.ExcelWriter(output_path, engine='openpyxl') as writer:
        # Sheet 1: Scenario mapping (state_machine_mapping.csv)
        if os.path.exists(mapping_csv):
            df_mapping = pd.read_csv(mapping_csv)
            df_mapping.to_excel(writer, sheet_name='Scenario Mapping', index=False)
        
        # Sheet 2: Requirements (state_machine_mapping_requirements.csv)
        if os.path.exists(requirements_csv):
            df_req = pd.read_csv(requirements_csv)
            df_req.to_excel(writer, sheet_name='Requirements', index=False)
        
        # Sheet 3: Bidirectional mapping (state_machine_bidirectional_mapping.csv)
        if os.path.exists(bidirectional_csv):
            df_bidi = pd.read_csv(bidirectional_csv)
            df_bidi.to_excel(writer, sheet_name='Bidirectional Mapping', index=False)
        
        # Sheet 4: JSON config summary (extract key fields)
        if os.path.exists(config_json):
            import json
            with open(config_json, 'r', encoding='utf-8') as f:
                config = json.load(f)
            # Flatten scenarios into a table
            scenarios = config.get('scenarios', [])
            rows = []
            for s in scenarios:
                rows.append({
                    'ID': s.get('id'),
                    'Scenario': s.get('scenario'),
                    'FSM Type': s.get('recommended_fsm_type'),
                    'Subtype': s.get('subtype'),
                    'Key Features': ', '.join(s.get('key_features', [])),
                    'API Functions': ', '.join(s.get('api_functions', [])),
                    'Code Files': ', '.join(s.get('code_files', [])),
                    'Mapping Level': s.get('mapping_level'),
                    'Priority': s.get('priority'),
                    'Notes': s.get('notes')
                })
            df_config = pd.DataFrame(rows)
            df_config.to_excel(writer, sheet_name='Config Summary', index=False)
        
        # Sheet 5: Mapping levels description
        mapping_levels = config.get('mapping_levels', {})
        df_levels = pd.DataFrame([
            {'Level': level, 'Description': desc}
            for level, desc in mapping_levels.items()
        ])
        df_levels.to_excel(writer, sheet_name='Mapping Levels', index=False)
        
        # Sheet 6: Bidirectional mapping matrix (type <-> code)
        if 'bidirectional_mapping' in config:
            bidi = config['bidirectional_mapping']
            # Type to code
            type_to_code = bidi.get('fsm_type_to_code', {})
            rows = []
            for fsm_type, files in type_to_code.items():
                rows.append({
                    'FSM Type': fsm_type,
                    'Code Files': ', '.join(files),
                    'Direction': 'Type → Code'
                })
            # Code to type
            code_to_type = bidi.get('code_to_fsm_type', {})
            for code_file, fsm_type in code_to_type.items():
                rows.append({
                    'FSM Type': fsm_type,
                    'Code Files': code_file,
                    'Direction': 'Code → Type'
                })
            df_matrix = pd.DataFrame(rows)
            df_matrix.to_excel(writer, sheet_name='Bidirectional Matrix', index=False)
    
    print(f"Excel workbook generated: {output_path}")
    print(f"Size: {os.path.getsize(output_path)} bytes")

if __name__ == '__main__':
    main()
