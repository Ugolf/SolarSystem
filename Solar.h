/*
 Solar.h
*/
float sizeEarth = 0.4;
float sizeMercury = sizeEarth * 0.4;
float sizeVenus = sizeEarth * 0.9;
float sizeMars = sizeEarth * 0.5;
float sizeJupiter = sizeEarth * 11.0;
float sizeSaturn = sizeEarth * 9.4;
float sizeUranus = sizeEarth * 4.0;
float sizeNeptune = sizeEarth * 3.8;
float sizeSun = sizeEarth * 109.0;

float distanceEarth = 20.0;
float distanceMercury = distanceEarth * 0.4;
float distanceVenus = distanceEarth * 0.7;
float distanceMars = distanceEarth * 1.5;
float distanceJupiter = distanceEarth * 5.2;
float distanceSaturn = distanceEarth * 9.5;
float distanceUranus = distanceEarth * 19.6;
float distanceNeptune = distanceEarth * 30;

float weightEarth = 1.0;
float weightMercury = weightEarth * 0.05;
float weightVenus = weightEarth * 0.81;
float weightMars = weightEarth * 0.107;
float weightJupiter = weightEarth * 317;
float weightSaturn = weightEarth * 95;
float weightUranus = weightEarth * 14;
float weightNeptune = weightEarth * 17;
float weightSun = weightEarth * 332950;

static void display(void );
static void Key_r(void );
static void Key_s(void );
static void Key_up(void );
static void Key_down(void );

static void normalKeys(unsigned char Key, int x, int y);
static void SpecialKeyFunc( int Key, int x, int y );
static void reshape(int w,int h);

void checkErrors(GLuint object);
void setupShaders(char *fileVert, char *fileFrag);
void drawPlanet(float distance, float size, GLuint texture);
