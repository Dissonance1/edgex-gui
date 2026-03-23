import re
import os

path = "/home/rog_strix/src/edgex-gui/src/AIRuntimeView.ui"

with open(path, 'r') as f:
    content = f.read()

# Define the new Metis Device row
metis_row = """                <item row="3" column="0">
                 <widget class="QLabel" name="lblMetisDevice">
                  <property name="text">
                   <string>Metis Device:</string>
                  </property>
                 </widget>
                </item>
                <item row="3" column="1">
                 <widget class="QComboBox" name="comboMetisDevice">
                  <item>
                   <property name="text">
                    <string>m2</string>
                   </property>
                  </item>
                  <item>
                   <property name="text">
                    <string>pcie</string>
                   </property>
                  </item>
                  <item>
                   <property name="text">
                    <string>auto</string>
                   </property>
                  </item>
                 </widget>
                </item>
"""

# Increment row numbers for items starting from row 3
def increment_rows(match):
    row_num = int(match.group(1))
    if row_num >= 3:
        return f'item row="{row_num + 1}"'
    return match.group(0)

# Shift existing rows 3 and 4 down
content = re.sub(r'item row="(\d+)"', increment_rows, content)

# Insert the new Metis row (row 3) after the item row="2" block ends
# Row 2 column 1 is comboDisplayMode. It ends with </item> after the </widget>
# Let's find the position after row 2 items.

pattern = r'(<item row="2" column="1">.*?</item>)'
match = re.search(pattern, content, re.DOTALL)

if match:
    insert_pos = match.end()
    # Insert newline and the new metis_row
    content = content[:insert_pos] + "\n" + metis_row + content[insert_pos:]
    with open(path, 'w') as f:
        f.write(content)
    print("SUCCESS: UI refactored with regex.")
else:
    print("FAILURE: Could not find Row 2 Column 1 to insert after.")
