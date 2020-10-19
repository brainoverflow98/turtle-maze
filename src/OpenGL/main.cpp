
#define _USE_MATH_DEFINES 
#include <cmath>  
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

#include <glew.h>
#include <freeglut.h>
#include <vec4.hpp>

#include <shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <model.h>

#include <filesystem.h>

#include <irrklang/irrKlang.h>
using namespace irrklang;

#include <turtle.h>
#include <ball.h>
#include <cabbage.h>
#include <maze.h>
#include <grass.h>

#include <camera.h>

#include <glui.h>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


bool GRAY_SHADER = false;

unsigned int cubemapTexture;
unsigned int skyboxVAO, skyboxVBO;
void SetupSkybox();
bool FREE_MODE = false;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void SetupTextRenderer(Shader program);

//----->VARIABLE DECLERATIONS<-----

glm::mat4 cabbage0_rot = glm::mat4(1.0f);
glm::mat4 turtle1_rot = glm::mat4(1.0f);
glm::mat4 turtle2_rot = glm::mat4(1.0f);
glm::mat4 light_rot = glm::mat4(1.0f);

float array_cabbage0_rot[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
float array_turtle1_rot[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
float array_turtle2_rot[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
float array_light_rot[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };


glm::vec3 turtle1_loc = { -0.3f, 0.011f, -21.934391f };
glm::vec3 turtle2_loc = { -0.6f, 0.0975f, -21.934391f };
glm::vec3 cabbage0_loc = { 0.1f, 0.011f, -20.634391f };
glm::vec3 light_loc = { 3.5f, 7.0f, 0.0f };

float obj_turtle1_pos[3] = { -0.3f, 0.011f, -21.934391f };
float obj_turtle2_pos[3] = { -0.6f, 0.0975f, -21.934391f };
float obj_cabbage0_pos[3] = { 0.1f, 0.011f, -20.634391f };
float obj_light_pos[3] = { 3.5f, 7.0f, 0.0f };

bool gluiActive = false;
bool show = false;
int oneTwoControl = 0;
float light_intensity = 1.0;
float light_diffuse = 1.4;

#define HIDE_ID              303
#define LIGHT_INTENSITY_ID  250

void control_cb(int control);
void GLUIWindow();


static int mainWindow;
GLUI *gluiWindow;


void toGlm() {
	int i;
	for (i = 0; i < 16; i++) {
		turtle1_rot[i / 4][i % 4] = array_turtle1_rot[i];
		turtle2_rot[i / 4][i % 4] = array_turtle2_rot[i];
		cabbage0_rot[i / 4][i % 4] = array_cabbage0_rot[i];
		light_rot[i / 4][i % 4] = array_light_rot[i];
	}
	for (i = 0; i < 3; i++) {
		turtle1_loc[i] = obj_turtle1_pos[i];
		turtle2_loc[i] = obj_turtle2_pos[i];
		cabbage0_loc[i] = obj_cabbage0_pos[i];
		light_loc[i] = obj_light_pos[i];
	}
}


void control_cb(int control) {
	if (control == HIDE_ID) {
		gluiWindow->hide();
		show = false;
		glutSetCursor(GLUT_CURSOR_NONE);
	}

	else if (control == LIGHT_INTENSITY_ID) {
		light_diffuse = 1.4 * light_intensity; //               ýþýk gücü burada ayarlanýyor **********************************
	}
}

void GLUIWindow() {
	gluiWindow = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_LEFT);
	gluiWindow->set_main_gfx_window(mainWindow);

	GLUI_Panel *panel = gluiWindow->add_panel("", false);
	GLUI_Rotation *view_rot = new GLUI_Rotation(panel, "Turtle-1", array_turtle1_rot);
	view_rot->set_spin(0.1);
	new GLUI_Column(panel, false);
	GLUI_Translation *trans_xy = new GLUI_Translation(panel, "Turtle-1", GLUI_TRANSLATION_XY, obj_turtle1_pos);
	trans_xy->set_speed(.005);
	new GLUI_Column(panel, false);
	GLUI_Translation *trans_z = new GLUI_Translation(panel, "Turtle-1", GLUI_TRANSLATION_Z, &obj_turtle1_pos[2]);
	trans_z->set_speed(.005);

	GLUI_Panel *panel2 = gluiWindow->add_panel("", false);
	GLUI_Rotation *view_rot2 = new GLUI_Rotation(panel2, "Turtle-2", array_turtle2_rot);
	view_rot->set_spin(0.1);
	new GLUI_Column(panel2, false);
	GLUI_Translation *trans_xy2 = new GLUI_Translation(panel2, "Turtle-2", GLUI_TRANSLATION_XY, obj_turtle2_pos);
	trans_xy2->set_speed(0.005);
	new GLUI_Column(panel2, false);
	GLUI_Translation *trans_z2 = new GLUI_Translation(panel2, "Turtle-2", GLUI_TRANSLATION_Z, &obj_turtle2_pos[2]);
	trans_z2->set_speed(0.005);


	GLUI_Panel *panel3 = gluiWindow->add_panel("", false);
	GLUI_Rotation *view_rot3 = new GLUI_Rotation(panel3, "Cabbage", array_cabbage0_rot);
	view_rot3->set_spin(0.1);
	new GLUI_Column(panel3, false);
	GLUI_Translation *trans_xyC = new GLUI_Translation(panel3, "Cabbage", GLUI_TRANSLATION_XY, obj_cabbage0_pos);
	trans_xyC->set_speed(0.005);
	new GLUI_Column(panel3, false);
	GLUI_Translation *trans_zC = new GLUI_Translation(panel3, "Cabbage", GLUI_TRANSLATION_Z, &obj_cabbage0_pos[2]);
	trans_zC->set_speed(0.005);


	GLUI_Panel *panel4 = gluiWindow->add_panel("", false);
	/*GLUI_Rotation *view_rot4 = new GLUI_Rotation(panel4, "Light", array_light_rot);
	view_rot3->set_spin(1.0);*/
	new GLUI_Column(panel4, false);
	GLUI_Translation *trans_xyL = new GLUI_Translation(panel4, "Light", GLUI_TRANSLATION_XY, obj_light_pos);
	trans_xyL->set_speed(5);
	new GLUI_Column(panel4, false);
	GLUI_Translation *trans_zL = new GLUI_Translation(panel4, "Light", GLUI_TRANSLATION_Z, &obj_light_pos[2]);
	trans_zL->set_speed(5);


	//    *******   ýþýk gücü deðiþtirme düzelmezse sadece þu aþaðýyý silersen baþka biþeye gerek kalmýyor.*******************************

	GLUI_Panel *panel5 = gluiWindow->add_panel("", true);
	GLUI_Scrollbar *sb = new GLUI_Scrollbar(panel5, "Intensity", GLUI_SCROLL_HORIZONTAL, &light_intensity, LIGHT_INTENSITY_ID, control_cb);
	GLUI_StaticText  *text = new GLUI_StaticText(panel5, "     Light Intensity");
	sb->set_float_limits(0, 1);

	/*GLUI_Spinner *light_spinner = new GLUI_Spinner(panel5, "Intensity:", &light_intensity, LIGHT_INTENSITY_ID, control_cb);
	light_spinner->set_speed(0.5);
	light_spinner->set_float_limits(0.0, 1.0);*/

	gluiWindow->add_separator();


	new GLUI_Button(gluiWindow, "Hide", HIDE_ID, control_cb);
	new GLUI_Button(gluiWindow, "Quit", 0, (GLUI_Update_CB)exit);

	gluiWindow->hide();
}


//GLUI END

//Models
Turtle turtle1(glm::vec3(-0.3f, 0.011f, -21.934391f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.0275f, 0.0275f, 0.0275f));
Turtle turtle2(glm::vec3(-0.6f, 0.0975f, -21.934391f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.025f, 0.025f, 0.025f));
Ball ball1(turtle1, glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
Ball ball2(turtle2, glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
Grass grass(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
Maze maze(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.4f, 1.2f, 0.4f));

Cabbage cabbages[20];

//Shader programs
Shader program;
Shader text;
Shader skyboxShader;
Shader gray;

//Sound Engine
ISoundEngine *SoundEngine = createIrrKlangDevice();

//Window
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 600;
bool TWO_PLAYER_MODE = false;

//Camera
Camera camera1(turtle1, glm::vec3(0.0f, 0.4f, -0.5f), glm::vec2(-15.0f, 0.0f));
Camera camera2(turtle2, glm::vec3(0.0f, 0.4f, -0.5f), glm::vec2(-15.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Movement
int oldTimeSinceStart = 0;
int deltaTime = 0;

// lighting
//glm::vec3 lightPos(0.0f, 7.0f, 0.0f);

//----->FUNCTION DECLERATIONS<----- 
static void RenderSceneCB();
static void InitializeGlutCallbacks();

void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);

void SpecialKeyDown(int key, int x, int y);
void SpecialKeyUp(int key, int x, int y);
void Idle(void);

void ProcessInput();
static void PassiveMouseCB(int xpos, int ypos);
static void ResizeWindow(int width, int height);

void LoadCabbages()
{
	Cabbage cabbage0(glm::vec3(0.6f, 0.011f, -19.634391f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[0] = cabbage0;

	Cabbage cabbage18(glm::vec3(0.0f, 0.011f, -19.634391f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[18] = cabbage18;

	Cabbage cabbage19(glm::vec3(-0.6f, 0.011f, -19.634391f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[19] = cabbage19;

	Cabbage cabbage1(glm::vec3(7.625021f, 0.02f, -18.674269f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[1] = cabbage1;

	Cabbage cabbage2(glm::vec3(-6.156809f, 0.02f, -15.434536f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[2] = cabbage2;

	Cabbage cabbage3(glm::vec3(-4.316179f, 0.02f, -11.506784f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[3] = cabbage3;

	Cabbage cabbage4(glm::vec3(-2.411890f, 0.02f, -7.402317f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[4] = cabbage4;

	Cabbage cabbage5(glm::vec3(-13.762760f, 0.02f, -4.540580f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[5] = cabbage5;

	Cabbage cabbage6(glm::vec3(-16.317966f, 0.02f, 0.435635f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[6] = cabbage6;

	Cabbage cabbage7(glm::vec3(-14.170121f, 0.02f, 4.477889f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[7] = cabbage7;

	Cabbage cabbage8(glm::vec3(-10.336159f, 0.02f, 14.398214f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[8] = cabbage8;

	Cabbage cabbage9(glm::vec3(-12.286559f, 0.02f, 18.397343f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[9] = cabbage9;

	Cabbage cabbage10(glm::vec3(7.804802f, 0.02f, 19.350838f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[10] = cabbage10;

	Cabbage cabbage11(glm::vec3(8.686171f, 0.02f, 18.516676f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[11] = cabbage11;

	Cabbage cabbage12(glm::vec3(14.664878f, 0.02f, 19.336449f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[12] = cabbage12;

	Cabbage cabbage13(glm::vec3(18.677271f, 0.02f, 16.387352f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[13] = cabbage13;

	Cabbage cabbage14(glm::vec3(12.562119f, 0.02f, 12.407640f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[14] = cabbage14;

	Cabbage cabbage15(glm::vec3(14.633048f, 0.02f, 8.470588f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[15] = cabbage15;

	Cabbage cabbage16(glm::vec3(15.818465f, 0.02f, 3.450904f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[16] = cabbage16;

	Cabbage cabbage17(glm::vec3(13.573684f, 0.02f, 3.427180f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.012f, 0.012f, 0.012f));
	cabbages[17] = cabbage17;

	

	for (int i = 0;i < 20;i++)
	{		
		cabbages[i].model.Init("models/cabbage/cabbage.obj");
		cabbages[i].SoundEngine = SoundEngine;
	}	
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	SCR_WIDTH = glutGet(GLUT_SCREEN_WIDTH);
	SCR_HEIGHT = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutInitWindowPosition(0, 0);
	mainWindow = glutCreateWindow("Turtle Maze");
	glutFullScreen();	
	glEnable(GL_DEPTH_TEST);	

	glutSetCursor(GLUT_CURSOR_INHERIT);


	InitializeGlutCallbacks();

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));
	
	//Load models
	program.Init("vertexShader.vs", "fragmentShader.fs");
	gray.Init("vertexShader.vs", "fragmentShader2.fs");
	turtle1.model.Init("models/turtle1/turtle1.obj");
	ball1.model.Init("models/ball1/ball1.obj");
	turtle2.model.Init("models/turtle2/turtle2.obj");
	ball2.model.Init("models/ball2/ball2.obj");
	grass.model.Init("models/grass/new/grass.obj");	
	maze.model.Init("models/maze/maze.obj");
	
	LoadCabbages();

	text.Init("text.vs", "text.fs");
	SetupTextRenderer(text);
	
	turtle1.SoundEngine = SoundEngine;
	turtle2.SoundEngine = SoundEngine;	

	irrklang::ISound* music =  SoundEngine->play2D("sound/music.wav", GL_TRUE);
	
	GLUIWindow();	

	SetupSkybox();
	skyboxShader.Init("skybox.vs", "skybox.fs");

	glutMainLoop();
	
	return 0;
}


bool MODE_SELECTED = false;
static void RenderSceneCB()
{
	//Delta time for movement precission
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	// Process Keyboard and Mouse Input
	ProcessInput();

	glClearColor(185.0f/255.0f, 251.0f/255.0f, 253.0f/255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glm::mat4 view_camera1 = camera1.GetViewMatrix(), view_camera2 = camera2.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	


	if (!GRAY_SHADER)
	{
		program.use();
	}
	else
	{
		gray.use();
	}
	
	program.setVec3("light.position", light_loc);
	program.setVec3("viewPos", camera1.Position);

	// light properties
	program.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
	program.setVec3("light.diffuse", light_diffuse, light_diffuse, light_diffuse);
	program.setVec3("light.specular", 0.4f, 0.4f, 0.4f);

	//VP matricies		
	
	

	if (TWO_PLAYER_MODE)
	{
		glViewport(SCR_WIDTH*0.5, 0, SCR_WIDTH*0.5, SCR_HEIGHT);
	}
	else
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	}	

	if (show) {
		toGlm();

		turtle1.Position = turtle1_loc;
		turtle2.Position = turtle2_loc;
		cabbages[0].Position = cabbage0_loc;

		turtle1.rot = turtle1_rot;
		turtle2.rot = turtle2_rot;
		cabbages[0].rot = cabbage0_rot;
	}

	// render Turtle1 and Ball1		
	turtle1.Draw(program, view_camera1, projection);
	ball1.Draw(program, view_camera1, projection);

	// render Turtle2 and Ball2		
	turtle2.Draw(program, view_camera1, projection);
	ball2.Draw(program, view_camera1, projection);

	// render Grass		
	grass.Draw(program, view_camera1, projection);

	// render Maze		
	maze.Draw(program, view_camera1, projection);
	
	// render Cabbage
	for (int i = 0;i < 20;i++)
	{
		cabbages[i].CheckCollision(turtle1);			
		cabbages[i].Draw(program, view_camera1, projection);
	}

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	glm::mat4 view = glm::mat4(glm::mat3(camera1.GetViewMatrix())); // remove translation from the view matrix
	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default

	// Render text
	std::stringstream s1;
	s1 << "Points: " << turtle1.points;
	std::string points = s1.str();	
	RenderText(text, points, (SCR_WIDTH / 2.0f) - 170.0f, SCR_HEIGHT - 40.0f, 0.7f, glm::vec3(0.5, 0.8f, 0.2f));	
	std::stringstream s2;
	s2 << "Time: " << timeSinceStart/1000;
	std::string second = s2.str();
	RenderText(text, second, (SCR_WIDTH / 2.0f) + 70.0f, SCR_HEIGHT - 40.0f, 0.7f, glm::vec3(0.5, 0.8f, 0.2f));
	

	if (TWO_PLAYER_MODE)
	{
		if (!GRAY_SHADER)
		{
			program.use();
		}
		else
		{
			gray.use();
		}
		program.setVec3("viewPos", camera2.Position);
		glViewport(0, 0, SCR_WIDTH*0.5, SCR_HEIGHT);

		// render Turtle1 and Ball1
		turtle1.Draw(program, view_camera2, projection);
		ball1.Draw(program, view_camera2, projection);

		// render Turtle2 and Ball2		
		turtle2.Draw(program, view_camera2, projection);
		ball2.Draw(program, view_camera2, projection);

		// render Grass	
		grass.Draw(program, view_camera2, projection);

		// render Maze	
		maze.Draw(program, view_camera2, projection);

		// render Cabbage
		for (int i = 0;i < 20;i++)
		{
			cabbages[i].CheckCollision(turtle2);			
			cabbages[i].Draw(program, view_camera2, projection);
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		skyboxShader.setInt("skybox", 0);
		glm::mat4 view = glm::mat4(glm::mat3(camera1.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// Render text
		std::stringstream s1;
		s1 << "Points: " << turtle2.points;
		std::string points = s1.str();
		RenderText(text, points, (SCR_WIDTH / 2.0f) - 170.0f, SCR_HEIGHT - 40.0f, 0.7f, glm::vec3(0.5, 0.8f, 0.2f));
		std::stringstream s2;
		s2 << "Time: " << timeSinceStart / 1000;
		std::string second = s2.str();
		RenderText(text, second, (SCR_WIDTH / 2.0f) + 70.0f, SCR_HEIGHT - 40.0f, 0.7f, glm::vec3(0.5, 0.8f, 0.2f));
	}		


	if (!MODE_SELECTED)
	{
		RenderText(text, "SELECT PLAYER MODE", (SCR_WIDTH / 2.0f) - 270.0f, SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f));
		RenderText(text, "(Press 1 or 2)", (SCR_WIDTH / 2.0f) - 75.0f, SCR_HEIGHT / 2.0f - 50.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
	}

	

	glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
	GLUI_Master.set_glutDisplayFunc(RenderSceneCB);
	GLUI_Master.set_glutIdleFunc(Idle);
	GLUI_Master.set_glutSpecialFunc(SpecialKeyDown);
	glutSpecialUpFunc(SpecialKeyUp);
	GLUI_Master.set_glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutPassiveMotionFunc(PassiveMouseCB);
	GLUI_Master.set_glutReshapeFunc(ResizeWindow);	
}

static void ResizeWindow(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}


static void PassiveMouseCB(int xpos, int ypos)
{
	if (FREE_MODE)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera1.ProcessMouseMovement(xoffset, yoffset);

		if (xpos < 100 || xpos > SCR_WIDTH - 100) {  //you can use values other than 100 for the screen edges if you like, kind of seems to depend on your mouse sensitivity for what ends up working best
			lastX = SCR_WIDTH / 2;   //centers the last known position, this way there isn't an odd jump with your cam as it resets
			lastY = SCR_HEIGHT / 2;
			glutWarpPointer(SCR_WIDTH / 2, SCR_HEIGHT / 2);  //centers the cursor
		}
		else if (ypos < 100 || ypos > SCR_HEIGHT - 100) {
			lastX = SCR_WIDTH / 2;
			lastY = SCR_HEIGHT / 2;
			glutWarpPointer(SCR_WIDTH / 2, SCR_HEIGHT / 2);
		}
	}
}

bool UP_PRESSED1 = false, DOWN_PRESSED1 = false, LEFT_PRESSED1 = false, RIGHT_PRESSED1 = false, PAGE_UP_PRESSED = false, PAGE_DOWN_PRESSED = false;
bool UP_PRESSED2 = false, DOWN_PRESSED2 = false, LEFT_PRESSED2 = false, RIGHT_PRESSED2 = false;
void KeyDown(unsigned char key, int x, int y)
{
	switch (key) {
		
	
	case 'q':
		glutFullScreenToggle();
		break;

	case 't':
		GRAY_SHADER = !GRAY_SHADER;		
		break;

	case 'y':
		if (FREE_MODE)
		{
			FREE_MODE = false;
			camera1.FREE_MODE = false;
		}
		else
		{
			FREE_MODE = true;
			camera1.FREE_MODE = true;
			TWO_PLAYER_MODE = false;
			gluiWindow->hide();
			show = false;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		break;

	case 'u':
		if (!show) {                             
			gluiWindow->show();
			FREE_MODE = false;
			TWO_PLAYER_MODE = false;
			show = true;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		else if (show) {
			gluiWindow->hide();			
			show = false;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		break;

	case '1':
		MODE_SELECTED = true;
		TWO_PLAYER_MODE = false;
		break;

	case '2':
		MODE_SELECTED = true;
		TWO_PLAYER_MODE = true;
		break;

	case 'k':
		camera1.ProcessKeyboard(Camera_Movement::BIRDSEYE, deltaTime);
		break;

	case 'g':
		camera2.ProcessKeyboard(Camera_Movement::BIRDSEYE, deltaTime);
		break;
	
	case 'w':
		UP_PRESSED2 = true;
		break;

	case 's':
		DOWN_PRESSED2 = true;
		break;

	case 'a':
		LEFT_PRESSED2 = true;
		break;

	case 'd':
		RIGHT_PRESSED2 = true;
		break;

	case 167:
		break;

	case 033: // octal equivalent of the Escape key
		glutLeaveMainLoop();
		break;

	case 040: // octal equivalent of the Space key
		printf("Position \n%f, %f, %f \n", turtle1.Position.x, turtle1.Position.y, turtle1.Position.z);
		/*printf("WorldUp \n%f, %f, %f \n", turtle1.WorldUp.x, turtle1.WorldUp.y, turtle1.WorldUp.z);
		printf("Yaw \n%f \n", turtle1.Yaw);
		printf("Pitch \n%f \n", turtle1.Pitch);*/
		break;
	}

}


void KeyUp(unsigned char key, int x, int y)
{
	switch (key) {

	case 'w':
		UP_PRESSED2 = false;
		break;

	case 's':
		DOWN_PRESSED2 = false;
		break;

	case 'a':
		LEFT_PRESSED2 = false;
		break;

	case 'd':
		RIGHT_PRESSED2 = false;
		break;
	}
}


void SpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	
	case GLUT_KEY_UP:		
		UP_PRESSED1 = true;
		break;	

	case GLUT_KEY_DOWN:
		DOWN_PRESSED1 = true;
		break;

	case GLUT_KEY_LEFT:
		LEFT_PRESSED1 = true;
		break;

	case GLUT_KEY_RIGHT:
		RIGHT_PRESSED1 = true;
		break;

	case GLUT_KEY_PAGE_UP:
		PAGE_UP_PRESSED = true;
		break;

	case GLUT_KEY_PAGE_DOWN:
		PAGE_DOWN_PRESSED = true;
		break;
	}


}

void SpecialKeyUp(int key, int x, int y)
{
	switch (key) {

	case GLUT_KEY_UP:
		UP_PRESSED1 = false;
		break;

	case GLUT_KEY_DOWN:
		DOWN_PRESSED1 = false;
		break;

	case GLUT_KEY_LEFT:
		LEFT_PRESSED1 = false;
		break;

	case GLUT_KEY_RIGHT:
		RIGHT_PRESSED1 = false;
		break;

	case GLUT_KEY_PAGE_UP:
		PAGE_UP_PRESSED = false;
		break;

	case GLUT_KEY_PAGE_DOWN:
		PAGE_DOWN_PRESSED = false;
		break;
	}

}

void ProcessInput()
{
	if (UP_PRESSED1)
	{
		if(camera1.birdseye==0 && !FREE_MODE)
		turtle1.ProcessKeyboard(Turtle_Movement::FORWARD, deltaTime);
		camera1.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
		ball1.ProcessKeyboard(Ball_Movement::FORWARD, deltaTime);
	}
	if (DOWN_PRESSED1)
	{
		if (camera1.birdseye == 0 && !FREE_MODE)
		turtle1.ProcessKeyboard(Turtle_Movement::BACKWARD, deltaTime);
		camera1.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
		ball1.ProcessKeyboard(Ball_Movement::BACKWARD, deltaTime);
	}
	if (LEFT_PRESSED1)
	{
		if (camera1.birdseye == 0 && !FREE_MODE)
		turtle1.ProcessKeyboard(Turtle_Movement::LEFT, deltaTime);
		camera1.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	if (RIGHT_PRESSED1)
	{
		if (camera1.birdseye == 0 && !FREE_MODE)
		turtle1.ProcessKeyboard(Turtle_Movement::RIGHT, deltaTime);
		camera1.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}

	if (UP_PRESSED2)
	{
		if (camera2.birdseye == 0)
			turtle2.ProcessKeyboard(Turtle_Movement::FORWARD, deltaTime);
		camera2.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
		ball2.ProcessKeyboard(Ball_Movement::FORWARD, deltaTime);
	}
	if (DOWN_PRESSED2)
	{
		if (camera2.birdseye == 0)
			turtle2.ProcessKeyboard(Turtle_Movement::BACKWARD, deltaTime);
		camera2.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
		ball2.ProcessKeyboard(Ball_Movement::BACKWARD, deltaTime);
	}
	if (LEFT_PRESSED2)
	{
		if (camera2.birdseye == 0)
			turtle2.ProcessKeyboard(Turtle_Movement::LEFT, deltaTime);
		camera2.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	if (RIGHT_PRESSED2)
	{
		if (camera2.birdseye == 0)
			turtle2.ProcessKeyboard(Turtle_Movement::RIGHT, deltaTime);
		camera2.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}

	if (PAGE_UP_PRESSED)
	{
		camera1.ProcessKeyboard(Camera_Movement::PAGE_UP, deltaTime);
	}
	if (PAGE_DOWN_PRESSED)
	{
		camera1.ProcessKeyboard(Camera_Movement::PAGE_DOWN, deltaTime);
	}
}


void Idle(void)
{	
	std::this_thread::sleep_for(std::chrono::milliseconds(7));
	glutPostRedisplay();
}


void SetupTextRenderer(Shader program)
{
	// Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Compile and setup the shader	
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
	program.use();
	glUniformMatrix4fv(glGetUniformLocation(program.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderText(Shader &program, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	program.use();
	glUniform3f(glGetUniformLocation(program.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData		

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void SetupSkybox()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	
	// skybox VAO	
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"models/skybox/right.jpg",
		"models/skybox/left.jpg",
		"models/skybox/top.jpg",
		"models/skybox/bottom.jpg",
		"models/skybox/front.jpg",
		"models/skybox/back.jpg"
	};
	cubemapTexture = loadCubemap(faces);
}
