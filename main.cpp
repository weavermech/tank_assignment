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
void render2dText(std::string text, float r, float g, float b, float x, float y);


//Global Variables
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		                // Vertex Position Attribute Location
GLuint vertexTexcoordAttribute; 					// Vertex Texcoord Attribute Location
GLuint TextureMapUniformLocation;					// Texture Map Location
GLuint crateTexture;
GLuint coinTexture;
GLuint chassisTexture;
GLuint bulletTexture;


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
		{1,2,1,1,0,0,0,1,0,1},		//	   \ /
		{0,0,0,1,1,1,1,2,0,1},		//		v
		{0,0,0,1,0,0,0,0,0,1},		//		x
		{0,0,0,1,1,2,2,2,2,2}
};

//edit humvee
float transHumx = 0.0, transHumy = 0.0, transHumz = 1.66;
float scaleHumvee = 0.3;
float rotateHumvee = 0.0;
float radHum;
float rotateTurret = 0.0;
float tiltTurret = 0.0;
float rotatewheel = 0.0;
float cpuScale = 0.001;
float zoom =0.1;

//bullet
bool fired = false;
float firedTime;
int count;
float bulletOriginx = 0.0, bulletOriginy = 0.0, bulletOriginz = 0.2;
float bulletAngle;
float bulletDir;
float launchTime;




