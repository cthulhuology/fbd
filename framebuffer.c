#include <SDL2/SDL.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define p720 1280*720*4

Uint32 draw_event(Uint32 interval, void *param) {
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;			// draw
	userevent.data1 = NULL;
	userevent.data2 = NULL;
	event.type = SDL_USEREVENT;
	event.user = userevent;
	SDL_PushEvent(&event);
	return interval;
}

int main(int argc, char** argv){
	struct stat st;
	char buffer[p720];
	int fd = open("./fb0", O_RDWR | O_CREAT, 0644);
	fstat(fd,&st);
	if (st.st_size != p720){
		memset(buffer, 0, p720);
		write(fd, buffer, p720);
		lseek(fd, 0, SEEK_SET);
	}
	char* video = (char*)mmap(NULL, p720, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
	fprintf(stderr,"got video %p\n", video);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Faux Frame Buffer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, 1280, 720);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 1280, 720);
	SDL_TimerID draw_timer = SDL_AddTimer( 1000 / 60, draw_event,NULL);
	SDL_Event event;
	for (;;) {
		SDL_PollEvent(&event);
		switch(event.type) {
	
		case SDL_USEREVENT:	// only user event is draw the screen
			SDL_UpdateTexture(texture,NULL,video,1280*4);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			break;
		}
	}
quit:	
	munmap(video,p720);
	close(fd);
	return 0;
}

