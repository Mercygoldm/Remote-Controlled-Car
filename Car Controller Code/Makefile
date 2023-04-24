# Makefile for building embedded application.
# by Brian Fraser
# Edit this file to compile extra C files into their own programs.
TARGET= driver
SOURCES= main.c driverModule.c ledModule.c audioMixer.c radio.c
PUBDIR = $(HOME)/cmpt433/Public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -Wall
# Asound Library
# - See the AudioGuide for steps to copy library from target to host.
LFLAGS = -L$(HOME)/cmpt433/Public/asound_lib_BBB
# -pg for supporting gprof profiling.
#CFLAGS += -pg
all: driver wav

driver:
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) $(LFLAGS) -lpthread -lasound

clean:
	rm -f $(OUTDIR)/$(TARGET)

systemd:
	cp *.service $(OUTDIR)/
wav:
	mkdir -p $(PUBDIR)/beatbox-wave-files/
	cp -R beatbox-wave-files/* $(PUBDIR)/beatbox-wave-files/ 
