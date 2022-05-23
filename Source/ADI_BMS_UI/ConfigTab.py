import wx
import os
import constant as const
import datetime as dt
from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import (
    FigureCanvasWxAgg as FigureCanvas)
import threading
import serial_communication as serial_com
from DataParser import DataFrame
import copy
import time

is_measurement_thread_created = False

class ConfigTab(wx.Panel):

	def __init__(self, parent, mainframe):
		wx.Panel.__init__(self, parent)

		# init variables
		self.mainframe = mainframe
		self.mainframe.board_num = 1
		self.sample_rate = 500
		self.uv_threshold = 3.1
		self.ov_threshold = 4.2
		self.axes = [None for i in range (0,3)]
		return

	def createConfigureTab(self):
		self.createTabHeader()
		self.ui_graph_panel_design()
		self.ui_system_status_panel_design()
		self.ui_configuration_panel_design()
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

	def ui_graph_panel_design(self):

		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_BOLD) 
		self.MCU_board_button = wx.Button(self, wx.ID_ANY, const.MCU_BOARD_TITLE, pos=(390, 315), size=(100, 50))
		self.MCU_board_button.SetFont(font)
		self.MCU_board_button.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.MCU_board_button.SetBackgroundColour((const.CONG_TXT_RGB_COLOR))

		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_BOLD) 
		self.BMS_AFE_1 = wx.Button(self, wx.ID_ANY, const.BMS_AFE_1_TITLE, pos=(500, 150), size=(100, 50))
		self.BMS_AFE_1.SetFont(font)
		self.BMS_AFE_1.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.BMS_AFE_1.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))

		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_BOLD) 
		self.BMS_AFE_2 = wx.Button(self, wx.ID_ANY, const.BMS_AFE_2_TITLE, pos=(500, 315), size=(100, 50))
		self.BMS_AFE_2.SetFont(font)
		self.BMS_AFE_2.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.BMS_AFE_2.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))

		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_BOLD) 
		self.BMS_AFE_3 = wx.Button(self, wx.ID_ANY, const.BMS_AFE_3_TITLE, pos=(500, 490), size=(100, 50))
		self.BMS_AFE_3.SetFont(font)
		self.BMS_AFE_3.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.BMS_AFE_3.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))

		rightline = wx.StaticBox(self, id=wx.ID_ANY, pos=(380, 75), size=(752, 528))
		rightline.SetForegroundColour((const.CONG_TXT_RGB_COLOR))

		# vertical text titles of graph (images)
		self.graph1_name = wx.Image(os.path.join('Stack_Voltage_AFE1_.png'),  
				       wx.BITMAP_TYPE_ANY).ConvertToBitmap()        
		self.graph1 = wx.StaticBitmap(
			self,
			wx.ID_ANY,
			self.graph1_name ,
			pos=(596, 101 ),
			size=(70, 150))

		self.graph2_name = wx.Image(os.path.join('Stack_Voltage_AFE2_.png'),  
				       wx.BITMAP_TYPE_ANY).ConvertToBitmap()        
		self.graph2 = wx.StaticBitmap(
			self,
			wx.ID_ANY,
			self.graph2_name ,
			pos=(598, 274 ),
			size=(62, 147))

		self.graph3_name = wx.Image(os.path.join('Stack_Voltage_AFE3_.png'),  
				       wx.BITMAP_TYPE_ANY).ConvertToBitmap()        
		self.graph3 = wx.StaticBitmap(
			self,
			wx.ID_ANY,
			self.graph3_name ,
			pos=(600, 438 ),
			size=(60, 149))

		#-------------------------
		# 3 graphs
		self.y1_list = const.STACK_GRAPH_Y1_LIST
		self.y2_list = const.STACK_GRAPH_Y2_LIST
		self.y3_list = const.STACK_GRAPH_Y3_LIST
		self.x_dates = [(dt.datetime.now() + dt.timedelta(seconds=i)).strftime('%H:%M:%S') for i in
				        range(7)]
		# declare figure graph for tab 1
		self.figure = Figure(figsize=(5.8, 5.8), dpi=70, facecolor='white', edgecolor='WHITE')
		self.figure.autofmt_xdate(bottom=0.2, rotation=90, ha='center')
		self.axes[0] = self.figure.add_subplot(311)
		self.axes[1] = self.figure.add_subplot(312)
		self.axes[2] = self.figure.add_subplot(313)

		self.axes[0].set_yticks(const.STACK_GRAPH_Y_LBL_LIST)

		self.axes[0].legend(['Stack Voltage (V)'], fontsize=8,
				          frameon=False, bbox_to_anchor=(0.95, 0.95))  # add label
		self.axes[0].set_ylabel(const.Graph_y_label, fontsize=8, color='black', loc="center")
		self.axes[0].set_xlabel(const.Graph_x_label, fontsize=8, color='black', loc="center") 

		self.axes[0].set_xticklabels(self.x_dates)
		self.axes[0].grid()

		self.axes[1].set_yticks(const.STACK_GRAPH_Y_LBL_LIST)

		self.axes[1].legend(['Stack Voltage (V)'], fontsize=8,
				          frameon=False, bbox_to_anchor=(0.95, 0.95))  # add label
		self.axes[1].set_ylabel(const.Graph_y_label, fontsize=8, color='black', loc="center")
		self.axes[1].set_xlabel(const.Graph_x_label, fontsize=8, color='black', loc="center")

		self.axes[1].set_xticklabels(self.x_dates)
		self.axes[1].grid()

		self.axes[2].set_yticks(const.STACK_GRAPH_Y_LBL_LIST)

		self.axes[2].legend(['Stack Voltage (V)'], fontsize=8,
				          frameon=False, bbox_to_anchor=(0.95, 0.95))  # add label
		self.axes[2].set_ylabel(const.Graph_y_label, fontsize=8, color='black', loc="center")
		self.axes[2].set_xlabel(const.Graph_x_label, fontsize=8, color='black', loc="center")

		self.axes[2].set_xticklabels(self.x_dates)
		self.axes[2].grid()

		self.axes[0].spines['top'].set_color('0.5')
		self.axes[0].spines['bottom'].set_color('0.5')
		self.axes[0].spines['left'].set_color('0.5')
		self.axes[0].spines['right'].set_color('0.5')

		self.canvas = FigureCanvas(self, -1, self.figure)
		self.canvas.SetSize((400, 500))
		self.canvas.SetPosition((650, 100))
		self.figure.tight_layout(h_pad=1.5)  # add horizontal padding between 2 subplots
		self.canvas.draw()
		return

	def ui_system_status_panel_design(self):	
		
		statusbox_line = wx.StaticBox(self, id=wx.ID_ANY, pos=(23, 450), size=(320, 153))
		font_statusbox_title = wx.Font(pointSize=15, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL,
						  weight=wx.FONTWEIGHT_BOLD, underline=False)
		statusbox_title = wx.StaticText(self, label=const.STATUSBOX_TITLE, pos=(80, 480), size=(20,40), style=wx.TE_RICH)
		statusbox_title.SetForegroundColour((const.CONG_TXT_RGB_COLOR))
		statusbox_title.SetFont(font_statusbox_title)

		font_statusbox_title = wx.Font(pointSize=10, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL,
						  weight=wx.FONTWEIGHT_NORMAL, underline=False)
		System_Operational_title = wx.StaticText(self, label=const.STATUSBOX_System_Operational, pos=(105, 520), style=wx.TE_RICH)
		System_Operational_title.SetFont(font_statusbox_title)

		SPI_line_status = wx.StaticText(self, label=const.STATUSBOX_SPI_line_status, pos=(105, 560), style=wx.TE_RICH)
		SPI_line_status.SetFont(font_statusbox_title)

		self.textboxGreen = wx.TextCtrl(self, id=wx.ID_ANY, pos=(70, 520), size=(20, 20), style=wx.TE_READONLY) 
		self.txtspifault = wx.TextCtrl(self, id=wx.ID_ANY, pos=(70, 560), size=(20, 20), style=wx.TE_READONLY)
		return


	def ui_configuration_panel_design(self):

		config_box = wx.StaticBox(  
			self,
			wx.ID_ANY,
			pos=(23, 70),
			size=(320, 370))

		config_box.SetForegroundColour((const.CONG_TXT_RGB_COLOR))

		lbl_box_title = wx.Font(pointSize=18, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL,
				          weight=wx.FONTWEIGHT_BOLD, underline=False)
		config_box_title = wx.StaticText(self, label=const.CONFIG_BOX_TITLE, pos=(100, 85), size=(20,40),style=wx.TE_RICH)
		config_box_title.SetForegroundColour((const.CONG_TXT_RGB_COLOR))
		config_box_title.SetFont(lbl_box_title)

		font = wx.Font(10, wx.DECORATIVE, wx.NORMAL, wx.FONTWEIGHT_BOLD)

		# Select COM port
		x_size = 58
		x_size_drop_down = 220
		self.SelectComPort = wx.StaticText(self, label="Select COM Port", pos=(x_size, 142), style=0,
				                           size=(1, 1))  # defualt checked
		self.SelectComPort.SetFont(font)
		self.com_dropdownlist = wx.ComboBox(self, id=wx.ID_ANY, value="", pos=(x_size_drop_down, 138), size=(90, 20),
				                        choices=const.n_com_list,
				                        style=wx.CB_DROPDOWN)
		self.com_dropdownlist.SetFont(font)
		self.com_dropdownlist.SetValue(const.n_com_list[0])

		# Select Baud rate
		self.SelectBaudRate = wx.StaticText(self, label="Select Baud Rate", pos=(x_size, 174), style=0,
				                            size=(1, 1))  # defualt checked
		self.SelectBaudRate.SetFont(font)
		self.baud_rate_dropdownlist = wx.ComboBox(self, id=wx.ID_ANY, value="",
				                                  pos=(x_size_drop_down, 172), size=(90, 20), choices=const.baud_rate_list,
				                                  style=wx.CB_DROPDOWN)
		self.baud_rate_dropdownlist.SetFont(font)
		self.baud_rate_dropdownlist.SetValue(const.baud_rate_list[0])

		# Select Number of boards
		self.SelectNoOfBoards = wx.StaticText(self, label="Select No. of Boards", pos=(x_size, 212), style=0,
				                              size=(1, 1))  # defualt checked
		self.SelectNoOfBoards.SetFont(font)
		self.num_board_list = wx.SpinCtrlDouble(self, id=wx.ID_ANY, pos=(x_size_drop_down, 207), size=(100, 25), value="1",
				                                style=wx.SP_ARROW_KEYS | wx.ALIGN_LEFT, min=1, max=3, initial=1,
				                                inc=1)
		self.num_board_list.SetFont(font)

		# Select Sample rate
		self.SampleRate = wx.StaticText(self, label="Sample rate", pos=(x_size, 248))
		self.SampleRate.SetFont(font)
		self.ms = wx.StaticText(self, label="ms", pos=(x_size_drop_down+78, 246))
		self.ms.SetFont(font)
		self.sample_rate_dropdown = wx.SpinCtrlDouble(self, id=wx.ID_ANY, pos=(x_size_drop_down, 246), size=(70, 22),
				                                      value="{const.sample_rate_value}",
				                                      style=wx.SP_ARROW_KEYS | wx.ALIGN_LEFT, min=0, max=1000,
				                                      initial=500, inc=10)
		self.sample_rate_dropdown.SetFont(font)

		# Set under voltage threshold
		self.Undervol = wx.StaticText(self, label="Under Voltage", pos=(x_size, 320))
		self.Undervol.SetFont(font)
		self.Txtunder_voltage = wx.TextCtrl(self, value="3.1", id=wx.ID_ANY, pos=const.pos_val_UV, size=(35, 24))
		self.Txtunder_voltage.SetFont(font)

		# Set over voltage threshold
		self.Overvol = wx.StaticText(self, label="Over Voltage", pos=(x_size, 285))
		self.Overvol.SetFont(font)
		self.Txtover_voltage = wx.TextCtrl(self, value="4", id=wx.ID_ANY, pos=const.pos_val_OV, size=(35, 24))
		self.Txtover_voltage.SetFont(font)

		# Start/stop measurement button
		self.START_button = wx.Button(self, wx.ID_ANY, const.BTN_START_NAME, pos=(190, 364), size=(125, 32))
		self.START_button.SetFont(font)
		self.START_button.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.START_button.SetBackgroundColour((const.CONG_TXT_RGB_COLOR))
		self.START_button.Bind(wx.EVT_BUTTON, self.config_start_button) ##TODO
		self.START_button.SetBackgroundColour((const.CONG_TXT_RGB_COLOR_DISABLE))
		self.START_button.Disable()

		# Connect/Disconnect button
		self.connectbutton = wx.Button(self, wx.ID_ANY, const.BTN_CONNECT_NAME, pos=(x_size, 364), size=(125, 32))
		self.connectbutton.SetFont(font)
		self.connectbutton.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.connectbutton.SetBackgroundColour((const.CONG_TXT_RGB_COLOR))
		self.connectbutton.Bind(wx.EVT_BUTTON, self.onConnectDisconnectButton) ##TODO

		# Board-connected status
		self.lbl_board_status = wx.Button(self, label="Not Connected", pos=(x_size + 30, 400), size=(200, 32))
		self.lbl_board_status.SetFont(font)
		self.lbl_board_status.SetForegroundColour(const.FAULT_WHITE_COLOR)
		self.lbl_board_status.SetBackgroundColour((const.RGB_RED_COLOR_CODE))

		return	


	def ui_update_graph_panel_panel_buttons(self, flag):
		if flag == 0: ## disable all
			self.BMS_AFE_1.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))
			self.BMS_AFE_2.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))
			self.BMS_AFE_3.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))
		else: ## enable as appropriate
			if self.mainframe.board_num == 1:
				self.BMS_AFE_1.SetBackgroundColour((const.BMS_AFE_TXT_MAINFRAME_RGB_COLOR))
				self.BMS_AFE_2.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))
				self.BMS_AFE_3.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))
			elif self.mainframe.board_num == 2:
				self.BMS_AFE_1.SetBackgroundColour((const.BMS_AFE_TXT_MAINFRAME_RGB_COLOR))
				self.BMS_AFE_2.SetBackgroundColour((const.BMS_AFE_TXT_MAINFRAME_RGB_COLOR))
				self.BMS_AFE_3.SetBackgroundColour((const.BMS_AFE_TXT_GRAY_COLOR))
			elif self.mainframe.board_num == 3:
				self.BMS_AFE_1.SetBackgroundColour((const.BMS_AFE_TXT_MAINFRAME_RGB_COLOR))
				self.BMS_AFE_2.SetBackgroundColour((const.BMS_AFE_TXT_MAINFRAME_RGB_COLOR))
				self.BMS_AFE_3.SetBackgroundColour((const.BMS_AFE_TXT_MAINFRAME_RGB_COLOR))
		return

	def ui_update_system_fault(self, AFE_Id, conn_status, op_status_fail, df_shadow):
		AFE_COMMfail=0
		# if AFE_COMMfail is set for any board, set error on ui
		if ( AFE_Id == 0):
			if df_shadow != None:
				for index in range(self.mainframe.board_num):
					if (df_shadow.AFE_COMMfail[AFE_Id] == '1'):
						AFE_COMMfail = 1
						break

			if (AFE_COMMfail == 1):
				self.txtspifault.SetBackgroundColour(const.ISO_SPI_FAIL_COLOR)
			else:
				self.txtspifault.SetBackgroundColour(const.FAULT_WHITE_COLOR)

			if (conn_status == 3) :
				debugPrint("Please Wait...")
				self.lbl_board_status.SetBackgroundColour((const.RGB_LIGHT_ORANGE_COLOR_CODE))
				self.lbl_board_status.SetLabel("Please Wait")
			elif (conn_status == 2) :
				self.lbl_board_status.SetBackgroundColour((const.RGB_RED_COLOR_CODE))
				self.lbl_board_status.SetLabel("Not Connected")
				self.textboxGreen.SetBackgroundColour(const.FAULT_WHITE_COLOR)
				self.txtspifault.SetBackgroundColour(const.FAULT_WHITE_COLOR)
			elif(conn_status == 1):
				self.lbl_board_status.SetBackgroundColour((const.RGB_RED_COLOR_CODE))
				self.lbl_board_status.SetLabel("Serial connection error")	
			else:						
				self.lbl_board_status.SetBackgroundColour((const.RGB_SOLID_GREEN_COLOR_CODE))
				self.lbl_board_status.SetLabel("Connected")
				# After connecting , check for AFECOM Failure
				if AFE_COMMfail == 0:
					self.txtspifault.SetBackgroundColour(const.ISO_SPI_COLOR)
					self.textboxGreen.SetBackgroundColour(const.SYS_OP_COLOR)
				
			self.lbl_board_status.Refresh()
			

		if(conn_status == 1) or ( (conn_status != 2) and (AFE_COMMfail == 1) ) :
			self.textboxGreen.SetBackgroundColour(const.SYS_OP_FAIL_COLOR)

		self.textboxGreen.Refresh()
		self.txtspifault.Refresh()
		return

	def get_config_params(self):
		self.mainframe.board_num = int(self.num_board_list.GetValue())
		self.serial_port = self.com_dropdownlist.GetValue()
		self.serial_baud_rate = self.baud_rate_dropdownlist.GetValue()
		self.serial_sample_rate = self.sample_rate_dropdown.GetValue()
		self.uv_threshold = self.Txtunder_voltage.GetValue()
		self.ov_threshold = self.Txtover_voltage.GetValue()

		debugPrint("get_option_selection", self.mainframe.board_num)
		debugPrint("get_com_port_selection", self.serial_port)
		debugPrint("get_baud_rate_selection", self.serial_baud_rate)
		debugPrint("get_sample_rate_selection", self.serial_sample_rate)
		debugPrint("get_uv_threshold", self.uv_threshold)
		debugPrint("get_ov_threshold", self.ov_threshold)

		return

	def onConnectDisconnectButton(self, event):

		if (self.connectbutton.GetLabel() == const.BTN_CONNECT_NAME) :

			# Get configuration parameter values			
			self.get_config_params()
			
			# create objects
			self.df = DataFrame(self.mainframe.board_num)

			self.ui_update_system_fault(0, 3, 0, self.df)  # AFE#, conn_sts, op_sts, AFECom_st
			self.serial_connection = serial_com.serial_com_connect(self.serial_port, self.serial_baud_rate)

			if (self.serial_connection == None):
				self.ui_update_system_fault(0, 1, 1, self.df) # AFE#, conn_sts, op_sts, AFECom_sts
				return			


			# Create all tabs
			self.mainframe.createTabsAsPerConfig()	## TODO do it once only
			self.ui_update_graph_panel_panel_buttons(1)			
			
			# AFE connect
			serial_com.serial_com_AFE_connect()
			output_data = serial_com.serial_com_AFE_receive_frame()
			self.df.parseFrame(output_data)

			error=1

			if (self.df.RC == "01"):
				
				error=0
		
				# AFE Config
				serial_com.serial_com_AFE_config(self.mainframe.board_num, self.sample_rate, self.uv_threshold, self.ov_threshold) 
				output_data = serial_com.serial_com_AFE_receive_frame()
				self.df.parseFrame(output_data)

				if (self.df.RC != "01"):
					error=1

				serial_com.serial_com_flush() # flush any unread data
				self.AFECOM_fault_detect()		

				if (self.df.RC != "01"):
					error=1
				
				if (error==0):
					self.create_measurement_thread( self.sample_rate)

			self.ui_update_system_fault(0, 0, error, self.df)  # AFE#, conn_sts, op_sts, AFECom_sts

			self.connectbutton.SetLabel(const.BTN_DISCONNECT_NAME)
			self.START_button.SetBackgroundColour((const.CONG_TXT_RGB_COLOR)) #Enable start button on connect
			self.START_button.Enable()
		else:
			#If measurement is still running, then on disconnect first stop the measurement 
			if (self.START_button.GetLabel() == const.BTN_STOP_NAME) : 
				return

			#After checking measurement, perform disconnect operation 
			self.ui_update_graph_panel_panel_buttons(0)

			serial_com.serial_com_AFE_disconnect()
			self.ui_update_system_fault(0, 2, 0, self.df)  # AFE#, conn_sts, op_sts, AFECom_sts

			# close/ Destroy
			serial_com.serial_com_disconnect()
			self.destroy_measurement_thread()
			del self.df

			self.connectbutton.SetLabel(const.BTN_CONNECT_NAME)
			self.START_button.SetBackgroundColour((const.CONG_TXT_RGB_COLOR_DISABLE)) #Disable start button on disconnect
			self.START_button.Disable()
		return

	def config_start_button(self, event):

		#Start only if connected
		if (self.connectbutton.GetLabel() == const.BTN_DISCONNECT_NAME):

			if (self.START_button.GetLabel() == const.BTN_START_NAME) :
				 
				serial_com.serial_com_AFE_start_measurement(self.mainframe.board_num, self.sample_rate)
				debugPrint("--if started before")
				self.start_measurement_thread()
				debugPrint("--if started after")
				
				self.START_button.SetLabel(const.BTN_STOP_NAME)
				self.connectbutton.SetBackgroundColour((const.CONG_TXT_RGB_COLOR_DISABLE)) #Disable connect button on start
				self.connectbutton.Disable()

				#start measurement thread
				self.draw_stack_graph()

			else:
				serial_com.serial_com_AFE_stop_measurement()
				self.stop_measurement_thread()
				serial_com.serial_connection.reset_input_buffer()

				self.START_button.SetLabel(const.BTN_START_NAME)
				self.connectbutton.SetBackgroundColour((const.CONG_TXT_RGB_COLOR)) #Enable connect button on stop
				self.connectbutton.Enable()

		return

	def stack_graph_init(self):


		for index in range (0,self.mainframe.board_num):
			self.axes[index].clear()
			self.axes[index].axis([0, 6, 0, 100])
			self.axes[index].grid()
			self.axes[index].set_ylabel(const.Graph_y_label, fontsize=8, color='black')
			self.axes[index].set_xlabel(const.Graph_x_label, fontsize=8, color='black')

		return

	def draw_stack_graph(self):
		if (self.START_button.GetLabel() == const.BTN_STOP_NAME) :

			debugPrint("draw_stack_graph")			
			# process frames and get list updated in class globals cell_v_list, gpio_v_list, stat_v_list, y
			#self.process() ##TODO This should be moved in other thread context upon receiving response

			# Copy object into shadow for plotting and ui updates
			l1 = [self.df]
			l2 = copy.deepcopy(l1)
			self.df_shadow = l2[0]
			#print self.df_shadow.AFE_cell_y
		
			# draw graph in AFE tabs			
			for index in range (0,self.mainframe.board_num):
				self.mainframe.tab[index+1].draw_graph(index, self.df_shadow)

			# draw stack graph
			self.x_dates = [(dt.datetime.now() + dt.timedelta(seconds=i)).strftime('%H:%M:%S') for i in
	                    range(7)]
			self.stack_graph_init()

			for index in range (0,self.mainframe.board_num):
				self.y1_list = self.df.AFE_stack_v[index]

				debugPrint( "Stack values")
				debugPrint( self.x_dates)
				debugPrint( self.y1_list)		

				self.axes[index].plot(self.x_dates, self.y1_list, 'b-', linewidth=1.5, label='linear')

			self.canvas.draw()	

			wx.CallLater(1000, self.draw_stack_graph)

		return

		
	def measurement_thread(self, sample_rate):

		try:
			global is_measurement_thread_created, is_measurement_started
			is_measurement_thread_created = True
			debugPrint("is_measurement_started :",  is_measurement_started)
			while is_measurement_thread_created == True:
				if is_measurement_started == True :
					output_data = serial_com.serial_com_AFE_receive_frame()
					##TODO: Validate response
					debugPrint("measurement_thread -- Response:", output_data)
					self.df.parseFrame(output_data)
					debugPrint("Index  : ", self.df.ICIndex)
					debugPrint("Opcode : ", self.df.OP)
					#if (self.df.")
