/******************************************************************************
 Myles Hosken 20124720 Assignment 1 Snow Scene
 ******************************************************************************/
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/
  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60
#define TRUE 1
#define FALSE 0
#define PI 3.141592653589793238
#define MAX_PARTICLES 1000

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;
// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;
// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;
/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/
 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
// characters typed by the user to lowercase, so the SHIFT key is ignored.
#define KEY_EXIT 113 // 'ESC' key
#define SNOW_TOGGLE 115 //'S' key 
#define LIGHT_TOGGLE 102 //'F' key
#define DIAGNOSTICS 100 //'D' key
/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

typedef struct Color {
	float r;
	float g;
	float b;
	float alpha;
}Color;

typedef struct Circle {
	float cx;
	float cy;
	float radius;
}Circle;

typedef struct Position2xy {
	float x;
	float y;
}Position2xy;

typedef struct Particle_t {
	Position2xy pos;
	float size;
	float dy;
	int timer;
	int active;
	Color color;
}Particle_t;

typedef struct Particle_flake {
	Position2xy pos;
	int randString;
	int fadeOut;
	float alpha;
	int active;
}Particle_flake;

int snowFall = FALSE;
int lightSwitch = FALSE;
int showDiagnostics = TRUE;

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void drawCircle(Circle c, Color color, int isClear);
void drawGround(void);
void drawBackGround(void);
void drawFace(Circle c);
void drawSnowMan(void);
void spawnParticle(Particle_t* p, Position2xy pos);
void initiliazeParticles(Particle_t p[]);
void spawnParticles(void);
void drawDiagnostics(void);
void printString(char* text, int value, float x, float y);
void resetParticle(Particle_t* p, int snowFall, double timeSince);
int countParticles(void);
void calculateBackGroundHeights(void);
void drawSnowFlakes(void);
void initializeFlakes(void);
void renderFlake(Particle_flake* p, int frameCounter);
/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/
void main(int argc, char** argv);
void init(void);
void think(void);
/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/
Particle_t particleSystem[MAX_PARTICLES];
Particle_flake flakeSystem[MAX_PARTICLES];
Circle globeCircle = { 0.0f, 0.0f, 1 };
Circle c1 = { 0.0f, -0.61f, 0.1f };
Circle c2 = { 0.0f, -0.5f, 0.075f };
Circle c3 = { 0.0f, -0.45f, 0.06f };//Circle responsible for head
const float windowWidth = 800.0f;
const float windowHeight = 800.0f;

float range[] = { -1.0f, 1.0f };
float activeHeight[] = { 1.2f, 1.45f };
float inactiveHeight[] = { 1.5f, 2.0f };
float fallRange[] = { 0.001f,0.002f };

int totalParticles = 0;
unsigned int frameCount = 0;
int flakeCounter = 0;

Position2xy layer1[12];
Position2xy layer2[10];
Position2xy layer3[3];

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/
void main(int argc, char** argv)
{
	srand((unsigned int)time(0));
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize((int)windowWidth, (int)windowHeight);
	glutCreateWindow("Animation");
	// Set up the scene.
	init();
	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(&keyPressed);
	glutIdleFunc(idle);
	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);
	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}
/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/
 /*
 Called when GLUT wants us to (re)draw the current animation frame.
 Note: This function must not do anything to update the state of our
simulated
 world. Animation (moving or rotating things, responding to keyboard input,
 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	frameCount++;
	/*
	TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE
	Separate reusable pieces of drawing code into functions, which you can
	add
	to the "Animation-Specific Functions" section below.
	Remember to add prototypes for any new functions to the "Animation-
	Specific
	Function Prototypes" section near the top of this template.
	*/
	drawBackGround();
	drawGround();
	drawSnowMan();
	spawnParticles();
	drawSnowFlakes();
	drawDiagnostics();
	glutSwapBuffers();
}
/*
Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{

}
/*
Called each time a character key (e.g. a letter, number, or symbol) is
pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
		TEMPLATE: Add any new character key controls here.
		Rather than using literals (e.g. "d" for diagnostics), create a
		new KEY_
		definition in the "Keyboard Input Handling Setup" section of this
		file.
		*/
	case LIGHT_TOGGLE:
		lightSwitch = !lightSwitch;
		break;
	case DIAGNOSTICS:
		showDiagnostics = !showDiagnostics;
		break;
	case SNOW_TOGGLE:
		snowFall = !snowFall;
		frameCount = 0;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}
