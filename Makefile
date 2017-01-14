#common makefile header

DIR_INC = ./xf_office/include
DIR_BIN = .
DIR_LIB = ./xf_office/libs

TARGET	= app_xf
BIN_TARGET = $(DIR_BIN)/$(TARGET)

CROSS_COMPILE =
CFLAGS = -g -Wall -I$(DIR_INC)

#可根据实际需要修改，选择所需平台SDK，默认为x64或x86平台。
LDFLAGS := -L$(DIR_LIB)/x64

#LDFLAGS := -L$(DIR_LIB)/x86

LDFLAGS += -lmsc -lm -lrt -ldl -lpthread -lasound

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

$(BIN_TARGET) : $(OBJECTS)
	-rm $(BIN_TARGET)
	$(CROSS_COMPILE)gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@rm *.o

%.o : %.c
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@

clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

#common makefile foot
