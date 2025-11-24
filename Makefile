CC = clang
SRCS  = ./main.c
CFILES = ./*.c
TARGET = ccube
OBJS = $(SRCS:.c=.o)
OBJS_TO_DELETE = $(CFILES:.c=.o)
DEPS_TO_DELETE = $(CFILES:.c=.d)

INCLUDES = -I./include
CCFLAGS = -O4 -Wall $(INCLUDES)
LDFLAGS = -lm

$(TARGET): $(OBJS) $(HEADS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS_TO_DELETE) $(DEPS_TO_DELETE) $(TARGET)

