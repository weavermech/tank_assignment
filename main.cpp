//!Includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <Shader.h>
#include <Vector.h>
#include <Matrix.h>
#include <Mesh.h>
#include <Texture.h>
#include <SphericalCameraManipulator.h>
#include <iostream>
#include <math.h>
#include <string>

//!Function Prototypes
bool initGL(int argc, char** argv);
void display(void);
void keyboard(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void handleKeys();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void Timer(int value);
void initShader();					                //Function to init Shader
void initTexture(std::string filename, GLuint & textureID);
void drawCrate(float x, float y);					//draws translated box
void drawCoin(float x, float y);					//draws translated coin
void drawTank();
void drawTurret ();
void drawBullet ();
void drawwheels();
void drawBox(GLuint str, float x, float z, float y, float r);
void render2dText(std::string text, float r, float g, float b, float x, float y);
void reset();
void skipLevel();
void Load1();
void Load2();
void Load3();
void Load4();
void lAccel();
void lDecel();
void rAccel();
void vDecel();



//Global Variables
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		                // Vertex Position Attribute Location


//Material Properties
GLuint LightPositionUniformLocation;                // Light Position Uniform
GLuint AmbientUniformLocation;
GLuint SpecularUniformLocation;
GLuint SpecularPowerUniformLocation;
Vector3f lightPosition= Vector3f(0.0,0.1,1.1);   // Light Position
Vector3f ambient    = Vector3f(0.01,0.01,0.01);
Vector3f specular   = Vector3f(0.1,0.1,0.1);
float specularPower = 10.0;







GLuint vertexNormalAttribute;
GLuint vertexTexcoordAttribute; 					// Vertex Texcoord Attribute Location


GLuint TextureMapUniformLocation;					// Texture Map Location
GLuint crateTexture;
GLuint coinTexture;
GLuint chassisTexture;
GLuint bulletTexture;
GLuint boxTexture;
GLuint frontTexture;
GLuint backTexture;
GLuint leftTexture;
GLuint rightTexture;
GLuint bottomTexture;
GLuint deathTexture;

float t_global = 0;
float time_diff(-1.1);





//Viewing
SphericalCameraManipulator cameraManip;
Matrix4x4 ModelViewMatrix;		                    // ModelView Matrix
GLuint MVMatrixUniformLocation;		                // ModelView Matrix Uniform
Matrix4x4 ProjectionMatrix;		                    // Projection Matrix
GLuint ProjectionUniformLocation;	                // Projection Matrix Uniform Location



//Mesh
Mesh crateMesh;
Mesh coinMesh;
Mesh chassisMesh;
Mesh front_wheelMesh;
Mesh back_wheelMesh;
Mesh turretMesh;
Mesh bulletMesh;

//map - can load this from file later !!top left must be a '1' for start position
int map[8][10];
int xtran(0);
int ytran(0);


//edit humvee
float transHumx = 2.0, transHumz = 1.92f, transHumy = 2.0;
float scaleHumvee = 0.18;
float rotateHumvee = 0.0;
float radHum;
float rotateTurret = 0.0;
float tiltTurret = 0.0;
float rotatewheel = 0.0;
float cpuScale = 0.12;
bool fall(false);

//physics
float lVelo(0);
float rVelo(0);
float vVelo(0);
int spin(0);
bool fspinState;
bool bspinState;

//coin
float coinHeight(0);
int coinsCollected(0);
int coinsLeft(0);

//bullet
bool fired = false;
float bulletPosx = 0.0, bulletPosz = 0.0, bulletPosy = 0.2;
float bulletAngle;
float bulletDir;
float launchTime;

//HUD
int level(1);
int countdown = 2;
int levelTxt (0);

float cameraz(0);




//! Screen size
int screenWidth   	        = 1720;
int screenHeight   	        = 1200;

//! Array of key states
bool keyStates[256];


//! Main Program Entry
int main(int argc, char** argv)
{	
	//init OpenGL
	if(!initGL(argc, argv))
		return -1;

    //Init Key States to false;    
    for(int i = 0 ; i < 256; i++)
        keyStates[i] = false;
    
    //Set up your program
	//Init OpenGL Shader
	initShader();

	//Init Mesh Geometry
	//mesh.initCube();
	crateMesh.loadOBJ("../models/cube.obj");
	coinMesh.loadOBJ("../models/coin.obj");
	chassisMesh.loadOBJ("../models/chassis.obj");
	front_wheelMesh.loadOBJ("../models/front_wheel.obj");
	back_wheelMesh.loadOBJ("../models/back_wheel.obj");
	turretMesh.loadOBJ("../models/turret.obj");
	bulletMesh.loadOBJ("../models/ball.obj");


	//Init Camera Manipultor
	cameraManip.setPanTiltRadius(rotateTurret,1.f,1.f);
	cameraManip.setFocus(Vector3f(transHumx, transHumz, transHumy));

	//load texture models
	initTexture("../models/Crate.bmp", crateTexture);
	initTexture("../models/coin.bmp", coinTexture);
	initTexture("../models/hamvee.bmp", chassisTexture);
	initTexture("../models/ball.bmp", bulletTexture);
	initTexture("../models/star6a.bmp", boxTexture);
	initTexture("../models/bfront.bmp", frontTexture);
	initTexture("../models/bbak.bmp", backTexture);
	initTexture("../models/bleft.bmp", leftTexture);
	initTexture("../models/bright.bmp", rightTexture);
	initTexture("../models/bbot.bmp", bottomTexture);
	initTexture("../models/skullbw4.bmp", deathTexture);


	reset();




	//Start main loop
	glutMainLoop();

	//Clean-Up
	glDeleteProgram(shaderProgramID);
    


    //Enter main loop
    glutMainLoop();

    //Delete shader program
	glDeleteProgram(shaderProgramID);

    return 0;
}

//! Function to Initlise OpenGL
bool initGL(int argc, char** argv)
{
	//Init GLUT
    glutInit(&argc, argv);
    
	//Set Display Mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	//Set Window Size
    glutInitWindowSize(screenWidth, screenHeight);
    
    // Window Position
    glutInitWindowPosition(200, 200);

	//Create Window
    glutCreateWindow("Tank Assignment");
    
    // Init GLEW
	if (glewInit() != GLEW_OK) 
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}
	
	//Set Display function
    glutDisplayFunc(display);
	
	//Set Keyboard Interaction Functions
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp); 

	//Set Mouse Interaction Functions
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	glutMotionFunc(motion);

	// depth testing for OpenGL
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

    //Start start timer function after 100 milliseconds
    glutTimerFunc(100,Timer, 0);

	return true;
}

