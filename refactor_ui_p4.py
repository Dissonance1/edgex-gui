import xml.etree.ElementTree as ET
import os

ui_file = r"\\wsl.localhost\Ubuntu\home\rog_strix\src\edgex-gui\src\AIRuntimeView.ui"

# Register namespaces to preserve them
ET.register_namespace('', "http://www.qt.io/struct")

tree = ET.parse(ui_file)
root = tree.getroot()

# 1. Add Profile Management Group to modelPage (top of verticalLayout_4)
v_layout_4 = root.find(".//layout[@name='verticalLayout_4']")
if v_layout_4 is not None:
    # Create the GroupBox
    grp_profiles = ET.Element("item")
    widget_grp = ET.SubElement(grp_profiles, "widget", {"class": "QGroupBox", "name": "groupProfiles"})
    ET.SubElement(widget_grp, "property", name="title").append(ET.fromstring("<string>Inference Profiles (Reuse Settings)</string>"))
    
    # Layout for the GroupBox
    p_layout = ET.SubElement(widget_grp, "layout", {"class": "QVBoxLayout", "name": "profileVLayout"})
    
    # Selection Row
    h_row1 = ET.SubElement(p_layout, "item")
    h_layout1 = ET.SubElement(h_row1, "layout", {"class": "QHBoxLayout", "name": "profileSelectLayout"})
    
    lbl_sel = ET.SubElement(h_layout1, "item")
    lbl_sel_w = ET.SubElement(lbl_sel, "widget", {"class": "QLabel", "name": "lblSelectProfile"})
    ET.SubElement(lbl_sel_w, "property", name="text").append(ET.fromstring("<string>Select Profile:</string>"))
    
    combo_p = ET.SubElement(h_layout1, "item")
    combo_p_w = ET.SubElement(combo_p, "widget", {"class": "QComboBox", "name": "comboProfiles"})
    
    # Name/Action Row
    h_row2 = ET.SubElement(p_layout, "item")
    h_layout2 = ET.SubElement(h_row2, "layout", {"class": "QHBoxLayout", "name": "profileActionLayout"})
    
    edit_n = ET.SubElement(h_layout2, "item")
    edit_n_w = ET.SubElement(edit_n, "widget", {"class": "QLineEdit", "name": "editProfileName"})
    ET.SubElement(edit_n_w, "property", name="placeholderText").append(ET.fromstring("<string>Profile Name...</string>"))
    
    btn_new = ET.SubElement(h_layout2, "item")
    btn_new_w = ET.SubElement(btn_new, "widget", {"class": "QPushButton", "name": "btnNewProfile"})
    ET.SubElement(btn_new_w, "property", name="text").append(ET.fromstring("<string>New</string>"))
    
    btn_save = ET.SubElement(h_layout2, "item")
    btn_save_w = ET.SubElement(btn_save, "widget", {"class": "QPushButton", "name": "btnSaveProfile"})
    ET.SubElement(btn_save_w, "property", name="text").append(ET.fromstring("<string>Save / Update</string>"))
    
    btn_del = ET.SubElement(h_layout2, "item")
    btn_del_w = ET.SubElement(btn_del, "widget", {"class": "QPushButton", "name": "btnDeleteProfile"})
    ET.SubElement(btn_del_w, "property", name="text").append(ET.fromstring("<string>Delete</string>"))

    # Linked Camera Selection Row in Model Settings
    h_row3 = ET.SubElement(p_layout, "item")
    h_layout3 = ET.SubElement(h_row3, "layout", {"class": "QHBoxLayout", "name": "profileCameraLayout"})
    
    lbl_cam = ET.SubElement(h_layout3, "item")
    lbl_cam_w = ET.SubElement(lbl_cam, "widget", {"class": "QLabel", "name": "lblProfileCamera"})
    ET.SubElement(lbl_cam_w, "property", name="text").append(ET.fromstring("<string>Linked Camera Source:</string>"))
    
    combo_cam = ET.SubElement(h_layout3, "item")
    combo_cam_w = ET.SubElement(combo_cam, "widget", {"class": "QComboBox", "name": "comboProfileCamera"})
    
    # Insert at index 0
    v_layout_4.insert(0, grp_profiles)

# 2. Update liveMonitoring tab (liveTopRow) to use Profile Selection
# Replace lblCameraSelection with lblActiveProfile
# We use a broader search in case they are not directly under liveTopRow in the current XML state
for lbl in root.findall(".//widget"):
    if lbl.get("name") == "lblCameraSelection":
        text_prop = lbl.find("./property[@name='text']/string")
        if text_prop is not None:
            text_prop.text = "Active Profile:"
        lbl.set("name", "lblActiveProfile")

for combo in root.findall(".//widget"):
    if combo.get("name") == "comboCameraSelection":
        combo.set("name", "comboActiveProfile")

tree.write("refactored_ui_p4.ui", encoding="utf-8", xml_declaration=True)
print("UI refactored for Phase 4 successfully.")
