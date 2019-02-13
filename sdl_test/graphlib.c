/*
	Biblioteca programada por Renato Louren‡o.
*/
//biblioteca para o modo grafico 13h

#include <stdlib.h>
#include "picture.h"
#include "vgafonts.h"

//defini‡äes de desenho
#define LIFI 0
#define LINE 1
#define FILL 2
#define COPY_D 0
#define XOR_D 1
#define AND_D 2
#define OR_D 3

#define PLOT(X,Y,C) screen[(X)+(Y)*320]=C
#define GETPLOT(X,Y) screen[(X)+(Y)*320]
#define PI M_PI

#define DEFAULT 0
#define USER 1

extern uint32_t palette[256];

void pchar(int x,int y, unsigned char c);
void writest(int x,int y,const char *st);
void fillscreen(char a);
void swap(int *a,int *b);
unsigned char DRAW=COPY_D;
unsigned char Color=15;
unsigned char FillColor=15;
unsigned char TextColor=7;
unsigned char TextBackGround=255;
unsigned char *dscreen=NULL;
unsigned char *screen=NULL;

//instrucoes graficas de interface com a bios
typedef unsigned char *CPT;
int modo13h();//modo grafico
void modo3h();//modo texto
unsigned char *getcpt();//retorna a posicao do mapa de caracteres
void SetOverScan(char c);
void SetDacRegs(unsigned a,unsigned char r,unsigned char g,unsigned char b);
void GetDacRegs(unsigned a,unsigned char *r,unsigned char *g,unsigned char *b);


//instrucoes de desenho
void DrawPixel(int x,int y,char c);
void DrawRect(int x1,int y1,int x2,int y2,char op);
//void DrawCircle(int x,int y,int r,char op);
//void DrawLine(int x1,int y1,int x2,int y2);
#define DrawLine dline

void dline(int x1, int y1, int x2, int y2);
void dcircle(int x0, int y0, int r);

//instru‡äes para manipula‡Æo de bitmaps
unsigned PicSize(int x1,int y1,int x2,int y2);
void GetPic(int x1,int y1,int x2,int y2,TPicture *pic);
void DrawPic(int x1,int y1,TPicture *pic);
void DrawPicB(int x1,int y1,TPicture *pic);
void DrawPicF(int x1,int y1,TPicture *pic);
int XPRes(TPicture *pic);
int YPRes(TPicture *pic);

//instru‡äes para ecran virtual
char ScrnT=DEFAULT;
void DefaultScreen();
void UserScreen(unsigned char *s);
char ScrnCpyUD();
char ScrnCpyDU();

void rotate_palette();

unsigned char *cart=0L;

#define SCREEN_W 320
#define SCREEN_H 200

struct {
	SDL_Window   *win;
	SDL_Renderer *renderer;
	SDL_Texture  *screenx;
	SDL_PixelFormat *pixelFormat;
} GraphSystem = {NULL, NULL, NULL, NULL};