//Init Shader
void initShader()
{
	//Create shader
	shaderProgramID = Shader::LoadFromFile("shader.vert","shader.frag");

	// Get a handle for our vertex position buffer
	vertexPositionAttribute = glGetAttribLocation(shaderProgramID, "aVertexPosition");
	vertexNormalAttribute = glGetAttribLocation(shaderProgramID,    "aVertexNormal");
	vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexcoord");

	//!
	MVMatrixUniformLocation = glGetUniformLocation(shaderProgramID,     "MVMatrix_uniform");
	ProjectionUniformLocation = glGetUniformLocation(shaderProgramID,   "ProjMatrix_uniform");

	// !!!!!!
	LightPositionUniformLocation    = glGetUniformLocation(shaderProgramID, "LightPosition_uniform");
	AmbientUniformLocation          = glGetUniformLocation(shaderProgramID, "Ambient_uniform");
	SpecularUniformLocation         = glGetUniformLocation(shaderProgramID, "Specular_uniform");
	SpecularPowerUniformLocation    = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");

	//initialise global variables for texture
	vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID,"aVertexTexcoord");
	TextureMapUniformLocation = glGetUniformLocation(shaderProgramID,"TextureMap_uniform");

}

void initTexture(std::string filename, GLuint & textureID)
{
	glGenTextures(1, &textureID);				//generate an openGL texture
	glBindTexture(GL_TEXTURE_2D, textureID);	//make it the current openGL texture

	//set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);


	//Get texture Data
	int width, height;
	char * data;
	//load texture data from file
	Texture::LoadBMP(filename, width, height, data);
	//copy to gpu
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//Cleanup data as copied to GPU
	delete[] data;
}



