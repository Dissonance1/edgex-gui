#!/usr/bin/env python3
import os
import re

src_dir = '/home/amit/src/edgex/edgex-gui/src'

for filename in os.listdir(src_dir):
    if filename.endswith(('.h', '.cpp')):
        filepath = os.path.join(src_dir, filename)
        with open(filepath, 'r') as f:
            content = f.read()
        
        # Replace folder prefixes in includes
        content = re.sub(r'#include "core/', '#include "', content)
        content = re.sub(r'#include "models/', '#include "', content)
        content = re.sub(r'#include "ui/', '#include "', content)
        content = re.sub(r'#include "app/', '#include "', content)
        
        with open(filepath, 'w') as f:
            f.write(content)

print("Include paths updated successfully!")
