
# Makefile for building port binaries
#
# Makefile targets:
#
# all/install   build and install the NIF
# clean         clean build products and intermediates
#
# Variables to override:
#
# MIX_APP_PATH  path to the build directory
#
# CC            C compiler
# CROSSCOMPILE	crosscompiler prefix, if any
# CFLAGS	compiler flags for compiling all C files
# ERL_CFLAGS	additional compiler flags for files using Erlang header files
# ERL_EI_INCLUDE_DIR include path to ei.h (Required for crosscompile)
# ERL_EI_LIBDIR path to libei.a (Required for crosscompile)
# LDFLAGS	linker flags for linking all binaries
# ERL_LDFLAGS	additional linker flags for projects referencing Erlang libraries
# PKG_CONFIG_SYSROOT_DIR sysroot for pkg-config (for finding libnl-3)
# PKG_CONFIG_PATH pkg-config metadata
#
ifeq ($(MIX_APP_PATH),)
calling_from_make:
	mix compile
endif

$(info build arduino-firmware for: $(MIX_TARGET))

PREFIX = $(MIX_APP_PATH)/priv
FIRMWARE_SRC := $(wildcard firmware_src/src/*.cpp)
FIRMWARE_SRC += $(wildcard firmware_src/include/*.h)

all: $(PREFIX) $(PREFIX)/firmware.hex

clean:
	platformio run -d firmware_src -t clean
	$(RM) $(PREFIX)/firmware.hex 

$(PREFIX):
	mkdir -p $(PREFIX)

$(PREFIX)/firmware.hex: $(FIRMWARE_SRC)
	PLATFORMIO_BUILD_FLAGS="-DMIX_TARGET_$(MIX_TARGET) -DMIX_TARGET=\"$(MIX_TARGET)\"" platformio run -d firmware_src/
	cp firmware_src/.pio/build/megaatmega2560/firmware.hex $(PREFIX)/firmware.hex