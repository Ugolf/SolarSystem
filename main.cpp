#ifdef _WIN32
#include "windows.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include "Targa.cpp"
#include "colors.h"
#include "Solar.h"

GLuint shaderVert, shaderFrag;	
GLuint shaderProg;	

enum
{
               
    UKLAD,               
    EXIT                 
};

// Light
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[] = { 0.0, 0.0, 1.0, 1.0 }; 
GLfloat ambient_light[] = {0.2, 0.2, 0.2, 1.0};

GLfloat mat_em[] = { 1.0, 0.9, 0.9, 0.0 };
GLfloat sun_pos[] = { 0.0 , 1.0, 0.0, 1.0 };
GLint loc;

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;

// Animation
static float HourOfDay = 0.0;
static float DayOfYear = 0.0;
static float AnimateIncrement = 24.0;  // time

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

GLuint textures[10];

// Loading shader
char *loadTextFile(const char *fileName) {
	
	FILE *plik = fopen(fileName, "rb");
	if(!plik)	return(NULL);

	// size file
	unsigned long fSize;
	fseek(plik, 0, SEEK_END);
	fSize = ftell(plik);
	fseek(plik, 0, SEEK_SET);

	// open bufor
	char *buff = (char*)malloc(fSize + 1);
	if(!buff)	return(NULL);

	// read data
	fread(buff, 1, fSize, plik);
	buff[fSize] = 0;	

	// close file
	fclose(plik);
	return(buff);
}

void checkErrors(GLuint object) {
	int res, logLen;
	char *buff;

	// check Shader
	glGetShaderiv(object, GL_COMPILE_STATUS, &res);
	if(res == GL_FALSE) {	
		puts("GL_COMPILE_STATUS ERROR!");

		// log size
		glGetShaderiv(object, GL_INFO_LOG_LENGTH,&logLen);
		// Allocate memory for message
		buff = (char *)malloc(logLen);

		glGetShaderInfoLog(object, logLen , NULL, buff);
		puts(buff);

		free(buff);
		exit(0);
	}

	
	glGetProgramiv(object, GL_LINK_STATUS, &res);
	if(res == GL_FALSE) {
		puts("GL_LINK_STATUS ERROR!");

		// log size
		glGetProgramiv(object, GL_INFO_LOG_LENGTH,&logLen);
		// Allocate memory for message
		buff = (char *)malloc(logLen);

		glGetProgramInfoLog(object, logLen, NULL, buff);
		puts(buff);

		free(buff);
		exit(0);
	}
}

// config shader
void setupShaders(char *fileVert, char *fileFrag) {
	char *strVert, *strFrag;

	// crate shaderów:
	shaderVert = glCreateShader(GL_VERTEX_SHADER);
	shaderFrag = glCreateShader(GL_FRAGMENT_SHADER);

	// read shader from file
	strVert = loadTextFile( fileVert );
	if(!strVert) {
		puts("** Error loading file *.vert");
		exit(0);
	}
	strFrag = loadTextFile( fileFrag );
	if(!strFrag) {
		puts("** Error loading file *.frag");
		exit(0);
	}
	// Load shader source code:
	glShaderSource(shaderVert, 1, (const GLchar**)(&strVert), NULL);
	glShaderSource(shaderFrag, 1, (const GLchar**)(&strFrag), NULL);

	// free mem
	free(strVert);free(strFrag);

	// Compiling Shader
	glCompileShader(shaderVert);
	glCompileShader(shaderFrag);

	// create program
	shaderProg = glCreateProgram();

	// Joining shaders to program:
	glAttachShader(shaderProg, shaderVert);
	glAttachShader(shaderProg, shaderFrag);

	// Linking and activation:
	glLinkProgram(shaderProg);
	glUseProgram(shaderProg);

    // Check for errors:
	checkErrors(shaderVert);
	checkErrors(shaderFrag);

	// Setting textures for shader:
	loc = glGetUniformLocation(shaderProg, "tex0");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(shaderProg, "tex1");
	glUniform1i(loc, 1);
}

// Configuration of textures
void setupTexture(){

    // Generating textures of objects:
   glGenTextures(10, textures);

    // Load data into one object texture:
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    if (!LoadTGATexture("tex0.tga"))
    {
        puts("error while loading textures");
        exit(1);
    }

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    if (!LoadTGATexture("tex1.tga"))
    {
        puts("error while loading textures");
        exit(1);
    }
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    if (!LoadTGATexture("earth.tga"))
    {
        puts("error while loading textures");
        exit(1);
    }
    if (!LoadTGAMipmap("earth.tga"))
    {
        puts("error while loading textures");
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
            x = s*sin(t);
            y = 0;
            z = c*cos(t);
            glVertex3f(x,y,z);
        }
    glEnd();
}

// Planet
void sun(){
    GLUquadricObj *slonce;//creating quadrics
    slonce = gluNewQuadric();

    gluQuadricDrawStyle(slonce, styl);
    gluQuadricOrientation(slonce, orientacja);
    gluQuadricNormals(slonce, wektor);
    gluQuadricTexture(slonce, tekstura);

    // Define rotation angle:
	glRotatef(a,sun_pos[0],sun_pos[1],sun_pos[2]);
	//glRotatef( 360.0*DayOfYear/365.0, sun_pos[0], sun_pos[1], sun_pos[2]);
	//glVertexAttrib1f(loc,0.0);

	// Set color of object:
	glColor4f(1.000000, 0.843137, 0.000000, 1.000000);

    // Setting the active textures and combining them with objects textures:
	glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, textures[0]);

    // Drawing a 3D object:
	gluSphere(slonce, 5.0, 30, 30 );

}