/*
Called by GLUT when it's not rendering a frame.
Note: We use this to handle animation and timing. You shouldn't need to
modify
this callback at all. Instead, place your animation logic (e.g. moving or
rotating
things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.
	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) -
		frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
			// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}
	// Begin processing the next frame.
	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.
	think(); // Update our simulated world before the next call to display().
	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}
/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/
 /*
 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	// set background color to be black
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// set the drawing to be blue
	glColor3f(0.0f, 0.0f, 0.0f);
	// set to projection mode
	glMatrixMode(GL_PROJECTION);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	// load the identity matrix
	glLoadIdentity();
	// set window mode to 2D orthographica and set the window size 
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	initiliazeParticles(particleSystem);
	initializeFlakes();
	calculateBackGroundHeights();
}
/*
Advance our animation by FRAME_TIME milliseconds.
Note: Our template's GLUT idle() callback calls this once before each new
frame is drawn, EXCEPT the very first frame drawn after our application
starts. Any setup required before the first frame is drawn should be placed
in init().
*/
void think(void)
{
	totalParticles = countParticles();
	if (frameCount > 10000)
	{
		frameCount = 0;
	}
	/*
	TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE
	In this function, we update all the variables that control the animated
	parts of our simulated world. For example: if you have a moving box,
	this is
	where you update its coordinates to make it move. If you have something
	that
	spins around, here's where you update its angle.
	NOTHING CAN BE DRAWN IN HERE: you can only update the variables that
	control
	how everything will be drawn later in display().
	How much do we move or rotate things? Because we use a fixed frame
	rate, we
	assume there's always FRAME_TIME milliseconds between drawing each
	frame. So,
	every time think() is called, we need to work out how far things should
	have
	moved, rotated, or otherwise changed in that period of time.
	Movement example:
	* Let's assume a distance of 1.0 GL units is 1 metre.
	* Let's assume we want something to move 2 metres per second on the x
	axis
	* Each frame, we'd need to update its position like this:
	x += 2 * (FRAME_TIME / 1000.0f)
	* Note that we have to convert FRAME_TIME to seconds. We can skip this
	by
	  using a constant defined earlier in this template:
	x += 2 * FRAME_TIME_SEC;
	Rotation example:
	* Let's assume we want something to do one complete 360-degree rotation
	every
	  second (i.e. 60 Revolutions Per Minute, or RPM).
	* Each frame, we'd need to update our object's angle like this (we'll
	use the
	  FRAME_TIME_SEC constant as per the example above):
	a += 360 * FRAME_TIME_SEC;
	This works for any type of "per second" change: just multiply the
	amount you'd
	want to move in a full second by FRAME_TIME_SEC, and add or subtract
	that
	from whatever variable you're updating.
	You can use this same approach to animate other things like color,
	opacity,
	brightness of lights, etc.
	*/

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		//if snowfall is on, trigger snow to fall
		//if the trigger is on and the particle timer is triggered, turn active on
		//if the snow is active, and snowfall is off, keep falling snow
		//if the snow hits the bottom, reset position
		//if the snow resets and snowfall is false, randomise timer

		Particle_t* snow = &particleSystem[i];
		Particle_flake* flake = &flakeSystem[i];

		if ((snowFall && snow->timer == 0) && !snow->active)
		{
			resetParticle(snow, !snowFall, frameCount);
		}

		if (snow->timer > 0)
		{
			if ((frameCount % snow->timer == 0) && snowFall)
			{
				snow->active = TRUE;
			}
		}

		if (snow->active == TRUE)
		{
			snow->pos.y -= snow->dy;
		}

		if (snow->pos.y < -1.0f)
		{
			resetParticle(snow, snowFall, frameCount);
		}

		if (!snowFall && !snow->active)
		{
			snow->timer = 0;
		}


		if (snow->pos.y < 1.0f)
		{
			flake->active = TRUE;
		}
		else
		{
			flake->active = FALSE;
		}
	}
}

