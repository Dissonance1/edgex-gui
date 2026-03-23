import xml.etree.ElementTree as ET
import sys
import os

ui_file = 'src/AIRuntimeView.ui'

if not os.path.exists(ui_file):
    print(f"Error: {ui_file} not found in {os.getcwd()}")
    sys.exit(1)

try:
    tree = ET.parse(ui_file)
    root = tree.getroot()

    # Find the formModel layout
    form_model = None
    for layout in root.iter('layout'):
        if layout.get('name') == 'formModel':
            form_model = layout
            break

    if form_model is not None:
        # Check if already added
        if not any(widget.get('name') == 'editClassMapPath' for widget in form_model.iter('widget')):
            # Row 4: Class Map
            item_lbl = ET.SubElement(form_model, 'item', {'row': '4', 'column': '0'})
            lbl = ET.SubElement(item_lbl, 'widget', {'class': 'QLabel', 'name': 'lblClassMap'})
            prop_text = ET.SubElement(lbl, 'property', {'name': 'text'})
            ET.SubElement(prop_text, 'string').text = 'Class Map (XML/JSON):'
            
            item_field = ET.SubElement(form_model, 'item', {'row': '4', 'column': '1'})
            lay = ET.SubElement(item_field, 'layout', {'class': 'QHBoxLayout', 'name': 'classMapLayout'})
            it_edit = ET.SubElement(lay, 'item')
            edit = ET.SubElement(it_edit, 'widget', {'class': 'QLineEdit', 'name': 'editClassMapPath'})
            ET.SubElement(edit, 'property', {'name': 'placeholderText'}).append(ET.fromstring('<string>Path to class_map...</string>'))
            it_btn = ET.SubElement(lay, 'item')
            btn = ET.SubElement(it_btn, 'widget', {'class': 'QPushButton', 'name': 'btnBrowseClassMap'})
            ET.SubElement(btn, 'property', {'name': 'text'}).append(ET.fromstring('<string>Browse...</string>'))

        if not any(widget.get('name') == 'editEmbeddingPath' for widget in form_model.iter('widget')):
            # Row 5: Embedding JSON
            item_lbl = ET.SubElement(form_model, 'item', {'row': '5', 'column': '0'})
            lbl = ET.SubElement(item_lbl, 'widget', {'class': 'QLabel', 'name': 'lblEmbedding'})
            prop_text = ET.SubElement(lbl, 'property', {'name': 'text'})
            ET.SubElement(prop_text, 'string').text = 'Embedding JSON:'
            
            item_field = ET.SubElement(form_model, 'item', {'row': '5', 'column': '1'})
            lay = ET.SubElement(item_field, 'layout', {'class': 'QHBoxLayout', 'name': 'embeddingLayout'})
            it_edit = ET.SubElement(lay, 'item')
            edit = ET.SubElement(it_edit, 'widget', {'class': 'QLineEdit', 'name': 'editEmbeddingPath'})
            ET.SubElement(edit, 'property', {'name': 'placeholderText'}).append(ET.fromstring('<string>Path to embedding.json...</string>'))
            it_btn = ET.SubElement(lay, 'item')
            btn = ET.SubElement(it_btn, 'widget', {'class': 'QPushButton', 'name': 'btnBrowseEmbedding'})
            ET.SubElement(btn, 'property', {'name': 'text'}).append(ET.fromstring('<string>Browse...</string>'))

    # Also apply the vertical layout polishing again just in case (as previous script might have been overwritten)
    group_profiles = None
    for widget in root.iter('widget'):
        if widget.get('name') == 'groupProfiles':
            group_profiles = widget
            break

    if group_profiles is not None:
        for prop in group_profiles.findall('property'):
            if prop.get('name') == 'maximumSize':
                size = prop.find('size')
                size.find('width').text = '380'

    # Write back
    tree.write(ui_file, encoding='utf-8', xml_declaration=True)
    print("Successfully added metadata fields to AIRuntimeView.ui")

except Exception as e:
    import traceback
    traceback.print_exc()
    sys.exit(1)