//! Display Loop
void display(void)
{

	//move camera with hummer
	if (!fall)
		cameraManip.setFocus(Vector3f(transHumx, transHumz+5, transHumy));

	//Handle keys
    handleKeys();

	//Set Viewport
	glViewport(0,0,screenWidth, screenHeight);
	
	// Clear the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //!Draw your scene
	//Use shader
	glUseProgram(shaderProgramID);


	//Projection Matrix - Perspective Projection
	ProjectionMatrix.perspective(40, 1.0, 0.0005, 100.0);




	coinsLeft=0;
	radHum = (rotateHumvee * M_PI / 180);		//convert bearing to rads


	coinHeight=(3.6 + 3*cpuScale*cos(t_global/0.5));
	for (int x(0); x<8;x++)
	{
		for (int y(0); y<10;y++)
		{
			if (map[x][y] == 1)
			{
				xtran=(x * 4);
				ytran=(y * 4);
				drawCrate(xtran, ytran);
			}
			else if (map[x][y] == 2)
			{
				coinsLeft++;
				xtran=(x * 4);
				ytran=(y * 4);
				drawCrate(xtran, ytran);
				drawCoin(xtran, ytran);
				//bullet collision check
				if ( (round(bulletPosx) == xtran+2)  && ( round(bulletPosy) == ytran+2 ) && ( (bulletPosz > (coinHeight-0.8)) && (bulletPosz < (coinHeight+0.8)) ))
				{
					map[x][y] = 1;
					fired = 0;
					coinsCollected ++;

				}

			}

		}

	}

	drawTank();
	drawwheels();
	drawTurret();
	drawBullet();

	//friction
	if (lVelo > 0.01 && lVelo <= 1)
		lVelo -= 0.01;
	else if (lVelo < -0.01 && lVelo >= -1)
		lVelo += 0.01;
	else
		lVelo = 0;


	transHumx += (lVelo * cpuScale * sin(radHum));
	transHumy += (lVelo *cpuScale * cos(radHum));
	rotatewheel += lVelo * 1000 * cpuScale;





	//Skybox
	float d (79.5);
	drawBox(frontTexture, d,0,0,0);
	//no roof
	drawBox(rightTexture, 0,0,d,0);
	drawBox(backTexture,  -d,0,0,0);
	drawBox(bottomTexture,0,-d,0,90);
	drawBox(leftTexture,  0,0,-d,0);


	glUniform3f(LightPositionUniformLocation, lightPosition.x,lightPosition.y,lightPosition.z);
	glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, vertexNormalAttribute);
	glUniform4f(SpecularUniformLocation, specular.x, specular.y, specular.z, vertexNormalAttribute);
	glUniform1f(SpecularPowerUniformLocation, specularPower);



	//Unuse Shader
	glUseProgram(0);
    
    //text


	char time[100];
	char coins[100];
	char map[100];


	sprintf(map, "Level %i", level);
	render2dText(map, 0.9, 0.9, 0.9, -.99, 0.66);

	if ((countdown !=0) && (!fall))
		countdown = int(20 + coinsCollected - t_global);

	sprintf(time, "Time = %i", countdown);
	render2dText(time, 0.9, 0.9, 0.9, -.87, 0.66);

	sprintf(coins, "Coins Collected = %i Coins Left = %i", coinsCollected,coinsLeft);
	render2dText(coins, 1.0, 1.0, 1.0, -0.76, 0.66);

	if (coinsLeft == 0)
	{
		skipLevel();
	}
	if (levelTxt == 1)
	{
		render2dText("LEVEL UP!",1,0,0,-0.05,0);
		if (t_global > 2)
			levelTxt = 0;
	}

	if (fall)
	{
		render2dText("Death Becomes You",1,0,0,-0.1,0);
		render2dText("Press <ENTER> to restart",1,0,0,-0.12,-0.1);
		lVelo = 0;
	}

	if (countdown == 0)
	{
		render2dText("TIME'S UP!!",1,0,0,-0.05,0);
		render2dText("Press <ENTER> to restart",1,0,0,-0.1,-0.1);
	}




	//Swap Buffers and post redisplay
	glutSwapBuffers();
	glutPostRedisplay();
}