void moon(){
   	glRotatef( 360.0*12.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glTranslatef( 0.7, 0.0, 0.0 );
    glColor3f( 0.3, 0.7, 0.3 );
    glutSolidSphere( 0.1, 5, 5 );
}

void earth(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();
    glTranslatef( distanceEarth, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[2]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 0.2, 0.2, 1.0 );
    glutSolidSphere( sizeEarth, 30, 30);
    moon();
    glPopMatrix();
}

void mercury(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();
    glTranslatef( distanceMercury, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[4]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 0.2, 0.2, 1.0 );
    glutSolidSphere( sizeMercury, 30, 30);
    glPopMatrix();
}

void venus(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();
    glTranslatef( distanceVenus, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[4]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 0.0, 0.0, 1.0 );
    glutSolidSphere( sizeVenus, 30, 30);
    glPopMatrix();
}

void mars(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();
    glTranslatef( distanceMars, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[5]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 1.0, 0.0, 0.0 );
    glutSolidSphere( sizeMars, 30, 30);
    glPopMatrix();
}

void jupiter(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );

    glPushMatrix();
    glTranslatef( distanceJupiter, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[6]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 0.2, 0.2, 1.0 );
    glutSolidSphere( sizeJupiter, 30, 30);
    glPopMatrix();
}

void saturn(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();
    glTranslatef( distanceSaturn, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[7]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );
    glColor3f( 0.2, 0.2, 1.0 );
    glutSolidSphere( sizeSaturn, 30, 30);
    glColor3f(0.5, 0.0, 0.5);
    for(float a = 5.0; a <= 7.0; a=a+0.2){
        DrawEllipse(a,a+0.2);
    }
    glPopMatrix();
}

void uranus(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();
    glTranslatef( distanceUranus, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[8]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 0.2, 0.2, 1.0 );
    glutSolidSphere( sizeUranus, 30, 30);
    glPopMatrix();
}

void neptune(){
    glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
    glPushMatrix();

    glTranslatef( distanceNeptune, 0.0, 0.0 );

    glBindTexture(GL_TEXTURE_2D, textures[9]);
	glRotatef( 360.0*HourOfDay/24.0, 0.0, 1.0, 0.0 );

    glColor3f( 0.2, 0.2, 1.0 );
    glutSolidSphere( sizeNeptune, 30, 30);
    glPopMatrix();
}

void setupScene(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // glShadeModel( GL_FLAT );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );

    // Setting the properties of the material:
   // glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   // glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_em);

    // Usatwienie lighting parameters:
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light);

    // Illumination:
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

	// Texturing:
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

void display ()
{
    // Delete the contents of the color and depth buffers:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (spinMode) {
		// Update the animation state
        HourOfDay += AnimateIncrement;
        DayOfYear += AnimateIncrement/24.0;

        HourOfDay = HourOfDay - ((int)(HourOfDay/24))*24;
        DayOfYear = DayOfYear - ((int)(DayOfYear/365))*365;
		}


    // Setting matrix modeling:
    glMatrixMode(GL_MODELVIEW);
    // Load the identity matrix:
	glLoadIdentity();

	// Setting the camera:

	/*
	gluLookAt(0.0,0.0,10.0,
		      centerx,centery,centerz,
			  0.0f,1.0f,0.0f);
*/



	// Withdrawal of a few individuals to many for a better view
    glTranslatef ( 0.0, 0.0, zoom );

	glRotatef( angle, rotatex, rotatey, rotatez );

    sun();
    mercury();
    venus();
    earth();
   // moon();
    mars();
    jupiter();
    saturn();
    uranus();
    neptune();
    //elliptical orbit, resembling asteroids
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
    // Cyclic scene rendering function call:
	glutPostRedisplay();

    // Calling swap buffers:
	glutSwapBuffers();
*/
}

static void reshape (int w,int h)
{
	float ratio = 1.0* w / h;

	// Setting the projection matrix:
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix:
	glLoadIdentity();

	// Sets the view model for whole-window:
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	display();
}
// The keyboard supports
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

    // draw the scene
    reshape (glutGet (GLUT_WINDOW_WIDTH),glutGet (GLUT_WINDOW_HEIGHT));
}

// specific keys
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

// Special Characters
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

  // Initialize GLUT:
    glutInit (&argc,argv);

  // Initialize the frame buffer:
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // Size of the main window:
    glutInitWindowSize (WIDTH,HEIGHT);

  // The creation of the main window:
    glutCreateWindow ("Solar system v0.1");

  // GLEW library initialization:
    glewInit();

  // Checking support OpenGL 2.0:
    if (!glewIsSupported("GL_VERSION_2_0"))
    {
        puts("OpenGL 2.0 is not supported\n");
        exit(1);
    }

  // Since the function changes the window
	glutReshapeFunc(reshape);

  // Since scene rendering function
	glutDisplayFunc(display);

  // Since the keyboard handler
	glutKeyboardFunc(normalKeys);

  // Special Characters
	glutSpecialFunc( SpecialKeyFunc );

  // Configuration Shader
	setupShaders("teksturowanie_osw.vert", "teksturowanie_osw.frag");

  // Configuration Texture:
    setupTexture();
  // Configuring Scenes
    setupScene();

  // The main loop of the program:
    glutMainLoop();

  return 0;
}