void calculateBackGroundHeights(void)
{

	//Background layer
	layer1[0] = (Position2xy){ 0, 0 };//1
	layer1[1] = (Position2xy){ -1.0f, -0.1f };//1
	layer1[2] = (Position2xy){ -1.0f, 0.7f };//2
	float temp = ((1.0f - 0.8f) * ((float)rand() / RAND_MAX)) + 0.8f;
	layer1[3] = (Position2xy){ -0.9f, temp };//3
	temp = ((0.7f - 0.5f) * ((float)rand() / RAND_MAX)) + 0.5f;
	layer1[4] = (Position2xy){ -0.7f, temp };//4
	temp = ((0.8f - 0.6f) * ((float)rand() / RAND_MAX)) + 0.6f;
	layer1[5] = (Position2xy){ -0.4f, temp };//5
	temp = ((0.6f - 0.4f) * ((float)rand() / RAND_MAX)) + 0.4f;
	layer1[6] = (Position2xy){ 0.2f, temp };//6
	temp = ((0.7f - 0.5f) * ((float)rand() / RAND_MAX)) + 0.5f;
	layer1[7] = (Position2xy){ 0.4f, temp };//7
	temp = ((0.5f - 0.3f) * ((float)rand() / RAND_MAX)) + 0.3f;
	layer1[8] = (Position2xy){ 0.7f, temp };//8
	temp = ((0.8f - 0.6f) * ((float)rand() / RAND_MAX)) + 0.6f;
	layer1[9] = (Position2xy){ 0.9f, temp };//9  
	layer1[10] = (Position2xy){ 1.1f, 0.6f };//10
	layer1[11] = (Position2xy){ 1.1f, -0.1f };//11

	//Middle layer
	layer2[0] = (Position2xy){ 0.0f, -0.2f };//1
	layer2[1] = (Position2xy){ -1.0f, -0.2f };//2
	layer2[2] = (Position2xy){ -1.0f, 0.35f };//3
	temp = ((0.4f - 0.3f) * ((float)rand() / RAND_MAX)) + 0.3f;
	layer2[3] = (Position2xy){ -0.8f, temp };//4
	temp = ((0.2f - 0.1f) * ((float)rand() / RAND_MAX)) + 0.1f;
	layer2[4] = (Position2xy){ -0.2f, temp };//5
	layer2[5] = (Position2xy){ 0.1f, temp };//6
	temp = ((0.4f - 0.3f) * ((float)rand() / RAND_MAX)) + 0.3f;
	layer2[6] = (Position2xy){ 0.7f, temp };//7
	layer2[7] = (Position2xy){ 1.1f, 0.2f };//8
	layer2[8] = (Position2xy){ 1.1f, -0.2f };//9
	layer2[9] = (Position2xy){ 0.0f, -0.2f };//10

	//Top layer
	temp = ((0.8f - 0.6f) * ((float)rand() / RAND_MAX)) + 0.6f;
	layer3[0] = (Position2xy){ temp * -1,-0.4f };
	temp = ((0.7f - 0.5f) * ((float)rand() / RAND_MAX)) + 0.5f;
	layer3[1] = (Position2xy){ 0.0f, temp };
	temp = ((0.7f - 0.5f) * ((float)rand() / RAND_MAX)) + 0.5f;
	layer3[2] = (Position2xy){ temp, -0.4f };

}

int countParticles(void)
{
	int count = 0;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		Particle_t* snow = &particleSystem[i];

		if (snow->pos.y >= -1.0f && snow->pos.y <= 1.0f)
		{
			count++;
		}
	}
	return count;
}

void drawCircle(Circle c, Color color, int isClear)
{
	float alphaValue = isClear ? color.alpha : 1;
	glColor4f(color.r, color.g, color.b, alphaValue);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(c.cx, c.cy);
	if (!isClear) { glColor3f(0.5, 0.5, 1); }

	for (int theta = 0; theta <= 360; theta++)
	{
		float angle = (float)(theta * PI / 180.0f);
		float x = (float)cos(angle) * c.radius + c.cx;
		float y = (float)sin(angle) * c.radius + c.cy;
		glVertex2f(x, y);
	}

	glEnd();
}

