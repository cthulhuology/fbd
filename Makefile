all : fb

fb : framebuffer.c
	gcc -o $@ $< `sdl2-config --cflags --libs` -lSDL2_net

.PHONY: clean
clean:
	rm -f fb fb0

.PHONY: coffee
coffee:
	dd if=K7MZ.rgba of=./fb0 conv=notrunc seek=0

red:
	dd if=red.rgba of=./fb0 conv=notrunc seek=0

sred:
	dd if=red.rgba | nc -4 localhost 6601

scoffee:
	dd if=K7MZ.rgba | nc -4 localhost 6601
