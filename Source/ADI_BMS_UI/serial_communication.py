# coding: utf-8
import serial
import time

max_boards =128

serial_connection=None
response_cnt = 0

def serial_com_connect(port_num,baud_rate):
    global serial_connection
    try:
        serial_connection = serial.Serial(
            port=str(port_num),
            baudrate=baud_rate,
            parity=serial.PARITY_NONE,
            bytesize=serial.EIGHTBITS,
            timeout=1,
            stopbits=serial.STOPBITS_ONE,
            write_timeout=4)

    except ValueError:
        debugPrint("\nParameter are out of range, e.g. baud rate ...")

    except serial.SerialException:
        debugPrint("\nDevice can not be found or can not be configured."
              "\nCheck the PORT number")

    return serial_connection

def serial_com_disconnect():

    if serial_connection.is_open == True: 	
        serial_connection.close()

    return


def serial_com_write(data_command):
    try:
        data_to_send = bytes.fromhex(data_command)       
        serial_connection.write(data_to_send)

    except Exception as error:
        debugPrint("Error: serial_com_write::", error)

    return


def serial_com_read(len_to_read):
    remaining_len = len_to_read
    output_data = ""

    try:
        while remaining_len > 0:
            read_data = serial_connection.read(remaining_len).decode("utf-8")
            output_data += read_data
            remaining_len = remaining_len - len(read_data)
            time.sleep(0.001)

    except Exception as error:
        debugPrint("Error: serial_com_read ", error)
    
    return output_data

def serial_com_flush():
    #Flushing out serial buffer after reading 3 responses, so that it can have latest response on next read
    debugPrint("Buf size before", serial_connection.inWaiting())

    global response_cnt
    response_cnt += 1

    if response_cnt == 3:
        response_cnt=0
        serial_connection.reset_input_buffer()
    debugPrint("Buf size after", serial_connection.inWaiting())

##################### AFE Commands wrapper APIs ########################################################

def serial_com_AFE_connect():

    # send command
    AFE_connect_cmd = "424D530008010003010100FF10"
    debugPrint("send AFE connect command::")
    debugPrint("AFE_connect_cmd:", AFE_connect_cmd)

    serial_com_write(AFE_connect_cmd)

def serial_com_AFE_disconnect():
    
    global measurement_thread
    # send command
    AFE_disconnect_cmd = "424D530008010003020100FF0F"
    debugPrint("send AFE disconnect command::")
    debugPrint("AFE_disconnect_cmd:", AFE_disconnect_cmd)
    serial_com_write(AFE_disconnect_cmd)

def serial_com_AFE_config(num_boards, sample_rate, uv_val, ov_val):
     
    # AFE_config_cmd is constructed using SOF + ML + MT + CL + OP_CODE + ICC + ICN + ICT + OPT_TYPE + DL + DATA + CHECK_SUM
    SOF		= "424D53"
    ML  	= "00A0"
    MT  	= "01"
    CL  	= "009B"
    OP_CODE 	= "03"
    MP  	= "00"
    ICC 	= get_ICC_val(num_boards)	
    ICN		= get_ICN_val(num_boards)
    ICT		= get_ICT_val(num_boards)
    OPT_TYPE	= "01"
    DL		= "07"
    DATA	= get_data(sample_rate, uv_val, ov_val)
    CHECKSUM	= get_checksum(SOF + ML + MT + CL + OP_CODE + ICC + ICN + ICT + OPT_TYPE + DL + DATA)

    AFE_config_cmd = SOF + ML + MT + CL + OP_CODE + ICC + ICN + ICT + OPT_TYPE + DL + DATA + CHECKSUM
    
    debugPrint("send AFE config command:", AFE_config_cmd)
    serial_com_write(AFE_config_cmd)

    return

def serial_com_AFE_fault_detect(num_boards):
	 
	# AFE_fault_detect_cmd is constructed using SOF + ML + MT + CL + OP_CODE + ICC + ICN + ICT + OPT_TYPE + DL + DATA + CHECK_SUM
	SOF		= "424D53"
	ML  	= "00A0"
	MT  	= "01"
	CL  	= "0016"
	OP_CODE 	= "04"
	ICC 	= get_ICC_val(num_boards)	
	ICN		= get_ICN_val(num_boards)
	ICT		= get_ICT_val(num_boards)
	OPT_TYPE	= "01"
	DL		= "02"
	DATA	= "8000" ##TODO Timeinterval
	CHECKSUM	= get_checksum(SOF + ML + MT + CL + OP_CODE + ICC + ICN + ICT + OPT_TYPE + DL + DATA)

	AFE_fault_detect_cmd = SOF + ML + MT + CL + OP_CODE + ICC + ICN + ICT + OPT_TYPE + DL + DATA + CHECKSUM

	debugPrint("serial_com_AFE_fault_detect", AFE_fault_detect_cmd)
	serial_com_write(AFE_fault_detect_cmd)
    
	return


def serial_com_AFE_start_measurement(num_boards, sample_rate):
 
    ## send command
    SOF		= "424D53"			# Identifier
    ML 		= "0019"			# Message Len
    MT 		= "01"				# MessageType = Command
    CL 		= "0014"			# Command Len
    OP_CODE 	= "05"  			# Operation = Start Measurement
    ICC 	= get_ICC_val(num_boards)	# Total IC Count
    ICN		= get_ICN_val(num_boards)	# Bitmask of ICs
    OPT_TYPE 	= "01" 				# Continuous
    DL 		= "00"				# Data len
    CHECKSUM	= get_checksum(SOF + ML + MT + CL + OP_CODE + ICC + ICN + OPT_TYPE + DL)

    AFE_start_measurement_cmd = SOF + ML + MT + CL + OP_CODE + ICC + ICN + OPT_TYPE + DL + CHECKSUM
        
    debugPrint("send AFE start measurement command:", AFE_start_measurement_cmd)
    serial_com_write(AFE_start_measurement_cmd)


    return 