//! Keyboard Interaction
void keyboard(unsigned char key, int x, int y)
{
	//Quits program when esc is pressed
	if (key == 27)	//esc key code
	{
		exit(0);
	}

	if (key == 13)	//enter key code
	{
		reset();
	}

	if (key == 'l')	//l key code
	{
		skipLevel();
	}
    
    //Set key status
    keyStates[key] = true;
}

//! Handle key up situation
void keyUp(unsigned char key, int x, int y)
{
    keyStates[key] = false;
}


//! Handle Keys
void handleKeys()
{
    //keys should be handled here

    if(!fall && countdown != 0)
	{

		if (keyStates['w'])
		{
			lAccel();
			/*transHumx += (lVelo * cpuScale * sin(radHum));
			transHumy += (lVelo *cpuScale * cos(radHum));
			rotatewheel += lVelo * 1000 * cpuScale;*/
		}

		if (keyStates['s'])
		{
			lDecel();
			/*transHumx += (lVelo * cpuScale * sin(radHum));
			transHumy += (lVelo * cpuScale * cos(radHum));
			rotatewheel -= lVelo * 1000 * cpuScale;*/
		}

		if (keyStates['a'])
		{
			rotateHumvee += (rVelo * 54 * cpuScale);
		}

		if (keyStates['d'])
		{
			rotateHumvee -= (rVelo * 54 * cpuScale);
		}

		/*if (keyStates['w'] || keyStates['s'])


		if (! (keyStates['w'] || keyStates['s'] ) )
		{
			lDecel();
		}*/

		if  (keyStates['a'] || keyStates['d'])
			rAccel();

		if (! (keyStates['a'] || keyStates['d']) )
			rVelo = 0;


		std::cout << fall << std::endl;



	}
}

//! Mouse Interaction
void mouse(int button, int state, int x, int y)
{

	cameraManip.handleMouse(button, state,x,y);
	glutPostRedisplay();

	//Left Button Action
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

	{
		if (!fired)		//prevents fired reset
		{
			fired = true;

			//fine tuning the bullet to appear at from the gun barrel
			bulletAngle = (tiltTurret+0.5) * M_PI / 180;		//convert turret bearing to rads;
			bulletDir = rotateTurret * M_PI / 180;				//convert turret bearing to rads
			bulletPosx = (transHumx-0.02) +sin(bulletDir);
			bulletPosz = (transHumz + 0.5);
			bulletPosy = (transHumy+0.07)+cos(bulletDir);
			launchTime = t_global;

		}



	}

}

//! Motion
void motion(int x, int y)
{

	cameraManip.handleMouseMotion(x,y);

	glutPostRedisplay();
}

//! Timer Function
void Timer(int value)
{
	//increment time normalised?
	t_global += 0.01;   //0.1 for cgilab machines?
    //Call function again after 10 milli seconds
	glutTimerFunc(10,Timer, 0);
}


//function to draw the maze

void drawCrate(float x, float y)
{

	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, crateTexture);
	glUniform1i(TextureMapUniformLocation, 0);



	//Set Projection Matrix
	glUniformMatrix4fv(
			ProjectionUniformLocation,  //Uniform location
			1,							//Number of Uniforms
			false,						//Transpose Matrix
			ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


	//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	ModelViewMatrix.translate(x+2, 0, y+2);
	ModelViewMatrix.scale(2,2,2);



	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	crateMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);
}

