CROSS_COMPILE := /opt/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

#SUBDIRS := $(shell ls -d ./drivers/*/)
TGTDIR := $(shell pwd)/exe
TARGET := $(TGTDIR)/tcpserver
SOURCES := tcpserver.c

all: $(SOURCES)
	$(CROSS_COMPILE) -Wall -Os -o $(TARGET) $(SOURCES) -lpthread -std=gnu99

clean:
	rm -f *.o  $(TARGET)
	#$(call make_subdir , clean)


