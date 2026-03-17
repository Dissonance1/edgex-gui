import xml.etree.ElementTree as ET
import os

ui_file = r"\\wsl.localhost\Ubuntu\home\rog_strix\src\edgex-gui\src\AIRuntimeView.ui"

# Register namespaces to preserve them
ET.register_namespace('', "http://www.qt.io/struct")

tree = ET.parse(ui_file)
root = tree.getroot()

# 1. Find the main layout and splitter
main_widget = root.find(".//widget[@class='QWidget'][@name='AIRuntimeView']")
main_layout = main_widget.find("layout")
splitter = main_layout.find(".//widget[@class='QSplitter']")

if splitter is not None:
    # Extract side panel elements we want to keep
    title_label = splitter.find(".//widget[@name='labelTitle']")
    btn_launch = splitter.find(".//widget[@name='btnLaunch']")
    btn_stop = splitter.find(".//widget[@name='btnStop']")
    content_stack = splitter.find(".//widget[@name='contentStack']")
    
    # Remove the splitter item from main_layout
    splitter_item = main_layout.find("item")
    main_layout.remove(splitter_item)
    
    # 2. Change main_layout to QVBoxLayout
    main_layout.set("class", "QVBoxLayout")
    main_layout.set("name", "mainVerticalLayout")
    # Reset margins/spacing if needed
    for prop in main_layout.findall("property"):
        if prop.get("name") in ["leftMargin", "topMargin", "rightMargin", "bottomMargin"]:
            prop.find("number").text = "10"
    
    # 3. Create a Top Navigation Bar (QHBoxLayout)
    top_bar_item = ET.SubElement(main_layout, "item")
    top_bar_layout = ET.SubElement(top_bar_item, "layout", {"class": "QHBoxLayout", "name": "topNavBarLayout"})
    
    # Add Title
    if title_label is not None:
        title_item = ET.SubElement(top_bar_layout, "item")
        title_item.append(title_label)
        # title_label.set("maximumWidth", "200")
        
    # Add Dropdown Navigation (comboNav)
    nav_item = ET.SubElement(top_bar_layout, "item")
    combo_nav = ET.SubElement(nav_item, "widget", {"class": "QComboBox", "name": "comboNav"})
    combo_nav_style = "QComboBox { height: 35px; min-width: 200px; font-size: 14px; }"
    ET.SubElement(combo_nav, "property", name="styleSheet").append(ET.fromstring(f"<string notr='true'>{combo_nav_style}</string>"))
    
    # Add items to comboNav
    combo_nav.append(ET.fromstring("<item><property name='text'><string>Live Monitoring</string></property></item>"))
    combo_nav.append(ET.fromstring("<item><property name='text'><string>Model Settings</string></property></item>"))
    combo_nav.append(ET.fromstring("<item><property name='text'><string>Camera Sources</string></property></item>"))
    combo_nav.append(ET.fromstring("<item><property name='text'><string>EdgeX Configuration</string></property></item>"))

    # Add Spacer
    spacer_item = ET.SubElement(top_bar_layout, "item")
    spacer = ET.SubElement(spacer_item, "spacer", name="navSpacer")
    ET.SubElement(spacer, "property", name="orientation").append(ET.fromstring("<enum>Qt::Horizontal</enum>"))
    ET.SubElement(spacer, "property", name="sizeHint").append(ET.fromstring("<size><width>40</width><height>20</height></size>"))

    # Add Launch/Stop Buttons
    if btn_launch is not None:
        # Ensure property minimumSize exists and has size child
        min_size = btn_launch.find("./property[@name='minimumSize']")
        if min_size is None:
            min_size = ET.SubElement(btn_launch, "property", name="minimumSize")
            size = ET.SubElement(min_size, "size")
            ET.SubElement(size, "width").text = "120"
            ET.SubElement(size, "height").text = "35"
        else:
            min_size.find("./size/width").text = "120"
            min_size.find("./size/height").text = "35"
        
        launch_item = ET.SubElement(top_bar_layout, "item")
        launch_item.append(btn_launch)
        
    if btn_stop is not None:
        min_size = btn_stop.find("./property[@name='minimumSize']")
        if min_size is None:
            min_size = ET.SubElement(btn_stop, "property", name="minimumSize")
            size = ET.SubElement(min_size, "size")
            ET.SubElement(size, "width").text = "120"
            ET.SubElement(size, "height").text = "35"
        else:
            min_size.find("./size/width").text = "120"
            min_size.find("./size/height").text = "35"
            
        stop_item = ET.SubElement(top_bar_layout, "item")
        stop_item.append(btn_stop)
        
    # 4. Add the Content Stack below the Top Bar
    if content_stack is not None:
        stack_item = ET.SubElement(main_layout, "item")
        stack_item.append(content_stack)

tree.write("refactored_ui_p5.ui", encoding="utf-8", xml_declaration=True)
print("UI refactored for Phase 5 (Navigation Dropdown) successfully.")
