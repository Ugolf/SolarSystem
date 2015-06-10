#ifdef _WIN32
#include "windows.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>
//#include "Targa.cpp"
#include "colors.h"
#include "Solar.h"

//#pragma comment(lib, "glew32.lib")

GLuint shaderVert, shaderFrag;	// Shadery
GLuint shaderProg;	// Program

enum
{
    /* menu              */
    UKLAD,               // uklad
    EXIT                 // wyjscie
};

typedef struct {
	GLuint bpp;		// iloœæ bitów na piksel
	GLuint width;	// rozmiary w pikselach
	GLuint height;
} TARGAINFO;


// Pozycja swiatla
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[] = { 0.0, 0.0, 1.0, 1.0 }; //ostatnia wartosc 1 - wektor pada z z,y,z
GLfloat ambient_light[] = {0.2, 0.2, 0.2, 1.0};

GLfloat mat_em[] = { 1.0, 0.9, 0.9, 0.0 };
GLfloat sun_pos[] = { 0.0 , 1.0, 0.0, 1.0 };
GLint loc;

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;

// Zmienne do animacji
static float HourOfDay = 0.0;
static float DayOfYear = 0.0;
static float AnimateIncrement = 24.0;  // Jednostka czasu

float a = 0;

int WIDTH = 1300;
int HEIGHT = 700;
int w1 = WIDTH;
int h1 = HEIGHT;


int styl = GLU_FILL;
int wybor = UKLAD;
int wektor = GLU_SMOOTH;
int tekstura = GLU_TRUE;
int orientacja = GLU_OUTSIDE;

float centerx = 0.0;
float centery = 0.0;
float centerz = -1.0;

float rotatex = 1.0;
float rotatey = 0.0;
float rotatez = 0.0;
float angle = 15.0;
float zoom = -30.0;

// obiekty tekstur
GLuint textures[10];



///////////////////////////////////////////////////////////////////////////////
// Procedura ładuje plik o podanej nazwie
// wypełnia podaną strukturę TARGAINFO
// oraz zwraca adres bufora z pikselami (jeśli nie było błędu)
// Jeśli był błąd - zwraca NULL
GLubyte *LoadTGAImage(char *filename, TARGAINFO *info)
{
	GLubyte	TGAHeader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// Nagłówek TGA bez kompresji
	GLubyte	TGACompare[12];			// Tu się załaduje dane z pliku
	GLubyte	Header[6];			// Pierwsze potrzebne 6 bajtów z pliku
	GLubyte *Bits = NULL;	// Wskaźnik na bufor z danymi pikseli

	FILE *plik = fopen(filename, "rb");	// Próba otwarcia do odczytu
	if (plik)
	{
		fread(TGACompare, 1, sizeof(TGACompare), plik);	// Odczytanie nagłówka pliku
		if (memcmp(TGAHeader, TGACompare, sizeof(TGAHeader)) == 0)	// Nagłówek identyczny?
		{
			fread(Header, 1, sizeof(Header), plik);	// Odczyt użytecznych danych

			// Wyłuskanie informacji o rozmiarach
			info->width = Header[1] * 256 + Header[0];
			info->height = Header[3] * 256 + Header[2];
			info->bpp = Header[4];

			// Sprawdzenie czy rozmiary > 0 oraz czy bitmapa 24 lub 32-bitowa
			if (info->width>0 && info->height>0 && (info->bpp == 24 || info->bpp == 32))
			{
				long ImageSize = info->height * info->width * info->bpp / 8;	// Obliczenie ilości danych
				Bits = (GLubyte*)malloc(ImageSize);	// Alokacja pamięci na dane

				if (Bits)
				{
					fread(Bits, 1, ImageSize, plik);	// Odczyt właściwych danych pikseli z pliku

					// Konwersja BGR -> RGB
					int i;
					GLubyte tmp;	// Miejsce przechowania jednej wartości koloru

					for (i = 0; i < ImageSize; i += info->bpp / 8)	// Wszystkie wartości RGB lub RGBA
					{
						tmp = Bits[i];
						Bits[i] = Bits[i + 2];
						Bits[i + 2] = tmp;
					}
				}
			}
		}

		fclose(plik);
	}

	return(Bits);
}

