#include "stdafx.h"
#include "gl_core_4_3.hpp"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <cstdlib>

#include <iostream>
#include <string>

#include "ResourceManager.h"
#include "Mesh.h"
#include "gameObject.h"
#include "camera.h"
#include "Robot.h"
#include "collisions.h"

float gamemode;
resourceManager RM;
GLFWwindow* window; // the render window
glm::mat4 modelMatrix(1.0); //the model matrix used for rendering
std::vector<gameObject> objects; //vector of objects in the game
camera Cam1(70.0f,0.1f,500.0f,16.0f/9.0f,glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f)); //set up the camera
camera Cam2(70.0f,0.1f,500.0f,16.0f/9.0f,glm::vec3(0.0f,5.0f,-5.0f),glm::vec3(0.0f,0.0f,-1.0f)); //set up the camera
bool cam1 = true; //is camera 1 being used
bool loaded = false; //is the engine loaded
collisions coll;


//the current and old positions of the mouse
double posX,posY,oldX,oldY;

void scroll_callback(GLFWwindow *window, double x, double y)
{
	//call back used when the mouse wheel is scrolled
	//zoom the camera in
	if(cam1 == false)
	{
	Cam2.zoom(y);
	}
}

void input()
{
	
		glfwGetCursorPos(window,&posX,&posY); //get the position of the cursor within the window
		//get the difference between the old and current mouse position
		float moveX = (float)(oldX - posX); 
		float moveY = (float)(oldY - posY);

	if(gamemode == 1) //if the game is playing, get input
	{
		//look around with the camea
		if(cam1 ==true){
		Cam1.look(moveX,moveY, 0.05f);}

		//move forwrds
		if (glfwGetKey(window,GLFW_KEY_W)){
			if(cam1 ==true){
				Cam1.forwards(0.05);} //move forward when W is pressed
		}

		if (glfwGetKey(window,GLFW_KEY_W) && glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)){
			if(cam1 ==true){
				Cam1.forwards(0.1);} //move forward when W is pressed
		}

		//move backwards
		if (glfwGetKey(window,GLFW_KEY_S)){
			if(cam1 ==true){
				Cam1.backwards(0.05);} //move backward when S is pressed
		}

		//move left
		if (glfwGetKey(window,GLFW_KEY_A)){
			if(cam1 ==true){
				Cam1.strafeLeft(0.05);} //move left when A is pressed
		}

		//move right
		if (glfwGetKey(window,GLFW_KEY_D)){
			if(cam1 ==true){
				Cam1.strafeRight(0.05);} //move right when D is pressed
		}

		if (glfwGetKey(window,GLFW_KEY_SPACE)){
			//if (Cam1.WALKING == true){
				//std::cout << "JUMP JUMP" << std::endl;
				Cam1.JUMPING_UP = true; //std::cout << "JUMPING_UP" << std::endl;
				Cam1.WALKING = false;
			//}
		}

		//switch to camera 1
		if (glfwGetKey(window,GLFW_KEY_1)){
			if(cam1 ==true)
			{
				cam1 = false;
			}
		}

		//switch to camera 2
		if (glfwGetKey(window,GLFW_KEY_2)){
			if(cam1 ==false)
			{
				cam1 = true;
			}
		}

		//pan the camera
		if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)){
			if(cam1 ==true){}
		else
			Cam2.pan(moveX*0.01f,moveY*0.01f);
		}

		//rotate the camera
		if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)){
			if(cam1 ==true){}
		else
			Cam2.rotate(moveX*0.01f,moveY*0.01f);
		}

		//go to the menu
	if (glfwGetKey(window,GLFW_KEY_ESCAPE)){
		gamemode = 3;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
	}
	}

	if(gamemode == 3) //game mode 3 is the menu
	{
		if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)){
		double xpos, ypos; 
		glfwGetCursorPos(window,&xpos,&ypos);//get the mosue input

		if(xpos >379 && xpos < 853 && ypos > 82 && ypos <268)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
			gamemode = 1;
		}

		if(xpos >379 && xpos < 853 && ypos > 369 && ypos <553)
		{
			//destroy window
			glfwDestroyWindow(window);
			//terminate glfw
			glfwTerminate();
			exit(0);
		}

		}
	}
	
	
	//make the old position of the mouse the current position
	oldX = posX;
	oldY = posY;
}

