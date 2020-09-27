CROSS_COMPILE := /usr/bin/aarch64-linux-gnu-gcc

#SUBDIRS := $(shell ls -d ./drivers/*/)
TGTDIR := $(shell pwd)/exe
TARGET := $(TGTDIR)/tcpserver
SOURCES := tcpserver.c

all: $(SOURCES)
	$(CROSS_COMPILE) -Wall -Os -o $(TARGET) $(SOURCES) -lpthread -std=gnu99

clean:
	rm -f *.o  $(TARGET)
	#$(call make_subdir , clean)


