# ------------------------------------------------------
# JSATX Native Build System
# Auto-detect correct JAVA_HOME (your system: java-21-openjdk)
# ------------------------------------------------------

JAVA_HOME ?= /usr/lib/jvm/java-21-openjdk

CC = gcc
# Performance flags: enable OpenMP and architecture tuning for local builds
ARCH_FLAGS = -march=native -Ofast -funroll-loops
OMP_FLAGS = -fopenmp

# Use OpenBLAS if available. BLAS lib may be cleared if detection fails at configure time.
BLAS_LIBS = -lopenblas



CFLAGS = -O3 -fPIC $(ARCH_FLAGS) $(OMP_FLAGS) \
	-Iinclude \
	-I$(JAVA_HOME)/include \
	-I$(JAVA_HOME)/include/linux

SRC = src/ndarray.c src/dataframe.c src/mlbase_jni.c
OBJ = $(SRC:.c=.o)

# ------------------------------------------------------
# Platform specific overrides
# ------------------------------------------------------


# Detect host platform by default; allow overrides via PLATFORM variable
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Linux)
  ifeq ($(UNAME_M),x86_64)
    DEFAULT_PLATFORM = linux-x86_64
  endif
  ifeq ($(UNAME_M),aarch64)
    DEFAULT_PLATFORM = linux-arm64
  endif
endif
ifeq ($(UNAME_S),Darwin)
  ifeq ($(UNAME_M),x86_64)
    DEFAULT_PLATFORM = macos-x86_64
  endif
  ifeq ($(UNAME_M),arm64)
    DEFAULT_PLATFORM = macos-arm64
  endif
endif

# Support explicit windows detection when running on MSYS2/MinGW or Windows CI
ifeq ($(UNAME_S),MSYS_NT-10.0)
  ifeq ($(UNAME_M),x86_64)
    DEFAULT_PLATFORM = windows-x86_64
  endif
endif

PLATFORM ?= $(DEFAULT_PLATFORM)

# Choose target filename and linker flags per platform
ifneq (,$(findstring macos,$(PLATFORM)))
  TARGET = libjsatx.dylib
  LDFLAGS = -dynamiclib
  CFLAGS = -O3 -fPIC \
	-Iinclude \
	-I$(JAVA_HOME)/include \
	-I$(JAVA_HOME)/include/darwin
else ifneq (,$(findstring windows,$(PLATFORM)))
  TARGET = jsatx.dll
  LDFLAGS = -shared
  CFLAGS = -O3 -fPIC \
	-Iinclude \
	-I$(JAVA_HOME)/include \
	-I$(JAVA_HOME)/include/win32
else
  TARGET = libjsatx.so
  LDFLAGS = -shared
  CFLAGS = -O3 -fPIC \
	-Iinclude \
	-I$(JAVA_HOME)/include \
	-I$(JAVA_HOME)/include/linux
endif

# Detect presence of cblas.h and enable USE_BLAS only when available
CBLAS_CHECK := $(shell echo '#include <cblas.h>' | $(CC) -x c - -o /dev/null 2>/dev/null && echo yes || echo no)
ifneq ($(CBLAS_CHECK),yes)
  $(info [INFO] cblas.h not found; building without BLAS acceleration)
  BLAS_LIBS :=
else
  $(info [INFO] cblas.h found; enabling BLAS acceleration)
  CFLAGS := $(CFLAGS) -DUSE_BLAS
endif

# ------------------------------------------------------
# BUILD TARGETS
# ------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "[LINK] Building shared library: $(TARGET)"
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(BLAS_LIBS)

%.o: %.c include/mlbase.h
	@echo "[CC] Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[CLEAN] Removing object files and library"
	rm -f src/*.o $(TARGET)

# Install the built native library into Java resources under a platform-specific path
# Usage: make install PLATFORM=linux-x86_64
install:
	@echo "[INSTALL] Copying $(TARGET) to Java resources (platform=$(PLATFORM))..."
	mkdir -p ../java/jsatx-java/src/main/resources/natives/$(PLATFORM)
	cp $(TARGET) ../java/jsatx-java/src/main/resources/natives/$(PLATFORM)/
	@echo "[DONE] Installed $(TARGET) to natives/$(PLATFORM)/"
