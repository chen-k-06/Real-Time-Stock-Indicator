CC = gcc
TARGET = indicators.so
C_FILES = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(C_FILES))
CFLAGS = -g -Wall -Werror -pedantic-errors -fPIC
LDLIBS = -lm
LDFLAGS = -shared

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

