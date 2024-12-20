DESTDIR=$(shell pwd)/sysroot

install: install-headers
	cd libc && DESTDIR=$(DESTDIR) make install
	cd kernel && DESTDIR=$(DESTDIR) make install

clean:
	cd libc && make clean
	cd kernel && make clean
	rm -rf sysroot/usr sysroot/boot
	rm -fv log.txt

run: install
	qemu-system-i386 -kernel kernel/beanos.kernel

install-headers:
	cd libc && DESTDIR=$(DESTDIR) make install-headers
	cd kernel && DESTDIR=$(DESTDIR) make install-headers

debug: install
	qemu-system-i386 -s -S -kernel kernel/beanos.kernel -d int -no-reboot -no-shutdown -D log.txt -monitor stdio