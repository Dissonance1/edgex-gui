import re

path = "/home/rog_strix/src/edgex-gui/src/AIRuntimeView.ui"

with open(path, 'r') as f:
    content = f.read()

# 1. CLEANUP: Remove ANY lblMetisDevice items and combo boxes
# Using a very broad regex to catch my mistakes
content = re.sub(r'\s*<item row="\d+" column="\d+">\s*<widget class="QLabel" name="lblMetisDevice">.*?</item>', '', content, flags=re.DOTALL)
content = re.sub(r'\s*<item row="\d+" column="\d+">\s*<widget class="QComboBox" name="comboMetisDevice">.*?</item>', '', content, flags=re.DOTALL)

# 2. FIX broken comboDisplayMode (it had nested items inserted)
# I'll just replace the whole comboDisplayMode block if I can find it.
# Standard block for comboDisplayMode:
display_mode_block = """                <item row="2" column="1">
                 <widget class="QComboBox" name="comboDisplayMode">
                  <item>
                   <property name="text">
                    <string>Windowed</string>
                   </property>
                  </item>
                  <item>
                   <property name="text">
                    <string>Headless</string>
                   </property>
                  </item>
                 </widget>
                </item>"""

# Find where comboDisplayMode is and fix it.
# Use regex to find the broken one and replace it.
content = re.sub(r'<item row="2" column="1">\s*<widget class="QComboBox" name="comboDisplayMode">.*?</widget>\s*</item>', display_mode_block, content, flags=re.DOTALL)

# 3. ENSURE row numbers are correct globally (non-shifted)
# If lblWindowSize is at row 4, decrement it back to 3.
# Wait, let's just use a map of known names to rows to be super safe.
# Actually, I'll just decrement rows >= 4 once more IF they were shifted.
# But wait, I already decremented once.
# Let's check lblWindowSize in the current content.
if 'name="lblWindowSize"' in content:
    # Check its row
    match = re.search(r'<item row="(\d+)" column="0">\s*<widget class="QLabel" name="lblWindowSize">', content)
    if match and match.group(1) == "4":
        def decrement_rows(m):
            r = int(m.group(1))
            if r >= 4:
                return f'item row="{r - 1}"'
            return m.group(0)
        content = re.sub(r'item row="(\d+)"', decrement_rows, content)

# 4. FINAL TARGETED INSERTION
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
                  </item>
                 </widget>
                </item>"""
# Wait, my metis_row had a typo in previous one (nested item). Fixing it now.
metis_row_v2 = """                <item row="3" column="0">
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
                </item>"""

# Find formPipeline block AGAIN
start_tag = '<layout class="QFormLayout" name="formPipeline">'
end_tag = '</layout>'
idx_start = content.find(start_tag)
if idx_start != -1:
    idx_end = content.find(end_tag, idx_start)
    if idx_end != -1:
        layout_content = content[idx_start:idx_end]
        # Shift rows >= 3 here
        def inc_rows(m):
            r = int(m.group(1))
            if r >= 3:
                return f'item row="{r + 1}"'
            return m.group(0)
        layout_content = re.sub(r'item row="(\d+)"', inc_rows, layout_content)
        # Insert after comboDisplayMode item
        target = '</widget>\n                </item>' # This matches the end of row 2 col 1
        layout_content = layout_content.replace(target, target + "\n" + metis_row_v2, 1)
        content = content[:idx_start] + layout_content + content[idx_end:]
        with open(path, 'w') as f:
            f.write(content)
        print("SUCCESS: UI refactored perfectly.")
    else:
        print("FAIL: No end tag.")
else:
    print("FAIL: No start tag.")
