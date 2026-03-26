import re

path = "/home/rog_strix/src/edgex-gui/src/AIRuntimeView.ui"

with open(path, 'r') as f:
    content = f.read()

# 1. Remove all lblMetisDevice items (and their combo boxes)
# These are the ones I inserted.
# They look like:
#                <item row="3" column="0">
#                 <widget class="QLabel" name="lblMetisDevice">
#                  ...
#                 </item>
#                 <item row="3" column="1">
#                  <widget class="QComboBox" name="comboMetisDevice">
#                  ...
#                 </item>

# Use a regex to find and remove them.
content = re.sub(r'\s*<item row="\d+" column="\d+">\s*<widget class="QL[ab]+el" name="lblMetisDevice">.*?</item>', '', content, flags=re.DOTALL)
content = re.sub(r'\s*<item row="\d+" column="\d+">\s*<widget class="QComboBox" name="comboMetisDevice">.*?</item>', '', content, flags=re.DOTALL)

# 2. Decrement row numbers that were shifted (row >= 4)
def decrement_rows(match):
    row_num = int(match.group(1))
    if row_num >= 4:
        return f'item row="{row_num - 1}"'
    return match.group(0)

content = re.sub(r'item row="(\d+)"', decrement_rows, content)

# 3. Now perform the TARGETED insertion in formPipeline
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

# Increment row numbers ONLY inside formPipeline starting from row 3
# Find the formPipeline layout block
start_tag = '<layout class="QFormLayout" name="formPipeline">'
end_tag = '</layout>'

idx_start = content.find(start_tag)
if idx_start != -1:
    idx_end = content.find(end_tag, idx_start)
    if idx_end != -1:
        layout_content = content[idx_start:idx_end]
        
        # Increment rows >= 3 in this snippet
        def inc_only_here(match):
            row_num = int(match.group(1))
            if row_num >= 3:
                return f'item row="{row_num + 1}"'
            return match.group(0)
            
        layout_content = re.sub(r'item row="(\d+)"', inc_only_here, layout_content)
        
        # Find row 2 column 1 to insert after
        match_row2 = re.search(r'(<item row="2" column="1">.*?</item>)', layout_content, re.DOTALL)
        if match_row2:
            pos = match_row2.end()
            layout_content = layout_content[:pos] + "\n" + metis_row + layout_content[pos:]
            
            # Reconstruct the file
            content = content[:idx_start] + layout_content + content[idx_end:]
            with open(path, 'w') as f:
                f.write(content)
            print("SUCCESS: UI fixed and refactored correctly.")
        else:
            print("FAILURE: Row 2 Column 1 not found in formPipeline.")
    else:
        print("FAILURE: End tag for formPipeline not found.")
else:
    print("FAILURE: Start tag for formPipeline not found.")
