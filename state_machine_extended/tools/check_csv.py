import csv
with open('state_machine_mapping_requirements.csv', 'r', encoding='utf-8') as f:
    reader = csv.reader(f)
    for i, row in enumerate(reader):
        print(i, len(row), row)