void drawCoin(float x, float y)
{

	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, coinTexture);
	glUniform1i(TextureMapUniformLocation, 0);



	//Set Projection Matrix
	glUniformMatrix4fv(
			ProjectionUniformLocation,  //Uniform location
			1,							//Number of Uniforms
			false,						//Transpose Matrix
			ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


	//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	ModelViewMatrix.translate(x+2, coinHeight, y+2);
	ModelViewMatrix.scale(.6,.6, .6);
	ModelViewMatrix.rotate(800*cpuScale*t_global,0,1,0);


	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	coinMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);
}


void drawTank ()
{


	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, chassisTexture);
	glUniform1i(TextureMapUniformLocation, 0);



	//Set Projection Matrix
	glUniformMatrix4fv(
			ProjectionUniformLocation,  //Uniform location
			1,							//Number of Uniforms
			false,						//Transpose Matrix
			ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


	//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	ModelViewMatrix.translate(transHumx, transHumz, transHumy);
	ModelViewMatrix.scale(scaleHumvee,scaleHumvee,scaleHumvee);

	ModelViewMatrix.translate(-0.1, 0, 0.35);


	ModelViewMatrix.rotate(rotateHumvee,0,1,0);

	ModelViewMatrix.translate(0.1, 0, -0.35);



	if ( rotateHumvee > 360) 	//restricts compass bearing value 0-360
	{
		rotateHumvee = 0;
	}

	if ( rotateHumvee < 0)
	{
		rotateHumvee = 360;
	}


	int fallx(floor (transHumx/4)), fally (floor(transHumy/4)); //maths to normalise location to map

	if ( ( (transHumx < 0) || (transHumy < 0) ) || ( map[fallx][fally] !=1 && map[fallx][fally] !=2) || transHumz < 0.5)// drop tank if map value is zero
	{
		vVelo = -0.01;
		vDecel();
		transHumz += vVelo * 5;
		if (transHumz < -7)
		{
			fall = true;
			cameraManip.setFocus(Vector3f(transHumx , transHumz +5 + cameraz, transHumy));
				cameraz +=0.30;
		}
		if (transHumz < -18 && cameraz < 80)
		{
			fall = true;
			cameraManip.setFocus(Vector3f(transHumx , transHumz +5 + cameraz, transHumy));
			cameraz +=0.31;
		}



		if (lVelo >0)
			fspinState = true;
		if (lVelo < 0)
			bspinState = true;

		if (fspinState) // spin forwards (do back later)
		{
			spin++;
		}


		if (bspinState) // spin forwards (do back later)
		{
			spin--;
		}

		ModelViewMatrix.translate(0, 1, 0);
		ModelViewMatrix.rotate((3 * spin), 1, 0, 0);
		ModelViewMatrix.translate(0, -1, 0);





	}






	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	chassisMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);
}

void drawwheels()
{
	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, chassisTexture);
	glUniform1i(TextureMapUniformLocation, 0);



	//Set Projection Matrix
	glUniformMatrix4fv(
			ProjectionUniformLocation,  //Uniform location
			1,							//Number of Uniforms
			false,						//Transpose Matrix
			ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


	Matrix4x4 front_wheel = cameraManip.apply(ModelViewMatrix);
	front_wheel.translate(0, 0.97, 2.1);
	front_wheel.rotate((rotatewheel/3), 1, 0.f, 0.f);
	front_wheel.translate(0, -0.97, -2.1);


	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, front_wheel.getPtr());
	front_wheelMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);


	Matrix4x4 back_wheel = cameraManip.apply(ModelViewMatrix);
	back_wheel.translate(0, 1.05, -1.28);
	back_wheel.rotate((rotatewheel/3), 1, 0.f, 0.f);
	back_wheel.translate(0, -1.05, 1.28);

	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, back_wheel.getPtr());
	back_wheelMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);

}

void drawTurret ()
{

//Set Colour after program is in use
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, chassisTexture);
glUniform1i(TextureMapUniformLocation, 0);