void drawGround(void)
{
	//Far layer
	glColor3f(0, 0, 0.3f);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 12; i++)
	{
		glVertex2f(layer1[i].x, layer1[i].y);
	}
	glEnd();

	//fading square in here (mist)
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex2f(1.0f, -1.1f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glVertex2f(1.0f, 1.1f);
	glVertex2f(-1.0f, 1.1f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, -1.1f);
	glEnd();

	//Middle layer
	glColor3f(29.0f / 255.0f, 39.0f / 255.0f, 138.0f / 255.0f);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 10; i++)
	{
		glVertex2f(layer2[i].x, layer2[i].y);
	}
	glEnd();

	//Closest mountain
	glColor3f(80.0f / 255.0f, 80.0f / 255.0f, 176.0f / 255.0f);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 3; i++)
	{
		glVertex2f(layer3[i].x, layer3[i].y);
	}
	glEnd();

	//far ground ellipse
	glColor3f(147.0f / 255.0f, 153.0f / 255.0f, 189.0f / 255.0f);
	int segments = 360;
	float xRad = 2.0f;
	float yRad = 0.6f;
	glBegin(GL_POLYGON);
	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.0f * PI * (float)i / (float)segments;
		float x = xRad * cosf(theta) + -0.7f;
		float y = yRad * sinf(theta) - 0.7f;
		glVertex2f(x, y);
	}
	glEnd();

	//closest ground ellipse
	glColor3f(191.0f / 255.0f, 198.0f / 255.0f, 242.0f / 255.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.0f * PI * (float)i / (float)segments;
		float x = xRad * cosf(theta) + 0.7f;
		float y = yRad * sinf(theta) - 0.75f;
		glVertex2f(x, y);
	}
	glEnd();


	float lampCentre = 0.8f;
	//light beam
	if (lightSwitch)
	{
		glColor4f(186.0f / 255.0f, 250.0f / 255.0f, 37.0f / 255.0f, 0);
		glBegin(GL_POLYGON);
		glVertex2f(lampCentre + 0.325f, -1.0f);
		glColor4f(186.0f / 255.0f, 250.0f / 255.0f, 37.0f / 255.0f, 1.0f);
		glVertex2f(lampCentre + 0.125f, 0.65f);
		glVertex2f(lampCentre - 0.125f, 0.65f);
		glColor4f(186.0f / 255.0f, 250.0f / 255.0f, 37.0f / 255.0f, 0);
		glVertex2f(lampCentre - 0.325f, -1.0f);
		glEnd();

		//lightbulb
		Circle globe = { lampCentre, 0.6f, 0.06f };
		Color globeColor = { 0, 255.0f, 255.0f, 0.5f };
		drawCircle(globe, globeColor, TRUE);
	}

	//lamppost
	glColor3f(14.0f / 255.0f, 14.0f / 255.0f, 38.0f / 255.0f);
	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.1f, -0.9f);
	glVertex2f(lampCentre + 0.1f, -0.85f);
	glVertex2f(lampCentre - 0.1f, -0.85f);
	glVertex2f(lampCentre - 0.1f, -0.9f);
	glEnd();
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.09f, -0.85f);
	glColor3f(31.0f / 255.0f, 31.0f / 255.0f, 46.0f / 255.0f);
	glVertex2f(lampCentre + 0.05f, -0.7f);
	glVertex2f(lampCentre - 0.05f, -0.7f);
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glVertex2f(lampCentre - 0.09f, -0.85f);
	glEnd();
	glColor3f(14.0f / 255.0f, 14.0f / 255.0f, 38.0f / 255.0f);
	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.05f, -0.7f);
	glVertex2f(lampCentre + 0.035f, -0.68f);
	glVertex2f(lampCentre - 0.035f, -0.68f);
	glVertex2f(lampCentre - 0.05f, -0.7f);
	glEnd();
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glBegin(GL_POLYGON);//Main post
	glVertex2f(lampCentre + 0.025f, -0.68f);
	glColor3f(31.0f / 255.0f, 31.0f / 255.0f, 46.0f / 255.0f);
	glVertex2f(lampCentre + 0.025f, 0.4f);
	glVertex2f(lampCentre - 0.025f, 0.4f);
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glVertex2f(lampCentre - 0.025f, -0.68f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.025f, 0.4f);
	glVertex2f(lampCentre + 0.035f, 0.4135f);
	glVertex2f(lampCentre - 0.035f, 0.4135f);
	glVertex2f(lampCentre - 0.025f, 0.4f);
	glEnd();
	glColor3f(31.0f / 255.0f, 31.0f / 255.0f, 46.0f / 255.0f);
	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.05f, 0.4135f);
	glVertex2f(lampCentre + 0.06f, 0.4215f);
	glVertex2f(lampCentre - 0.06f, 0.4215f);
	glVertex2f(lampCentre - 0.05f, 0.4135f);
	glEnd();

	//Glass pane
	glColor4f(207.0f / 255.0f, 207.0f / 255.0f, 227.0f / 255.0f, 0.5f);
	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.06f, 0.4215f);
	glColor4f(165.0f / 255.0f, 165.0f / 255.0f, 227.0f / 184.0f, 0.8f);
	glVertex2f(lampCentre + 0.125f, 0.65f);
	glVertex2f(lampCentre - 0.125f, 0.65f);
	glColor4f(207.0f / 255.0f, 207.0f / 255.0f, 227.0f / 255.0f, 0.5f);
	glVertex2f(lampCentre - 0.06f, 0.4215f);
	glEnd();

	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glLineWidth(1.5f);
	//glass frame
	glBegin(GL_LINE_LOOP);
	glVertex2f(lampCentre + 0.06f, 0.4215f);
	glVertex2f(lampCentre + 0.125f, 0.65f);
	glVertex2f(lampCentre - 0.125f, 0.65f);
	glVertex2f(lampCentre - 0.06f, 0.4215f);
	glEnd();
	//glass frame 1
	glBegin(GL_LINES);
	glVertex2f(lampCentre + 0.04f, 0.4215f);
	glVertex2f(lampCentre + 0.06f, 0.65f);
	glEnd();
	//glass frame 2
	glBegin(GL_LINES);
	glVertex2f(lampCentre - 0.04f, 0.4215f);
	glVertex2f(lampCentre - 0.06f, 0.65f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(lampCentre + 0.155f, 0.65f);
	glVertex2f(lampCentre + 0.155f, 0.665f);
	glVertex2f(lampCentre - 0.155f, 0.665f);
	glVertex2f(lampCentre - 0.155f, 0.65f);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glVertex2f(lampCentre + 0.135f, 0.665f);
	glColor3f(31.0f / 255.0f, 31.0f / 255.0f, 46.0f / 255.0f);
	glVertex2f(lampCentre + 0.055f, 0.7f);
	glVertex2f(lampCentre - 0.055f, 0.7f);
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glVertex2f(lampCentre - 0.135f, 0.665f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(lampCentre + 0.05f, 0.7f);
	glColor3f(31.0f / 255.0f, 31.0f / 255.0f, 46.0f / 255.0f);
	glVertex2f(lampCentre, 0.8f);
	glColor3f(48.0f / 255.0f, 48.0f / 255.0f, 69.0f / 255.0f);
	glVertex2f(lampCentre - 0.05f, 0.7f);
	glEnd();

}

void drawSnowMan(void)
{
	Color snowManColor = { 124.0f, 229.0f, 252.0f, 0.8f };
	drawCircle(c1, snowManColor, FALSE);
	drawCircle(c2, snowManColor, FALSE);
	drawCircle(c3, snowManColor, FALSE);
	drawFace(c3);

	glColor3f(0.0f, 0.0f, 0.0f);
	int seg = 360;
	float xRad = c3.radius;
	float yRad = 0.017f;
	glBegin(GL_POLYGON);
	for (int i = 0; i <= seg; i++)
	{
		float theta = 2.0f * 3.141f * (float)i / (float)seg;
		float x = xRad * cosf(theta);
		float y = yRad * sinf(theta) - 0.4f;
		glVertex2f(x, y);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(c3.cx + 0.045f, -0.4f);
	glColor3f(82.0f / 255.0f, 80.0f / 255.0f, 74.0f / 255.0f);
	glVertex2f(c3.cx + 0.045f, -0.3f);
	glVertex2f(c3.cx - 0.045f, -0.3f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(c3.cx - 0.045f, -0.4f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex2f(c3.cx + 0.045f, -0.4f);
	glVertex2f(c3.cx + 0.045f, -0.39f);
	glVertex2f(c3.cx - 0.045f, -0.39f);
	glVertex2f(c3.cx - 0.045f, -0.4f);
	glEnd();
}

void drawFace(Circle c)
{
	glColor3f(0, 0, 0);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glVertex2f(c3.cx - 0.03f, c3.cy + 0.025f);
	glVertex2f(c3.cx + 0.03f, c3.cy + 0.025f);
	glEnd();
}

void initializeFlakes(void)
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		int temp = (rand() % 4) + 1;
		Position2xy pos = { 0,0 };
		switch (temp)
		{
		case 1:
			pos.x = (1.0f - (-1.0f)) * ((float)rand() / RAND_MAX) + (-1.0f);
			pos.y = (1.0f - (0.8f)) * ((float)rand() / RAND_MAX) + (0.8f);
			break;
		case 2:
			pos.x = (1.0f - (0.8f)) * ((float)rand() / RAND_MAX) + (0.8f);
			pos.y = (1.0f - (-1.0f)) * ((float)rand() / RAND_MAX) + (-1.0f);
			break;
		case 3:
			pos.x = (1.0f - (-1.0f)) * ((float)rand() / RAND_MAX) + (-1.0f);
			pos.y = (-1.0f - (-0.8f)) * ((float)rand() / RAND_MAX) + (-0.8f);
			break;
		case 4:
			pos.x = (-1.0f - (-0.8f)) * ((float)rand() / RAND_MAX) + (-0.8f);
			pos.y = (1.0f - (-1.0f)) * ((float)rand() / RAND_MAX) + (-1.0f);
			break;
		default:
			break;
		}
		flakeSystem[i].fadeOut = (rand() % 15) + 1;
		flakeSystem[i].pos = pos;
		flakeSystem[i].randString = (rand() % 7) + 1;
		flakeSystem[i].alpha = (1.0f - (0.1f)) * ((float)rand() / RAND_MAX) + (0.1f);
		flakeSystem[i].active = FALSE;
	}
}

void drawSnowFlakes(void)
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (flakeSystem[i].active)
		{
			renderFlake(&flakeSystem[i], TRUE);
		}
		else
		{
			renderFlake(&flakeSystem[i], FALSE);
		}
	}

}

