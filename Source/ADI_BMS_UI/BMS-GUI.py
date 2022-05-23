import wx
import constant as const
from ConfigTab import ConfigTab
from BoardTab import BoardTab
from RefTab import RefTab

class MainFrame(wx.Frame):

	def __init__(self):
		self.tab=[None for i in range(0, const.MAX_TABS)]
		self.board_num = 0
		self.createUI()

	def createUI(self):
		no_resize = wx.DEFAULT_FRAME_STYLE & ~ (wx.RESIZE_BORDER | wx.MAXIMIZE_BOX)
		wx.Frame.__init__(self, None, title="ADBMS Master", size=(1180, 695), style=no_resize)
		self.Center()
		self.Layout()

		# Create a panel and notebook (tabs holder)
		p = wx.Panel(self)
		self.nb = wx.Notebook(p)

		# Set noteboook in a sizer to create the layout
		sizer = wx.BoxSizer()
		sizer.Add(self.nb, 1, wx.EXPAND)
		p.SetSizer(sizer)

		index=0
		self.tab[index] = ConfigTab(self.nb, self) ##TODO
		self.tab[index].createConfigureTab()
		self.nb.AddPage(self.tab[index], const.Main_tab_name)

		return

	def createTabsAsPerConfig(self):
		
		if 	self.tab[1] != None:
			debugPrint("ERROR: Can not create tabs. Please restart")

		for index in range (1,self.board_num+1):
			if 	self.tab[index] == None:
				self.tab[index] = BoardTab(self.nb, self)
				self.tab[index].createBoardTab()	    
				self.nb.AddPage(self.tab[index], (str(const.board_tab_name) + str(index)))

		index= self.board_num+1
		if self.tab[index] == None:
			self.tab[index] = RefTab(self.nb)
			self.tab[index].createRefTab()
			self.nb.AddPage(self.tab[index], const.reflink_tab_name)

		return


def debugPrint(strn, val=""):
	print(f"{strn} : {val}")
	return

if __name__ == "__main__":
    app = wx.App()
    MainFrame().Show()
    app.MainLoop()
