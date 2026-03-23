import os

path = "/home/rog_strix/src/edgex-gui/src/AIRuntimeView.ui"

# We want to replace the content of <widget class="QWidget" name="scrollAreaWidgetContents">
# from its start to its end.

clean_scroll_contents = """            <layout class="QVBoxLayout" name="verticalLayout_4">
             <item>
              <widget class="QGroupBox" name="groupModelFile">
               <property name="title">
                <string>Model Configuration (YAML / Zoo)</string>
               </property>
               <layout class="QFormLayout" name="formModel">
                <item row="0" column="0"><widget class="QLabel" name="lblModelZoo"><property name="text"><string>Select Model (Zoo):</string></property></widget></item><item row="0" column="1"><widget class="QComboBox" name="comboModelZoo"><property name="editable"><bool>true</bool></property><property name="placeholderText"><string>Loading Model Zoo...</string></property></widget></item><item row="1" column="0"><widget class="QLabel" name="lblCustomModel"><property name="text"><string>Custom YAML:</string></property></widget></item><item row="1" column="1"><layout class="QHBoxLayout" name="customModelLayout"><item><widget class="QLineEdit" name="editModelPath"><property name="readOnly"><bool>true</bool></property><property name="placeholderText"><string>Path to custom model.yaml...</string></property></widget></item><item><widget class="QPushButton" name="btnBrowseModelYaml"><property name="text"><string>Browse...</string></property></widget></item></layout></item><item row="2" column="0"><widget class="QLabel" name="lblModelInfoTitle"><property name="text"><string>Model Info:</string></property></widget></item><item row="2" column="1"><widget class="QLabel" name="lblModelInfo"><property name="text"><string>Select a model to see details.</string></property><property name="wordWrap"><bool>true</bool></property><property name="styleSheet"><string notr="true">color: #6c757d; font-style: italic;</string></property></widget></item><item row="3" column="0"><widget class="QLabel" name="lblConfidence"><property name="text"><string>Confidence Threshold:</string></property></widget></item><item row="3" column="1"><widget class="QDoubleSpinBox" name="spinConfidence"><property name="value"><number>70</number></property></widget></item><item row="4" column="0"><widget class="QLabel" name="lblClassMap"><property name="text"><string>Class Map (XML/JSON):</string></property></widget></item><item row="4" column="1"><layout class="QHBoxLayout" name="classMapLayout"><item><widget class="QLineEdit" name="editClassMapPath"><property name="placeholderText"><string>Path to class_map...</string></property></widget></item><item><widget class="QPushButton" name="btnBrowseClassMap"><property name="text"><string>Browse...</string></property></widget></item></layout></item><item row="5" column="0"><widget class="QLabel" name="lblEmbedding"><property name="text"><string>Embedding JSON:</string></property></widget></item><item row="5" column="1"><layout class="QHBoxLayout" name="embeddingLayout"><item><widget class="QLineEdit" name="editEmbeddingPath"><property name="placeholderText"><string>Path to embedding.json...</string></property></widget></item><item><widget class="QPushButton" name="btnBrowseEmbedding"><property name="text"><string>Browse...</string></property></widget></item></layout></item></layout>
              </widget>
             </item>
             <item>
              <widget class="QGroupBox" name="groupPipeline">
               <property name="title">
                <string>Pipeline Control</string>
               </property>
               <layout class="QFormLayout" name="formPipeline">
                <item row="0" column="0">
                 <widget class="QLabel" name="lblPipelineType">
                  <property name="text">
                   <string>Pipeline Type:</string>
                  </property>
                 </widget>
                </item>
                <item row="0" column="1">
                 <widget class="QComboBox" name="comboPipelineType">
                  <item>
                   <property name="text">
                    <string>torch</string>
                   </property>
                  </item>
                  <item>
                   <property name="text">
                    <string>gst</string>
                   </property>
                  </item>
                  <item>
                   <property name="text">
                    <string>torch-aipu</string>
                   </property>
                  </item>
                 </widget>
                </item>
                <item row="1" column="0">
                 <widget class="QLabel" name="lblFrameLimit">
                  <property name="text">
                   <string>Frame Limit:</string>
                  </property>
                 </widget>
                </item>
                <item row="1" column="1">
                 <widget class="QSpinBox" name="spinFrameLimit">
                  <property name="maximum">
                   <number>999999</number>
                  </property>
                 </widget>
                </item>
                <item row="2" column="0">
                 <widget class="QLabel" name="lblDisplayMode">
                  <property name="text">
                   <string>Display Mode:</string>
                  </property>
                 </widget>
                </item>
                <item row="2" column="1">
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
                </item>
                <item row="3" column="0">
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
                <item row="4" column="0">
                 <widget class="QLabel" name="lblWindowSize">
                  <property name="text">
                   <string>Window Size:</string>
                  </property>
                 </widget>
                </item>
                <item row="4" column="1">
                 <widget class="QLineEdit" name="editWindowSize">
                  <property name="placeholderText">
                   <string>e.g. 640x480</string>
                  </property>
                 </widget>
                </item>
                <item row="5" column="0">
                 <widget class="QLabel" name="lblSavePath">
                  <property name="text">
                   <string>Save Output Path:</string>
                  </property>
                 </widget>
                </item>
                <item row="5" column="1">
                 <layout class="QHBoxLayout" name="horizontalLayout_7">
                  <item>
                   <widget class="QLineEdit" name="editSaveOutputPath" />
                  </item>
                  <item>
                   <widget class="QPushButton" name="btnBrowseOutput">
                    <property name="text">
                     <string>Browse...</string>
                    </property>
                   </widget>
                  </item>
                 </layout>
                </item>
               </layout>
              </widget>
             </item>
             <item>
              <widget class="QGroupBox" name="groupHwAccel">
               <property name="title">
                <string>Hardware &amp; Performance</string>
               </property>
               <layout class="QVBoxLayout" name="verticalLayout_5">
                <item>
                 <layout class="QGridLayout" name="gridLayout">
                  <item row="0" column="0">
                   <widget class="QCheckBox" name="chkHwCodec">
                    <property name="text">
                     <string>Enable Hardware Codec</string>
                    </property>
                   </widget>
                  </item>
                  <item row="0" column="1">
                   <widget class="QCheckBox" name="chkVaapi">
                    <property name="text">
                     <string>Enable VAAPI</string>
                    </property>
                   </widget>
                  </item>
                  <item row="1" column="0">
                   <widget class="QCheckBox" name="chkOpencl">
                    <property name="text">
                     <string>Enable OpenCL</string>
                    </property>
                   </widget>
                  </item>
                  <item row="3" column="1">
                   <widget class="QCheckBox" name="chkOpengl">
                    <property name="text">
                     <string>Enable OpenGL</string>
                    </property>
                   </widget>
                  </item>
                 </layout>
                </item>
               </layout>
              </widget>
             </item>
             <item>
              <widget class="QGroupBox" name="groupAIPU">
               <property name="title">
                <string>AIPU Core Management &amp; Live Status</string>
               </property>
               <layout class="QVBoxLayout" name="verticalLayout_6">
                <item><layout class="QHBoxLayout" name="aipuStatusLayout"><item><widget class="QGroupBox" name="core0Status"><property name="title"><string>Core 0</string></property><layout class="QVBoxLayout" name="c0Layout"><item><widget class="QLabel" name="lblCore0State"><property name="text"><string>Idle</string></property><property name="alignment"><enum>Qt::AlignCenter</enum></property></widget></item></layout></widget></item><item><widget class="QGroupBox" name="core1Status"><property name="title"><string>Core 1</string></property><layout class="QVBoxLayout" name="c1Layout"><item><widget class="QLabel" name="lblCore1State"><property name="text"><string>Idle</string></property><property name="alignment"><enum>Qt::AlignCenter</enum></property></widget></item></layout></widget></item><item><widget class="QGroupBox" name="core2Status"><property name="title"><string>Core 2</string></property><layout class="QVBoxLayout" name="c2Layout"><item><widget class="QLabel" name="lblCore2State"><property name="text"><string>Idle</string></property><property name="alignment"><enum>Qt::AlignCenter</enum></property></widget></item></layout></widget></item><item><widget class="QGroupBox" name="core3Status"><property name="title"><string>Core 3</string></property><layout class="QVBoxLayout" name="c3Layout"><item><widget class="QLabel" name="lblCore3State"><property name="text"><string>Idle</string></property><property name="alignment"><enum>Qt::AlignCenter</enum></property></widget></item></layout></widget></item></layout></item><item>
                 <layout class="QFormLayout" name="formAIPU">
                  <item row="0" column="0">
                   <widget class="QLabel" name="lblAipuCores">
                    <property name="text">
                     <string>AIPU Cores (Selection):</string>
                    </property>
                   </widget>
                  </item>
                  <item row="0" column="1">
                   <widget class="QLineEdit" name="editAipuCores">
                    <property name="placeholderText">
                     <string>e.g. 0,1 (Busy check active)</string>
                    </property>
                   </widget>
                  </item>
                 </layout>
                </item>
                <item>
                 <widget class="QCheckBox" name="chkShowHostFps">
                  <property name="text">
                   <string>Show Host FPS</string>
                  </property>
                 </widget>
                </item>
                <item>
                 <widget class="QCheckBox" name="chkShowStreamTiming">
                  <property name="text">
                   <string>Show Stream Timing</string>
                  </property>
                 </widget>
                </item>
               </layout>
              </widget>
             </item>
             <item>
              <spacer name="verticalSpacer_3">
               <property name="orientation">
                <enum>Qt::Vertical</enum>
               </property>
               <property name="sizeHint" stdset="0">
                <size>
                 <width>20</width>
                 <height>40</height>
                </size>
               </property>
              </spacer>
             </item>
            </layout>"""