void IrenderFlake(Particle_flake p, int frameCounter)
{
	//int currentTime = frameCounter;

	if (frameCounter == TRUE)
	{
		glColor4f(1, 1, 1, p.alpha);
		glRasterPos2f(p.pos.x, p.pos.y);
		switch (p.randString)
		{
		case 1:
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '*');
			break;
		case 2:
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '*');
			break;
		case 3:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, '*');
			break;
		case 4:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '*');
			break;
		case 5:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '*');
			break;
		case 6:
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '*');
			break;
		case 7:
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '*');
			break;
		default:
			break;
		}
	}
}

void renderFlake(Particle_flake* p, int frameCounter)
{
	if (frameCounter == TRUE)
	{
		glColor4f(1, 1, 1, p->alpha);
		glRasterPos2f(p->pos.x, p->pos.y);
		switch (p->randString)
		{
		case 1:
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '*');
			break;
		case 2:
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '*');
			break;
		case 3:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, '*');
			break;
		case 4:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '*');
			break;
		case 5:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '*');
			break;
		case 6:
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '*');
			break;
		case 7:
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '*');
			break;
		default:
			break;
		}
	}
	else {
		glColor4f(1, 1, 1, 0);
		glRasterPos2f(p->pos.x, p->pos.y);
		switch (p->randString)
		{
		case 1:
			glRasterPos2f(p->pos.x, p->pos.y);
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ' ');
			break;
		case 2:
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
			break;
		case 3:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, ' ');
			break;
		case 4:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ' ');
			break;
		case 5:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ' ');
			break;
		case 6:
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, ' ');
			break;
		case 7:
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
			break;
		default:
			break;
		}
	}
}

