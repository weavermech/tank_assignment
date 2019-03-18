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
void drawMaze(float x, float y);					//draws translated box



//Global Variables
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		                // Vertex Position Attribute Location
GLuint vertexTexcoordAttribute; 					// Vertex Texcoord Attribute Location
GLuint TextureMapUniformLocation;					// Texture Map Location
GLuint texture;

//Viewing
SphericalCameraManipulator cameraManip;
Matrix4x4 ModelViewMatrix;		                    // ModelView Matrix
GLuint MVMatrixUniformLocation;		                // ModelView Matrix Uniform
Matrix4x4 ProjectionMatrix;		                    // Projection Matrix
GLuint ProjectionUniformLocation;	                // Projection Matrix Uniform Location

//Mesh
Mesh mesh;				                            // Mesh

//map - can load this from file later
int map[8][10] = {
		{0,0,0,0,0,0,0,1,1,1},
		{2,2,2,2,0,0,0,1,0,0},
		{1,0,0,1,0,0,0,2,0,0},
		{1,0,0,1,0,0,0,1,0,1},
		{1,2,1,1,0,0,0,1,0,1},
		{0,0,0,1,1,1,1,2,0,1},
		{0,0,0,1,0,0,0,0,0,1},
		{0,0,0,1,1,2,2,2,2,2}
};


//! Screen size
int screenWidth   	        = 720;
int screenHeight   	        = 720;

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
	mesh.loadOBJ("../models/cube.obj");

	//Init Camera Manipultor
	cameraManip.setPanTiltRadius(0.f,0.f,2.f);
	cameraManip.setFocus(mesh.getMeshCentroid());

	//load texture models
	initTexture("../models/Crate.bmp", texture);

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



//! Display Loop
void display(void)
{
    //Handle keys
    handleKeys();

	//Set Viewport
	glViewport(0,0,screenWidth, screenHeight);
	
	// Clear the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //!Draw your scene
	//Use shader
	glUseProgram(shaderProgramID);

	//Set Colour after program is in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(TextureMapUniformLocation, 0);

	//Projection Matrix - Perspective Projection
	ProjectionMatrix.perspective(90, 1.0, 0.0001, 100.0);

	//Set Projection Matrix
	glUniformMatrix4fv(
			ProjectionUniformLocation,  //Uniform location
			1,							//Number of Uniforms
			false,						//Transpose Matrix
			ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues


	//use 5.1 to get tank texture/model

	for (int xtran(0); xtran<20;xtran+=2)
	{
		for (int ztran(0); ztran<20;ztran+=2)
		{
			drawMaze(xtran, ztran);
		}

	}
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
	if(keyStates['a'])
    {

    }

	//Quits program when esc is pressed
	if (keyStates['a'])	//esc key code
	{
		exit(0);
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
    
    //Call function again after 10 milli seconds
	glutTimerFunc(10,Timer, 0);
}


//function to draw the maze

void drawMaze(float x, float z)
{


	//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	ModelViewMatrix.translate(x, 0.0, z);

	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(
			MVMatrixUniformLocation,    //Uniform location
			1,                            //Number of Uniforms
			false,                        //Transpose Matrix
			m.getPtr());                //Pointer to Matrix Values



	//Call Draw Geometry Function
	mesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);
}

