TARGET = ccube

CC = clang
SRCS  = ./main.c
CFILES = ./*.c
OBJS = $(SRCS:.c=.o)
OBJS_TO_DELETE = $(CFILES:.c=.o)
DEPS_TO_DELETE = $(CFILES:.c=.d)

INCLUDES = -I./include
CCFLAGS = -O2 -Wall $(INCLUDES)
LDFLAGS = -lm


all: $(TARGET)

$(TARGET): $(OBJS) $(HEADS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

run: all
	@./$(TARGET)

.PHONY: depend clean
depend:
	$(CC) $(INCLUDES) -MM $(SRCS) > $(DEPS)
	@sed -i -E "s/^(.+?).o: ([^ ]+?)\1/\2\1.o: \2\1/g" $(DEPS)

clean:
	$(RM) $(OBJS_TO_DELETE) $(DEPS_TO_DELETE) $(TARGET)

