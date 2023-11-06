COMPILE        ?=gcc
TARGET         ?=UdpSubcontract

CC             :=$(COMPILE)
CFLAGS         ?=-Wall -c -g

INCUDIRS       ?=include/ 
SRCDIRS        ?=src/

INCLUDE        :=$(patsubst %, -I %,$(INCUDIRS))
CFILES         :=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
CFILENDIR      :=$(notdir $(CFILES))
COBJS          :=$(patsubst %, bin/%, $(CFILENDIR:.c=.o))
OBJS           :=$(COBJS)
VPATH          :=$(SRCDIRS)    

.PHONY:clean



$(TARGET): $(OBJS)
	$(CC) $^ -o  $@ -g -lpthread

$(OBJS) : bin/%.o : %.c
	$(CC) $(CFLAGS)  $< -o $@ $(INCLUDE) 

clean:
	rm -rf  $(OBJS) $(TARGET) 