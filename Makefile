CC=gcc
CFLAGS= -std=c11 -W -Wall -O2 -Iinclude/  -Wno-missing-field-initializers -Wno-unused-parameter -Wno-unused-variable
TARGET=bin/sky.exe

SRCS := $(wildcard *.c)
HDRS := $(wildcard *.h)
OBJS := $(patsubst %.c,bin/%.o,$(SRCS))

$(TARGET): $(OBJS) $(HDRS) Makefile
	@mkdir -p bin
	$(CC) $(CFLAGS) $(OBJS) -ljanet -lraylib -lGL -lGLEW -lm -lpthread -ldl -lrt -lX11 -o $(TARGET)

bin/%.o: %.c $(HDRS) Makefile
	@mkdir -p bin
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
