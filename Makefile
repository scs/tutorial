# Executable name and system suffix
HOST_SUFFIX = _host
TARGET_SUFFIX = _target

# Cross-Compiler executables and flags
TARGET_CC = bfin-uclinux-gcc
TARGET_CFLAGS = -Wall -pedantic -O2 -DOSC_TARGET
TARGET_LDFLAGS = -DOSC_TARGET -Wl,-elf2flt="-s 1048576" -lbfdsp

# Host-Compiler executables and flags
HOST_CC = gcc 
HOST_CFLAGS = $(HOST_FEATURES) -Wall -pedantic -DOSC_HOST -g
HOST_LDFLAGS = -lm

all: bmp_target bmp_host cam_target cam_host cfg_target cfg_host dma_target dma_host sup_target sup_host hello-world_target hello-world_host alarm_target

bmp_host: bmp.c inc/*.h lib/libosc_host.a
	@echo "Compiling for host.."
	$(HOST_CC) bmp.c lib/libosc_host.a $(HOST_CFLAGS) \
	$(HOST_LDFLAGS) -o bmp$(HOST_SUFFIX)
	@echo "Host executable done."
	cp bmp$(HOST_SUFFIX) bmp; exit 0

bmp_target: bmp.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) bmp.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o bmp$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp bmp$(TARGET_SUFFIX) /tftpboot/bmp; exit 0

cam_host: cam.c inc/*.h lib/libosc_host.a
	@echo "Compiling for host.."
	$(HOST_CC) cam.c lib/libosc_host.a $(HOST_CFLAGS) \
	$(HOST_LDFLAGS) -o cam$(HOST_SUFFIX)
	@echo "Host executable done."
	cp cam$(HOST_SUFFIX) cam; exit 0

cam_target: cam.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) cam.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o cam$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp cam$(TARGET_SUFFIX) /tftpboot/cam; exit 0

cfg_host: cfg.c inc/*.h lib/libosc_host.a
	@echo "Compiling for host.."
	$(HOST_CC) cfg.c lib/libosc_host.a $(HOST_CFLAGS) \
	$(HOST_LDFLAGS) -o cfg$(HOST_SUFFIX)
	@echo "Host executable done."
	cp cfg$(HOST_SUFFIX) cfg; exit 0

cfg_target: cfg.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) cfg.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o cfg$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp cfg$(TARGET_SUFFIX) /tftpboot/cfg; exit 0
	[ -d /tftpboot ] && cp config.txt /tftpboot/config.txt; exit 0

dma_host: dma.c inc/*.h lib/libosc_host.a
	@echo "Compiling for host.."
	$(HOST_CC) dma.c lib/libosc_host.a $(HOST_CFLAGS) \
	$(HOST_LDFLAGS) -o dma$(HOST_SUFFIX)
	@echo "Host executable done."
	cp dma$(HOST_SUFFIX) dma; exit 0

dma_target: dma.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) dma.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o dma$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp dma$(TARGET_SUFFIX) /tftpboot/dma; exit 0

sup_host: sup.c inc/*.h lib/libosc_host.a
	@echo "Compiling for host.."
	$(HOST_CC) sup.c lib/libosc_host.a $(HOST_CFLAGS) \
	$(HOST_LDFLAGS) -o sup$(HOST_SUFFIX)
	@echo "Host executable done."
	cp sup$(HOST_SUFFIX) sup; exit 0

sup_target: sup.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) sup.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o sup$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp sup$(TARGET_SUFFIX) /tftpboot/sup; exit 0

hello-world_host: hello-world.c inc/*.h lib/libosc_host.a
	@echo "Compiling for host.."
	$(HOST_CC) hello-world.c lib/libosc_host.a $(HOST_CFLAGS) \
	$(HOST_LDFLAGS) -o hello-world$(HOST_SUFFIX)
	@echo "Host executable done."
	cp hello-world$(HOST_SUFFIX) hello-world; exit 0

hello-world_target: hello-world.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) hello-world.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o hello-world$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp hello-world$(TARGET_SUFFIX) /tftpboot/hello-world; exit 0

alarm_target: alarm.c inc/*.h lib/libosc_target.a
	@echo "Compiling for target.."
	$(TARGET_CC) alarm.c lib/libosc_target.a $(TARGET_CFLAGS) \
	$(TARGET_LDFLAGS) -o alarm$(TARGET_SUFFIX)
	@echo "Target executable done."
	[ -d /tftpboot ] && cp alarm$(TARGET_SUFFIX) /tftpboot/alarm; exit 0


# Set symlinks
get:
	@ rm -rf inc lib
	@ ln -s ../../oscar/staging/inc ./inc
	@ ln -s ../../oscar/staging/lib ./lib
	@ echo "Configured Oscar framework."


clean:
	rm -f bmp bmp$(HOST_SUFFIX) bmp$(TARGET_SUFFIX) modified.bmp osc_log osc_simlog
	rm -f cam cam$(HOST_SUFFIX) cam$(TARGET_SUFFIX)
	rm -f cfg cfg$(HOST_SUFFIX) cfg$(TARGET_SUFFIX)
	rm -f dma dma$(HOST_SUFFIX) dma$(TARGET_SUFFIX)
	rm -f sup sup$(HOST_SUFFIX) sup$(TARGET_SUFFIX)
	rm -f hello-world hello-world$(HOST_SUFFIX) hello-world$(TARGET_SUFFIX) hello-world.bmp
	rm -f alarm alarm$(HOST_SUFFIX) alarm$(TARGET_SUFFIX)
	-rm -f $(EXEC) *.elf *.gdb *.o

