import wx
import os
import constant as const
import datetime as dt
from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import (
    FigureCanvasWxAgg as FigureCanvas)


class BoardTab(wx.Panel):

	def __init__(self, parent, mainframe):
		wx.Panel.__init__(self, parent)
		self.mainframe = mainframe
		self.CellVoltage = [ (i) for i in range(0, 18) ]
		self.GPIOVoltage = [ (i) for i in range(0, 9) ]
		self.StatsValue = [ (i) for i in range (0,7) ]
		return

	def createBoardTab(self):
		self.createTabHeader()
		self.ui_fault_panel_design()
		self.ui_cell_voltage_panel_design()
		self.ui_gpio_voltage_panel_design()
		self.ui_stats_panel_design()
		self.ui_select_cell_panel_design()
		self.ui_select_gpio_panel_design()
		self.ui_graph_panel_design()
		return


	def createTabHeader(self):

		self.SetBackgroundColour('WHITE')  # background colour tab 1
		png = wx.Image(os.path.join('analog_logo.png'),  # image for tab 1
				       wx.BITMAP_TYPE_ANY).ConvertToBitmap()
		wx.StaticBitmap(
			self,
			-1,
			png,
			pos=(1060, 2),
			size=(png.GetWidth(), png.GetHeight()))


		# Add Text
		st = wx.StaticText(self, label=const.CONFIG_PAGE_TITLE, pos=(423, 5), size=(20,40), style=wx.TE_RICH)
		st.SetForegroundColour((const.CONG_TXT_RGB_COLOR))
		font = wx.Font(pointSize=30, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL, weight=wx.FONTWEIGHT_BOLD,
				       underline=False)       
		st.SetFont(font)


		st_left_title = wx.StaticText(self, label=const.CONFIG_PAGE_LEFT_TITLE, pos=(2, 5), size=(20,40), style=wx.TE_RICH)
		txtfont = wx.Font(pointSize=11, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL, weight=wx.FONTWEIGHT_BOLD, underline=False)
		st_left_title.SetForegroundColour((const.CONG_TXT_RGB_COLOR))
		st_left_title.SetFont(txtfont)

		txttopline = wx.TextCtrl(self, id=wx.ID_ANY, pos=(0, 50), size=(1160, 8), style= wx.BORDER_NONE | wx.TE_READONLY)
		txttopline.SetBackgroundColour((const.CONG_TXT_RGB_COLOR))

		return


	def ui_fault_panel_design(self):

		# Board Fault Box
		FaultBox = wx.StaticBox(  # box for tab 2
			self,
			wx.ID_ANY,
			'Fault',
			pos=(30, 61),
			size=(455, 69))
		font = wx.Font(11, wx.DECORATIVE, wx.NORMAL, wx.BOLD)
		FaultBox.SetFont(font)
		FaultBox.SetForegroundColour((40, 96, 134))
		fault_font = wx.Font(7, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_BOLD)

		# Cell Over voltage
		self.CellOV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[0], pos=(60, 84), size=(10, 5),
				                    style=0)  # defualt checked
		self.CellOV.SetFont(fault_font)
		self.txtCellOV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(39, 82), size=(16, 16), style=wx.TE_READONLY)

		# Cell Under voltage
		self.CellUV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[1], pos=(60, 107),
				                    size=(10, 5),
				                    style=0)  # defualt checked
		self.CellUV.SetFont(fault_font)
		self.txtCellUV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(39, 105), size=(16, 16), style=wx.TE_READONLY)

		# Cell Open-wire
		self.CellOW = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[2], pos=(125, 84), size=(10, 5),
				                    style=0)  # defualt checked
		self.CellOW.SetFont(fault_font)
		self.txtCellOW = wx.TextCtrl(self, id=wx.ID_ANY, pos=(105, 82), size=(16, 16), style=wx.TE_READONLY)

		# Line Open-wire
		self.LineOW = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[3], pos=(125, 107), size=(10, 5),
				                    style=0)  # defualt checked
		self.LineOW.SetFont(fault_font)
		self.txtLineOW = wx.TextCtrl(self, id=wx.ID_ANY, pos=(105, 105), size=(16, 16), style=wx.TE_READONLY)

		# VReg Over voltage
		self.VREGOV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[4], pos=(190, 84), size=(20, 10),
				                    style=0)  # defualt checked
		self.VREGOV.SetFont(fault_font)
		self.txtVREGOV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(171, 82), size=(16, 16), style=wx.TE_READONLY)

		# VReg Under voltage
		self.VREGUV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[5], pos=(190, 107), size=(20, 10),
				                    style=0)  # defualt checked
		self.VREGUV.SetFont(fault_font)
		self.txtVREGUV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(171, 105), size=(16, 16), style=wx.TE_READONLY)

		# VRegD Over voltage
		self.VREGDOV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[6], pos=(256, 84), size=(20, 10),
				                     style=0)  # defualt checked
		self.VREGDOV.SetFont(fault_font)
		self.txtVREGDOV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(237, 82), size=(16, 16), style=wx.TE_READONLY)

		# VRegD Over voltage
		self.VREGDUV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[7], pos=(256, 107), size=(20, 10),
				                     style=0)  # defualt checked
		self.VREGDUV.SetFont(fault_font)
		self.txtVREGDUV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(237, 105), size=(17, 17), style=wx.TE_READONLY)

		# Stack Over voltage
		self.STACKOV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[8], pos=(330, 84), size=(20, 10),
				                     style=0)  # defualt checked
		self.STACKOV.SetFont(fault_font)
		self.txtSTACKOV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(310, 82), size=(16, 16), style=wx.TE_READONLY)

		# Stack Under voltage
		self.STACKUV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[9], pos=(330, 107), size=(20, 10),
				                     style=0)  # defualt checked
		self.STACKUV.SetFont(fault_font)
		self.txtVSTACKUV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(310, 105), size=(17, 17), style=wx.TE_READONLY)        

		# GPIO Over voltage
		self.GPIO_OV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[10], pos=(400, 84), size=(20, 10),
				                     style=0)  # defualt checked
		self.GPIO_OV.SetFont(fault_font)
		self.txtGPIO_OV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(380, 82), size=(16, 16), style=wx.TE_READONLY)

		# GPIO Under voltage
		self.GPIO_UV = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[11], pos=(400, 107), size=(20, 10),
				                     style=0)  # defualt checked
		self.GPIO_UV.SetFont(fault_font)
		self.txtGPIO_UV = wx.TextCtrl(self, id=wx.ID_ANY, pos=(380, 105), size=(17, 17), style=wx.TE_READONLY)

		# Die Over temperature
		self.DIE_OT = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[12], pos=(462, 84),
				                    style=0)  # defualt checked
		self.DIE_OT.SetFont(fault_font)
		self.txtDIE_OT = wx.TextCtrl(self, id=wx.ID_ANY, pos=(443, 82), size=(16, 16), style=wx.TE_READONLY)

		# Die Under temperature
		self.DIE_UT = wx.StaticText(self, label=list(const.FAULT_TYPES.keys())[13], pos=(462, 107),
				                    style=0)  # defualt checked
		self.DIE_UT.SetFont(fault_font)
		self.txtDIE_UT = wx.TextCtrl(self, id=wx.ID_ANY, pos=(443, 105), size=(16, 16), style=wx.TE_READONLY)
		return


	def ui_cell_voltage_panel_design(self):

		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.BOLD)
		btn_font = wx.Font(7, wx.DECORATIVE, wx.NORMAL, wx.BOLD)

		# Cell Voltage Box
		box5 = wx.StaticBox(  # box for tab 2
			self,
			wx.ID_ANY,
			'Cell Voltages',
			pos=(30, 138),
			size=(200, 480))#(200, 548))
		box5.SetFont(font)
		box5.SetForegroundColour((40, 96, 134))
		font = wx.Font(9, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_NORMAL)

		self.i = 0
		y_pos = 159
		for i in range(18):
			Lab = ("C" + str(i + 1) + "V")
			self.CellVoltageText = wx.StaticText(self, label=Lab, pos=(48, y_pos))
			self.CellVoltageText.SetFont(font)
			self.CellVoltage[i] = wx.TextCtrl(self, id=wx.ID_ANY, pos=(118, y_pos), size=(80, 20))
			y_pos = y_pos + 25

		return



	def ui_gpio_voltage_panel_design(self):

		font = wx.Font(pointSize=10, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL,
				   weight=wx.BOLD, underline=False)

		# GPIO Voltage Box
		box6 = wx.StaticBox(  # box for tab 2
			self,
			wx.ID_ANY,
			'GPIO Voltages',
			pos=(270, 137),
			size=(200, 260))
		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.BOLD)

		box6.SetFont(font)
		box6.SetForegroundColour((40, 96, 134))

		y_pos = 155
		for i in range(9):
			Lab = ("G" + str(i + 1) + "V")
			self.GPIOVoltageText = wx.StaticText(self, label=Lab, pos=(288, y_pos))
			self.GPIOVoltageText.SetFont(font)
			self.GPIOVoltage[i] = wx.TextCtrl(self, id=wx.ID_ANY, pos=(357, y_pos), size=(80, 20))
			y_pos = y_pos + 27

		return

	def ui_stats_panel_design(self):
		# Stat Value Box
		box7 = wx.StaticBox(  # box for tab 2
		self,
		wx.ID_ANY,
		'Stat Value',
		pos=(270, 400),
		size=(200, 218))
		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.BOLD)
		box7.SetFont(font)
		box7.SetForegroundColour((40, 96, 134))

		StateValueName = const.StateNameList
		font = wx.Font(pointSize=10, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL,
				   weight=wx.FONTWEIGHT_NORMAL, underline=False)

		y_pos = 425
		for i in range(7):
			Lab = StateValueName[i]
			self.StatsValueText = wx.StaticText(self, label=Lab, pos=(276, y_pos))
			self.StatsValue[i] = wx.TextCtrl(self, id=wx.ID_ANY, pos=(357, y_pos), size=(80, 20))
			y_pos = y_pos + 27

		return


	def ui_graph_panel_design(self):

		# plot text
		Plots = wx.StaticText(self, label=const.Graph_title, pos=const.pos_graph_title, size=(20,40))
		font = wx.Font(pointSize=15, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL,
				       weight=wx.FONTWEIGHT_NORMAL, underline=False)
		Plots.SetFont(font)
		Plots.SetForegroundColour((40, 96, 134))


		# geenrate empty list initially for cell selection
		self.CellList = []  # cell selected list
		self.GpioList = []	# selected Gpio list
		self.legendCellList = []  # legend cell label display list
		self.legendCellColorCellList = []  # final legend display color lest
		self.legendColorGpioList =[]

		# pre-asigned legend colour to each cell
		self.legendColorCellList = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b",
				                	"#e377c2", "#7f7f7f", "#bcbd22", "#17becf", "#1f77b4", "#ff7f0e",
				                	"#2ca02c", "#d62728", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f"]

		self.legendColorGpioList = ["#2c03fc", "#857bba", "#d61a91", "#179c0e", "#d16981", "#170f30",
				                	"#f0e00a", "#e06c07", "#07e0bc"]

		self.legendList =[]
		self.legendCellList =[]
		self.legendGpioList =[]
		
		self.dates = [(dt.datetime.now() + dt.timedelta(seconds=i)).strftime('%H:%M:%S') for i in range(7)]



		# Add figure graph
		self.figure1 = Figure(figsize=(5, 1), dpi=90, facecolor='white')
		self.figure1.autofmt_xdate(bottom=0.2, rotation=90, ha='center')

		self.axes1tab2 = self.figure1.add_subplot(111)
		self.axes1tab2.axis([0, 6, 0, 7])
		self.axes1tab2.spines['bottom'].set_color('black')
		self.axes1tab2.spines['left'].set_color('black')
		self.axes1tab2.spines['bottom'].set_linewidth(0.5)
		self.axes1tab2.spines['left'].set_linewidth(0.3)
		self.axes1tab2.set_xticklabels(self.dates)
		self.axes1tab2.set_yticks([0, 1, 2, 3, 4, 5, 6, 7])
		self.axes1tab2.set_ylabel(const.Graph_y_label, fontsize=8, color='black', loc="center") ##TODO
		self.axes1tab2.set_xlabel(const.Graph_x_label, fontsize=8, color='black', loc="center") ##TODO

		# axis ticks customization
		self.axes1tab2.tick_params(axis='x', labelsize=9, color='white',
				                   colors='black')  # axis line param , NOTE: color = axis tick color and colors: axis label color
		self.axes1tab2.tick_params(axis='y', labelsize=9, color='white', colors='black')  # axis line param

		self.axes1tab2.grid()
		
		self.canvas1 = FigureCanvas(self, -1, self.figure1)
		self.canvas1.SetSize((650, 250))
		self.canvas1.SetPosition((485, 110))
		self.canvas1.draw()
		return


	def ui_select_cell_panel_design(self):

		box7 = wx.StaticBox( 
			self,
			wx.ID_ANY,
			'Select Cell',
			pos=(567, 359),
			size=(508, 150))
		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.BOLD)
		box7.SetFont(font)
		box7.SetForegroundColour((40, 96, 134))

		# Check box for Cell
		self.CheckBoxCVAll = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(590, 380), size=(15, 15))
		self.CheckBoxCVAll.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCVALL)
		self.CheckBoxCV1 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(665, 380), size=(15, 15))
		self.CheckBoxCV1.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV1)
		self.CheckBoxCV2 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(740, 380), size=(15, 15))
		self.CheckBoxCV2.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV2)
		self.CheckBoxCV3 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(815, 380), size=(15, 15))
		self.CheckBoxCV3.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV3)
		self.CheckBoxCV4 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(890, 380), size=(15, 15))
		self.CheckBoxCV4.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV4)
		self.CheckBoxCV5 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(965, 380), size=(15, 15))
		self.CheckBoxCV5.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV5)
		self.CheckBoxCV6 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(1040, 380), size=(15, 15))
		self.CheckBoxCV6.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV6)
		self.CheckBoxCV7 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(590, 425), size=(15, 15))
		self.CheckBoxCV7.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV7)
		self.CheckBoxCV8 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(665, 425), size=(15, 15))
		self.CheckBoxCV8.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV8)
		self.CheckBoxCV9 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(740, 425), size=(15, 15))
		self.CheckBoxCV9.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV9)
		self.CheckBoxCV10 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(815, 425), size=(15, 15))
		self.CheckBoxCV10.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV10)
		self.CheckBoxCV11 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(890, 425), size=(15, 15))
		self.CheckBoxCV11.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV11)
		self.CheckBoxCV12 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(965, 425), size=(15, 15))
		self.CheckBoxCV12.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV12)
		self.CheckBoxCV13 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(1040, 425), size=(15, 15))
		self.CheckBoxCV13.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV13)
		self.CheckBoxCV14 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(590, 465), size=(15, 15))
		self.CheckBoxCV14.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV14)
		self.CheckBoxCV15 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(665, 465), size=(15, 15))
		self.CheckBoxCV15.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV15)
		self.CheckBoxCV16 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(740, 465), size=(15, 15))
		self.CheckBoxCV16.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV16)
		self.CheckBoxCV17 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(815, 465), size=(15, 15))
		self.CheckBoxCV17.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV17)
		self.CheckBoxCV18 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(890, 465), size=(15, 15))
		self.CheckBoxCV18.Bind(wx.EVT_CHECKBOX, self.onCheckBoxCV18)

		# Static Text for Checkbox
		Lable_List_Cell = ["ALL", "C1V", "C2V", "C3V", "C4V", "C5V", "C6V",
				      "C7V", "C8V", "C9V", "C10V", "C11V", "C12V", "C13V",
				      "C14V", "C15V", "C16V", "C17V", "C18V"]

		X_List_Cell = [587, 660, 735, 810, 885, 960, 1035,
				  585, 660, 735, 807, 882, 956, 1032,
				  581, 656, 731, 807, 882]

		Y_List_Cell = [395, 395, 395, 395, 395, 395, 395,
				  440, 440, 440, 440, 440, 440, 440,
				  480, 480, 480, 480, 480]

		for i in range(19):
			self.CGV = wx.StaticText(self, id=wx.ID_ANY, label=Lable_List_Cell[i], pos=(X_List_Cell[i], Y_List_Cell[i]))
			self.CGV.SetFont(font)
			self.CGV.SetForegroundColour('black')
		return


	def ui_select_gpio_panel_design(self):
		# Select GPIO Box
		box8 = wx.StaticBox(  # box for tab 2
			self,
			wx.ID_ANY,
			'Select GPIO',
			pos=(567, 526),
			size=(508, 82))
		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.BOLD)
		box8.SetFont(font)
		box8.SetForegroundColour((40, 96, 134))
		# Check box for GPIO
		self.CheckBoxGVAll = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(590, 545), size=(12, 12))
		self.CheckBoxGVAll.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGVALL)
		self.CheckBoxGV1 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(665, 545), size=(12, 12))
		self.CheckBoxGV1.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV1)
		self.CheckBoxGV2 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(740, 545), size=(12, 12))
		self.CheckBoxGV2.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV2)
		self.CheckBoxGV3 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(815, 545), size=(12, 12))
		self.CheckBoxGV3.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV3)
		self.CheckBoxGV4 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(890, 545), size=(12, 12))
		self.CheckBoxGV4.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV4)
		self.CheckBoxGV5 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(965, 545), size=(12, 12))
		self.CheckBoxGV5.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV5)
		self.CheckBoxGV6 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(1040, 545), size=(12, 12))
		self.CheckBoxGV6.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV6)
		self.CheckBoxGV7 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(590, 575), size=(12, 12))
		self.CheckBoxGV7.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV7)
		self.CheckBoxGV8 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(665, 575), size=(12, 12))
		self.CheckBoxGV8.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV8)
		self.CheckBoxGV9 = wx.CheckBox(self, id=wx.ID_ANY, label="", pos=(740, 575), size=(12, 12))
		self.CheckBoxGV9.Bind(wx.EVT_CHECKBOX, self.onCheckBoxGV9)

		Lable_List_GPIO = ["ALL", "G1V", "G2V", "G3V", "G4V", "G5V", "G6V",
					  "G7V", "G8V", "G9V"]

		X_List_GPIO = [587, 660, 735, 810, 885, 960, 1035,
				  585, 660, 735]

		Y_List_GPIO = [557, 557, 557, 557, 557, 557, 557,
				  588, 588, 588]

		for i in range(10):
			self.CGV = wx.StaticText(self, id=wx.ID_ANY, label=Lable_List_GPIO[i], pos=(X_List_GPIO[i], Y_List_GPIO[i]))
			self.CGV.SetFont(font)
			self.CGV.SetForegroundColour('black')
		return


	def graph_init(self):
		self.axes1tab2.clear()  # clear previous plottings if any

		self.dates = [(dt.datetime.now() + dt.timedelta(seconds=i)).strftime('%H:%M:%S') for i in range(7)]

		self.axes1tab2.set_facecolor('white')  # set graph background colour
		self.axes1tab2.spines['bottom'].set_color('black')
		self.axes1tab2.spines['left'].set_color('black')
		self.axes1tab2.spines['bottom'].set_linewidth(0.5)
		self.axes1tab2.spines['left'].set_linewidth(0.3)
		self.axes1tab2.grid()
		# axis ticks customization
		self.axes1tab2.tick_params(axis='x', labelsize=9, color='white',
				                   colors='black')  # axis line param , NOTE: color = axis tick color and colors: axis label color
		self.axes1tab2.tick_params(axis='y', labelsize=9, color='white', colors='black')  # axis line param
		return

	# if checkbox is selected, ennable cell display value
	def onCheckBoxCVALL(self, event):
		if self.CheckBoxCVAll.GetValue() == True:
			self.CheckBoxCV1.SetValue(True)
			self.CheckBoxCV2.SetValue(True)
			self.CheckBoxCV3.SetValue(True)
			self.CheckBoxCV4.SetValue(True)
			self.CheckBoxCV5.SetValue(True)
			self.CheckBoxCV6.SetValue(True)
			self.CheckBoxCV7.SetValue(True)
			self.CheckBoxCV8.SetValue(True)
			self.CheckBoxCV9.SetValue(True)
			self.CheckBoxCV10.SetValue(True)
			self.CheckBoxCV11.SetValue(True)
			self.CheckBoxCV12.SetValue(True)
			self.CheckBoxCV13.SetValue(True)
			self.CheckBoxCV14.SetValue(True)
			self.CheckBoxCV15.SetValue(True)
			self.CheckBoxCV16.SetValue(True)
			self.CheckBoxCV17.SetValue(True)
			self.CheckBoxCV18.SetValue(True)
			del self.CellList[:]
			for i in range(0, 18):
				self.CellList.append(i+1)
		else:
			self.CheckBoxCV1.SetValue(False)
			self.CheckBoxCV2.SetValue(False)
			self.CheckBoxCV3.SetValue(False)
			self.CheckBoxCV4.SetValue(False)
			self.CheckBoxCV5.SetValue(False)
			self.CheckBoxCV6.SetValue(False)
			self.CheckBoxCV7.SetValue(False)
			self.CheckBoxCV8.SetValue(False)
			self.CheckBoxCV9.SetValue(False)
			self.CheckBoxCV10.SetValue(False)
			self.CheckBoxCV11.SetValue(False)
			self.CheckBoxCV12.SetValue(False)
			self.CheckBoxCV13.SetValue(False)
			self.CheckBoxCV14.SetValue(False)
			self.CheckBoxCV15.SetValue(False)
			self.CheckBoxCV16.SetValue(False)
			self.CheckBoxCV17.SetValue(False)
			self.CheckBoxCV18.SetValue(False)
			for i in range(0, 18):
				self.CellList.remove(i+1)

		return


	def onCheckBoxCV1(self, event):
		if self.CheckBoxCV1.IsChecked() == True:  # if checkbox for a particular cell selected
			self.CellList.append(1)  # appen the cell number to CellList to know which cells are selected by the user
		else:
			self.CellList.remove(1)  # remove item from list as it was unselected by the user

		return

	def onCheckBoxCV2(self, event):
		if self.CheckBoxCV2.IsChecked() == True:
			self.CellList.append(2)
		else:
			self.CellList.remove(2)

		return

	def onCheckBoxCV3(self, event):
		if self.CheckBoxCV3.IsChecked() == True:
			self.CellList.append(3)
		else:
			self.CellList.remove(3)

		return

	def onCheckBoxCV4(self, event):
		if self.CheckBoxCV4.IsChecked() == True:
			self.CellList.append(4)
		else:
			self.CellList.remove(4)

		return

	def onCheckBoxCV5(self, event):
		if self.CheckBoxCV5.IsChecked() == True:
			self.CellList.append(5)
		else:
			self.CellList.remove(5)

		return

	def onCheckBoxCV6(self, event):
		if self.CheckBoxCV6.IsChecked() == True:
			self.CellList.append(6)
		else:
			self.CellList.remove(6)

		return

	def onCheckBoxCV7(self, event):
		if self.CheckBoxCV7.IsChecked() == True:
			self.CellList.append(7)
		else:
			self.CellList.remove(7)

		return

	def onCheckBoxCV8(self, event):
		if self.CheckBoxCV8.IsChecked() == True:
			self.CellList.append(8)
		else:
			self.CellList.remove(8)

		return

	def onCheckBoxCV9(self, event):
		if self.CheckBoxCV9.IsChecked() == True:
			self.CellList.append(9)
		else:
			self.CellList.remove(9)

		return

	def onCheckBoxCV10(self, event):
		if self.CheckBoxCV10.IsChecked() == True:
			self.CellList.append(10)
		else:
			self.CellList.remove(10)

		return

	def onCheckBoxCV11(self, event):
		if self.CheckBoxCV11.IsChecked() == True:
			self.CellList.append(11)
		else:
			self.CellList.remove(11)

		return

	def onCheckBoxCV12(self, event):
		if self.CheckBoxCV12.IsChecked() == True:
			self.CellList.append(12)
		else:
			self.CellList.remove(12)

	def onCheckBoxCV13(self, event):
		if self.CheckBoxCV13.IsChecked() == True:
			self.CellList.append(13)
		else:
			self.CellList.remove(13)

		return

	def onCheckBoxCV14(self, event):
		if self.CheckBoxCV14.IsChecked() == True:
			self.CellList.append(14)
		else:
			self.CellList.remove(14)

		return

	def onCheckBoxCV15(self, event):
		if self.CheckBoxCV15.IsChecked() == True:
			self.CellList.append(15)
		else:
			self.CellList.remove(15)

		return

	def onCheckBoxCV16(self, event):
		if self.CheckBoxCV16.IsChecked() == True:
			self.CellList.append(16)
		else:
			self.CellList.remove(16)

		return

	def onCheckBoxCV17(self, event):
		if self.CheckBoxCV17.IsChecked() == True:
			self.CellList.append(17)
		else:
			self.CellList.remove(17)

		return

	def onCheckBoxCV18(self, event):
		if self.CheckBoxCV18.IsChecked() == True:
			self.CellList.append(18)
		else:
			self.CellList.remove(18)

		return

	def onCheckBoxGVALL(self, event):
		if self.CheckBoxGVAll.GetValue() == True:
			self.CheckBoxGV1.SetValue(True)
			self.CheckBoxGV2.SetValue(True)
			self.CheckBoxGV3.SetValue(True)
			self.CheckBoxGV4.SetValue(True)
			self.CheckBoxGV5.SetValue(True)
			self.CheckBoxGV6.SetValue(True)
			self.CheckBoxGV7.SetValue(True)
			self.CheckBoxGV8.SetValue(True)
			self.CheckBoxGV9.SetValue(True)
			del self.GpioList[:]
			for i in range(0, 9):
				self.GpioList.append(i+1)
		else:
			self.CheckBoxGV1.SetValue(False)
			self.CheckBoxGV2.SetValue(False)
			self.CheckBoxGV3.SetValue(False)
			self.CheckBoxGV4.SetValue(False)
			self.CheckBoxGV5.SetValue(False)
			self.CheckBoxGV6.SetValue(False)
			self.CheckBoxGV7.SetValue(False)
			self.CheckBoxGV8.SetValue(False)
			self.CheckBoxGV9.SetValue(False)
			for i in range(0, 9):				
				self.GpioList.remove(i+1)

		return

	def onCheckBoxGV1(self, event):
		if self.CheckBoxGV1.IsChecked() == True:
			self.GpioList.append(1)
		else:
			self.GpioList.remove(1)

		return

	def onCheckBoxGV2(self, event):
		if self.CheckBoxGV2.IsChecked() == True:
			self.GpioList.append(2)
		else:
			self.GpioList.remove(2)

		return

	def onCheckBoxGV3(self, event):
		if self.CheckBoxGV3.IsChecked() == True:
			self.GpioList.append(3)
		else:
			self.GpioList.remove(3)

		return

	def onCheckBoxGV4(self, event):
		if self.CheckBoxGV4.IsChecked() == True:
			self.GpioList.append(4)
		else:
			self.GpioList.remove(4)

		return

	def onCheckBoxGV5(self, event):
		if self.CheckBoxGV5.IsChecked() == True:
			self.GpioList.append(5)
		else:
			self.GpioList.remove(5)

		return

	def onCheckBoxGV6(self, event):
		if self.CheckBoxGV6.IsChecked() == True:
			self.GpioList.append(6)
		else:
			self.GpioList.remove(6)

		return

	def onCheckBoxGV7(self, event):
		if self.CheckBoxGV7.IsChecked() == True:
			self.GpioList.append(7)
		else:
			self.GpioList.remove(7)

		return

	def onCheckBoxGV8(self, event):
		if self.CheckBoxGV8.IsChecked() == True:
			self.GpioList.append(8)
		else:
			self.GpioList.remove(8)

		return

	def onCheckBoxGV9(self, event):
		if self.CheckBoxGV9.IsChecked() == True:
			self.GpioList.append(9)
		else:
			self.GpioList.remove(9)

		return


	def ui_update_measurements(self, AFE_Id, df_shadow):

		# update measurement values in text boxes based on response
		for z in range(0,18):
			self.CellVoltage[z].SetValue(str(df_shadow.AFE_cell_y[AFE_Id][z][6]))

		for z in range(0,9):
			self.GPIOVoltage[z].SetValue(str(df_shadow.AFE_gpio_y[AFE_Id][z][6]))

		for z in range(0,7):
			self.StatsValue[z].SetValue(str(df_shadow.AFE_stat_y[AFE_Id][z][6]))

		return

	def ui_update_fault(self, AFE_Id, df_shadow):

		debugPrint( "FAULTS:")
		
		# Update Cell voltage and GPIO Text box background colour based on fault
		for z in range(0,18):
			if (df_shadow.AFE_cell_ow_fault_list[AFE_Id][z]== '1'):
				self.CellVoltage[z].SetBackgroundColour(const.CELL_OW_COLOR)
			elif (df_shadow.AFE_cell_ov_fault_list[AFE_Id][z]== '1'):
				self.CellVoltage[z].SetBackgroundColour(const.CELL_OV_COLOR)
			elif (df_shadow.AFE_cell_uv_fault_list[AFE_Id][z]== '1'):
				self.CellVoltage[z].SetBackgroundColour(const.CELL_UV_COLOR)
			else:
				self.CellVoltage[z].SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.CellVoltage[z].Refresh()

		for z in range(0,9):
			if (df_shadow.AFE_gpio_uv_fault_list[AFE_Id][z]== '1'):
				self.GPIOVoltage[z].SetBackgroundColour(const.GPIO_UV_COLOR)
			elif (df_shadow.AFE_gpio_ov_fault_list[AFE_Id][z]== '1'):
				self.GPIOVoltage[z].SetBackgroundColour(const.GPIO_OV_COLOR)
			else:
				self.GPIOVoltage[z].SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.GPIOVoltage[z].Refresh()

		# update fault panel
		
		fault = '1'
		if fault in (df_shadow.AFE_cell_uv_fault_list[AFE_Id]):
			self.txtCellUV.SetBackgroundColour(const.CELL_UV_COLOR)
			self.txtCellUV.Refresh()
		else:
			self.txtCellUV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtCellUV.Refresh()
		if fault in (df_shadow.AFE_cell_ov_fault_list[AFE_Id]):
			self.txtCellOV.SetBackgroundColour(const.CELL_OV_COLOR)
			self.txtCellOV.Refresh()
		else:
			self.txtCellOV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtCellOV.Refresh()
		if fault in (df_shadow.AFE_cell_ow_fault_list[AFE_Id]):		
			self.txtCellOW.SetBackgroundColour(const.CELL_OW_COLOR)
			self.txtCellOW.Refresh()
		else:
			self.txtCellOW.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtCellOW.Refresh()
		if fault in (df_shadow.AFE_gpio_uv_fault_list[AFE_Id]):
			self.txtGPIO_UV.SetBackgroundColour(const.GPIO_UV_COLOR)
			self.txtGPIO_UV.Refresh()
		else:
			self.txtGPIO_UV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtGPIO_UV.Refresh()
		if fault in (df_shadow.AFE_gpio_ov_fault_list[AFE_Id]):
			self.txtGPIO_OV.SetBackgroundColour(const.GPIO_OV_COLOR)
			self.txtGPIO_OV.Refresh()
		else:
			self.txtGPIO_OV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtGPIO_OV.Refresh()
												
		self.txtLineOW.SetBackgroundColour(const.FAULT_WHITE_COLOR)
 
		if (df_shadow.AFE_VAuv[AFE_Id] == '1'):
			self.txtVREGUV.SetBackgroundColour(const.VREG_UV_COLOR)
			self.txtVREGUV.Refresh()
		else:
			self.txtVREGUV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtVREGUV.Refresh()
		if (df_shadow.AFE_VAov[AFE_Id] == '1'):
			self.txtVREGOV.SetBackgroundColour(const.VREG_OV_COLOR)
			self.txtVREGOV.Refresh()
		else:
			self.txtVREGOV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtVREGOV.Refresh()
		if (df_shadow.AFE_VDuv[AFE_Id] == '1'):
			self.txtVREGDUV.SetBackgroundColour(const.VREGD_UV_COLOR)
			self.txtVREGDUV.Refresh()
		else:
			self.txtVREGDUV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtVREGDUV.Refresh()
		if (df_shadow.AFE_VDov[AFE_Id] == '1'):
			self.txtVREGDOV.SetBackgroundColour(const.VREGD_OV_COLOR)
			self.txtVREGDOV.Refresh()
		else:
			self.txtVREGDOV.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtVREGDOV.Refresh()
		if (df_shadow.AFE_STKov[AFE_Id] == '1'):
			self.txtVSTACKUV.SetBackgroundColour(const.STACK_UV_COLOR) 
			self.txtVSTACKUV.Refresh()
		else:
			self.txtVSTACKUV.SetBackgroundColour(const.FAULT_WHITE_COLOR) 
			self.txtVSTACKUV.Refresh()
		if (df_shadow.AFE_STKov[AFE_Id] == '1'):
			self.txtSTACKOV.SetBackgroundColour(const.STACK_OV_COLOR) 
			self.txtSTACKOV.Refresh()
		else:
			self.txtSTACKOV.SetBackgroundColour(const.FAULT_WHITE_COLOR) 
			self.txtSTACKOV.Refresh()
		if (df_shadow.AFE_DIEut[AFE_Id] == '1'):
			debugPrint("AFE_DIEut -- if", df_shadow.AFE_DIEut)
			self.txtDIE_UT.SetBackgroundColour(const.DIET_UT_COLOR)
			self.txtDIE_UT.Refresh()
		else:
			debugPrint("AFE_DIEut -- else", df_shadow.AFE_DIEut)
			self.txtDIE_UT.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			self.txtDIE_UT.Refresh()
		if (df_shadow.AFE_DIEot[AFE_Id] == '1'):
			self.txtDIE_OT.SetBackgroundColour(const.DIET_OT_COLOR) 
			self.txtDIE_OT.Refresh()
		else:
			self.txtDIE_OT.SetBackgroundColour(const.FAULT_WHITE_COLOR) 
			self.txtDIE_OT.Refresh()

		return
 

	def draw_graph(self, AFE_Id, df_shadow):
		debugPrint("draw_graph")
		self.graph_init()

		self.axes1tab2.axis([0, 6, 0, 7])
		debugPrint(str(AFE_Id))
		del self.legendCellList[:]
		self.CellList.sort()	
		# draw cell voltage lines
		for i in range(0,len(self.CellList)):

			self.y = df_shadow.AFE_cell_y[AFE_Id][self.CellList[i]-1]
			debugPrint("Cell values")
			debugPrint(self.dates)
			debugPrint(self.y)
		
			self.legendCellList.append("C%dV" % (self.CellList[i]))
			self.axes1tab2.plot(self.dates, self.y,
						        self.legendColorCellList[self.CellList[i]- 1],
						        linewidth=1.0, label='linear')

		del self.legendGpioList[:]
		self.GpioList.sort()
		# draw gpio voltage lines
		for i in range(0,len(self.GpioList)):
			# gp_index= int(self.GpioList[i])

			self.y = df_shadow.AFE_gpio_y[AFE_Id][self.GpioList[i] - 1]
			debugPrint("Gpio values")
			debugPrint(self.dates)	
			debugPrint(self.y)
		
			self.legendGpioList.append("GP%dV" % (self.GpioList[i]))
			self.axes1tab2.plot(self.dates, self.y,
						        self.legendColorGpioList[self.GpioList[i] - 1],
						        linewidth=1.0, label='linear')

		self.legendList= (self.legendCellList + self.legendGpioList)
		
		debugPrint (self.legendCellList)
		debugPrint (self.legendList)

		self.axes1tab2.legend(labels=self.legendList, fontsize=7, frameon=False,
				              bbox_to_anchor=(1.01, 1.15), loc="best")  # add label

		self.canvas1.draw()

		# update measurement values in text boxes based on response	
		self.ui_update_measurements(AFE_Id, df_shadow)
		self.ui_update_fault(AFE_Id, df_shadow)
		self.mainframe.tab[0].ui_update_system_fault(AFE_Id, 0, 0, df_shadow)

		return

 
def debugPrint(strn, val=""):
	print(f"{strn} : {val}")
	return	

