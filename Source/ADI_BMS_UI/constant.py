
MAX_TABS = 5 # 1 configuration, 3 board tab, 1 last tab

Main_tab_name = "  PC/Host Configuration "
board_tab_name = " AFE Board "
reflink_tab_name = " Reference Links "
test_tab_name	= "    Reference Links- New"

CONG_TXT_RGB_COLOR = 0, 95, 189
CONG_TXT_RGB_COLOR_DISABLE = 0,95,150
FAULT_WHITE_COLOR = "WHITE"
BMS_AFE_TXT_MAINFRAME_RGB_COLOR = 0, 178, 169
BMS_AFE_TXT_GRAY_COLOR = 195, 195, 195
RGB_SOLID_GREEN_COLOR_CODE = 0,255,0 #19, 170, 19

RGB_PINK_COLOR_CODE = 255, 192, 203
RGB_LIGHT_YELLOW_COLOR_CODE = 255, 255, 100 #224
RGB_LIGHT_ORANGE_COLOR_CODE = 255, 165, 0
RGB_LIGHT_BLUE_COLOR_CODE = 173, 216, 230
RGB_ORANGE_RED_COLOR_CODE = 255, 69, 0
RGB_LIGHT_PURPLE_COLOR_CODE = 203, 195, 227
RGB_LIGHT_NAVY_COLOR_CODE = 40, 96, 134
RGB_LIGHT_KHAKI_COLOR_CODE = 240, 230, 140
RGB_LAWN_GREEN_COLOR_CODE = 124, 252, 0
RGB_CYAN_COLOR_CODE = 0, 255, 255
RGB_LIGHT_CYAN_COLOR_CODE = 224, 255, 255
RGB_DARK_MAGENTA_COLOR_CODE = 139, 0, 139
RGB_BROWN_COLOR_CODE = 165, 42, 42
RGB_INDIGO_COLOR_CODE = 75, 0, 130
RGB_RED_COLOR_CODE = 255, 0, 0


# --------------- Tab - PC/Host Configuration ------------------------
CONFIG_PAGE_TITLE = "BMS CTRL Unit"
CONFIG_PAGE_LEFT_TITLE = "Sustainable\n Energy BU"
MCU_BOARD_TITLE = "MCU board"
BMS_AFE_1_TITLE = "BMS AFE 1"
BMS_AFE_2_TITLE = "BMS AFE 2" 
BMS_AFE_3_TITLE = "BMS AFE 3"

CONFIG_BOX_TITLE = "Configuration"
STATUSBOX_TITLE = " System Status"
STATUSBOX_System_Operational = "System Operational"
STATUSBOX_SPI_line_status = "isoSPI/SPI line status"
n_com_list = ["COM%s" % (i) for i in range(1, 21)]
baud_rate_list = ['115200']

#Threshold box position
pos_val_OV = (220, 279)
pos_val_UV = (220, 317)
pos_lbl_OV = (58, 295)
pos_lbl_UV = (58, 334)

BTN_START_NAME = "START"
BTN_STOP_NAME = "STOP"
BTN_CONNECT_NAME = "CONNECT"
BTN_DISCONNECT_NAME = "DISCONNECT"

# Stack graph
STACK_GRAPH_Y1_LIST = [0, 0, 0, 0, 0, 0, 0]
STACK_GRAPH_Y2_LIST = [0, 0, 0, 0, 0, 0, 0]
STACK_GRAPH_Y3_LIST = [0, 0, 0, 0, 0, 0, 0]
STACK_GRAPH_Y_LBL_LIST = [0, 25, 50, 75, 100, 125, 150]
Graph_x_label = "Time [s]"
Graph_y_label = "Voltage [V]"


# --------------- Tab - AFE Board ------------------------

FAULT_TYPES = {'CELL OV': RGB_PINK_COLOR_CODE, 'CELL UV': RGB_LIGHT_ORANGE_COLOR_CODE, 'CELL OW': RGB_LIGHT_YELLOW_COLOR_CODE, 'LINE OW': RGB_LIGHT_BLUE_COLOR_CODE, 'VREG OV': RGB_ORANGE_RED_COLOR_CODE,
               'VREG UV': RGB_LIGHT_PURPLE_COLOR_CODE, 'VREGD OV': RGB_LIGHT_NAVY_COLOR_CODE, 'VREGD UV': RGB_LIGHT_KHAKI_COLOR_CODE, 'STACK OV': RGB_LAWN_GREEN_COLOR_CODE,
               'STACK UV': RGB_CYAN_COLOR_CODE, 'GPIO OV': RGB_LIGHT_CYAN_COLOR_CODE, 'GPIO UV': RGB_DARK_MAGENTA_COLOR_CODE, 'DOT': RGB_BROWN_COLOR_CODE, 'DUT': RGB_INDIGO_COLOR_CODE
               }

StateNameList = ['Stack(voltage)', 'ITMP', 'VregA', 'Vregd', 'REV', 'MUXFAIL', 'THSD']
Graph_title = "Cell voltage measurement"
pos_graph_title = (685, 87)

CELL_UV_COLOR = RGB_LIGHT_ORANGE_COLOR_CODE
CELL_OV_COLOR = RGB_PINK_COLOR_CODE
CELL_OW_COLOR = RGB_LIGHT_YELLOW_COLOR_CODE
GPIO_UV_COLOR = RGB_DARK_MAGENTA_COLOR_CODE
GPIO_OV_COLOR = RGB_LIGHT_CYAN_COLOR_CODE
VREG_UV_COLOR = RGB_LIGHT_PURPLE_COLOR_CODE
VREG_OV_COLOR = RGB_ORANGE_RED_COLOR_CODE
VREGD_UV_COLOR = RGB_LIGHT_KHAKI_COLOR_CODE
VREGD_OV_COLOR = RGB_LIGHT_NAVY_COLOR_CODE
STACK_UV_COLOR = RGB_CYAN_COLOR_CODE
STACK_OV_COLOR = RGB_LAWN_GREEN_COLOR_CODE
DIET_UV_COLOR = RGB_INDIGO_COLOR_CODE
DIET_OV_COLOR = RGB_BROWN_COLOR_CODE
ISO_SPI_FAIL_COLOR = RGB_RED_COLOR_CODE
SYS_OP_COLOR = RGB_SOLID_GREEN_COLOR_CODE
SYS_OP_FAIL_COLOR = RGB_RED_COLOR_CODE
ISO_SPI_COLOR = RGB_SOLID_GREEN_COLOR_CODE
DIET_UT_COLOR = RGB_INDIGO_COLOR_CODE
DIET_OT_COLOR = RGB_BROWN_COLOR_CODE


# --------------- Tab - Reference design ------------------------

# links to open Datasheet of specific Part from "BMS CTRL Unit - Reference-"  page
Buck_converter_LT8609A_str = "https://www.analog.com/en/products/lt8609.html"
Ideal_Diode_LTC4415_str = "https://www.analog.com/en/products/ltc4415.html"
LDO_ADP323_str = "https://www.analog.com/en/products/adp323.html"
LDO_ADM7170_str = "https://www.analog.com/en/products/adm7170.html"
MCU_MAX_32626_str = "https://www.maximintegrated.com/en/products/microcontrollers/MAX32626.html"
iso_CAN_transceiver_ADM3057_str = "https://www.analog.com/en/products/adm3055e.html"
iso_SPI_transceiver_LTC6820_str = "https://www.analog.com/en/products/ltc6820.html"
link_of_Schematics_str = "https://www.analog.com/en/applications/markets/automotive-pavilion-home/vehicle-electrification/battery-management-systems-bms.html"