//all copied score stuff
//Score
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
	cameraManip.setPanTiltRadius(rotateTurret,0.f,2.f);
	cameraManip.setFocus(Vector3f(transHumx, transHumz, transHumy));

	//load texture models
	initTexture("../models/Crate.bmp", crateTexture);
	initTexture("../models/coin.bmp", coinTexture);
	initTexture("../models/hamvee.bmp", chassisTexture);
	initTexture("../models/ball.bmp", bulletTexture);



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
	drawwheels();
	drawTurret();
	drawBullet();

	//Unuse Shader
	glUseProgram(0);
    
    //text

    int showinfo = 1; /// to toggle info
    int level = 1;
	char time[100];
	char score[100];
	char coinleft[100];
	char levelnow[100];

	if (showinfo == 1)
	{
		render2dText("Manually Level Select(1-6)", 1.0, 1.0, 1.0, -1.0, -0.70);
		render2dText("Tank Move(WSAD)", 1.0, 1.0, 1.0, -1.0, -0.75);
		render2dText("turret Rotate(JK) Reset(L) Lock(^L)", 1.0, 1.0, 1.0, -1.0, -0.80);
		render2dText("Change View (V)", 1.0, 1.0, 1.0, -1.0, -0.85);
		render2dText("Manual View On(E) Reset(^E)", 1.0, 1.0, 1.0, -1.0, -0.90);
		render2dText("Mesh View On(Q) Reset(^Q)", 1.0, 1.0, 1.0, -1.0, -0.95);
		render2dText("Zoom In(=) Out(-)", 1.0, 1.0, 1.0, -1.0, -1.00);
	}

	sprintf(levelnow, "Level %i", level);
	render2dText(levelnow, 1.0, 1.0, 1.0, -1.0, 0.95);

	countdown = int(40 - t_global);
	sprintf(time, "Time = %i", countdown);
	render2dText(time, 1.0, 1.0, 1.0, -1.0, 0.90);

	sprintf(score, "Total Score = %i", totalscore);
	render2dText(score, 1.0, 1.0, 1.0, -1.0, 0.85);

	sprintf(coinleft, "Coin(s) Left = %i/%i", count, (count + s));
	render2dText(coinleft, 1.0, 1.0, 1.0, -1.0, 0.80);

	render2dText("Info Open(I) Close(O)", 1.0, 1.0, 1.0, -1.0, 0.75);

	if (temp ==1) render2dText("Level Completed", 1.0, 1.0, 1.0, 0.0, 0.75);
	if (temp ==2) render2dText("Game Over", 1.0, 1.0, 1.0, 0.0, 0.70);

	//reset
	//tanky = 0;


	//end text


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

	//Left Button Action
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

	{
		if (!fired)		//prevents fired reset
		{
			fired = true;
			bulletOriginx = transHumx;
			bulletOriginz = transHumz + 0.2;
			bulletOriginy = transHumy;
			bulletAngle = (tiltTurret+0.2) * M_PI / 180;		//convert turret bearing to rads;
			bulletDir = rotateTurret * M_PI / 180;		//convert turret bearing to rads
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
	//std::cout << " |"<< t_global << "| |" << (9.8*((t_global*launchTime)*(t_global*launchTime))) << std::endl;
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

    /*float wheex(cos(rotatewheel));
    float wheey(sin(rotatewheel));
	std::cout << " xx" <<  rotatewheel << "xx " << std::endl;
	Matrix4x4 front_wheel = cameraManip.apply(ModelViewMatrix);
	front_wheel.translate(0,0, -1);
	front_wheel.rotate((rotatewheel/10), 1, 0.f, 0.f);

	//front_wheel.translate(0,-wheex,-wheey);

	Matrix4x4 front_wheelmesh = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, front_wheel.getPtr());
	front_wheelMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);

	//-------------------------------------------------------------------back_wheel
	Matrix4x4 back_wheel = cameraManip.apply(ModelViewMatrix);
	//back_wheel.translate(0, 0, 0);
	//back_wheel.rotate((rotatewheel), 1, 0.f, 0.f);
	//back_wheel.translate(0, 0, 0);

	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, back_wheel.getPtr());
	back_wheelMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);
*/
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


	//float wheex(cos(rotatewheel));
	//float wheey(sin(rotatewheel));
	std::cout << " xx" <<  rotatewheel << "xx " << std::endl;
	Matrix4x4 front_wheel = cameraManip.apply(ModelViewMatrix);
	front_wheel.translate(0, 0.97, 2.1);
	front_wheel.rotate((rotatewheel/10), 1, 0.f, 0.f);
	front_wheel.translate(0, -0.97, -2.1);

	//Matrix4x4 front_wheelmesh = cameraManip.apply(ModelViewMatrix);
	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, front_wheel.getPtr());
	front_wheelMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);


	//-------------------------------------------------------------------back_wheel
	Matrix4x4 back_wheel = cameraManip.apply(ModelViewMatrix);
	back_wheel.translate(0, 1.05, -1.28);
	back_wheel.rotate((rotatewheel/10), 1, 0.f, 0.f);
	back_wheel.translate(0, -1.05, 1.28);

	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, back_wheel.getPtr());
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
	tiltTurret = cameraManip.getTilt() * -180 / M_PI;

	//std::cout << rotateTurret << " #" << tiltTurret << "# " << transHumx << " "<< transHumz << " " << transHumy << std::endl;
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
		ModelViewMatrix.translate(bulletOriginx,bulletOriginz, bulletOriginy );
		ModelViewMatrix.scale(0.03, 0.03, 0.03);


		Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
		glUniformMatrix4fv(
				MVMatrixUniformLocation,    //Uniform location
				1,                            //Number of Uniforms
				false,                        //Transpose Matrix
				m.getPtr());                //Pointer to Matrix Values



		//Call Draw Geometry Function
		bulletMesh.Draw(vertexPositionAttribute, -1, vertexTexcoordAttribute);

		bulletOriginx +=sin(bulletDir)*.4;
		bulletOriginz +=(cpuScale*cos(bulletAngle)) -(1*((t_global-launchTime)*(t_global-launchTime)));
		bulletOriginy +=cos(bulletDir)*.4;

	    //std::cout << bulletDir << "| |" << sin(bulletDir) << "|  bullet  x" << bulletOriginx << "x x" << bulletOriginx  << "x x" << bulletOriginx << "x" << std::endl;
		//std::cout << bulletOriginz << "| |"  << std::endl;
		count++;	//when bullet dies

		int collidex(ceil ((bulletOriginx-1)/2)), collidey (ceil((bulletOriginx-1)/2)); //maths to normalise location to map

		if (sqrt((bulletOriginx*bulletOriginx) + (bulletOriginy*bulletOriginy)) > 180 || bulletOriginz < -7 || (bulletOriginz < 1 && map[collidex][collidex] == 1) || (bulletOriginz < 1 && map[collidex][collidex] == 2)) //cancel bullet if too far from origin, way below map, or collides with box
		{
			fired = false;
			count = 0;
		}


	}

}


void render2dText(std::string text, float r, float g, float b, float x, float y)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(r,g,b);
	glRasterPos2f(x, y); // window coordinates
	for(unsigned int i = 0; i < text.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}