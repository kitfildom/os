# This is a Makefile meant for GNU Make. Assert that.
ifneq (,)
This makefile requires GNU Make.
endif
 
# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := iceos.elf
 
# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
ifeq ($(origin CC), default)
CC := clang -target x86_64-elf
endif
 
# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".
ifeq ($(origin LD), default)
LD := ld.lld
endif
 
# User controllable CFLAGS.
CFLAGS ?= -Wall -Wextra -O2 -pipe
 
# User controllable linker flags. We set none by default.
LDFLAGS ?=
 
# Internal C flags that should not be changed by the user.
override INTERNALCFLAGS :=   \
	-I.                  \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD
 
# Internal linker flags that should not be changed by the user.
override INTERNALLDFLAGS :=    \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static
 
# Use find to glob all *.c files in the directory and extract the object names.
override CFILES := $(shell find kernel -type f -name '*.c')
override OBJ := $(CFILES:.c=.o)
override HEADER_DEPS := $(CFILES:.c=.d)
 
# Default target.
.PHONY: all
all: $(KERNEL)
 
# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@
 
# Compilation rules for *.c files.
-include $(HEADER_DEPS)
%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

buildimg:
	@echo "Preparing .iso build..."
	@make
	@cp -v iceos.elf limine.cfg limine/limine.sys \
      limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	@xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o iceos.iso
	@./limine/limine-install IceOS.iso
	@echo "Success!"

runn:
	@echo "Running IceOS.iso... (argument n -> no disk creation)"
	@qemu-system-x86_64 -cdrom IceOS.iso
	@echo "Finished!"

run:
	@echo "Running IceOS"
	@echo "Phase 1/2: Creating disk..."
	@make buildimg
	@echo "Phase 2/2: Running IceOS.iso..."
	@qemu-system-x86_64 -cdrom IceOS.iso
	@echo "Finished running!"

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)
