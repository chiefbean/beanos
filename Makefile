DESTDIR=$(shell pwd)/sysroot

install: install-headers
	cd libc && DESTDIR=$(DESTDIR) make install
	cd kernel && DESTDIR=$(DESTDIR) make install

clean:
	cd libc && make clean
	cd kernel && make clean
	rm -rf sysroot/usr sysroot/boot

run: install
	qemu-system-i386 -kernel kernel/beanos.kernel -d int -no-reboot -no-shutdown

install-headers:
	cd libc && DESTDIR=$(DESTDIR) make install-headers
	cd kernel && DESTDIR=$(DESTDIR) make install-headers