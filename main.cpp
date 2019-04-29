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

//Global Variables
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		                // Vertex Position Attribute Location
GLuint vertexTexcoordAttribute; 					// Vertex Texcoord Attribute Location
GLuint TextureMapUniformLocation;					// Texture Map Location
GLuint crateTexture;
GLuint coinTexture;
GLuint chassisTexture;


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

//map - can load this from file later !!top left must be a '1' for start position
int map[8][10] = {
		{1,0,0,0,0,0,0,1,1,1},    	//	   z⦿ --------> y
		{2,2,2,2,0,0,0,1,0,0},		//		|
		{1,0,0,1,0,0,0,2,0,0},		//		|
		{1,0,0,1,0,0,0,1,0,1},		//		|
		{1,2,1,1,0,0,0,1,0,1},		//	   \ /
		{0,0,0,1,1,1,1,2,0,1},		//		v
		{0,0,0,1,0,0,0,0,0,1},		//		x
		{0,0,0,1,1,2,2,2,2,2}
};

//edit humvee
float transHumx = 0.0, transHumy = 0.0, transHumz = 1.75;
float scaleHumvee = 0.1;
float rotateHumvee = 0.0;
float radHum;
float rotateTurret = 0.0;
float rotatewheel = 0.0;
float cpuScale = 0.07;
float zoom =0.1;


//! Screen size
int screenWidth   	        = 1280;
int screenHeight   	        = 800;

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
	coinMesh.loadOBJ("../models/ball.obj");
	chassisMesh.loadOBJ("../models/chassis.obj");
	front_wheelMesh.loadOBJ("../models/front_wheel.obj");
	back_wheelMesh.loadOBJ("../models/back_wheel.obj");
	turretMesh.loadOBJ("../models/turret.obj");


	//Init Camera Manipultor
	cameraManip.setPanTiltRadius(rotateTurret,0.f,2.f);
	cameraManip.setFocus(Vector3f(transHumx, transHumz, transHumy));

	//load texture models
	initTexture("../models/Crate.bmp", crateTexture);
	initTexture("../models/fbsz.bmp", coinTexture);
	initTexture("../models/hamvee.bmp", chassisTexture);


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
	glEnable(GL_TEXTURE_2D);
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

	//!
	MVMatrixUniformLocation = glGetUniformLocation(shaderProgramID,     "MVMatrix_uniform");
	ProjectionUniformLocation = glGetUniformLocation(shaderProgramID,   "ProjMatrix_uniform");

	//initialise global variables for texture
	vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID,
												  "aVertexTexcoord");
	TextureMapUniformLocation = glGetUniformLocation(shaderProgramID,
													 "TextureMap_uniform");

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



//! Display Loop     **look at 5.3 for lighting
void display(void)
{




	//move camera with hummer
	cameraManip.setFocus(Vector3f(transHumx, transHumz, transHumy));



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


	for (int x(0); x<8;x++)
	{
		for (int z(0); z<10;z++)
		{
			if (map[x][z] == 1)
			{
				int xtran(x * 2);
				int ztran(z * 2);
				drawCrate(xtran, ztran);
			}
			else if (map[x][z] == 2)
			{
				int xtran(x * 2);
				int ztran(z * 2);
				drawCrate(xtran, ztran);
				drawCoin(xtran, ztran);

			}

		}

	}

	drawTank();
	drawTurret ();

	//Unuse Shader
	glUseProgram(0);
    
    
    
    
    

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

	//translate hummer
	/*if (key == 'w')
	{
	}*/
    
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
	if(keyStates['w'])
    {
		transHumx+=cpuScale*sin(radHum);
		transHumy+=cpuScale*cos(radHum);
		rotatewheel+=1000*cpuScale;
    }

	if(keyStates['s'])
	{
		transHumx-=cpuScale*sin(radHum);
		transHumy-=cpuScale*cos(radHum);
		rotatewheel-=1000*cpuScale;
	}

	if(keyStates['a'])
	{
		rotateHumvee+=50*cpuScale;
	}

	if(keyStates['d'])
	{
		rotateHumvee-=50*cpuScale;
	}
}

//! Mouse Interaction
void mouse(int button, int state, int x, int y)
{

	cameraManip.handleMouse(button, state,x,y);
	glutPostRedisplay();

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

void drawCrate(float x, float z)
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
	ModelViewMatrix.translate(x, 0.8, z);



	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	crateMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);
}

void drawCoin(float x, float z)
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
	ModelViewMatrix.translate(x, (2.8 + 3*cpuScale*cos(t_global/0.5)), z);
	ModelViewMatrix.scale(.3,.3, .3);
	ModelViewMatrix.rotate(800*cpuScale*t_global,0,1,0);


	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	coinMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);
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
	ModelViewMatrix.rotate(rotateHumvee,0,1,0);
	if ( rotateHumvee > 360) 	//restricts compass bearing value 0-360
	{
		rotateHumvee = 0;
	}

	if ( rotateHumvee < 0)
	{
		rotateHumvee = 360;
	}

	int fallx(ceil ((transHumx-1)/2)), fally (ceil((transHumy-1)/2)); //maths no normalise location to map

	if (map[fallx][fally] == 0 ) // drop tank if map value is zero
		transHumz -=1;

	if (transHumz < -30)
	{

		for (int inf(1); inf < 2; inf--)
		{
			std::cout << " END " << std::endl;
		}
	}





	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	chassisMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);


	ModelViewMatrix.rotate(rotatewheel,0,0,1);
	//front_wheel Call Draw Geometry Function
	front_wheelMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);

	//back_wheel Call Draw Geometry Function
	back_wheelMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);


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
	std::cout << rotateTurret << " " << transHumx << " "<< transHumz << " " << transHumy << std::endl;
	ModelViewMatrix.rotate(rotateTurret,0,1,0);


Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
glUniformMatrix4fv(
MVMatrixUniformLocation,    //Uniform location
1,                            //Number of Uniforms
false,                        //Transpose Matrix
m.getPtr());                //Pointer to Matrix Values



	//turret Call Draw Geometry Function


turretMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);
}