//Set Projection Matrix
glUniformMatrix4fv(
ProjectionUniformLocation,  //Uniform location
1,							//Number of Uniforms
false,						//Transpose Matrix
ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	ModelViewMatrix.translate(transHumx, transHumz, transHumy);
	ModelViewMatrix.scale(scaleHumvee,scaleHumvee,scaleHumvee);

	rotateTurret = cameraManip.getPan() * 180 / M_PI;
	tiltTurret = cameraManip.getTilt() * -180 / M_PI;

	ModelViewMatrix.translate(-0.1, 0, 0.35);
	ModelViewMatrix.rotate(rotateTurret,0,1,0);
	ModelViewMatrix.translate(0.1, 0, -0.35);




	if (bspinState || fspinState) // spin forwards (do back later)
	{

		ModelViewMatrix.rotate((3 * spin), 1, 1, 1);
	}


Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
glUniformMatrix4fv(
MVMatrixUniformLocation,    //Uniform location
1,                            //Number of Uniforms
false,                        //Transpose Matrix
m.getPtr());                //Pointer to Matrix Values



	//turret Call Draw Geometry Function




turretMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);
}

void drawBullet()
{

	if (fired)
	{
		//Set Colour after program is in use
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bulletTexture);
		glUniform1i(TextureMapUniformLocation, 0);

		//Set Projection Matrix
		glUniformMatrix4fv(
				ProjectionUniformLocation,  //Uniform location
				1,                            //Number of Uniforms
				false,                        //Transpose Matrix
				ProjectionMatrix.getPtr());    //Pointer to ModelViewMatrixValues

		//Apply Camera Manipluator to Set Model View Matrix on GPU
		ModelViewMatrix.toIdentity();
		ModelViewMatrix.translate(bulletPosx,bulletPosz, bulletPosy );
		ModelViewMatrix.scale(0.05, 0.05, 0.05);


		Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
		glUniformMatrix4fv(
				MVMatrixUniformLocation,    //Uniform location
				1,                            //Number of Uniforms
				false,                        //Transpose Matrix
				m.getPtr());                //Pointer to Matrix Values



		//Call Draw Geometry Function
		bulletMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);

		//move the bullet in the direction fired
		bulletPosx +=sin(bulletDir)*.4;
		bulletPosz +=(cpuScale*cos(bulletAngle)) -(1*((t_global-launchTime)*(t_global-launchTime)));
		bulletPosy +=cos(bulletDir)*.4;


		int collidex(floor ((bulletPosx)/4)), collidey (floor((bulletPosy)/4)); //maths to normalise location to map
		// delete bullet if way below map, or collides with box
		if ( bulletPosz < -7 || (bulletPosz < 1 && map[collidex][collidey] == 1) || (bulletPosz < 1 && map[collidex][collidey] == 2))
		{
			fired = false;

		}





	}

}


void drawBox(GLuint str, float x, float z, float y, float r)
{

	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, str);
	if (fall)
		glBindTexture(GL_TEXTURE_2D, deathTexture);

	glUniform1i(TextureMapUniformLocation, 0);



	//Set Projection Matrix
	glUniformMatrix4fv(
			ProjectionUniformLocation,  //Uniform location
			1,							//Number of Uniforms
			false,						//Transpose Matrix
			ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


	//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	if (!fall)
		ModelViewMatrix.translate(transHumx+x,transHumz+z, transHumy+y);
	else
		ModelViewMatrix.translate(transHumx+x,z, transHumy+y);

	ModelViewMatrix.scale(40, 40, 40);
	ModelViewMatrix.rotate(r,0,1,0);




	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values




	//Call Draw Geometry Function
	crateMesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);


}

