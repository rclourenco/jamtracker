#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "graphlib.c"


char *items[] = {
	"C-300123", "C-301123", "C-302123", "C-303123",
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"C#300123", "C#300123", "C#300123", "C#300123",
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"D-300123", "D-300123", "D-300123", "D-300123",
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"D#300123", "D#300123", "D#300123", "D#300123",
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"--------", "--------", "--------", "--------", 
	"E-300123", "E-300123", "--------", "--------",
	"F-3 00123",
	"F#3 00123",
	"G-3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123",
	"C#3 00123"
};

int main(int argc, char *argv[]) {
	modo13h();

    int16_t rx = 0x1234;
    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
		rx++;
            if (e.type == SDL_QUIT) {
                break;
            }
        }

		if (rx%100==0) {
			int y,x;
			int w = 320, h= 200;
			for (y = 0; y < h; y++) {
    			for (x = 0; x < w; x++) {

	    			uint8_t ci = 0;
	    			switch ((rx/100)%3) {
					case 0:
	    				ci = ((int)(sin(x*6.28/80.0)*20)%60) | ((int)(sin(y*6.28/80.0)*20)%60);
					break;
					case 1:
						ci = ((int)(sin(x*6.28/80.0)*20)%60) ^ ((int)(sin(y*6.28/80.0)*20)%60);
					break;
					default:
						ci = ((int)(sin(x*6.28/80.0)*20)%60) & ((int)(sin(y*6.28/80.0)*20)%60);
					}
					PLOT(x, y, ci);
    			}
			}
			TextBackGround=0;
			TextColor=15;

			for(y=0;y<10;y++) {
				int i = rx/100;
				TextColor=30+y;
				writest(5,y*10+10, "00");
				for(x=0;x<4;x++) {
					int idx = ((i%20+y)*4+x)%40;
					if (idx>40) 
						continue;
					writest(25+x*75, y*10+10, items[idx]);
				}
			}
		}

//	rx++;
		rotate_palette();
		graph_refresh();
		SDL_Delay(20);
		rx++;
    }

	modo3h();

    return 0;
}