///////////////////////////////////////////////////////////////////////////////
// Procedury na podstawie przekazanych danych ładują
// i tworzą teksturę lub teksturę z Mip-Map'ami
bool LoadTGATexture(char *filename)
{
	TARGAINFO info;	// Dane o bitmapie
	GLubyte *bits;	// Dane o pikselach

	// ładowanie pliku
	bits = LoadTGAImage(filename, &info);	// Próba wczytania tekstury
	if (bits == NULL)	return(false);	// ERROR

	// Ustawienie parametrów tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (info.bpp == 24)	// Bitmapa z danymi RGB
		glTexImage2D(GL_TEXTURE_2D, 0, 3, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);
	else	// Bitmapa z danymi RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, 4, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	free(bits);
	return(true);
}

bool LoadTGAMipmap(char *filename)
{
	TARGAINFO info;	// Dane o bitmapie
	GLubyte *bits;	// Dane o pikselach

	// ładowanie pliku
	bits = LoadTGAImage(filename, &info);	// Próba wczytania tekstury
	if (bits == NULL)	return(false);	// ERROR

	// Ustawienie parametrów tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (info.bpp == 24)	// Bitmapa z danymi RGB
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, info.width, info.height, GL_RGB, GL_UNSIGNED_BYTE, bits);
	else	// Bitmapa z danymi RGBA
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, info.width, info.height, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	free(bits);
	return(true);
}



// Wczytanie kodu zrodlowego shadera z pliku
char *loadTextFile(const char *fileName) {
	// Otwarcie pliku:
	FILE *plik = fopen(fileName, "rb");
	if(!plik)	return(NULL);

	// Zbadanie rozmiaru pliku:
	unsigned long fSize;
	fseek(plik, 0, SEEK_END);
	fSize = ftell(plik);
	fseek(plik, 0, SEEK_SET);

	// Utworzenie bufora:
	char *buff = (char*)malloc(fSize + 1);
	if(!buff)	return(NULL);

	// Odczyt danych:
	fread(buff, 1, fSize, plik);
	buff[fSize] = 0;	// Terminator

	// Zamknięcie pliku:
	fclose(plik);
	return(buff);
}

// Sprawdzenie błędów
void checkErrors(GLuint object) {
	int res, logLen;
	char *buff;

	// Sprawdzenie SHADERA:
	glGetShaderiv(object, GL_COMPILE_STATUS, &res);
	if(res == GL_FALSE) {	// Sprawdzenie InfoLog
		puts("GL_COMPILE_STATUS ERROR!");

		// Pobranie długości loga:
		glGetShaderiv(object, GL_INFO_LOG_LENGTH,&logLen);
		// Przydzielenie pamięci na wiadomość:
		buff = (char *)malloc(logLen);

		glGetShaderInfoLog(object, logLen , NULL, buff);
		puts(buff);

		free(buff);
		exit(0);
	}

	// Sprawdzenie PROGRAMU:
	glGetProgramiv(object, GL_LINK_STATUS, &res);
	if(res == GL_FALSE) {
		puts("GL_LINK_STATUS ERROR!");

		// Pobranie długości loga:
		glGetProgramiv(object, GL_INFO_LOG_LENGTH,&logLen);
		// Przydzielenie pamięci na wiadomość:
		buff = (char *)malloc(logLen);

		glGetProgramInfoLog(object, logLen, NULL, buff);
		puts(buff);

		free(buff);
		exit(0);
	}
}

