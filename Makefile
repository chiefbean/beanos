
install:
	cd libc && make install
	cd kernel && make install

clean:
	cd libc && make clean
	cd kernel && make clean
	rm -rf sysroot/usr sysroot/boot

run: install
	qemu-system-i386 -kernel kernel/beanos.kernel