void drawDiagnostics(void)
{
	if (showDiagnostics)
	{
		glColor3f(0, 0, 0);
		glBegin(GL_QUADS);
		glVertex2f(-0.5f, 0.70f);
		glVertex2f(-0.5f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(-1.0f, 0.70f);
		glEnd();
		glColor3f(1, 1, 1);
		printString("Total particles: ", totalParticles, -0.97f, 0.95f);
		glRasterPos2f(-0.97f, 0.90f);
		glutBitmapString(GLUT_BITMAP_8_BY_13, "Toggle snow: 's'");
		glRasterPos2f(-0.97f, 0.85f);
		glutBitmapString(GLUT_BITMAP_8_BY_13, "Toggle light: 'f'");
		glRasterPos2f(-0.97f, 0.80f);
		glutBitmapString(GLUT_BITMAP_8_BY_13, "Toggle diagnostics: 'd'");
		glRasterPos2f(-0.97f, 0.75f);
		glutBitmapString(GLUT_BITMAP_8_BY_13, "Quit program: 'q'");
	}
}

void printString(char* text, int value, float x, float y)
{
	char temp[100];
	snprintf(temp, 100, "%s: %d", text, value);
	glColor3f(1, 1, 1);
	glRasterPos2f(x, y);
	for (int i = 0; i < strlen(temp); i++) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, temp[i]);
	}

}

