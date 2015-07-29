//Dragan Erak
//Student #: 1205351
//CS 3GC3
//Assignment 2
//November 5, 2014




#include <windows.h>
#include <stdio.h>
#include <gl/glut.h>
#include <gl/gl.h>

#include <stdlib.h> 
#include <ctime>

#include <iostream> //Used for the vector class.
#include <vector>


//I took the code for drawing the box from snowman.cpp, which was posted on avenue by Dr.Teather.
//I used this site for help on using the rand() function. http://www.cplusplus.com/forum/beginner/98214/
//I used this site for help on using the vector class. http://www.cplusplus.com/reference/vector/vector/
//I used this site to figure out the ascii for spacebar http://compsci.ca/v3/viewtopic.php?t=17634



typedef struct particle //This struct holds all of the variables that will be needed for every particle of type particle made.
{
  float px, py, pz; //current particle position
  float dx, dy, dz; //particle movement direction
  float pspeed; //particle movement speed
  float rx, ry, rz; //rotation parameters for particle
  int size; //size of particle
  float r, g, b; //rgb color values of the particle
  float age; //age of particle
}particle;


std::vector<particle> myList; //making the list that I will be using to store all of my particles


//global variable declarations

float verts[8][3] = { {-1,-1,1}, {-1,1,1}, {1,1,1}, {1,-1,1}, {-1,-1,-1}, {-1,1,-1}, {1,1,-1}, {1,-1,-1} }; //vertices for box
float cols[2][3] = { {0,1,1}, {0,1,0} }; //vertices for color of the faces of the box

float camPos[] = {-3, 5, 15};//camera position

float particleorigin[3] = {-4,5,-4}; //origin of particles
float gravity = 0.04; //gravity constant
int frictioncounter; //Frictioncounteer; set in keyboard() with f. Used for turning Friction mode on and off. Used in keyboard().
int trailcounter=1; //Trail counter; set in keyboard() with t. Used for turning Trail mode on and off. Used in keyboard() and display().
int pausecounter=1; //Pause counter; set in keyboard() with p. Used for pausing and playing. Used in keyboard() and idle().


//Cube Drawing Functions. I put them at the beginning so the more important functions are after it.
/* drawPolygon - takes 4 indices and an array of vertices
 *   and draws a polygon using the vertices indexed by the indices
 */
void drawPolygon(int a, int b, int c, int d, float v[8][3])
{ //function for drawing cube (taken from Avenue)
	glBegin(GL_POLYGON);
		glVertex3fv(v[a]);
		glVertex3fv(v[b]);
		glVertex3fv(v[c]);
		glVertex3fv(v[d]);
	glEnd();
}


/* cube - takes an array of 8 vertices, and draws 6 faces
 *  with drawPolygon, making up a box
 */
void cube(float v[8][3]) //function for drawing cube (taken from Avenue)
{
	glColor3fv(cols[0]);
	drawPolygon(0, 3, 2, 1, v);

	glColor3fv(cols[0]);
	drawPolygon(1, 0, 4, 5, v);

	glColor3fv(cols[1]);
	drawPolygon(5, 1, 2, 6, v);
	
	glColor3fv(cols[0]);
	drawPolygon(2, 3, 7, 6, v);
	
	glColor3fv(cols[0]);
	drawPolygon(6, 5, 4, 7, v);

	glColor3fv(cols[1]);
	drawPolygon(4, 0, 3, 7, v);
}


/* drawBox - takes centre point, width, height and depth of a box,
 *  calculates its corner vertices, and draws it with the cube function
 */
void drawBox(float* c, float w, float h, float d) //function for drawing cube(taken from Avenue)
{
	float vertices[8][3] = { {c[0]-w/2, (c[1]-h/2)-1, c[2]+d/2},
							 {c[0]-w/2, (c[1]+h/2)-1, c[2]+d/2},
							 {c[0]+w/2, (c[1]+h/2)-1, c[2]+d/2},
							 {c[0]+w/2, (c[1]-h/2)-1, c[2]+d/2}, 
							 {c[0]-w/2, (c[1]-h/2)-1, c[2]-d/2}, 
							 {c[0]-w/2, (c[1]+h/2)-1, c[2]-d/2}, 
							 {c[0]+w/2, (c[1]+h/2)-1, c[2]-d/2},
							 {c[0]+w/2, (c[1]-h/2)-1, c[2]-d/2} };

	cube(vertices);
}


void init(void)
{
	glClearColor(0, 0, 0, 0); //set background color to black
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION); //set projection matrix
	glLoadIdentity(); //load identity matrix
	gluPerspective(45, 1, 1, 100); //set perspective matrix
}


