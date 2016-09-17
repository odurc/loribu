
CC ?= gcc

# source directory and output name
SRC_DIR = src
OUTPUT = libloribu.so

# flags for debugging
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -g -DDEBUG
else
CFLAGS += -O3
endif

# flags
CFLAGS += $(CONFIG) -Wall -Wextra -fPIC -std=gnu99
LDFLAGS += -shared

# libraries
LIBS =

# source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

$(OUTPUT): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(SRC_DIR)/*.o $(OUTPUT)*
