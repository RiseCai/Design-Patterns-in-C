import os
import json
from jinja2 import Environment, FileSystemLoader

class DDDFrameworkGenerator:
    def __init__(self):
        self.template_dir = os.path.join(os.path.dirname(__file__), 'templates')
        self.env = Environment(loader=FileSystemLoader(self.template_dir))
        
    def generate_file(self, template_name, output_path, context={}):
        template = self.env.get_template(template_name)
        content = template.render(context)
        
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(content)

    def generate_entity(self, output_dir):
        self.generate_file('entity.h.template', 
                         os.path.join(output_dir, 'entity.h'))
        self.generate_file('entity.c.template',
                         os.path.join(output_dir, 'entity.c'))

    def generate_value_object(self, output_dir):
        self.generate_file('value_object.h.template',
                          os.path.join(output_dir, 'value_object.h'))

    def generate_aggregate(self, output_dir):
        self.generate_file('aggregate.h.template',
                         os.path.join(output_dir, 'aggregate.h'))
    
    def generate_platform_abstraction(self, output_dir):
        self.generate_file('platform_abstraction.h.template',
                         os.path.join(output_dir, 'platform_abstraction.h'))
    
    def generate_async_handler(self, output_dir):
        self.generate_file('async_handler.h.template',
                         os.path.join(output_dir, 'async_handler.h'))
        self.generate_file('aggregate.c.template',
                         os.path.join(output_dir, 'aggregate.c'))
    
    def generate_task_system(self, output_dir):
        self.generate_file('task_specification.h.template',
                         os.path.join(output_dir, 'task_specification.h'))
        self.generate_file('task_executor.h.template',
                         os.path.join(output_dir, 'task_executor.h'))
    
    def generate_domain_models(self, output_dir):
        # Power domain
        os.makedirs(os.path.join(output_dir, 'power'), exist_ok=True)
        self.generate_file('domains/power/charging_template.h.template',
                         os.path.join(output_dir, 'power/charging_template.h'))
        
        # Connectivity domain
        os.makedirs(os.path.join(output_dir, 'connectivity'), exist_ok=True)
        self.generate_file('domains/connectivity/transport_evaluator.h.template',
                         os.path.join(output_dir, 'connectivity/transport_evaluator.h'))
        
        # Learning module
        os.makedirs(os.path.join(output_dir, 'learning'), exist_ok=True)
        self.generate_file('domains/learning/learning_module.h.template',
                         os.path.join(output_dir, 'learning/learning_module.h'))
        
        # Media domain
        os.makedirs(os.path.join(output_dir, 'media'), exist_ok=True)
        self.generate_file('domains/media/audio_processor.h.template',
                         os.path.join(output_dir, 'media/audio_processor.h'))
        
        # Domain analysis
        self.generate_file('domain_analysis.template',
                         os.path.join(output_dir, 'domain_analysis.h'))

if __name__ == "__main__":
    generator = DDDFrameworkGenerator()
    generator.generate_entity('./output')
    generator.generate_value_object('./output')
    generator.generate_aggregate('./output')
    generator.generate_platform_abstraction('./output')
    generator.generate_async_handler('./output')
    generator.generate_task_system('./output')
    generator.generate_domain_models('./output')
