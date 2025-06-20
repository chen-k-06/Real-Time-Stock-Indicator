CC = gcc
TARGET = indicators
C_FILES = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(C_FILES))
CFLAGS = -g -Wall -Werror -pedantic-errors
# LDFLAGS = <any linker flags>
# LDLIBS = <libraries to link in, commonly –lm, for the math library>
.PHONY: all clean

all: $(TARGET)
$(TARGET): $(OBJS) $(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

%.o: %.c %.h $(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c $(CC) $(CFLAGS) -c -o $@ $<

clean: rm -f $(OBJS) $(TARGET) $(TARGET).exe