def serial_com_AFE_stop_measurement():
    
    ## send command
    AFE_stop_measurement_cmd = "424D5300190100140B01000000000000000000000000000000010300FEE0"
    
    debugPrint("send AFE stop measurement command:", AFE_stop_measurement_cmd)
    serial_com_write(AFE_stop_measurement_cmd)

    return


def serial_com_AFE_receive_frame():

    global is_measurement_started
    ## SOF: Detect SOF 
    SOF=""

    recv_len=6
    SOF= serial_com_read(recv_len)

    debugPrint("-- serial_com_AFE_receive_frame:")

    # read 1-byte at a time until SOF is detected
    recv_len=1	
    while (SOF != "424D53"):
        SOF= SOF + serial_com_read(recv_len)
        SOF=SOF[1:]

    debugPrint("SOF:", SOF)

    ##ML: Now receive, message len
    recv_len=4
    ML= serial_com_read(recv_len)
    messageLen = int(ML, base=16)	
    debugPrint("ML:", ML)

    #Now receive rest of message (ML bytes)
    recv_len=(messageLen*2)
    output_data= serial_com_read(recv_len)

    MT = output_data[:2]
    debugPrint("MT:", MT)

    DATA = output_data[2:((messageLen*2)-4)]
    debugPrint("DATA:", DATA)
    
    #serial_com_flush()
    # check sum
    CHECKSUM = output_data[(((messageLen*2)-4)):(messageLen*2)]
    debugPrint("CHECKSUM:", CHECKSUM)

    computeChksum = calculate_checksum(SOF+ML+MT+DATA)
    debugPrint("computeChksum:", computeChksum)

    frame = SOF+ML+MT+DATA+CHECKSUM

    '''
    if (CHECKSUM.upper() == (calculate_checksum(SOF+ML+MT+DATA)).upper()):
        frame = SOF+ML+MT+DATA+CHECKSUM
    else:
        debugPrint("Dropping Frame")
        frame=None
    '''
    return frame    

def calculate_checksum(str_checksum):

	n = 2
	out = [('0x' + str_checksum[i:i + n]) for i in range(0, len(str_checksum), n)]
	checksum_total = hex(sum([int(k, 16) for k in out]))
	checksum = int('0x10000', 16) - int(checksum_total, 16)
	final_checksum = hex(checksum)
	final_checksum = (final_checksum.replace('0x', ''))

	return final_checksum


############################ HELPER FUNCTIONS ###################################################

def get_ICC_val(num_boards):
    icc_str = "00"
    numb_of_board = num_boards
    icc = hex(numb_of_board)
    icc = icc.replace('0x', '')
    icc_val = icc_str[len(str(icc)):] + str(icc)

    return icc_val

def get_ICN_val(num_boards):
    icn_str = "00000000000000000000000000000000"
    icn_total = pow(2,num_boards) - 1
    icn = hex(int(icn_total))
    icn = icn.replace('0x', '')
    icn_val = icn_str[len(str(icn)):] + str(icn)

    return icn_val

def get_ICT_val(num_boards):
    # Based on IC type ADBMS1818 (01) or ADBMS1816(02), create array
    # TODO : Currently this function only supports ADBMS1818
    ## ICT = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01  
    ## (For count 1 -->  replace last byte 01) 
    ## (For count 2, replace last two bytes as 01 01) 
    ## (For count 3, replace last three bytes as 01 01 01) 
    num_of_board = num_boards

    string = ""
    for i in range (0, max_boards, 1):
        string+= "00"

    n = 2  # divide string in every two characters
    out = [(string[i:i + n]) for i in range(0, len(string), n)]

    list_of_indexes = [i for i in range(1, num_boards + 1)]

    new_character = '1'
    # Replace characters at index positions in list
    for i in list_of_indexes:
        for j in out:
            str_out = out[-i][:1] + new_character + out[-i][2:]
            out[-i] = str(str_out)  # replace string on specific index
            break

    ict_val = ''.join([str(elem) for elem in out])
    return ict_val


def get_data(sample_rate,uv_val, ov_val):

    # Sample rate - 2 bytes
    sample_rate = hex(int(sample_rate))
    sample_rate = str(sample_rate).replace("0x", "")

    if len(sample_rate) <= 4:
        for i in range(0, 3):
            sample_rate = "0" + sample_rate
            if len(sample_rate) == 4:
                break

    # UV - 2 bytes, OV - 2 bytes
    uv = (float(uv_val) * 10000)
    uv = hex(int(uv))
    ov = (float(ov_val) * 10000)
    ov = hex(int(ov))

    # Fault data
    # 11111 -> Request for the all fault response
    fault_data = "1F"

    # Construct final data	
    data = sample_rate + uv + ov + fault_data
    data = str(data).replace("0x", "")

    return data

def get_checksum(str_checksum):
    n = 2
    out = [('0x' + str_checksum[i:i + n]) for i in range(0, len(str_checksum), n)]
    checksum_total = hex(sum([int(_, 16) for _ in out]))
    checksum = int('0x10000', 16) - int(checksum_total, 16)
    final_checksum = hex(checksum)
    final_checksum = final_checksum.replace('0x', '')

    return final_checksum


def debugPrint(strn, val=""):
    print(f"{strn} : {val}")
    #print (strn + val)
    return


