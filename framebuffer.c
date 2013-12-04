#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define p720 1280*720*4
#define PORT 6601
#define PACKET_SIZE 640 

enum UserEvents {
	DRAW = 1,
	NETWORK = 2
};

Uint32 draw_event(Uint32 interval, void *param) {
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = DRAW;
	userevent.data1 = NULL;
	userevent.data2 = NULL;
	event.type = SDL_USEREVENT;
	event.user = userevent;
	SDL_PushEvent(&event);
	return interval;
}

Uint32 net_event(Uint32 interval, void* param) {
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = NETWORK;
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
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Faux Frame Buffer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 
		SDL_WINDOW_MAXIMIZED);
		// SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, 1280, 720);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 1280, 720);
	SDLNet_Init();
	IPaddress address;
	address.host = 0;
	address.port = htons(PORT);
	TCPsocket tcp = SDLNet_TCP_Open(&address);
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);
	SDLNet_TCP_AddSocket(set,tcp);
	SDL_TimerID draw_timer = SDL_AddTimer( 1000 / 60, draw_event,NULL);
	SDL_TimerID net_timer = SDL_AddTimer( 1000 / 20, net_event, NULL);
	SDL_Event event;
	int offset = 0;
	int n = 0;
	int i = 0;
	TCPsocket client;
	for (;;) {
		SDL_PollEvent(&event);
		switch(event.type) {
		case SDL_QUIT:
			goto quit;
		case SDL_USEREVENT:
			switch (event.user.code) {
			case DRAW:
				SDL_UpdateTexture(texture,NULL,video,1280*4);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
				break;
			case NETWORK:
				n = SDLNet_CheckSockets(set,0);
				if ( n <= 0) break;
				if ( SDLNet_SocketReady(tcp) ) {
					client = SDLNet_TCP_Accept(tcp);	
					offset = 0;
					for (offset = 0; offset < p720; ) {
						int i = SDLNet_TCP_Recv(client,video+offset,p720-offset);
						fprintf(stderr,"displaying %d bytes at %d vs %d\n", i, offset, p720);
						offset += i;
					}
					fprintf(stderr,"done\n");
					SDLNet_TCP_Close(client);
				}
				break;
			}
			break;
		}
	}
quit:	
	SDL_Quit();
	munmap(video,p720);
	close(fd);
	return 0;
}

