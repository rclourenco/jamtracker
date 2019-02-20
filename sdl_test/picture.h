#ifndef _PICTURE_H
	#define _PICTURE_H
	
typedef struct{
	unsigned w,h;
	char data[1];
}TPicture;

typedef struct{
	unsigned char B,G,R,U;
}Trgb;

typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  char   *comment;
  char	 *pixel_data;
} GraphPicture;

#endif
