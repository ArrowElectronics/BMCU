
class DataFrame:
    
	def __init__(self, board_num):
		
		self.board_num = board_num
		self.MSGPAYLOAD = []

		# Initialize lists - 7 points for each list, 18 Cells, 10 GPIOs 10, 6 stats, 
		self.AFE_cell_y = [[[0 for x in range (7)] for z in range (18)] for k in range (board_num)]
		self.AFE_gpio_y = [[[0 for x in range (7)] for z in range (9)] for k in range (board_num)]
		self.AFE_stat_y = [[[0 for x in range (7)] for z in range (7)] for k in range (board_num)]

		self.AFE_stack_v = [[0 for x in range (7)] for k in range (board_num)]

		# Faults lists
		self.AFE_cell_uv_fault_list=[[ 0 for z in range (18)] for k in range (board_num)]
		self.AFE_cell_ov_fault_list=[[ 0 for z in range (18)] for k in range (board_num)]
		self.AFE_cell_ow_fault_list=[[ 0 for z in range (18)] for k in range (board_num)]
		self.AFE_gpio_uv_fault_list=[[ 0 for z in range (9)] for k in range (board_num)]
		self.AFE_gpio_ov_fault_list=[[ 0 for z in range (9)] for k in range (board_num)]

		self.AFE_VAuv = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_VAov = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_VDuv = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_VDov = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_STKuv = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_STKov = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_DIEot = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_DIEut = [[0 for x in range (1)] for k in range (board_num)]

		self.AFE_SPIfail = [[0 for x in range (1)] for k in range (board_num)]
		self.AFE_COMMfail= [[0 for x in range (1)] for k in range (board_num)]
		
		return

	def parseFrame(self,Response):

		self.Response = Response

		self.parseTLFrame()
		self.parseALFrame()

		#self.updateICIndex()
		if ( int(self.OP,base=16) == 5 ): 
			self.parseMeasurementData()	

		if ( int(self.OP, base=16) == 4):
			self.parseFaultData()	
	

		##TODO: Parse frame based on message type and opcode. 
		##       Currently only suppports opcode=5 (measurement)


	def validateFrame():
		##TODO: Checksum validation and any other general validation as needed
		return


	def parseTLFrame(self):

		# Transport header 
		SOFBASE = 0   
		self.SOF = self.Response[SOFBASE:(SOFBASE+6)]	# Refer Protocol specifications for offsets
		self.ML  = self.Response[(SOFBASE+6):(SOFBASE+10)]		

		# payload
		TLPAYLOADLBASE = (SOFBASE+10)
		TLPAYLOADLEN = int(self.ML, base=16) - 2  # adjust length 2 bytes checksum 
		self.TLPAYLOAD = self.Response[TLPAYLOADLBASE : (TLPAYLOADLBASE+(TLPAYLOADLEN*2))]
		
		# Checksum
		CHKSUMBASE = (TLPAYLOADLBASE+(TLPAYLOADLEN*2))
		self.CHKSUM  = self.Response[CHKSUMBASE:(CHKSUMBASE+4)]		

	def parseALFrame(self):
		# Payload
		PLBASE = 0
		self.MT	= self.TLPAYLOAD[PLBASE:(PLBASE+2)]		# Refer Protocol specifications for offsets
		self.RL  = self.TLPAYLOAD[(PLBASE+2):(PLBASE+6)]
		self.OP  = self.TLPAYLOAD[(PLBASE+6):(PLBASE+8)]
		if self.OP != "01" and self.OP != "02":
			self.ICN  = self.TLPAYLOAD[(PLBASE+8):(PLBASE+8+32)]  
			self.updateICIndex()  
			self.RC  = self.TLPAYLOAD[(PLBASE+8+32):(PLBASE+8+32+2)]
			self.DL  = self.TLPAYLOAD[(PLBASE+8+32+2):(PLBASE+8+32+2+2)]
			debugPrint("DL : ", self.DL)
			if self.DL != "00":
				self.MSGPAYLOAD = self.TLPAYLOAD[(PLBASE+8+32+2+2): ((PLBASE+8+32+2+2) + int(self.DL,base=16)*2)]
		else :
			self.RC  = self.TLPAYLOAD[(PLBASE+8):(PLBASE+8+2)]
			self.DL  = self.TLPAYLOAD[(PLBASE+8+2):(PLBASE+8+2+2)]
			debugPrint("DL : ", self.DL)
			if self.DL != "00":
				self.MSGPAYLOAD = self.TLPAYLOAD[(PLBASE+8+2+2): ((PLBASE+8+2+2) + int(self.DL,base=16)*2)]		


	def parseMeasurementData(self):
		T1BASE=0
		self.T1  = self.MSGPAYLOAD[T1BASE:(T1BASE+2)]
		self.T1L  = self.MSGPAYLOAD[(T1BASE+2):(T1BASE+4)]
		self.T1V  = self.MSGPAYLOAD[(T1BASE+4): (T1BASE+4)+(int(self.T1L, base=16)*2)]

		T2BASE=(T1BASE+4)+(2*int(self.T1L, base=16))
		self.T2  = self.MSGPAYLOAD[T2BASE:(T2BASE+2)]
		self.T2L  = self.MSGPAYLOAD[(T2BASE+2):(T2BASE+4)]
		self.T2V  = self.MSGPAYLOAD[(T2BASE+4): (T2BASE+4)+(int(self.T2L, base=16)*2)]

		T4BASE=(T2BASE+4)+(2*int(self.T2L, base=16))
		self.T4  = self.MSGPAYLOAD[T4BASE:(T4BASE+2)]
		self.T4L  = self.MSGPAYLOAD[(T4BASE+2):(T4BASE+4)]
		self.T4V  = self.MSGPAYLOAD[(T4BASE+4): (T4BASE+4)+(int(self.T4L, base=16)*2)] 

		# Prepare lists - self.AFE_stack_v, self.AFE_cell_y, self.AFE_gpio_y, self.AFE_stat_y
		self.prepareCellVoltageList()
		self.prepareGPIOVoltageList()
		self.prepareStatList()  

		return

	def parseFaultData(self):
		self.prepareCellVoltageFaultList()
		self.prepareGpioVoltageFaultList()
		self.prepareCellOpenWireFaultList()
		self.prepareOtherFaultList()
		self.prepareSystemFaultList()		

		return				

	def printParsedFrame(self):
		debugPrint("Response:", self.Response)
		debugPrint("SOF:", self.SOF)
		debugPrint("ML:", self.ML)
		debugPrint("TLPAYLOAD:",self.TLPAYLOAD)
		debugPrint("MT:", self.MT)
		debugPrint("RL:", self.RL)
		debugPrint("OP:", self.OP)
		debugPrint("ICN:", self.ICN)
		debugPrint("RC:", self.RC)
		debugPrint("DL:", self.DL)
		debugPrint("MSGPAYLOAD:", self.MSGPAYLOAD)		
		debugPrint("T1:", self.T1)
		debugPrint("T1L:", self.T1L)
		debugPrint("T1V:", self.T1V)  
		debugPrint("T2:", self.T2)
		debugPrint("T2L:", self.T2L)
		debugPrint("T2V:", self.T2V) 
		debugPrint("T4:", self.T4)
		debugPrint("T4L:", self.T4L)
		debugPrint("T4V:", self.T4V)		
		debugPrint("CHKSUM:", self.CHKSUM)

	def updateICIndex(self):
		# find position of "1" in ICN 
		b_str = bin(int(self.ICN, base=16)).replace('0b', '')
		self.ICIndex = (len(b_str) - b_str.rfind("1")-1)

		return
	
	############################# Measurement data specific functions ##################################	
	
	def prepareCellVoltageList(self):
		
		cell_data = self.T1V

		# Cell voltage list
		res = [cell_data[i:i + 4] for i in range(0, len(cell_data), 4)]
		del res[3:len(res):4] ## What is purpose of this? Delete every 4th element from list
		resp = [i[len(i) // 2:] + i[:len(i) // 2] for i in res]  ## swap bytes
		cell_v_list = [round((int(("0x" + i), 0) * 0.0001), 2) for i in resp]

		debugPrint("IC Index:", self.ICIndex)

		# update board specific measurement list self.AFE_cell_y
		for z in range (0,len(cell_v_list)):			
			self.AFE_cell_y[self.ICIndex][z].append(cell_v_list[z])			
			if len(self.AFE_cell_y[self.ICIndex][z]) > 7:
				del self.AFE_cell_y[self.ICIndex][z][0]

		# update Sum total stack voltage using most recent value AFE_y[k][z][6]
		for k in range (0,self.board_num):
			stack_val = 0
			for z in range (0,len(cell_v_list)):
				stack_val=stack_val + self.AFE_cell_y[k][z][6]

			self.AFE_stack_v[k].append(stack_val)

			# Delete most oldest
			if len(self.AFE_stack_v[k]) > 7:
				del self.AFE_stack_v[k][0]	

		return

	def prepareGPIOVoltageList(self):

		GPIO_data = self.T2V		
		
		# GPIO voltage list
		res = [GPIO_data[i:i + 4] for i in range(0, len(GPIO_data), 4)]
		del res[3:len(res):4] # Delete every 4th element from list
		res = res[:len(res) - 2]
		del res[5]
		resp = [i[len(i) // 2:] + i[:len(i) // 2] for i in res]
		gpio_v_list = [round((int(("0x" + i), 0) * 0.0001), 7) for i in resp] 

		# update board specific measurement list self.AFE_gpio_y
		for z in range (0,len(gpio_v_list)):			
			self.AFE_gpio_y[self.ICIndex][z].append(gpio_v_list[z])			
			if len(self.AFE_gpio_y[self.ICIndex][z]) > 7:
				del self.AFE_gpio_y[self.ICIndex][z][0]

		return

	def prepareStatList(self):

		stat_data = self.T4V

		res = [stat_data[i:i + 4] for i in range(0, len(stat_data), 4)]
		del res[3:len(res):4]
		resp = [i[len(i) // 2:] + i[:len(i) // 2] for i in res]
		
		SC = round((int(("0x" + resp[0]), 0) * 0.0001 * 30), 4)
		ITMP = round(((int(("0x" + resp[1]), 0) * (0.0001 / 0.0076)) - 276), 4)		
		VA = round((int(("0x" + resp[2]), 0) * 0.0001), 4)		
		VD = round((int(("0x" + resp[3]), 0) * 0.0001), 4)		
		REV = ((int(resp[5][2:], 16) & int("0xF0", 16)) >> 4)
		THRESHOLD = (int(resp[5][2:], 16) & int("0x01", 16))
		MUXFAIL = ((int(resp[5][2:], 16) & int("0x02", 16)) >> 1)
		if MUXFAIL == 0:
			MUXFAIL=False
		else:
			MUXFAIL=True
 
		stat_v_list = [ SC, ITMP, VA, VD, REV, MUXFAIL, THRESHOLD] 

		# update board specific measurement list self.AFE_stat_y
		for z in range (0,len(stat_v_list)):			
			self.AFE_stat_y[self.ICIndex][z].append(stat_v_list[z])			
			if len(self.AFE_stat_y[self.ICIndex][z]) > 7:
				del self.AFE_stat_y[self.ICIndex][z][0]

		return


	############################ Fault specific functions ###########################

	def prepareCellVoltageFaultList(self):
		# Cell OV, UV	
		ovuvdata = self.MSGPAYLOAD[0:16] # byte 0 - 7
		
		out = [(ovuvdata[i:i+2]) for i in range(0, len(ovuvdata), 2)]

		fs=""

		for byteval in out:

			tmp= (bin(int(byteval,16))).replace('0b',"").zfill(8) # convert byte into binary
			tmp = tmp[::-1] # reverse for little endian format

			fs = fs+tmp # accumulate all bits

		# seperate out uv and ov data
		uv = fs[::2]
		ov = fs[1:][::2]
		
		# Prepare UV list		
		for bit, val in enumerate(uv) :
			if bit >= 18:
				break
			self.AFE_cell_uv_fault_list[self.ICIndex][bit]=val

		debugPrint("Cell uv fault list:", self.AFE_cell_uv_fault_list)

		# Prepare OV list
		for bit, val in enumerate(ov) :
			if bit >= 18:
				break
			self.AFE_cell_ov_fault_list[self.ICIndex][bit]=val

		debugPrint("Cell ov fault list:", self.AFE_cell_ov_fault_list)

		return



	def prepareGpioVoltageFaultList(self):
		# Cell OV, UV	
		ovuvdata = self.MSGPAYLOAD[16:32] # byte 8 - 15
		
		out = [(ovuvdata[i:i+2]) for i in range(0, len(ovuvdata), 2)]

		fs=""

		for byteval in out:

			tmp= (bin(int(byteval,16))).replace('0b',"").zfill(8) # convert byte into binary
			tmp = tmp[::-1] # reverse for little endian format

			fs = fs+tmp # accumulate all bits

		# seperate out uv and ov data
		uv = fs[::2]
		ov = fs[1:][::2]

		# Prepare UV list
		for bit, val in enumerate(uv) :
			if bit >= 9:
				break
			self.AFE_gpio_uv_fault_list[self.ICIndex][bit]=val

		debugPrint("GPIO uv fault list:" , self.AFE_gpio_uv_fault_list)

		
		# Prepare OV list
		for bit, val in enumerate(ov) :
			if bit >= 9:
				break
			self.AFE_gpio_ov_fault_list[self.ICIndex][bit]=val

		debugPrint ("GPIO ov fault list:", self.AFE_gpio_ov_fault_list)

		return


	def prepareOtherFaultList(self):
		
		# other faults, 1 byte info, all other bytes are reserved
		otherdata = self.MSGPAYLOAD[32:48] # byte 16 - 23

		out = [(otherdata[i:i+2]) for i in range(0, len(otherdata), 2)]

		oth=""

		for byteval in out:

			tmp= (bin(int(byteval,16))).replace('0b',"").zfill(8) # convert byte into binary
			tmp = tmp[::-1] # reverse for little endian format

			oth = oth+tmp # accumulate all bits

		# Prepare ow list
		other_fault_list=[[ 0 for z in range (8)] for k in range (self.board_num)]
		for bit, val in enumerate(oth) :
			if bit >= 8:
				break
			other_fault_list[self.ICIndex][bit]=val


		debugPrint ("Other fault list:", other_fault_list)

		
		self.AFE_VAuv[self.ICIndex] = other_fault_list[self.ICIndex][0]
		self.AFE_VAov[self.ICIndex] = other_fault_list[self.ICIndex][1]
		self.AFE_VDuv[self.ICIndex] = other_fault_list[self.ICIndex][2]
		self.AFE_VDov[self.ICIndex] = other_fault_list[self.ICIndex][3]
		self.AFE_STKuv[self.ICIndex] = other_fault_list[self.ICIndex][4]
		self.AFE_STKov[self.ICIndex] = other_fault_list[self.ICIndex][5]
		self.AFE_DIEut[self.ICIndex] = other_fault_list[self.ICIndex][6]
		self.AFE_DIEot[self.ICIndex] = other_fault_list[self.ICIndex][7]
		
		return


	def prepareCellOpenWireFaultList(self):
		# Cell OpenWire
		owdata = self.MSGPAYLOAD[48:64] # byte 24 - 31
		
		out = [(owdata[i:i+2]) for i in range(0, len(owdata), 2)]
		debugPrint ("Cell open-wire  ", out)

		ow=""

		for byteval in out:

			tmp= (bin(int(byteval,16))).replace('0b',"").zfill(8) # convert byte into binary
			tmp = tmp[::-1] # reverse for little endian format

			ow = ow+tmp # accumulate all bits

		# Prepare ow list
		for bit, val in enumerate(ow) :
			if bit >= 18:
				break
			self.AFE_cell_ow_fault_list[self.ICIndex][bit]=val

		debugPrint ("Cell open-wire fault list:", self.AFE_cell_ow_fault_list)

		return


	def prepareSystemFaultList(self):
		
		# other faults, 1 byte info, all other bytes are reserved
		sysdata = self.MSGPAYLOAD[64:80] # byte 32 - 39

		out = [(sysdata[i:i+2]) for i in range(0, len(sysdata), 2)]

		sys=""

		for byteval in out:

			tmp= (bin(int(byteval,16))).replace('0b',"").zfill(8) # convert byte into binary
			tmp = tmp[::-1] # reverse for little endian format

			sys = sys+tmp # accumulate all bits

		# Prepare ow list
		sys_fault_list=[[ 0 for z in range (2)] for k in range (self.board_num)]
		for bit, val in enumerate(sys) :
			if bit >= 2:
				break
			sys_fault_list[self.ICIndex][bit]=val
		
		debugPrint("System fault list:",  sys_fault_list)

		self.AFE_SPIfail[self.ICIndex] = sys_fault_list[self.ICIndex][0]
		self.AFE_COMMfail[self.ICIndex] = sys_fault_list[self.ICIndex][1]

		# OUTPUT: self.AFE_SPIfail, self.AFE_COMMfail 

		return

	
	def doCleanup(self):
		del self.AFE_cell_y
		del self.AFE_gpio_y
		del self.AFE_stat_y
		del self.AFE_stack_v[:]

		return


def debugPrint(strn, val=""):
	print(f"{strn} : {val}")
	return
	



	

	
