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
#include "moveable.h"
#include "player.h"

float gamemode;
resourceManager RM;
GLFWwindow* window; // the render window
glm::mat4 modelMatrix(1.0); //the model matrix used for rendering
std::vector<gameObject> objects; //vector of objects in the game
//camera Cam1(70.0f,0.1f,500.0f,16.0f/9.0f,glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f)); //set up the camera
bool loaded = false; //is the engine loaded
collisions coll;
moveable move;
player player1;

//the current and old positions of the mouse
double posX,posY,oldX,oldY;

void input()
{
	glfwGetCursorPos(window,&posX,&posY); //get the position of the cursor within the window
	//get the difference between the old and current mouse position
	float moveX = (float)(oldX - posX); 
	float moveY = (float)(oldY - posY);

	if(gamemode == 1) //if the game is playing, get input
	{
		//look around with the camea
		player1.playerCam.look(moveX,moveY, 0.05f);

		//move forwrds
		if (glfwGetKey(window,GLFW_KEY_W)){
		player1.playerCam.forwards(0.05);} //move forward when W is pressed

		if (glfwGetKey(window,GLFW_KEY_W) && glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)){
				player1.playerCam.forwards(0.1);} //move forward when W is pressed

		//move backwards
		if (glfwGetKey(window,GLFW_KEY_S)){
				player1.playerCam.backwards(0.05);} //move backward when S is pressed

		//move left
		if (glfwGetKey(window,GLFW_KEY_A)){
				player1.playerCam.strafeLeft(0.05);} //move left when A is pressed

		//move right
		if (glfwGetKey(window,GLFW_KEY_D)){
				player1.playerCam.strafeRight(0.05);} //move right when D is pressed

		if (glfwGetKey(window,GLFW_KEY_SPACE)){
			if (player1.getCanJump() == true){
				//std::cout << "JUMP JUMP" << std::endl;
				glm::vec3 vel = player1.playerCam.getVelocity();
				vel.y = 20;
				player1.playerCam.setVelocity(vel);
				player1.setCanJump(false);

			}
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

			gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
			gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &player1.playerCam.getViewMatrix()[0][0]);
			gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &player1.playerCam.getProjectionMatrix()[0][0]);

			//================================================
			//RENDER
			//================================================
			objects[i].render(); //render the object
		}
}

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

	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, &modelMatrix[0][0]);
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, &player1.playerCam.getViewMatrix()[0][0]);
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, &player1.playerCam.getProjectionMatrix()[0][0]);


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
	gameObject block1(RM.getMesh("block"), "texture1.png", "shader", glm::vec3(0.0,-0.5,5.0));
	gameObject block2(RM.getMesh("block"), "texture2.png", "shader", glm::vec3(7.0,7.0,-5.0));
	gameObject block3(RM.getMesh("block"), "texture3.png", "shader", glm::vec3(-7.0,14.0,-5.0));
	gameObject block4(RM.getMesh("block"), "face.png", "shader", glm::vec3(9.5,9.0,-15.0));
	gameObject block5(RM.getMesh("block"), "texture3.png", "shader", glm::vec3(-14.5,14.0,-25.0));
	gameObject block6(RM.getMesh("block"), "texture1.png", "shader", glm::vec3(-14.5,14.0,-37.0));
	gameObject block7(RM.getMesh("block"), "texture4.png", "shader", glm::vec3(7.5,9.0,-37.0));
	gameObject block8(RM.getMesh("block"), "texture2.png", "shader", glm::vec3(-17.0,7.0,-5.0));
	gameObject block9(RM.getMesh("block"), "face.png", "shader", glm::vec3(17.0,7.0,-5.0));
	gameObject block10(RM.getMesh("block"), "texture4.png", "shader", glm::vec3(-22.0,9.0,5.0));

	block1.scale(glm::vec3(20,1,5));
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

	player1.setUpPlayerObject(RM.getMesh("block"), "texture4.png", "shader", glm::vec3(0.0,0.0,5.0));
	player1.setUpPlayerCamera(70.0f,0.1f,500.0f,16.0f/9.0f,glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f));

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
		//=====================
		// MAIN LOOP GOES HERE
		//=====================
		 //get and proecss input
		glm::vec3 previousPosition = player1.playerCam.getPosition();
		player1.setPosition(player1.playerCam.getPosition());
		input();
		player1.setPosition(player1.playerCam.getPosition());
		if(gamemode == 1)
		{
			for(int i = 1; i < objects.size(); i ++)
			{
				if (coll.checkCollision(player1,objects.at(i)) == true)
				{
					std::cout << "COLLISION XZ " << i << std::endl;
					player1.playerCam.setPosition(previousPosition);
					
				}
			}

			//MOVE ALL THE TIME
			move.euler(&player1.playerCam);
			player1.setPosition(player1.playerCam.getPosition());
			
			for(int i = 1; i < objects.size(); i ++)
			{
				if (coll.checkCollision(player1,objects.at(i)) == true)
				{
					player1.setCanJump(true);
					std::cout << "COLLISION Y " << i << std::endl;
					float dif = objects.at(i).bb.getMax().y - player1.bb.getMin().y;
					float height = (player1.bb.getMax().y - player1.bb.getMin().y)/2;
					player1.playerCam.setPosition(glm::vec3(player1.playerCam.getPosition().x,previousPosition.y,player1.playerCam.getPosition().z));
					player1.playerCam.setVelocity(glm::vec3(0,0,0));
					player1.setPosition(player1.playerCam.getPosition());
				}
			}
			render(); //render all the objects
		}
	
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