// Konfiguracja shaderów
void setupShaders(char *fileVert, char *fileFrag) {
	char *strVert, *strFrag;

	// Utworzenie shaderów:
	shaderVert = glCreateShader(GL_VERTEX_SHADER);
	shaderFrag = glCreateShader(GL_FRAGMENT_SHADER);

	// Odczyt kodu shaderów z pliku:
	strVert = loadTextFile( fileVert );
	if(!strVert) {
		puts("** Błąd ładowania pliku *.vert");
		exit(0);
	}
	strFrag = loadTextFile( fileFrag );
	if(!strFrag) {
		puts("** Błąd ładowania pliku *.frag");
		exit(0);
	}
	// Wczytanie kodu źródłowego shadera:
	glShaderSource(shaderVert, 1, (const GLchar**)(&strVert), NULL);
	glShaderSource(shaderFrag, 1, (const GLchar**)(&strFrag), NULL);

	// Zwolnienie pamięci
	free(strVert);free(strFrag);

	// Kompilacja shaderów:
	glCompileShader(shaderVert);
	glCompileShader(shaderFrag);

	// Utworzenie programu:
	shaderProg = glCreateProgram();

	// Dołączenie shaderów do programu:
	glAttachShader(shaderProg, shaderVert);
	glAttachShader(shaderProg, shaderFrag);

	// Linkowanie i aktywacja:
	glLinkProgram(shaderProg);
	glUseProgram(shaderProg);

    // Sprawdzenie błędów:
	checkErrors(shaderVert);
	checkErrors(shaderFrag);

	// Ustawienie tekstur dla shadera:
	loc = glGetUniformLocation(shaderProg, "tex0");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(shaderProg, "tex1");
	glUniform1i(loc, 1);
}

// Funkcja konfiguracji tekstur
void setupTexture(){

    // Wygenerowanie obiektów tekstur:
   glGenTextures(10, textures);

    // Wczytanie danych do 1 obiektu tekstury:
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    if (!LoadTGATexture("tex0.tga"))
    {
        puts("blad podczas wczytywania tekstury");
        exit(1);
    }

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    if (!LoadTGATexture("tex1.tga"))
    {
        puts("blad podczas wczytywania tekstury");
        exit(1);
    }
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    if (!LoadTGATexture("earth.tga"))
    {
        puts("blad podczas wczytywania tekstury");
        exit(1);
    }
    if (!LoadTGAMipmap("earth.tga"))
    {
        puts("blad podczas wczytywania tekstury");
        exit(1);
    }
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    LoadTGATexture("mercury.tga");
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    LoadTGATexture("venus.tga");
    glBindTexture(GL_TEXTURE_2D, textures[5]);
    LoadTGATexture("mars.tga");
    glBindTexture(GL_TEXTURE_2D, textures[6]);
    LoadTGATexture("jupiter.tga");
    glBindTexture(GL_TEXTURE_2D, textures[7]);
    LoadTGATexture("saturn.tga");
    glBindTexture(GL_TEXTURE_2D, textures[8]);
    LoadTGATexture("uranus.tga");
    glBindTexture(GL_TEXTURE_2D, textures[9]);
    LoadTGATexture("neptune.tga");
}

//rysuj elipse z punktow
void DrawEllipse(float sinus, float cosinus)
{
    glColor3f(1,1,1);
	float x,y,z;
	int t;
	float s = sinus;
	float c = cosinus;
    glBegin(GL_POINTS);
        for(t = 0; t <= 360; t +=1)
        {
            x = s*sin((double)t);
            y = 0;
            z = c*cos((double)t);
            glVertex3f(x,y,z);
        }
    glEnd();
}

