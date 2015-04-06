#******************************************************************************
#
# Makefile - Rules for building the Quickstart RGB.
#
# Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
#
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
#
# This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM4F120H5QR

#
# Set the processor variant.
#
VARIANT=cm4f

#
# The base directory for StellarisWare.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../drivers
VPATH+=../../../utils
VPATH+=./src

#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=../../..
IPATH+=./inc

#
# The default rule, which causes the Quickstart RGB to be built.
#
all: ${COMPILER}
all: ${COMPILER}/splitalarm.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the Quickstart RGB.
#
#${COMPILER}/splitalarm.axf: ${COMPILER}/buttons.o
${COMPILER}/splitalarm.axf: ${COMPILER}/cmdline.o
${COMPILER}/splitalarm.axf: ${COMPILER}/main.o
${COMPILER}/splitalarm.axf: ${COMPILER}/cmd.o
${COMPILER}/splitalarm.axf: ${COMPILER}/lcd44780_LP.o
${COMPILER}/splitalarm.axf: ${COMPILER}/clock.o
${COMPILER}/splitalarm.axf: ${COMPILER}/alarm.o
${COMPILER}/splitalarm.axf: ${COMPILER}/xbee.o
${COMPILER}/splitalarm.axf: ${COMPILER}/store.o
#${COMPILER}/splitalarm.axf: ${COMPILER}/rgb.o
#${COMPILER}/splitalarm.axf: ${COMPILER}/rgb_commands.o
${COMPILER}/splitalarm.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/splitalarm.axf: ${COMPILER}/uartstdio.o
${COMPILER}/splitalarm.axf: ${COMPILER}/ustdlib.o
${COMPILER}/splitalarm.axf: ${COMPILER}/crc.o
${COMPILER}/splitalarm.axf: ${ROOT}/usblib/${COMPILER}-cm4f/libusb-cm4f.a
${COMPILER}/splitalarm.axf: ${ROOT}/driverlib/${COMPILER}-cm4f/libdriver-cm4f.a
${COMPILER}/splitalarm.axf: ./src/splitalarm.ld
SCATTERgcc_splitalarm=./src/splitalarm.ld
ENTRY_splitalarm=ResetISR
CFLAGSgcc=-DTARGET_IS_BLIZZARD_RA2 -DUART_BUFFERED

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
