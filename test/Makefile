
CC ?= gcc

# source directory
SRC_DIR = .

# flags for debugging
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -g -DDEBUG
else
CFLAGS += -O3
endif

# flags
CFLAGS += -Wall -Wextra
LDFLAGS += -L.. -Wl,-rpath=..

# includes and libraries
INCS = -I../src
LIBS = -lringbuff

# source, object and output
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)
OUTPUTS = $(SRC:.c=.bin)

all: $(OUTPUTS)

%.bin: $(OBJ)
	$(CC) $(@:.bin=.o) $(LDFLAGS) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -o $@ -c $<

clean:
	rm -f $(SRC_DIR)/*.o *.bin

run-tests:
	@find -name "*.bin" -exec valgrind --leak-check=full --show-leak-kinds=all {} \;
