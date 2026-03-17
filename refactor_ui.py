import xml.etree.ElementTree as ET
import os

ui_file = r"\\wsl.localhost\Ubuntu\home\rog_strix\src\edgex-gui\src\AIRuntimeView.ui"

# Register namespaces to preserve them
ET.register_namespace('', "http://www.qt.io/struct")

tree = ET.parse(ui_file)
root = tree.getroot()

# Find the groupModelFile
group_model = root.find(".//widget[@name='groupModelFile']")
if group_model is not None:
    # Update title
    title_prop = group_model.find("./property[@name='title']/string")
    if title_prop is not None:
        title_prop.text = "Model Configuration (YAML / Zoo)"
    
    # Get the layout
    layout = group_model.find("./layout[@name='formModel']")
    if layout is not None:
        # Clear the layout and rebuild it
        for item in list(layout):
            layout.remove(item)
        
        # Row 0: Model Zoo
        item0_lbl = ET.SubElement(layout, "item", row="0", column="0")
        lbl_zoo = ET.SubElement(item0_lbl, "widget", {"class": "QLabel", "name": "lblModelZoo"})
        ET.SubElement(lbl_zoo, "property", name="text").append(ET.fromstring("<string>Select Model (Zoo):</string>"))
        
        item0_val = ET.SubElement(layout, "item", row="0", column="1")
        combo_zoo = ET.SubElement(item0_val, "widget", {"class": "QComboBox", "name": "comboModelZoo"})
        ET.SubElement(combo_zoo, "property", name="editable").append(ET.fromstring("<bool>true</bool>"))
        ET.SubElement(combo_zoo, "property", name="placeholderText").append(ET.fromstring("<string>Loading Model Zoo...</string>"))
        
        # Row 1: Custom Model
        item1_lbl = ET.SubElement(layout, "item", row="1", column="0")
        lbl_custom = ET.SubElement(item1_lbl, "widget", {"class": "QLabel", "name": "lblCustomModel"})
        ET.SubElement(lbl_custom, "property", name="text").append(ET.fromstring("<string>Custom YAML:</string>"))
        
        item1_val = ET.SubElement(layout, "item", row="1", column="1")
        h_layout = ET.SubElement(item1_val, "layout", {"class": "QHBoxLayout", "name": "customModelLayout"})
        
        h_item_edit = ET.SubElement(h_layout, "item")
        edit_path = ET.SubElement(h_item_edit, "widget", {"class": "QLineEdit", "name": "editModelPath"})
        ET.SubElement(edit_path, "property", name="readOnly").append(ET.fromstring("<bool>true</bool>"))
        ET.SubElement(edit_path, "property", name="placeholderText").append(ET.fromstring("<string>Path to custom model.yaml...</string>"))
        
        h_item_btn = ET.SubElement(h_layout, "item")
        btn_browse = ET.SubElement(h_item_btn, "widget", {"class": "QPushButton", "name": "btnBrowseModelYaml"})
        ET.SubElement(btn_browse, "property", name="text").append(ET.fromstring("<string>Browse...</string>"))
        
        # Row 2: Model Info
        item2_lbl = ET.SubElement(layout, "item", row="2", column="0")
        lbl_info_title = ET.SubElement(item2_lbl, "widget", {"class": "QLabel", "name": "lblModelInfoTitle"})
        ET.SubElement(lbl_info_title, "property", name="text").append(ET.fromstring("<string>Model Info:</string>"))
        
        item2_val = ET.SubElement(layout, "item", row="2", column="1")
        lbl_info = ET.SubElement(item2_val, "widget", {"class": "QLabel", "name": "lblModelInfo"})
        ET.SubElement(lbl_info, "property", name="text").append(ET.fromstring("<string>Select a model to see details.</string>"))
        ET.SubElement(lbl_info, "property", name="wordWrap").append(ET.fromstring("<bool>true</bool>"))
        ET.SubElement(lbl_info, "property", name="styleSheet").append(ET.fromstring("<string notr=\"true\">color: #6c757d; font-style: italic;</string>"))
        
        # Row 3: Confidence
        item3_lbl = ET.SubElement(layout, "item", row="3", column="0")
        lbl_conf = ET.SubElement(item3_lbl, "widget", {"class": "QLabel", "name": "lblConfidence"})
        ET.SubElement(lbl_conf, "property", name="text").append(ET.fromstring("<string>Confidence Threshold:</string>"))
        
        item3_val = ET.SubElement(layout, "item", row="3", column="1")
        spin_conf = ET.SubElement(item3_val, "widget", {"class": "QDoubleSpinBox", "name": "spinConfidence"})
        ET.SubElement(spin_conf, "property", name="value").append(ET.fromstring("<number>70</number>"))

# Find groupAIPU and add status indicators
group_aipu = root.find(".//widget[@name='groupAIPU']")
if group_aipu is not None:
    # Update title
    title_prop = group_aipu.find("./property[@name='title']/string")
    if title_prop is not None:
        title_prop.text = "AIPU Core Management & Live Status"
    
    layout = group_aipu.find("./layout[@name='verticalLayout_6']")
    if layout is not None:
        # Insert status layout at the top
        status_item = ET.Element("item")
        status_layout = ET.SubElement(status_item, "layout", {"class": "QHBoxLayout", "name": "aipuStatusLayout"})
        
        for i in range(4):
            core_item = ET.SubElement(status_layout, "item")
            core_grp = ET.SubElement(core_item, "widget", {"class": "QGroupBox", "name": f"core{i}Status"})
            ET.SubElement(core_grp, "property", name="title").append(ET.fromstring(f"<string>Core {i}</string>"))
            c_layout = ET.SubElement(core_grp, "layout", {"class": "QVBoxLayout", "name": f"c{i}Layout"})
            c_item = ET.SubElement(c_layout, "item")
            lbl_state = ET.SubElement(c_item, "widget", {"class": "QLabel", "name": f"lblCore{i}State"})
            ET.SubElement(lbl_state, "property", name="text").append(ET.fromstring("<string>Idle</string>"))
            ET.SubElement(lbl_state, "property", name="alignment").append(ET.fromstring("<enum>Qt::AlignCenter</enum>"))
            
        layout.insert(0, status_item)

tree.write("refactored_ui.ui", encoding="utf-8", xml_declaration=True)
print("UI refactored successfully.")
