/*
 Targa.h
*/
typedef struct {
	GLuint bpp;		
	GLuint width;	
	GLuint height;
} TARGAINFO;


GLubyte *LoadTGAImage(char *filename, TARGAINFO *info);
bool LoadTGATexture(char *filename);
bool LoadTGAMipmap(char *filename);