void render()
{
	//clear depth buffer before rendering
	gl::Clear(gl::DEPTH_BUFFER_BIT);
	//clear color buffer before rendering
	gl::Clear(gl::COLOR_BUFFER_BIT);
	
		for(int i=1; i<objects.size();i++)
		{
			RM.getTexture(objects[i].getTextureName()).useTexture(); //bind the texture for rendering
			RM.getShader(objects[i].getShaderName()).useProgram(); //ready the shader for rendering
			modelMatrix = objects[i].getTransformMatrix(); //set the model matrix for rendering

			//set up the model, view and porjection matrix for use with the shader
			GLuint modelMatrixID = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "mModel");
			GLuint viewMatrixID = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "mView");
			GLuint projectionMatrixID = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "mProjection");

			if(cam1 ==true)
			{
			gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
			gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &Cam1.getViewMatrix()[0][0]);
			gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &Cam1.getProjectionMatrix()[0][0]);
			}

			if(cam1 == false)
			{
			gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
			gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &Cam2.getViewMatrix()[0][0]);
			gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &Cam2.getProjectionMatrix()[0][0]);
			}

		//================================================
		//LIGHTING
		//================================================
			/*
		//set up light
		//diffuse intensity
		GLuint LD = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "Ld");
		gl::Uniform3f(LD,1.0f,1.0f,1.0f);
		//Ambient intensity
		GLuint LA = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "La");
		gl::Uniform3f(LA,0.0f,0.0f,0.0f);
		//light position
		GLuint Pos = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "LightPos");
		gl::Uniform3f(Pos,10.0f,10.0f,10.0f);

		glm::mat4 MV = Cam1.getViewMatrix() * modelMatrix;
		glm::mat3 mv = glm::mat3(glm::vec3(MV[0]),glm::vec3(MV[1]),glm::vec3(MV[2]));

		GLuint NormMat = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "NormalMatrix");
		gl::UniformMatrix3fv( NormMat, 1, gl::FALSE_, &mv[0][0]);

		//Set up blocks properties
		//diffuse reflectivity
		GLuint KD = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "Kd");
		gl::Uniform3f(KD,1.0f, 1.0f, 1.0f);
		//ambient reflectvity
		GLuint KA = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "Ka");
		gl::Uniform3f(KA,0.0f, 0.0f, 0.0f);
		//specular reflectivity
		GLuint KS = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "Ks");
		gl::Uniform3f(KS,1.0f, 1.0f, 1.0f);
		//specular reflection exponent
		int N = gl::GetUniformLocation(RM.getShader(objects[i].getShaderName()).getProgramID(), "n");
		gl::Uniform1f(N,64);
		*/

			//================================================
			//RENDER
			//================================================
			objects[i].render(); //render the object
		}
}

/*void renderSplashScreen()
{
	//clear depth buffer before rendering
	gl::Clear(gl::DEPTH_BUFFER_BIT);
	//clear color buffer before rendering
	gl::Clear(gl::COLOR_BUFFER_BIT);

	RM.getTexture(objects[1].getTextureName()).useTexture(); //bind the texture for rendering
	RM.getShader(objects[1].getShaderName()).useProgram(); //ready the shader for rendering
	modelMatrix = objects[1].getTransformMatrix(); //set the model matrix for rendering

	//set up the model, view and porjection matrix for use with the shader
	GLuint modelMatrixID = gl::GetUniformLocation(RM.getShader(objects[1].getShaderName()).getProgramID(), "mModel");
	GLuint viewMatrixID = gl::GetUniformLocation(RM.getShader(objects[1].getShaderName()).getProgramID(), "mView");
	GLuint projectionMatrixID = gl::GetUniformLocation(RM.getShader(objects[1].getShaderName()).getProgramID(), "mProjection");

	if(cam1 ==true)
	{
	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &Cam1.getViewMatrix()[0][0]);
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &Cam1.getProjectionMatrix()[0][0]);
	}

	if(cam1 == false)
	{
	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &Cam2.getViewMatrix()[0][0]);
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &Cam2.getProjectionMatrix()[0][0]);
	}

	objects[1].render();
}*/

void renderMenu()
{
	//clear depth buffer before rendering
	gl::Clear(gl::DEPTH_BUFFER_BIT);
	//clear color buffer before rendering
	gl::Clear(gl::COLOR_BUFFER_BIT);

	RM.getTexture(objects[0].getTextureName()).useTexture(); //bind the texture for rendering
	RM.getShader(objects[0].getShaderName()).useProgram(); //ready the shader for rendering
	modelMatrix = objects[0].getTransformMatrix(); //set the model matrix for rendering

	//set up the model, view and porjection matrix for use with the shader
	GLuint modelMatrixID = gl::GetUniformLocation(RM.getShader(objects[0].getShaderName()).getProgramID(), "mModel");
	GLuint viewMatrixID = gl::GetUniformLocation(RM.getShader(objects[0].getShaderName()).getProgramID(), "mView");
	GLuint projectionMatrixID = gl::GetUniformLocation(RM.getShader(objects[0].getShaderName()).getProgramID(), "mProjection");

	if(cam1 ==true)
	{
	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &Cam1.getViewMatrix()[0][0]);
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &Cam1.getProjectionMatrix()[0][0]);
	}

	if(cam1 == false)
	{
	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &Cam2.getViewMatrix()[0][0]);
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &Cam2.getProjectionMatrix()[0][0]);
	}

	objects[0].render();
}

