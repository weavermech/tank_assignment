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
void drawBox(float x, float y);
void render2dText(std::string text, float r, float g, float b, float x, float y);
void reset();
void skipLevel();
void Load1();
void Load2();
void Load3();
void Load4();


//Global Variables
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		                // Vertex Position Attribute Location


//Material Properties
GLuint LightPositionUniformLocation;                // Light Position Uniform
GLuint AmbientUniformLocation;
GLuint SpecularUniformLocation;
GLuint SpecularPowerUniformLocation;
Vector3f lightPosition= Vector3f(0.0,0.1,0.1);   // Light Position
Vector3f ambient    = Vector3f(0.11,0.11,0.11);
Vector3f specular   = Vector3f(0.1,0.1,0.1);
float specularPower = 5.0;







GLuint vertexNormalAttribute;
GLuint vertexTexcoordAttribute; 					// Vertex Texcoord Attribute Location


GLuint TextureMapUniformLocation;					// Texture Map Location
GLuint crateTexture;
GLuint coinTexture;
GLuint chassisTexture;
GLuint bulletTexture;
GLuint boxTexture;


float t_global = 0;





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
int map[8][10] = {
		{1,0,0,0,0,0,0,1,1,1},    	//	   z⦿ --------> y
		{2,2,2,2,0,0,0,1,0,0},		//		|
		{1,0,0,1,0,0,0,2,0,0},		//		|
		{1,0,0,1,0,0,0,1,0,1},		//		|
		{1,2,1,1,0,0,0,1,0,1},		//	    |
		{0,0,0,1,1,1,1,2,0,1},		//	   \ /
		{0,0,0,1,0,0,0,0,0,1},		//		v
		{0,0,0,1,1,2,2,2,2,2}		//		x
};
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
float zoom =0.1;
bool fall(false);

//coin
float coinHeight(0);
int coinsCollected(0);

//bullet
bool fired = false;
float firedTime;
int count;
float bulletPosx = 0.0, bulletPosz = 0.0, bulletPosy = 0.2;
float bulletAngle;
float bulletDir;
float launchTime;




//all copied score stuff
//Score
int level(1);

int s = 0;
int shoot = 0;
int shoottime = 0;
int endgame = 0;
int countdown = 1;
int temp;
int showinfo = 0;
int totalscore = 0;


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
	initTexture("../models/glass.bmp", boxTexture);



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
	drawBox(5,4);


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

	countdown = int(20 + coinsCollected - t_global);

	sprintf(time, "Time = %i", countdown);
	render2dText(time, 0.9, 0.9, 0.9, -.87, 0.66);

	sprintf(coins, "Coins = %i", coinsCollected);
	render2dText(coins, 1.0, 1.0, 1.0, -0.76, 0.66);

	if (fall)
	{
		render2dText("Death Becomes You",1,0,0,-0.05,0);
		render2dText("Press <ENTER> to restart",1,0,0,-0.07,-0.1);
	}

	if (countdown == 0)
	{
		render2dText("TIME'S UP!!",1,0,0,-0.05,0);
		render2dText("Press <ENTER> to restart",1,0,0,-0.07,-0.1);
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

	if (key == 13)	//esc key code
	{
		reset();
	}

	if (key == 'l')	//enter key code
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

    if(!fall)
	{

		if (keyStates['w'])
		{
			transHumx += cpuScale * sin(radHum);
			transHumy += cpuScale * cos(radHum);
			rotatewheel += 1000 * cpuScale;
		}

		if (keyStates['s'])
		{
			transHumx -= cpuScale * sin(radHum);
			transHumy -= cpuScale * cos(radHum);
			rotatewheel -= 1000 * cpuScale;
		}

		if (keyStates['a'])
		{
			rotateHumvee += 54 * cpuScale;
		}

		if (keyStates['d'])
		{
			rotateHumvee -= 40 * cpuScale;
		}
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

	//chassis

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


	//int fallx(floor (abs((transHumx-1)/4))), fally (floor(abs((transHumy-1)/4))); //maths no normalise location to map
	int fallx(floor (transHumx/4)), fally (floor(transHumy/4)); //maths to normalise location to map

	if ( ( (transHumx < 0) || (transHumy < 0) ) || ( map[fallx][fally] !=1 && map[fallx][fally] !=2) )// drop tank if map value is zero
	{
		transHumz -= 0.5;

		if (transHumz < 0.5)
		{
			if (!fall)
				fall = true;
		}
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

		//coin collision

		//round bullet position to simulate a hit box



	}

}


void drawBox(float x, float y)
{

	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
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
		ModelViewMatrix.translate(transHumx,transHumz, transHumy);

	ModelViewMatrix.scale(40, 40, 40);



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
	fall = false;
	coinsCollected = 0;
	t_global=0;
	countdown = int(20 + coinsCollected - t_global);
	bulletPosx = 0;
	bulletPosz = 0;
	bulletPosy = 0;
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
	std::copy(&map1[0][0], &map1[7][9],&map[0][0]);

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
	std::copy(&map2[0][0], &map2[7][9],&map[0][0]);

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
	std::copy(&map3[0][0], &map3[7][9],&map[0][0]);

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
	std::copy(&map4[0][0], &map4[7][9],&map[0][0]);

}