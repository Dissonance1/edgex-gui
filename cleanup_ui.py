import xml.etree.ElementTree as ET
import os

ui_file = r"\\wsl.localhost\Ubuntu\home\rog_strix\src\edgex-gui\src\AIRuntimeView.ui"

# Register namespaces
ET.register_namespace('', "http://www.qt.io/struct")

tree = ET.parse(ui_file)
root = tree.getroot()

# Find the verticalLayout_4 where profiles are inserted
v_layout_4 = root.find(".//layout[@name='verticalLayout_4']")

if v_layout_4 is not None:
    # 1. Purge ANY item that contains a widget named 'groupProfiles'
    # or ANY element that is literally <QLineEdit ...> (malformed ones)
    to_remove = []
    for item in v_layout_4.findall("item"):
        # Check for widget name
        widget = item.find("widget[@name='groupProfiles']")
        if widget is not None:
            to_remove.append(item)
            continue
        
        # Check for malformed QLineEdit tag directly under item/layout
        # (The malformed ones might be inside layouts we added)
        bad_le = item.find(".//QLineEdit")
        if bad_le is not None:
            to_remove.append(item)
            continue

    for item in to_remove:
        v_layout_4.remove(item)
        print("Removed a duplicate or malformed profile item.")

# Save the cleaned file
tree.write("cleaned_ui.ui", encoding="utf-8", xml_declaration=True)
print("UI cleaned successfully.")
