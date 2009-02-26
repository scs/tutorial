# Cross-Compiler executables and flags
TARGET_CC = bfin-uclinux-gcc
TARGET_CFLAGS = -Wall -Wno-long-long -pedantic -O2 -DOSC_TARGET
TARGET_LDFLAGS = -DOSC_TARGET -Wl,-elf2flt="-s 1048576" -lbfdsp

# Host-Compiler executables and flags
HOST_CC = gcc 
HOST_CFLAGS = $(HOST_FEATURES) -Wall -Wno-long-long -pedantic -DOSC_HOST -g
HOST_LDFLAGS = -lm

PROJECTS = bmp cam cfg dma sup hello-world
TARGET_ONLY_PROJECTS = alarm

HOST_PROJETCS = $(addsuffix _host, $(PROJECTS))
TARGET_PROJETCS = $(addsuffix _target, $(PROJECTS) $(TARGET_ONLY_PROJECTS))

.PHONY: all
all: $(TARGET_PROJETCS) $(HOST_PROJETCS)

$(HOST_PROJETCS): %_host: %.c oscar/staging/lib/libosc_host.a
	@ echo "Building $@ ..."
	@ $(HOST_CC) $^ $(HOST_CFLAGS) $(HOST_LDFLAGS) -o $@
	@ echo "Done."

$(TARGET_PROJETCS): %_target: %.c oscar/staging/lib/libosc_target.a
	@ echo "Building $@ ..."
	@ $(TARGET_CC) $^ $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -o $@
	@ ! [ -d /tftpboot ] || cp $@ /tftpboot/$*
	@ echo "Done."

oscar/staging/lib/libosc_host.a oscar/staging/lib/libosc_target.a:
	make get

# Set symlinks
.PHONY: get
get:
	@ echo "Configuring Oscar framework ..."
	@ [ -d "../../oscar/" ] && ln -sf ../../oscar/ oscar
	@ echo "Done."

.PHONY: clean
clean:
	@ echo "Configuring Oscar framework ..."
	@ rm -f $(HOST_PROJETCS) $(TARGET_PROJETCS)
	@ rm -f modified.bmp osc_log osc_simlog
	@ rm -f *.elf *.gdb *.o oscar
	@ echo "Done."

