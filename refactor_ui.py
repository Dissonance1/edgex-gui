import os

path = "/home/rog_strix/src/edgex-gui/src/AIRuntimeView.ui"

with open(path, 'r') as f:
    content = f.read()

# Target block for row 3 column 0
target_lbl = '<item row="3" column="0">\n                 <widget class="QLabel" name="lblWindowSize">'
# Target block for row 3 column 1
target_edit = '<item row="3" column="1">\n                 <widget class="QLineEdit" name="editWindowSize">'
# Target block for row 4 column 0
target_lbl_save = '<item row="4" column="0">\n                 <widget class="QLabel" name="lblSavePath">'
# Target block for row 4 column 1
target_lay_save = '<item row="4" column="1">\n                 <layout class="QHBoxLayout" name="horizontalLayout_7">'

# The new Metis Device row
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
                </item>\n"""

# Perform replacements
# We first shift row 4 to row 5
content = content.replace(target_lbl_save, '<item row="5" column="0">\n                 <widget class="QLabel" name="lblSavePath">')
content = content.replace(target_lay_save, '<item row="5" column="1">\n                 <layout class="QHBoxLayout" name="horizontalLayout_7">')

# Then we shift row 3 to row 4
content = content.replace(target_lbl, '<item row="4" column="0">\n                 <widget class="QLabel" name="lblWindowSize">')
content = content.replace(target_edit, '<item row="4" column="1">\n                 <widget class="QLineEdit" name="editWindowSize">')

# Now we insert the new Metis row before row 4
if '<item row="4" column="0">' in content:
    content = content.replace('<item row="4" column="0">', metis_row + '                <item row="4" column="0">', 1)
    with open(path, 'w') as f:
        f.write(content)
    print("SUCCESS: UI refactored successfully.")
else:
    print("FAILURE: Could not find anchor for insertion.")
