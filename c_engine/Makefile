CC = gcc
TARGET = indicators
C_FILES = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(C_FILES))
CFLAGS = -g -Wall -Werror -pedantic-errors
LDLIBS = -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $^ $(LDLIBS)

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f $(OBJS) $(TARGET) $(TARGET).exe
