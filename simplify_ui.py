import re
import os

path = "/home/rog_strix/src/edgex-gui/src/AIRuntimeView.ui"

with open(path, 'r') as f:
    content = f.read()

# 1. Remove lblMetisDevice, comboMetisDevice, lblWindowSize, editWindowSize
# These are within <layout class="QFormLayout" name="formPipeline">
# We'll remove their <item> blocks.

# Remove Metis Device (row 3)
content = re.sub(r'\s*<item row="3" column="[01]">.*?lblMetisDevice.*?</item>', '', content, flags=re.DOTALL)
content = re.sub(r'\s*<item row="3" column="[01]">.*?comboMetisDevice.*?</item>', '', content, flags=re.DOTALL)

# Remove Window Size (row 4)
content = re.sub(r'\s*<item row="4" column="[01]">.*?lblWindowSize.*?</item>', '', content, flags=re.DOTALL)
content = re.sub(r'\s*<item row="4" column="[01]">.*?editWindowSize.*?</item>', '', content, flags=re.DOTALL)

# 2. Shift row 5 (Save Output Path) up to row 3
content = content.replace('item row="5" column="0"', 'item row="3" column="0"')
content = content.replace('item row="5" column="1"', 'item row="3" column="1"')

# 3. Remove groupHwAccel entirely
content = re.sub(r'\s*<item>\s*<widget class="QGroupBox" name="groupHwAccel">.*?</widget>\s*</item>', '', content, flags=re.DOTALL)

with open(path, 'w') as f:
    f.write(content)

print("SUCCESS: UI simplified.")