void render2dText(std::string text, float r, float g, float b, float x, float y)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(r,g,b);
	glRasterPos2f(x, y); // window coordinates
	for(unsigned int i = 0; i < text.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void reset(){
	transHumx = 2;
	transHumy = 2;
	transHumz = 2;
	coinsCollected = 0;
	countdown = int(20 + coinsCollected - t_global);
	t_global = bulletPosx = bulletPosz = bulletPosy = lVelo = rVelo = cameraz = 0;
	if (level == 1)
	{
		Load1();
	}
	else if (level == 2)
	{
		Load2();
	}
	else if (level == 3)
	{
		Load3();
	}

	else if (level == 4)
	{
		Load4();
	}


	fall = fspinState = bspinState = false;
}


void skipLevel()
{
	if (level == 1)
	{
		level = 2;
		Load2();
	}
	else if (level == 2)
	{
		level = 3;
		Load3();
	}
	else if (level == 3)
	{
		level = 4;
		Load4();
	}

	else if (level == 4)
	{
		level = 1;
		Load1();
	}

	reset();
	levelTxt = 1;

}

void Load1() //top left must be a '1' for start position
{
	int map1[8][10] = {
			{1,0,0,0,0,0,0,1,1,1},    	//	   z⦿ --------> y
			{2,2,2,2,0,0,0,1,0,0},		//		|
			{1,0,0,1,0,0,0,2,0,0},		//		|
			{1,0,0,1,0,0,0,1,0,1},		//		|
			{1,2,1,1,0,0,0,1,0,1},		//	    |
			{0,0,0,1,1,1,1,2,0,1},		//	   \ /
			{0,0,0,1,0,0,0,0,0,1},		//		v
			{0,0,0,1,1,2,2,2,2,2}		//		x
	};
	std::copy(&map1[0][0], &map1[7][10],&map[0][0]);

}


void Load2() //top left must be a '1' for start position
{
	int map2[8][10] = {
			{1,0,0,0,0,0,0,0,0,0},    	//	   z⦿ --------> y
			{1,0,1,2,1,2,1,2,1,2},		//		|
			{2,0,2,0,0,0,0,0,0,1},		//		|
			{1,0,2,0,1,2,1,2,0,2},		//		|
			{2,0,2,0,0,0,0,1,0,1},		//	    |
			{1,0,1,2,1,2,1,2,0,2},		//	   \ /
			{1,0,0,0,0,0,0,0,0,1},		//		v
			{2,1,2,1,2,1,2,1,2,1}		//		x
	};
	std::copy(&map2[0][0], &map2[7][10],&map[0][0]);

}

void Load3() //top left must be a '1' for start position
{
	int map3[8][10] = {
			{1,1,0,1,2,1,0,1,2,1},    	//	   z⦿ --------> y
			{0,2,1,2,0,2,1,2,0,2},		//		|
			{0,0,0,0,0,0,0,0,2,1},		//		|
			{0,1,2,1,0,2,0,2,1,0},		//		|
			{1,2,0,2,1,2,0,1,0,0},		//	    |
			{2,0,0,0,0,0,0,2,1,2},		//	   \ /
			{1,2,1,0,1,2,1,0,0,1},		//		v
			{0,0,2,1,2,0,1,2,1,2}		//		x
	};
	std::copy(&map3[0][0], &map3[7][10],&map[0][0]);

}

void Load4() //top left must be a '1' for start position
{
	int map4[8][10] = {
			{2,2,2,2,2,2,2,2,2,2},    	//	   z⦿ --------> y
			{2,0,0,0,2,2,0,0,0,2},		//		|
			{2,0,0,0,2,2,0,0,0,2},		//		|
			{2,2,2,2,2,2,2,2,2,2},		//		|
			{2,2,2,2,2,2,2,2,2,2},		//	    |
			{2,0,0,0,2,2,0,0,0,2},		//	   \ /
			{2,0,0,0,2,2,0,0,0,2},		//		v
			{2,2,2,2,2,2,2,2,2,2},		//		x
	};
	std::copy(&map4[0][0], &map4[7][10],&map[0][0]);

}

void lAccel() //linear acceleration
{
	lVelo += 0.04;
	if (lVelo >=1)
		lVelo=1;

}

void lDecel() // linear decel
{
	lVelo -= 0.04;
	if (lVelo <= -1)
		lVelo = -1;
}

void rAccel() // rotational acceleration
{
	rVelo += 0.03;
	if (rVelo >1)
		rVelo=1;

}

void vDecel() //vertical decelleration
{
	vVelo -= 0.05;
	if (lVelo <= -2)
		lVelo = -2;
}