int modo13h()
{
	dscreen = (unsigned char *)malloc(SCREEN_W*SCREEN_H);
	if(!dscreen)
		goto _abort;

	screen  = dscreen;

    int posX = -1, posY = -1, width = 800, height = 600;

    SDL_Init(SDL_INIT_VIDEO);

    GraphSystem.win         = SDL_CreateWindow("Splash!", posX, posY, width, height,SDL_WINDOW_RESIZABLE );
    GraphSystem.renderer    = SDL_CreateRenderer(GraphSystem.win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	GraphSystem.screenx     = SDL_CreateTexture(GraphSystem.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
	GraphSystem.pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

	//Init SDL	
	return 1;
_abort:
	if(dscreen)
		free(dscreen);
	return 0;
}

void modo3h()
{
	if (dscreen) {
		free(dscreen);
		dscreen = NULL;
	}

	if (GraphSystem.screenx) {
    	SDL_DestroyTexture(GraphSystem.screenx);
		GraphSystem.screenx = NULL;
	}

	if (GraphSystem.renderer) {
    	SDL_DestroyRenderer(GraphSystem.renderer);
		GraphSystem.renderer = NULL;
	}

	if (GraphSystem.win) {
    	SDL_DestroyWindow(GraphSystem.win);
		GraphSystem.win = NULL;
	}

    SDL_Quit();


	//Close SDL
}

unsigned char *getcpt()
{
	cart = vgafont8;
	return cart;
}

void graph_refresh()
{
	int pitch;
	uint8_t *pixels;
	SDL_LockTexture(GraphSystem.screenx, NULL, (void **)&pixels, &pitch);

	int y,x;
	for (y = 0; y < SCREEN_H; y++) {
    	uint32_t *p = (uint32_t *)(pixels + pitch*y); // cast for a pointer increments by 4 bytes.(RGBA)
    	for (x = 0; x < SCREEN_W; x++) {
				unsigned char ci = screen[y*SCREEN_W+x];
				*p = palette[ci%256];
      			p++;
    	}
	}

	SDL_UnlockTexture(GraphSystem.screenx);
    SDL_RenderClear(GraphSystem.renderer);
    SDL_RenderCopy(GraphSystem.renderer, GraphSystem.screenx, NULL, NULL);
    SDL_RenderPresent(GraphSystem.renderer);
}

void pchar(int x,int y, unsigned char c)
{
	int cx,cy;
	unsigned char v;
	if(!cart)
		cart=getcpt();
	for(cy=0;cy<8;cy++)
	{
		v=cart[8*c+cy];
		for(cx=0;cx<8;cx++)
		{
			if((v&128)==128)
				PLOT(x+cx,y+cy,TextColor);
			else
			if(TextBackGround!=255)
				PLOT(x+cx,y+cy,TextBackGround);
			v=v<<1;
		}
	}
}

void writest(int x,int y,const char *st)
{
	while(*st)
	{
		pchar(x,y,(unsigned char)*st);
		st++;
		x+=8;
	}
}

void fillscreen(char a)
{
	memset(screen,a,64000);
}

//instru‡äes de desenho
void DrawRect(int x1,int y1,int x2,int y2,char op)
{
	int x,y;
	int cor;
	if(x1>x2) swap(&x1,&x2);
	if(y1>y2) swap(&y1,&y2);
	if(op!=1)
	{
		for(x=x1+1;x<x2;x++)
		for(y=y1+1;y<y2;y++)
			DrawPixel(x,y,FillColor);
	}
	if(op==2)
		cor=FillColor;
	else
		cor=Color;
	for(x=x1;x<=x2;x++)
	{
		DrawPixel(x,y1,cor);
		DrawPixel(x,y2,cor);
	}
	for(y=y1+1;y<y2;y++)
	{
		DrawPixel(x1,y,cor);
		DrawPixel(x2,y,cor);
	}
}

/*
void DrawCircle(int x,int y,int r,char op)
{
	int c1,c2;
	int r2=r-1;
	double alfa;
	double inca;
	int px,py;
	char cor;
	if ((r==0)||(r>180))
	{
		DrawPixel(x,y,Color);
		return;
	}
	inca=PI/(PI*r);
	if(op!=1)
	{
		for(c1=x-r2;c1<=x+r2;c1++)
		for(c2=y-r2;c2<=y+r2;c2++)
		{
			if((long double)((x-c1)*(x-c1))+((y-c2)*(y-c2))<(long double)(r*r)-r)
				DrawPixel(c1,c2,FillColor);
		}
	}
	if(op==2)
		cor=FillColor;
	else
		cor=Color;
	for(alfa=0;alfa<PI/2;alfa+=inca)
	{
		px=cos(alfa)*r;
		py=sin(alfa)*r;
		DrawPixel(x+px,y+py,cor);
		DrawPixel(x-px,y-py,cor);
		DrawPixel(x-px,y+py,cor);
		DrawPixel(x+px,y-py,cor);
	}
}
*/

void DrawPixel(int x,int y,char c)
{
	if ((x>=0)&&(y>=0)&&(x<320)&&(y<200))
	{
		switch (DRAW)
		{
			case XOR_D:PLOT(x,y,c^GETPLOT(x,y));break;
			case AND_D:PLOT(x,y,c&GETPLOT(x,y));break;
			case OR_D:PLOT(x,y,c|GETPLOT(x,y));break;
			default:PLOT(x,y,c);
		}

	}

}


void dline(int x1, int y1, int x2, int y2)
{
	int dx,dy,d,y,x;
	int ax,ay;
	int sx,sy;

	dx = x2-x1;
	dy = y2-y1;

	sx = (x2<x1?-1:1);
	sy = (y2<y1?-1:1);
	ax = 2*(dx<0?-dx:dx);
	ay = 2*(dy<0?-dy:dy);

	x=x1;
	y=y1;
	if( ax >= ay ) {
		d=ay-ax/2;
		while(1)
		{
			PLOT(x,y,Color);
			if(x==x2) break;
			if(d>=0) {
				y=y+sy;
				d=d-ax;
			}
			d=d+ay;
			x=x+sx;
		}
	}
	else {
		d=ax-ay/2;
		while(1)
		{
			PLOT(x,y,Color);
			if(y==y2) break;
			if(d>=0) {
				x=x+sx;
				d=d-ay;
			}
			d=d+ax;
			y=y+sy;
		}
	}

}

void dcircle(int x0, int y0, int r)
{
	int x = r - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (r<<1);

	while(x>=y)
	{
		DrawPixel(x0+x,y0+y,Color);
		DrawPixel(x0+x,y0-y,Color);
		DrawPixel(x0-x,y0+y,Color);
		DrawPixel(x0-x,y0-y,Color);

		DrawPixel(x0+y,y0+x,Color);
		DrawPixel(x0+y,y0-x,Color);
		DrawPixel(x0-y,y0+x,Color);
		DrawPixel(x0-y,y0-x,Color);

		if(err<=0)
		{
			y++;
			err+=dy;
			dy+=2;
		}
		if(err>0)
		{
			x--;
			dx+=2;
			err += dx  - (r<<1);
		}
	}
}



/*
void DrawLineX(int x1,int y1,int x2,int y2)
{
	long vx,vy;
	int x,y;
	double a,nv;
	vx=x2-x1;
	vy=y2-y1;
	nv=(double)sqrt((vx*vx)+(vy*vy));
	if(nv!=0)
	for(a=0;a<=1;a+=(1/nv))
	{
		x=x1+a*vx+.5;
		y=y1+a*vy+.5;
		DrawPixel(x,y,Color);
	}
	DrawPixel(x2,y2,Color);
}
*/

unsigned PicSize(int x1,int y1,int x2,int y2)
{
	int x,y;
	if (x1>x2) swap(&x1,&x2);
	if (y1>y2) swap(&y1,&y2);
	x=x2-x1+1;
	y=y2-y1+1;
	if((x>320)&&(y>200))
		return 0;
	return x*y+10;
}

void swap(int *a,int *b)
{
	int temp;
	temp=*a;
	*a=*b;
	*b=temp;
}

void GetPic(int x1,int y1,int x2,int y2,TPicture *pic)
{
	int x,y;
	int cy,cx;
	char *d;
	if (x1>x2) swap(&x1,&x2);
	if (y1>y2) swap(&y1,&y2);
	x=x2-x1+1;
	y=y2-y1+1;

	pic->w=x;
	pic->h=y;

	d=pic->data;
	for(cy=y1;cy<=y2;cy++)
	for(cx=x1;cx<=x2;cx++)
	{
		*d=GETPLOT(cx,cy);
		d++;
	}
}

int XPRes(TPicture *pic)
{
	return pic->w;
}

int YPRes(TPicture *pic)
{
	return pic->h;
}

void DrawPicF(int x1,int y1,TPicture *pic)
{
	char *data=pic->data;
	int x2=pic->w;
	int y2=pic->h;
	int py,y;

	py=y1;
	for(y=0;y<y2;y++)
	{
		py=(y1+y)*320;
		memcpy(&screen[x1+py],data,x2);
		data+=x2;
	}
}


void DrawPic(int x1,int y1,TPicture *pic)
{
	char *data=pic->data;
	int x2=x1+pic->w-1;
	int y2=y1+pic->h-1;
	int x,y;
	
	for(y=y1;y<=y2;y++)
	for(x=x1;x<=x2;x++)
	{
		if (*data!=0) PLOT(x,y,*data);
		data++;
	}

}

void DrawPicB(int x1,int y1,TPicture *pic)
{
	char *data=pic->data;
	int x2=x1+pic->w-1;
	int y2=y1+pic->h-1;
	int x,y;

	for(y=y1;y<=y2;y++)
	for(x=x1;x<=x2;x++)
	{
		if (*data!=-1) DrawPixel(x,y,*data);
		data++;
	}
}

void DefaultScreen()
{
	ScrnT=DEFAULT;
	screen=dscreen;
}

void UserScreen(unsigned char *s)
{
	ScrnT=USER;
	screen=s;
}

char ScrnCpyUD()
{
	if (ScrnT!=USER) return 0;
	memcpy(dscreen,screen,64000);
	return 1;
}

char ScrnCpyDU()
{
	if (ScrnT!=USER) return 0;
	memcpy(screen,dscreen,64000);
	return 1;
}

void SetOverScan(char c)
{
}

/*
void SetDacRegs(unsigned a,unsigned char r,unsigned char g,unsigned char b)
{
	struct REGPACK regs;
	regs.r_ax=0x1010;
	regs.r_bx=a;
	regs.r_cx=(g<<8)+b;
	regs.r_dx=r<<8;
	intr(0x10,&regs);
}

void GetDacRegs(unsigned a,unsigned char *r,unsigned char *g,unsigned char *b)
{
	struct REGPACK regs;
	regs.r_ax=0x1015;
	regs.r_bx=a;
	intr(0x10,&regs);
	*r=regs.r_dx>>8;
	*g=regs.r_cx>>8;
	*b=(regs.r_cx<<8)>>8;
}
*/

uint32_t palette[256] = {
0x000000, 0x0000AA, 0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA,
0x555555, 0x5555FF, 0x55FF55, 0x55FFFF, 0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF, 
0x000000, 0x101010, 0x202020, 0x353535, 0x454545, 0x555555, 0x656565, 0x757575, 
0x8A8A8A, 0x9A9A9A, 0xAAAAAA, 0xBABABA, 0xCACACA, 0xDFDFDF, 0xEFEFEF, 0xFFFFFF, 
0x0000FF, 0x4100FF, 0x8200FF, 0xBE00FF, 0xFF00FF, 0xFF00BE, 0xFF0082, 0xFF0041,
0xFF0000, 0xFF4100, 0xFF8200, 0xFFBE00, 0xFFFF00, 0xBEFF00, 0x82FF00, 0x41FF00,
0x00FF00, 0x00FF41, 0x00FF82, 0x00FFBE, 0x00FFFF, 0x00BEFF, 0x0082FF, 0x0041FF, 
0x8282FF, 0x9E82FF, 0xBE82FF, 0xDF82FF, 0xFF82FF, 0xFF82DF, 0xFF82BE, 0xFF829E,
0xFF8282, 0xFF9E82, 0xFFBE82, 0xFFDF82, 0xFFFF82, 0xDFFF82, 0xBEFF82, 0x9EFF82,
0x82FF82, 0x82FF9E, 0x82FFBE, 0x82FFDF, 0x82FFFF, 0x82DFFF, 0x82BEFF, 0x829EFF,
0xBABAFF, 0xCABAFF, 0xDFBAFF, 0xEFBAFF, 0xFFBAFF, 0xFFBAEF, 0xFFBADF, 0xFFBACA, 
0xFFBABA, 0xFFCABA, 0xFFDFBA, 0xFFEFBA, 0xFFFFBA, 0xEFFFBA, 0xDFFFBA, 0xCAFFBA,
0xBAFFBA, 0xBAFFCA, 0xBAFFDF, 0xBAFFEF, 0xBAFFFF, 0xBAEFFF, 0xBADFFF, 0xBACAFF,
0x000071, 0x1C0071, 0x390071, 0x550071, 0x710071, 0x710055, 0x710039, 0x71001C,
0x710000, 0x711C00, 0x713900, 0x715500, 0x717100, 0x557100, 0x397100, 0x1C7100,
0x007100, 0x00711C, 0x007139, 0x007155, 0x007171, 0x005571, 0x003971, 0x001C71,
0x393971, 0x453971, 0x553971, 0x613971, 0x713971, 0x713961, 0x713955, 0x713945,
0x713939, 0x714539, 0x715539, 0x716139, 0x717139, 0x617139, 0x557139, 0x457139,
0x397139, 0x397145, 0x397155, 0x397161, 0x397171, 0x396171, 0x395571, 0x394571,
0x515171, 0x595171, 0x615171, 0x695171, 0x715171, 0x715169, 0x715161, 0x715159,
0x715151, 0x715951, 0x716151, 0x716951, 0x717151, 0x697151, 0x617151, 0x597151,
0x517151, 0x517159, 0x517161, 0x517169, 0x517171, 0x516971, 0x516171, 0x515971, 
0x000041, 0x100041, 0x200041, 0x310041, 0x410041, 0x410031, 0x410020, 0x410010,
0x410000, 0x411000, 0x412000, 0x413100, 0x414100, 0x314100, 0x204100, 0x104100, 
0x004100, 0x004110, 0x004120, 0x004131, 0x004141, 0x003141, 0x002041, 0x001041,
0x202041, 0x282041, 0x312041, 0x392041, 0x412041, 0x412039, 0x412031, 0x412028, 
0x412020, 0x412820, 0x413120, 0x413920, 0x414120, 0x394120, 0x314120, 0x284120, 
0x204120, 0x204128, 0x204131, 0x204139, 0x204141, 0x203941, 0x203141, 0x202841, 
0x2D2D41, 0x312D41, 0x352D41, 0x3D2D41, 0x412D41, 0x412D3D, 0x412D35, 0x412D31, 
0x412D2D, 0x41312D, 0x41352D, 0x413D2D, 0x41412D, 0x3D412D, 0x35412D, 0x31412D,
0x2D412D, 0x2D4131, 0x2D4135, 0x2D413D, 0x2D4141, 0x2D3D41, 0x2D3541, 0x2D3141,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};

void rotate_palette()
{
	Uint32 l = palette[255];
	int i;
	for(i=255;i>0;i--) {
		palette[i] = palette[i-1];
	}
	palette[0] = l;
}