// Planety
void sun(){
    GLUquadricObj *slonce;//tworzenie kwadryki
    slonce = gluNewQuadric();

    gluQuadricDrawStyle(slonce, styl);
    gluQuadricOrientation(slonce, orientacja);
    gluQuadricNormals(slonce, wektor);
    gluQuadricTexture(slonce, tekstura);

    // Zdefiniowanie obrotu o kąt a:
	glRotatef(a,sun_pos[0],sun_pos[1],sun_pos[2]);
	//glRotatef( 360.0*DayOfYear/365.0, sun_pos[0], sun_pos[1], sun_pos[2]);
	//glVertexAttrib1f(loc,0.0);

	// Ustawienie koloru obiektu:
	glColor4f(1.000000, 0.843137, 0.000000, 1.000000);

    // Ustawienie aktywnych tekstur i połączenie ich z obiektami tekstur:
	glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, textures[0]);

    // Narysowanie obiektu 3D:
	gluSphere(slonce, 5.0, 30, 30 );

}



void moon(){
	glRotatef(360.0*12.0*DayOfYear / 365.0, 0.0, 1.0, 0.0);
	glTranslatef(0.7, 0.0, 0.0);
	glColor3f(0.3, 0.7, 0.3);
	glutSolidSphere(0.1, 5, 5);
}

void drawPlanet(float distance, float size, GLuint texture){
	glRotatef(360.0*DayOfYear / 365.0, 0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(distance, 0.0, 0.0);
	glColor3f(0.2, 0.2, 1.0);

	GLUquadricObj *planet;
	planet = gluNewQuadric();

	gluQuadricDrawStyle(planet, styl);
	gluQuadricOrientation(planet, orientacja);
	gluQuadricNormals(planet, wektor);
	gluQuadricTexture(planet, tekstura);

	if (texture != NULL){ glBindTexture(GL_TEXTURE_2D, texture); }

	gluSphere(planet, size, 30, 30);
	if (distance == distanceEarth){ moon(); }
	glPopMatrix();
}

// Funkcja konfiguracji sceny
void setupScene(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // glShadeModel( GL_FLAT );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );

    // Ustawienie właściwości materiału:
   // glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   // glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_em);

    // Usatwienie parametrów oświetlenia:
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light);

    // Włączenie oświetlenia:
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

	// Włączenie teksturowania :
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE4); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE5); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE6); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE7); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE8); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE9); glEnable(GL_TEXTURE_2D);
}

// Funkcja renderująca scenę
void display ()
{
    // Usunięcie zawartości buforów koloru i głębi:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (spinMode) {
		// Update the animation state
        HourOfDay += AnimateIncrement;
        DayOfYear += AnimateIncrement/24.0;

        HourOfDay = HourOfDay - ((int)(HourOfDay/24))*24;
        DayOfYear = DayOfYear - ((int)(DayOfYear/365))*365;
		}


    // Ustawienie macierzy modelowania:
    glMatrixMode(GL_MODELVIEW);
    // Wczytanie macierzy jednostkowej:
	glLoadIdentity();

	// Ustawienie kamery:

	/*
	gluLookAt(0.0,0.0,10.0,
		      centerx,centery,centerz,
			  0.0f,1.0f,0.0f);
*/



	// Wycofanie o kilka jednostek do tylu dla lepszego widoku
    glTranslatef ( 0.0, 0.0, zoom );

	glRotatef( angle, rotatex, rotatey, rotatez );
  //   slonce();
	
    sun();
	drawPlanet(distanceMercury, sizeMercury, textures[3]);
	drawPlanet(distanceVenus, sizeVenus, textures[4]);
	drawPlanet(distanceEarth, sizeEarth, textures[2]);
	drawPlanet(distanceMars, sizeMars, textures[5]);
	drawPlanet(distanceJupiter, sizeJupiter, textures[6]);
	drawPlanet(distanceSaturn, sizeSaturn, textures[7]);
	drawPlanet(distanceUranus, sizeUranus, textures[8]);
	drawPlanet(distanceNeptune, sizeNeptune, textures[9]);


	//elipsy, orbity, przypominajace asteroidy
    //pas 1
    for(float a = 34.0; a <= 85.0; a=a+5.0){
        DrawEllipse(a,a);
    }

    glFlush();
    glutSwapBuffers();

	if ( singleStep ) {
		spinMode = GL_FALSE;
	}

	glutPostRedisplay();