#				else:
#					time.sleep(1)
		except Exception as error:
			debugPrint("Exception in measurement_thread ::", error)


	def start_measurement_thread(self):

		global is_measurement_started, measurement_thread_instance
		is_measurement_started = True  
		
	def stop_measurement_thread(self):
		global is_measurement_started
		is_measurement_started = False

	def create_measurement_thread(self, sample_rate):

		try:
			global measurement_thread_instance, is_measurement_started
			is_measurement_started = False
			measurement_thread_instance = threading.Thread(target=self.measurement_thread, args=(sample_rate,))
			debugPrint("create_measurement_thread : Measurement thread created")
			measurement_thread_instance.start()

		except Exception as error:
			debugPrint("Exception in create_measurement_thread ::", error)

	def destroy_measurement_thread(self):

		global measurement_thread_instance, is_measurement_thread_created
		is_measurement_thread_created = False #in order to close thread first while loop need to be terminated
		measurement_thread_instance.join()
		debugPrint("destroy_measurement_thread : Measurement thread destroyed")

	def AFECOM_fault_detect(self):

		# enable measurement temporarily to detect fault
		serial_com.serial_com_AFE_start_measurement(self.mainframe.board_num, self.sample_rate)

		for i in range (20):
			output_data = serial_com.serial_com_AFE_receive_frame()
			self.df.parseFrame(output_data)

			# stop once fault response is received.
			if (self.df.OP == "04"):
				break

		serial_com.serial_com_AFE_stop_measurement()
		serial_com.serial_com_flush() # flush any unread data

def debugPrint(strn, val=""):
	print(f"{strn} : {val}")

	return