void drawBackGround(void)
{
	glBegin(GL_POLYGON);
	glColor3f(0.8f, 1, 1);
	glVertex2f(1.0f, -1.0f);
	glColor3f(0, 0.3f, 1);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glColor3f(0.8f, 1, 1);
	glVertex2f(-1.0f, -1.0f);
	glEnd();
}

void spawnParticle(Particle_t* p, Position2xy pos)
{
	glPointSize(p->size);
	glColor4f(p->color.r, p->color.g, p->color.b, p->color.alpha);
	glBegin(GL_POINTS);
	glVertex2f(pos.x, pos.y);
	glEnd();
}

void spawnParticles(void)
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		Particle_t* snow = &particleSystem[i];
		spawnParticle(snow, snow->pos);
	}
}

void resetParticle(Particle_t* p, int snowFall, double timeSince)
{
	float min = 0;
	float max = 0;

	switch (snowFall)
	{
	case FALSE:
	{
		min = inactiveHeight[0];
		max = inactiveHeight[1];
		break;
	}
	case TRUE:
	{
		min = activeHeight[0];
		max = activeHeight[1];
		break;
	}
	default:
		break;
	}


	Position2xy startingPos = { ((range[1] - range[0]) * ((float)rand() / RAND_MAX)) + range[0],
								((max - min) * ((float)rand() / RAND_MAX)) + min };
	p->pos = startingPos;
	p->size = (float)(rand() % 6) + 2;
	p->dy = (((p->size) * FRAME_TIME_SEC / 40));
	p->color.alpha = ((range[1] - range[0]) * ((float)rand() / RAND_MAX)) + range[0];
	p->timer = ((rand() % 15) + 1) + timeSince;
	if (!snowFall)
	{
		p->active = FALSE;
	}
}

void initiliazeParticles(Particle_t p[])
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		Position2xy startingPos = { ((range[1] - range[0]) * ((float)rand() / RAND_MAX)) + range[0],
									((inactiveHeight[1] - inactiveHeight[0]) * ((float)rand() / RAND_MAX)) + inactiveHeight[0] };
		p[i].pos = startingPos;
		p[i].size = (float)(rand() % 6) + 2;
		p[i].dy = ((((p[i].size) / 100)) * FRAME_TIME_SEC);
		p[i].active = FALSE;
		p[i].timer = 0;
		p[i].color.alpha = ((range[1] - range[0]) * ((float)rand() / RAND_MAX)) + range[0];
		p[i].color.r = 1;
		p[i].color.g = 1;
		p[i].color.b = 1;
	}
}
/******************************************************************************/