/*
    // Cykliczne wywołanie funkcji renderującej scenę:
	glutPostRedisplay();

    // Wywołanie zamiany buforów:
	glutSwapBuffers();
*/
}

// Funckja zmiany wielkości okna
static void reshape (int w,int h)
{
	float ratio = 1.0* w / h;

	// Ustawienie macierzy rzutowania:
	glMatrixMode(GL_PROJECTION);
	// Wczytanie macierzy jednostkowej:
	glLoadIdentity();

	// Ustawienie widoku modelu dla całęgo okna:
    glViewport(0, 0, w, h);

	// Ustawienie poprawnej perspektywy
	gluPerspective(45,ratio,1,1000);

	display();
}
// Funkcja bosługująca klawiaturę
static void normalKeys(unsigned char Key, int x, int y) {
  switch ( Key ) {
	case 'R':
	case 'r':
		Key_r();	break;
	case 's':
	case 'S':
		Key_s();	break;
    case 'q':  angle++;   break;
    case 'a':  angle--;   break;
    case 'z':  rotatex++; break;
    case 'x':  rotatex--; break;
    case 'c':  rotatey++; break;
    case 'v':  rotatey--; break;
    case 'b':  rotatez++; break;
    case 'n':  rotatez--; break;

	case 27:	// Escape key
		exit(1);
	}

    // narysowanie sceny
    reshape (glutGet (GLUT_WINDOW_WIDTH),glutGet (GLUT_WINDOW_HEIGHT));
}

// Konkretne klawisze
static void Key_r(void)
{
	if ( singleStep ) {			// If ending single step mode
		singleStep = GL_FALSE;
		spinMode = GL_TRUE;		// Restart animation
	}
	else {
		spinMode = !spinMode;	// Toggle animation on and off.
	}
}

static void Key_s(void)
{
	singleStep = GL_TRUE;
	spinMode = GL_TRUE;
}

static void Key_up(void)
{
    AnimateIncrement *= 1.1;
}

static void Key_down(void)
{
    AnimateIncrement /= 1.1;

}

// Znaki specjalne
static void SpecialKeyFunc( int Key, int x, int y )
{
	switch ( Key ) {
	case GLUT_KEY_UP:
		Key_up();
		break;
	case GLUT_KEY_DOWN:
		Key_down();
		break;
    case GLUT_KEY_LEFT:
        zoom--; break;
    case GLUT_KEY_RIGHT:
        zoom++; break;
	}
}

int main(int argc, char *argv[])
{

  // Inicjalizacja biblioteki GLUT:
    glutInit (&argc,argv);

  // Inicjalizacja bufora ramki:
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // Rozmiary głównego okna programu:
    glutInitWindowSize (WIDTH,HEIGHT);

  // Utworzenie głównego okna programu:
    glutCreateWindow ("Uklad sloneczny v0.2");

  // Inicjalizacja biblioteki GLEW:
    glewInit();

  // Sprawdzenie wsparcia OpenGL 2.0:
    if (!glewIsSupported("GL_VERSION_2_0"))
    {
        puts("OpenGL 2.0 nie jest wspierany\n");
        exit(1);
    }

  // Rejestracja funkcji zmiany okna
	glutReshapeFunc(reshape);

  // Rejestracja funkcji renderującej scenę
	glutDisplayFunc(display);

  // Rejestracja funkcji obsługującej klawiaturę
	glutKeyboardFunc(normalKeys);

  // Znaki specjalne
	glutSpecialFunc( SpecialKeyFunc );

  // Konfiguracja shaderów:
	setupShaders("teksturowanie_osw.vert", "teksturowanie_osw.frag");

  // Konfiguracja Tekstur:
    setupTexture();
  // Konfiguracja Sceny:
    setupScene();

  // Główna pętla programu:
    glutMainLoop();

  return 0;
}



