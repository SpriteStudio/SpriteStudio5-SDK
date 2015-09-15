#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


 SDL_Surface *screen = 0;

void one_iter() {
  // process input
  // render to screen
  //printf("hello, world!\n");	
	
	SDL_Flip(screen); 
}



extern "C" int main(int argc, char** argv) {
  printf("IMG_LOAD\n");

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE);


	SDL_Surface* image;
	image = IMG_Load("sample.png");
//	image = SDL_LoadBMP("sample.bmp");

	if ( image == 0 )
	{
		printf( "image not loaded\n" );
	}


#ifdef TEST_SDL_LOCK_OPTS
  EM_ASM("SDL.defaults.copyOnLock = false; SDL.defaults.discardOnLock = true; SDL.defaults.opaqueFrontBuffer = false;");
#endif

  if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 256; j++) {
#ifdef TEST_SDL_LOCK_OPTS
      // Alpha behaves like in the browser, so write proper opaque pixels.
      int alpha = 255;
#else
      // To emulate native behavior with blitting to screen, alpha component is ignored. Test that it is so by outputting
      // data (and testing that it does get discarded)
      int alpha = (i+j) % 255;
#endif
      *((Uint32*)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, i, j, 255-i, alpha);
    }
  }
  if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  SDL_Flip(screen); 

  printf("you should see a smoothly-colored square - no sharp lines but the square borders!\n");
  printf("and here is some text that should be HTML-friendly: amp: |&| double-quote: |\"| quote: |'| less-than, greater-than, html-like tags: |<cheez></cheez>|\nanother line.\n");

  #ifdef EMSCRIPTEN
	emscripten_set_main_loop(one_iter, 16 , 1);
  #else
	  while (1) {
		one_iter();
		SDL_Delay(time_to_next_frame());
	  }
  #endif


  SDL_Quit();

  return 0;
}

