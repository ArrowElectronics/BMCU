import wx
import os
import constant as const

class RefTab(wx.Panel):

	def __init__(self, parent):
		wx.Panel.__init__(self, parent)

	def createRefTab(self):
		self.createTabHeader()

		# Background image of reference diagram
		self.reference_diag = wx.Image(os.path.join('reference_diag.png'), 
				wx.BITMAP_TYPE_ANY).ConvertToBitmap()

		self.reference_diag_bitmap = wx.StaticBitmap(
				self,
				wx.ID_ANY,
				self.reference_diag ,
				pos=(180, 100),
				size=(self.reference_diag.GetWidth(), self.reference_diag.GetHeight()))

		font = wx.Font(pointSize=11, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL, weight=wx.FONTWEIGHT_BOLD,
				underline=True)
				
		self.linkbutton = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'Click here for Schematics', pos=(45, 450), size=(200, 55), style=wx.BORDER_NONE)
		self.linkbutton.SetFont(font)
		self.linkbutton.SetOwnForegroundColour("blue")
		self.linkbutton.SetBackgroundColour("white")
		self.linkbutton.Bind(wx.EVT_BUTTON, self.linkbutton_Schematics) ##TODO

		font = wx.Font(pointSize=8, family=wx.FONTFAMILY_DEFAULT, style=wx.FONTSTYLE_NORMAL, weight=wx.FONTWEIGHT_BOLD,
				underline=True)
		
		self.LT8609Abutton = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'Buck converter\n LT8609A', pos=(272, 42), size=(158, 46), style=wx.BORDER_NONE)
		self.LT8609Abutton.Bind(wx.EVT_BUTTON, self.Buck_converter_LT8609A)
		self.LT8609Abutton.SetBackgroundColour("light blue")

		self.LTC4415button = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'Ideal Diode\n LTC4415 ', pos=(303, 141), size=(95, 106), style=wx.BORDER_NONE)
		self.LTC4415button.Bind(wx.EVT_BUTTON, self.Ideal_Diode_LTC4415)
		self.LTC4415button.SetBackgroundColour("light blue")

		self.ADM7170button1 = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'LDO \nADM7170', pos=(439, 124), size=(75, 40), style=wx.BORDER_NONE)
		self.ADM7170button1.Bind(wx.EVT_BUTTON, self.LDO_ADM7170)
		self.ADM7170button1.SetBackgroundColour("light blue")

		self.ADM7170button2 = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'LDO \nADM7170', pos=(439, 175), size=(75, 39), style=wx.BORDER_NONE)
		self.ADM7170button2.Bind(wx.EVT_BUTTON, self.LDO_ADM7170)
		self.ADM7170button2.SetBackgroundColour("light blue")

		self.ADP323button = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'LDO \nADP323', pos=(439, 226), size=(75, 36), style=wx.BORDER_NONE)
		self.ADP323button.Bind(wx.EVT_BUTTON, self.LDO_ADP323)
		self.ADP323button.SetBackgroundColour("light blue")

		self.MAX32626button = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'MCU MAX 32626', pos=(284, 274), size=(169, 108), style=wx.BORDER_NONE)
		self.MAX32626button.Bind(wx.EVT_BUTTON, self.MCU_MAX_32626)
		self.MAX32626button.SetBackgroundColour("light blue")

		self.LTC6820button1 = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'iso SPI transceiver\n LTC6820', pos=(535, 281), size=(95, 41), style=wx.BORDER_NONE)
		self.LTC6820button1.Bind(wx.EVT_BUTTON, self.iso_SPI_transceiver_LTC6820)
		self.LTC6820button1.SetBackgroundColour("light blue")

		self.LTC6820button2 = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'iso SPI transceiver\n LTC6820', pos=(536, 330), size=(95, 42), style=wx.BORDER_NONE)
		self.LTC6820button2.Bind(wx.EVT_BUTTON, self.iso_SPI_transceiver_LTC6820)
		self.LTC6820button2.SetBackgroundColour("light blue")

		self.ADM3057button = wx.Button(self.reference_diag_bitmap, wx.ID_ANY, 'iso CAN\n transceiver\n ADM3057', pos=(419, 432), size=(88, 54), style=wx.BORDER_NONE)
		self.ADM3057button.Bind(wx.EVT_BUTTON, self.iso_SPI_transceiver_LTC6820)
		self.ADM3057button.SetBackgroundColour("light blue")

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

	def linkbutton_Schematics(self, event):
		debugPrint("link of scematics - clicked")
		wx.LaunchDefaultBrowser(const.link_of_Schematics_str)
		
	def Buck_converter_LT8609A(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.Buck_converter_LT8609A_str)
		
	def Ideal_Diode_LTC4415(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.Ideal_Diode_LTC4415_str)
		
	def LDO_ADM7170(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.LDO_ADM7170_str)
		
	def LDO_ADP323(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.LDO_ADP323_str)
		
	def MCU_MAX_32626(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.MCU_MAX_32626_str)

	def iso_CAN_transceiver_ADM3057(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.iso_CAN_transceiver_ADM3057_str)

	def iso_SPI_transceiver_LTC6820(self, event):
		debugPrint( "Button pressed.")
		wx.LaunchDefaultBrowser(const.iso_SPI_transceiver_LTC6820_str)


def debugPrint(strn, val=""):
	print(f"{strn} : {val}")
	return