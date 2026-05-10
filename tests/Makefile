CC = gcc

INC_DIR    = ../include
SOURCE_DIR = ../src
OBJECT_DIR = build
TARGET     = bzip2_test

COMMON_CFLAGS  = -Wall -I$(INC_DIR)
DEBUG_CFLAGS   = -O0 -g -fsanitize=address -fno-omit-frame-pointer
RELEASE_CFLAGS = -O2

CFLAGS ?= $(COMMON_CFLAGS) $(DEBUG_CFLAGS)

# ---- explicit sources (kept as you want) ----
SOURCES = \
	$(SOURCE_DIR)/config.c \
	$(SOURCE_DIR)/ini.c \
	$(SOURCE_DIR)/rle.c \
	$(SOURCE_DIR)/bwt.c \
	$(SOURCE_DIR)/mtf.c \
	$(SOURCE_DIR)/bzip2.c \
	$(SOURCE_DIR)/bm.c \
	test.c

# ---- convert to object files ----
OBJECTS = \
	$(OBJECT_DIR)/config.o \
	$(OBJECT_DIR)/ini.o \
	$(OBJECT_DIR)/rle.o \
	$(OBJECT_DIR)/bwt.o \
	$(OBJECT_DIR)/mtf.o \
	$(OBJECT_DIR)/bzip2.o \
	$(OBJECT_DIR)/bm.o \
	$(OBJECT_DIR)/test.o

all: $(OBJECT_DIR) $(OBJECT_DIR)/$(TARGET)

$(OBJECT_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# src files
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# test.c (special case)
$(OBJECT_DIR)/test.o: test.c | $(OBJECT_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(OBJECT_DIR)

windows:
	$(MAKE) CC=x86_64-w64-mingw32-gcc \
	        CFLAGS="$(COMMON_CFLAGS) $(RELEASE_CFLAGS)" \
	        TARGET=bzip2_test.exe