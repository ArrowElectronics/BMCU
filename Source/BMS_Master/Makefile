################################################################################
 # Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 #
 # Permission is hereby granted, free of charge, to any person obtaining a
 # copy of this software and associated documentation files (the "Software"),
 # to deal in the Software without restriction, including without limitation
 # the rights to use, copy, modify, merge, publish, distribute, sublicense,
 # and/or sell copies of the Software, and to permit persons to whom the
 # Software is furnished to do so, subject to the following conditions:
 #
 # The above copyright notice and this permission notice shall be included
 # in all copies or substantial portions of the Software.
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 # OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 # MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 # IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 # OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 # ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 # OTHER DEALINGS IN THE SOFTWARE.
 #
 # Except as contained in this notice, the name of Maxim Integrated
 # Products, Inc. shall not be used except as stated in the Maxim Integrated
 # Products, Inc. Branding Policy.
 #
 # The mere transfer of this software does not imply any licenses
 # of trade secrets, proprietary technology, copyrights, patents,
 # trademarks, maskwork rights, or any other form of intellectual
 # property whatsoever. Maxim Integrated Products, Inc. retains all
 # ownership rights.
 #
 ###############################################################################

# This is the name of the build output file
ifeq "$(PROJECT)" ""
PROJECT = BMS_Master
endif

# Specify the target processor
ifeq "$(TARGET)" ""
TARGET = MAX32625
endif

# Create Target name variables
TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)

# Select 'GCC' or 'IAR' compiler
COMPILER=GCC

#----
MXC_OPTIMIZE_CFLAGS=-Og #Optimize Level
#MXC_OPTIMIZE_CFLAGS=-Os

# Specify the board used
ifeq "$(BOARD)" ""
BOARD=EvKit_V1
endif

# This is the path to the CMSIS root directory
LIBS_DIR   = Libraries/Maxim
CMSIS_ROOT = $(LIBS_DIR)/CMSIS

# Source files for this test (add path to VPATH below)
SRCS  =  bms_main.c
SRCS  += bms_gui_comms.c
SRCS  += bms_application.c
SRCS  += bms_jumper.c
SRCS  += bms_adbms_debug_log.cpp
SRCS  += ADBMS1818.cpp
SRCS  += ADBMS181x.cpp
SRCS  += MCP2518FD.c
SRCS  += bms_gpio.c
SRCS  += bms_adc.c
SRCS  += bms_uart.c
SRCS  += bms_usb.c
SRCS  += bms_i2c.c
SRCS  += bms_dura_frc_spi.c
SRCS  += bms_arduino_spi.c
SRCS  += bms_can.c
SRCS  += bms_rtc.c
SRCS  += bms_eeprom.c
SRCS  += bms_delay.c
SRCS  += bms_debug_log.c
SRCS  += bms_adbms_lib_link.cpp

# Where to find source files for this test
VPATH = . ./Source/
VPATH += . ./Libraries/ADBMS/ADBMS1818/
VPATH += . ./Libraries/MCP2518FD/
VPATH += . ./Drivers/

# Where to find header files for this test
IPATH = . ./Source/
IPATH += . ./Libraries/ADBMS/ADBMS1818/
IPATH += . ./Libraries/MCP2518FD/
IPATH += . ./Drivers/

# Enable assertion checking for development
PROJ_CFLAGS+=-DMXC_ASSERT_ENABLE

################################################################################
# Include external library makefiles here

# Include the peripheral driver
PERIPH_DRIVER_DIR=$(LIBS_DIR)/$(TARGET_UC)PeriphDriver
include $(PERIPH_DRIVER_DIR)/periphdriver.mk

# Include the USB driver
MAXUSB_DIR=$(LIBS_DIR)/MAXUSB
include $(MAXUSB_DIR)/maxusb.mk

# Include the FreeRTOS library, and specify a local FreeRTOSConfig.h file
RTOS_CONFIG_DIR=.
RTOS_DIR=$(LIBS_DIR)/FreeRTOS
include $(RTOS_DIR)/freertos.mk

################################################################################
# Include the rules for building for this target. All other makefiles should be
# included before this one.
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk

# The rule to clean out all the build products.
distclean: clean
	$(MAKE) -C ${PERIPH_DRIVER_DIR} clean
