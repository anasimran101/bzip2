# all     - Compile complete project
# clean   - Remove object files and executable
# windows - Cross compile for Windows

CC = gcc
CFLAGS = -Wall -O2

TARGET = bzip2_impl

INC_DIR    = include
SOURCE_DIR = src
OBJECT_DIR = build

SOURCES = \
	$(SOURCE_DIR)/main.c \
	$(SOURCE_DIR)/config.c \
	$(SOURCE_DIR)/ini.c

OBJECTS = \
	$(OBJECT_DIR)/main.o \
	$(OBJECT_DIR)/config.o \
	$(OBJECT_DIR)/ini.o

all: $(OBJECT_DIR) $(OBJECT_DIR)/$(TARGET)

$(OBJECT_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(OBJECT_DIR)/$(TARGET)

windows:
	$(MAKE) CC=x86_64-w64-mingw32-gcc TARGET=bzip2_impl.exe