void newParticle() //creates a new particle
{
	particle myParticle; //Make a particle called myParticle.
	
	myParticle.px= particleorigin[0]; //Set it's origin to the general particle origin which is particleorigin(initialized at top of page)
	myParticle.py= particleorigin[1];	
	myParticle.pz= particleorigin[2];	
	
	myList.push_back(myParticle); //send myParticle to end of myList
	
	float x1 = (rand()%19+1)-5;	 //initialize x1 and y1 which will be used to calculate dx and dy
	float y1 = -5;
	
	myList.back().dx = (x1-myList.back().px) / 500; //Divided by 100 to make it as small as possible(an attempt at normalization).
	myList.back().dy = (-1) * (myList.back().py-y1) / 500;
	myParticle.dz = 0.022; //aquired by trial and error for best value
	
	myParticle.rx=rand()%359+1; //set rotation parameters to random numbers from 1 to 359 degrees
	myParticle.ry=rand()%359+1;
	myParticle.rz=rand()%359+1;
	
	myParticle.age = rand()%200+1; //arbitrary age value
	
	myParticle.pspeed = (rand()%(2+1))+2; //speed values of 4, 5, 6
	myParticle.pspeed*=0.5; //speed values converted to 1, 1.5 ,2
	
	myParticle.dx*=myParticle.pspeed; //adjust direction variables to work with speed given
	myParticle.dy*=myParticle.pspeed;
	myParticle.dz*=myParticle.pspeed;
}


void idle(void) //infinitely creates new particles
{	
    if(pausecounter % 2 == 1) //turning newParticle on and off
	newParticle();
	
	glutPostRedisplay();
}


void update(int i) //The update function. It updates the particles with every step of the animation.
{ 
	if(myList[i].py<=0) //If particles touch the floor, go in opposite direction.
			myList[i].dy*=(-1);
	myList[i].age--; //decrease particles' age
	myList[i].dy-=gravity; //implement gravity
	myList[i].px+=myList[i].dx; //adjust particle positions
	myList[i].py+=myList[i].dy;
	myList[i].pz+=myList[i].dz;
}


void display(void)
{
	float origin[3] = {0,0,0}; //variable holding origin
	
	if(trailcounter % 2 == 1) //Adding and removing color buffer for Trail mode.
		glClear(GL_COLOR_BUFFER_BIT);
		
	glClear(GL_DEPTH_BUFFER_BIT);	
	
	
	glMatrixMode(GL_MODELVIEW); //implement modelview matrix
	glLoadIdentity();
	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0); //initialize program's viewpoints
	glColor3f(1,1,1);
	
	drawBox(origin, 10, 1, 10);//draw cube base
	
	for(int i = 0; i < myList.size(); i++){ //draw particles
		update(i); //update particle variables for each step of the animation
		glPushMatrix();
		glTranslatef(myList[i].px, myList[i].py, myList[i].pz); //draw particle on new coordinates relative to the particle origin
		glRotatef(myList[i].rx, 1, 0, 0); //rotate particle by x axis
		glRotatef(myList[i].ry, 0, 1, 0); //rotate particle by y axis
		glRotatef(myList[i].rz, 0, 0, 1); //rotate particle by z axis
		glColor3f(1,0,0); //red color for particle
		glutWireSphere(0.1, 16, 16); //draw particle
		glPopMatrix(); //return to particle origin for the drawing of the next particle
		if(myList[i].age<=0) //erase the particle if age is over the set limit
				myList.erase(myList.begin()+i);
		}

	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y) //Keyboard function.
{
	switch(key) //Cases depending on what key is pressed.
	{
		case 'r': //Reset the list of all shapes to a blank scene.
			myList.clear();
			break;
			
		case 'f': //Toggle friction mode. I was not able to make friction mode work	
				  //properly, it just turns gravity on and off.
			frictioncounter++;
			
			if(frictioncounter % 2 == 0) //Every even click
				gravity=0.4; //normal gravity
			if(frictioncounter % 2 == 1){ //Every odd click
				gravity=0; //no gravity
				for(int i = 0; i<myList.size(); i++){
					if(myList[i].py>=4) //if particle goes under or on origin for the y axis, make it go in opposite direction
						myList[i].dy*=1;
				}
			}
			
		case 't': //Turns Trail mode on and off in display(). Increments trailcounter used in display().
			trailcounter++; //used in display()
			break;
			
			
		case 'p': //Used for pausing and playing the simulation. Increments pausecounter used in idle().
			pausecounter++; //used in init()
			break;
	}
}


void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
			camPos[0]-=0.1; //rotate about y axis to the left
			break;

		case GLUT_KEY_RIGHT:
			camPos[0]+=0.1; //rotate about y axis to the right
			break;

		
		case GLUT_KEY_UP:
			camPos[1] += 0.1; //rotate about x axis in the up direction
			break;

		case GLUT_KEY_DOWN:
			camPos[1] -= 0.1; //rotate about x axis in the down direction
			break;
	}
	glutPostRedisplay();
}


void timer(int value) //Timer function.	
{
	glutTimerFunc(-10, timer, 0); //slows down animation to make it look nicer
	glutPostRedisplay();
}


/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	
	glutInitWindowSize(800, 800);
	
	glutInitWindowPosition(10, 10);

	glutCreateWindow("Comp Sci 3GC3 Assignment 2");	//creates the window
	glEnable(GL_CULL_FACE); //enable culling
	glCullFace(GL_BACK); //enable back face culling
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard); //keyboard function
	glutSpecialFunc(special); //special keyboard function
	

	glEnable(GL_DEPTH_TEST);
	init();
	glutIdleFunc(idle);
	glutTimerFunc(-10, timer, 0); //timer function
	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}