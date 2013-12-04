all : fb

fb : framebuffer.c
	gcc -o $@ $< `sdl2-config --cflags --libs`

.PHONY: clean
clean:
	rm -f fb fb0

.PHONY: coffee
coffee:
	dd if=K7MZ.rgba of=./fb0 conv=notrunc seek=0

red:
	dd if=red.rgba of=./fb0 conv=notrunc seek=0
