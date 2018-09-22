#pragma once

#include "stdafx.h"

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

UINT glmain(LPVOID pParam);

class GLHelper
{
public:
	bool isLooping = false;
	bool isSuspended = false;
	int GLPixelIndex = 0;
	HGLRC GLContext = NULL;
	HDC hDC;
	CWinThread* pThread;
	GLuint width, height;

	GLHelper(){}
	~GLHelper(){}

	void Prepare(HDC hDC, GLuint width, GLuint height){
		this->hDC = hDC;
		this->width = width;
		this->height = height;
	}

	void Start(){
		isLooping = true;
		pThread = AfxBeginThread(glmain, (LPVOID)this);
	}

	void SetSize(GLuint width, GLuint height){
		this->width = width;
		this->height = height;
	}

	void Stop(){
		isLooping = false;
	}
};

bool SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_DRAW_TO_BITMAP |
		PFD_SUPPORT_OPENGL |
		PFD_SUPPORT_GDI |
		PFD_STEREO_DONTCARE;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;

	int GLPixelIndex = ChoosePixelFormat(hDC, &pixelDesc);
	if (GLPixelIndex == 0) // Let's choose a default index.  
	{
		GLPixelIndex = 1;
		if (DescribePixelFormat(hDC, GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
		{
			return FALSE;
		}
	}
	if (SetPixelFormat(hDC, GLPixelIndex, &pixelDesc) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
bool CreateViewGLContext(HDC hDC, HGLRC *pGLContext)
{
	HGLRC &GLContext = *pGLContext;
	GLContext = wglCreateContext(hDC);//用当前DC产生绘制环境(RC)   
	if (GLContext == NULL)
	{
		return FALSE;
	}
	if (wglMakeCurrent(hDC, GLContext) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
void Destroy(HGLRC *pGLContext){
	HGLRC &GLContext = *pGLContext;
	if (wglGetCurrentContext() != NULL)
	{
		// make the rendering context not current   
		wglMakeCurrent(NULL, NULL);
	}
	if (GLContext != NULL)
	{
		wglDeleteContext(GLContext);
		GLContext = NULL;
	}
}

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

// The MAIN function, from here we start the application and run the game loop
UINT glmain(LPVOID pParam)
{
	AfxMessageBox(_T("glmain"));
	GLHelper &glhp = *((GLHelper*)pParam);
	if (SetWindowPixelFormat(glhp.hDC) == FALSE)
		return 0;
	if (CreateViewGLContext(glhp.hDC,&glhp.GLContext) == FALSE)
		return 0;

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	int width = glhp.width;
	int height = glhp.height;

	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		//std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		//std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		//std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, // Left  
		0.5f, -0.5f, 0.0f, // Right 
		0.0f, 0.5f, 0.0f  // Top   
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	// Game loop
	while (glhp.isLooping)
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		//glfwPollEvents();

		// Define the viewport dimensions
		if (width != glhp.width || height != glhp.height)
			glViewport(0, 0, width, height);

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw our first triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap the screen buffers
		SwapBuffers(glhp.hDC);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	Destroy(&glhp.GLContext);
	return 0;
}