int _tmain(int argc, _TCHAR* argv[])
{
	//=====================================================================================
	//Initialize GLFW and openGL
	//=====================================================================================

	// Initialize GLFW
	if(!glfwInit()) 
	exit( EXIT_FAILURE );

	// Select OpenGL 4.3 with a forward compatible core profile.
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, TRUE);

	//=====================================================================================
	//Create the window
	//=====================================================================================

	GLint height = 720; //height of the window
	GLint width = 1280; //width of the window
	std::string title = "Game Engine"; //name of the window

	window = glfwCreateWindow(width,height,title.c_str(), NULL, NULL); //create the render window
	
	//check window has opened
	if(window == NULL){
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;}

	//set the created window to the one that should be used
	glfwMakeContextCurrent(window);

	// Load the OpenGL functions.
    gl::exts::LoadTest didLoad = gl::sys::LoadFunctions();
    if (!didLoad) {
		glfwTerminate();
		exit(EXIT_FAILURE);}

	//make the window blue
	gl::ClearColor(0.0,0.5,1.0,1.0);
	//dont render faces that cant be seen
	gl::Enable(gl::CULL_FACE);

	//enable depth testing
	gl::Enable(gl::DEPTH_TEST);
	

	glfwSetScrollCallback(window,scroll_callback);

	//set the mouse current and old positions
	oldX = 0.0;
	oldY = 0.0;
	posX = 0.0;
	posY = 0.0;

	//=====================================================================================
	//LOAD ASSETS
	//=====================================================================================

	RM.loadMesh("block.txt", "block"); //create a block
	RM.loadMesh("Plane.txt", "plane"); //create a block
	RM.loadMesh("window.txt", "window"); //create a block

	RM.loadShader("../shader/basic3.frag","../shader/basic3.vert", "shader"); //load a shader
	RM.loadShader("../shader/basic5.frag","../shader/basic5.vert", "menuShader"); //load a shader

	RM.loadTexture("face.png"); //load a texture
	RM.loadTexture("grass.png"); //load a texture
	RM.loadTexture("Splash.png"); //load a texture
	RM.loadTexture("Menu.png"); //load a texture
	RM.loadTexture("texture1.png"); //load a texture
	RM.loadTexture("texture2.png"); //load a texture
	RM.loadTexture("texture3.png"); //load a texture
	RM.loadTexture("texture4.png"); //load a texture

	//create objects to use
	gameObject plane(RM.getMesh("plane"), "grass.png", "shader", glm::vec3(0.0,-1.0,0.0));
	gameObject block1(RM.getMesh("block"), "texture1.png", "shader", glm::vec3(0.0,9.0,5.0));
	gameObject block2(RM.getMesh("block"), "texture2.png", "shader", glm::vec3(7.0,7.0,-5.0));
	gameObject block3(RM.getMesh("block"), "texture3.png", "shader", glm::vec3(-7.0,14.0,-5.0));
	gameObject block4(RM.getMesh("block"), "face.png", "shader", glm::vec3(9.5,9.0,-15.0));
	gameObject block5(RM.getMesh("block"), "texture3.png", "shader", glm::vec3(-14.5,14.0,-25.0));
	gameObject block6(RM.getMesh("block"), "texture1.png", "shader", glm::vec3(-14.5,14.0,-37.0));
	gameObject block7(RM.getMesh("block"), "texture4.png", "shader", glm::vec3(7.5,9.0,-37.0));
	gameObject block8(RM.getMesh("block"), "texture2.png", "shader", glm::vec3(-17.0,7.0,-5.0));
	gameObject block9(RM.getMesh("block"), "face.png", "shader", glm::vec3(17.0,7.0,-5.0));
	gameObject block10(RM.getMesh("block"), "texture4.png", "shader", glm::vec3(-22.0,9.0,5.0));
	gameObject player(RM.getMesh("block"), "texture4.png", "shader", glm::vec3(0.0,0.0,0.0));

	block1.scale(glm::vec3(20,20,5));
	block2.scale(glm::vec3(5,16,5));
	block3.scale(glm::vec3(5,30,5));
	block4.scale(glm::vec3(10,20,10));
	block5.scale(glm::vec3(20,30,10));
	block6.scale(glm::vec3(20,30,10));
	block7.scale(glm::vec3(20,20,10));
	block8.scale(glm::vec3(10,16,5));
	block9.scale(glm::vec3(10,16,5));
	block10.scale(glm::vec3(20,20,5));

	gameObject menu(RM.getMesh("window"), "Menu.png", "menuShader", glm::vec3(0.0,0.0,0.0));
	gameObject splash(RM.getMesh("window"), "Splash.png", "menuShader", glm::vec3(0.0,0.0,-10.0));
	
	objects.push_back(menu);
	objects.push_back(player);
	objects.push_back(block1); //add the block to the objects vector for rendering
	objects.push_back(block2); //add the block to the objects vector for rendering
	objects.push_back(block3); //add the block to the objects vector for rendering
	objects.push_back(block4); //add the block to the objects vector for rendering
	objects.push_back(block5); //add the block to the objects vector for rendering
	objects.push_back(block6); //add the block to the objects vector for rendering
	objects.push_back(block7); //add the block to the objects vector for rendering
	objects.push_back(block8); //add the block to the objects vector for rendering
	objects.push_back(block9); //add the block to the objects vector for rendering
	objects.push_back(block10); //add the player to the objects vector for rendering
	objects.push_back(plane); //create a plane for the world

	gamemode = 2;

	//=====================================================================================
	//GAME LOOP
	//=====================================================================================

	//while the renderwindow isnt closed, keep running through the loop
	while(!glfwWindowShouldClose(window))
	{
		GLint width,height;
		//get the size of the framebuffer of the current context (current window)
		glfwGetFramebufferSize(window, &width, &height);
		//set the viewport size
		gl::Viewport(0,0,width,height);
		//clear color buffer before rerendering
		gl::Clear(gl::COLOR_BUFFER_BIT);

		if(loaded == false && glfwGetTime() > 0)
		{
			gamemode  = 3;
			loaded  = true;
		}
		float elapsedTime = glfwGetTime();

		if(glfwGetTime()-elapsedTime > 1/60)
		{

			glm::vec3 cam(0.f,0.f,0.f);
			cam = Cam1.getPosition();

			if (Cam1.JUMPING_UP == true){
				Cam1.setPosition(glm::vec3(cam.x,cam.y+0.1,cam.z));
				if (cam.y > Cam1.MAX_JUMP) {
					//std::cout << "Y: " << cam.y << std::endl;
					Cam1.JUMPING_UP = false; 
					Cam1.JUMPING_DOWN = true; //std::cout << "JUMPING_DOWN" << std::endl;
				}
			}

			if (Cam1.JUMPING_DOWN == true){
				Cam1.setPosition(glm::vec3(cam.x,cam.y-0.1,cam.z));
				if (cam.y <= 0) {
					Cam1.JUMPING_DOWN = false;
					Cam1.WALKING = true; //std::cout << "WALKING" << std::endl;
				}
			}

			

			/*if (Cam1.WALKING == true){
				Cam1.setPosition(glm::vec3(cam.x,cam.y,cam.z));
			}*/

			glm::vec3 previousPosition = Cam1.getPosition();
		//=====================
		// MAIN LOOP GOES HERE
		//=====================
		input(); //get and proecss input
		if(gamemode == 1)
		{
			player.setPosition(Cam1.getPosition());
			//std::cout << player.getPosition().x << " " << player.getPosition().z << " " << Cam1.getPosition().x << " " << Cam1.getPosition().z <<std::endl;
			
			for(int i = 2; i < objects.size()-1; i ++)
			{
				if (coll.checkCollision(player,objects.at(i)) == true)
				{
					std::cout << "COLLISION " << i << std::endl;
					Cam1.setPosition(previousPosition);
				}
			}
			render(); //render all the objects
		}
		/*if(gamemode == 2)
		{
			renderSplashScreen(); //render all the objects
		}*/

		if(gamemode == 3)
		{
			renderMenu(); //render all the objects
		}
		
		//get inputs
		glfwPollEvents();
		//redraws the window
		glfwSwapBuffers(window);

		elapsedTime = glfwGetTime();
		}


	}

	
	//destroy window
	glfwDestroyWindow(window);
	//terminate glfw
	glfwTerminate();
	exit(0);
	
}

