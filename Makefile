DESTDIR=$(shell pwd)/sysroot

install: install-headers
	cd libc && DESTDIR=$(DESTDIR) make install
	cd kernel && DESTDIR=$(DESTDIR) make install

clean:
	cd libc && make clean
	cd kernel && make clean
	rm -rf sysroot/usr sysroot/boot
	rm -fv log.txt
	rm -f disk.img

run: install disk.img
	qemu-system-i386 -kernel kernel/beanos.kernel -drive file=disk.img,format=raw,if=ide

rerun: clean
	make run

install-headers:
	cd libc && DESTDIR=$(DESTDIR) make install-headers
	cd kernel && DESTDIR=$(DESTDIR) make install-headers

debug: install disk.img
	qemu-system-i386 -s -kernel kernel/beanos.kernel -d int -no-reboot -no-shutdown -D log.txt -monitor stdio -drive file=disk.img,format=raw,if=ide

disk.img:
	dd if=/dev/zero of=disk.img bs=1M count=100
	mkfs.fat -F 16 -n BEANOS disk.img
	mmd -i disk.img ::SUB
	printf "rerun test" > test.txt
	mcopy -i disk.img test.txt ::SUB/TEST.TXT
	rm test.txt

.PHONY: run rerun install clean debug install-headers \
        format format-check check-headers fix-headers lint install-hooks \
        compile-commands

# --- style ----------------------------------------------------------------

# Third-party headers we do not restyle (kept in sync with
# tools/header-check-exclude.txt).
STYLE_EXCLUDE := kernel/arch/i386/multiboot.h

CFILES := $(filter-out $(STYLE_EXCLUDE),\
    $(shell find kernel libc \( -name '*.c' -o -name '*.h' \) -print))

format:
	clang-format -i $(CFILES)

format-check:
	clang-format --dry-run --Werror $(CFILES)

check-headers:
	./tools/check-headers.sh

fix-headers:
	./tools/check-headers.sh --fix

# clangd's compile database. Regenerate after adding or moving a source file.
compile-commands:
	./tools/gen-compile-commands.sh

lint: format-check check-headers

install-hooks:
	ln -sf ../../tools/pre-commit .git/hooks/pre-commit
	@echo "installed .git/hooks/pre-commit"