with open(path, 'r') as f:
    content = f.read()

# Identify the widget block to replace
start_marker = '<widget class="QWidget" name="scrollAreaWidgetContents">'
end_marker = '</widget>\n          </widget>' # Corresponding to the end of scrollAreaWidgetContents and scrollModelSettings?
# No, lets find the exact closing tag for scrollAreaWidgetContents.

# The current file has:
# 112:            <widget class="QWidget" name="scrollAreaWidgetContents">
# ...
# 384:           </widget>

start_idx = content.find(start_marker)
if start_idx != -1:
    # Find the closing </widget> for this specific level.
    # We know verticalLayout_4 ends at line 383, and widget ends at 384.
    # The next thing is </widget> for scrollModelSettings (line 385).
    
    # Let's search for the end of verticalLayout_4 and then the widget closing tag.
    end_layout_marker = '</layout>\n          </widget>'
    end_idx = content.find(end_layout_marker, start_idx)
    
    if end_idx != -1:
        # We replace the content BETWEEN start_marker and the end of the widget.
        full_replacement = start_marker + "\n" + clean_scroll_contents + "\n          </widget>"
        
        # We need to be careful with the exact slice.
        # The content before replacement is from start_idx to end_idx + len('</layout>\n          </widget>')
        
        old_block_end = end_idx + len(end_layout_marker)
        
        new_content = content[:start_idx] + full_replacement + content[old_block_end:]
        
        with open(path, 'w') as f:
            f.write(new_content)
        print("SUCCESS: Full UI reconstruction complete.")
    else:
        print("FAIL: Could not find end marker.")
else:
    print("FAIL: Could not find start marker.")
