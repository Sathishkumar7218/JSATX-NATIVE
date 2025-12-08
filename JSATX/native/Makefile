# ------------------------------------------------------
# JSATX Native Build System
# Auto-detect correct JAVA_HOME (your system: java-21-openjdk)
# ------------------------------------------------------

JAVA_HOME ?= /usr/lib/jvm/java-21-openjdk

CC = gcc
CFLAGS = -O3 -fPIC \
	-Iinclude \
	-I$(JAVA_HOME)/include \
	-I$(JAVA_HOME)/include/linux

LDFLAGS = -shared

SRC = src/ndarray.c src/dataframe.c src/mlbase_jni.c
OBJ = $(SRC:.c=.o)

TARGET = libjsatx.so

# ------------------------------------------------------
# BUILD TARGETS
# ------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "[LINK] Building shared library: $(TARGET)"
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c include/mlbase.h
	@echo "[CC] Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[CLEAN] Removing object files and library"
	rm -f src/*.o $(TARGET)

# Install .so into Java resources so IntelliJ + Maven can load it
install:
	@echo "[INSTALL] Copying libjsatx.so to Java resources..."
	mkdir -p ../java/jsatx-java/src/main/resources/natives
	cp $(TARGET) ../java/jsatx-java/src/main/resources/natives/
	@echo "[DONE] Installed libjsatx.so"
