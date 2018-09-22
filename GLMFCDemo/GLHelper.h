#pragma once

#include <iostream>
#include <queue>
#include <sstream>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

// Buffer offset
#define BOFF(offset) (static_cast<char*>(0) + (offset))

#define GLKEYACT_PRESS 1
#define GLKEYACT_RELEASE 0
#define GLKEY_MOUSE_LEFT 1
#define GLKEY_MOUSE_RIGHT 2
#define GLKEY_MOUSE_MID 3

bool SetWindowPixelFormat(HDC hDC);
bool CreateViewGLContext(HDC hDC, HGLRC *pGLContext);
void Destroy(HGLRC *pGLContext);

bool prepare_context();
bool init_resource();
void do_movement();
void logic();
void render();
void mainloop();
void free_resource();
UINT glmain(LPVOID pParam);

void callback_key(int key, int action);
void callback_mousemove(int xpos, int ypos);
void callback_mousebutton(int button, int action, int xpos, int ypos);
void callback_mouseleave();
void callback_scroll(double xoffset, double yoffset);

class GLHelper
{
public:
	bool isLooping = false;
	bool isSuspended = false;
	bool isDestroyed = false;
	int GLPixelIndex = 0;
	LARGE_INTEGER liStart = { { 0 } };
	LARGE_INTEGER liLast = { { 0 } };
	LARGE_INTEGER liFreq = { { 0 } };
	HGLRC GLContext = NULL;
	HDC hDC;
	CWinThread* pThread;
	GLuint width, height;

	std::queue<std::string> events;

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 GetVP(){ return projection * view; }

	GLHelper();
	~GLHelper();

	float diffTime = 0.0f;

	long TimeMilli();
	float Timef();
	double Timed();
	float Tick();
	float Fps(){ return (diffTime == 0.0f ? 0.0f : 1.0f / diffTime); }

	void Prepare(HDC hDC, GLuint width, GLuint height);
	void Start();
	void Suspend();
	void Resume();
	void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void Stop();

	//void callkey(int key, int action){ callback_key(key, action); }
	//void callmousemove(int xpos, int ypos){ callback_mousemove(xpos, ypos); }
	//void callmousebutton(int button, int action, int xpos, int ypos){ callback_mousebutton(button, action, xpos, ypos); }
	//void callscroll(double xoffset, double yoffset){ callback_scroll(xoffset, yoffset); }
	void callkey(int key, int action){ 
		std::stringstream ss;
		ss << "key" << ' ' << key << ' ' << action;
		events.push(ss.str()); 
	}
	void callmousemove(int xpos, int ypos){ 
		std::stringstream ss;
		ss << "mousemove" << ' ' << xpos << ' ' << ypos;
		events.push(ss.str()); 
	}
	void callmousebutton(int button, int action, int xpos, int ypos){
		std::stringstream ss;
		ss << "mousebutton" << ' ' << button << ' ' << action << ' ' << xpos << ' ' << ypos;
		events.push(ss.str());
	}
	void callmouseleave(){
		events.push("mouseleave");
	}
	void callscroll(double xoffset, double yoffset){
		std::stringstream ss;
		ss << "scroll" << ' ' << xoffset << ' ' << yoffset;
		events.push(ss.str()); 
	}
};
