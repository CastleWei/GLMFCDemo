#include "stdafx.h"
#include "GLHelper.h"

GLHelper::GLHelper()
{
}


GLHelper::~GLHelper()
{
}

long GLHelper::TimeMilli(){
	LARGE_INTEGER liNow;
	QueryPerformanceCounter(&liNow);
	return (long)((liNow.QuadPart - liStart.QuadPart) * 1000L / liFreq.QuadPart);
}
float GLHelper::Timef(){
	LARGE_INTEGER liNow;
	QueryPerformanceCounter(&liNow);
	return (float)(liNow.QuadPart - liStart.QuadPart) / liFreq.QuadPart;
}
double GLHelper::Timed(){
	LARGE_INTEGER liNow;
	QueryPerformanceCounter(&liNow);
	return (double)(liNow.QuadPart - liStart.QuadPart) / liFreq.QuadPart;
}
float GLHelper::Tick(){
	if (liLast.QuadPart == 0){
		QueryPerformanceCounter(&liLast);
		return 0.0f;
	}
	else{
		LARGE_INTEGER liNow, liDelta;
		QueryPerformanceCounter(&liNow);
		liDelta.QuadPart = liNow.QuadPart - liLast.QuadPart;
		liLast.QuadPart = liNow.QuadPart;
		diffTime = (float)((double)liDelta.QuadPart / liFreq.QuadPart);
		return diffTime;
	}
}

void GLHelper::Prepare(HDC hDC, GLuint width, GLuint height){
	this->hDC = hDC;
	this->width = width;
	this->height = height;
}

void GLHelper::Suspend(){
	if (!isSuspended){
		pThread->SuspendThread();
		isSuspended = true;
	}
}

void GLHelper::Resume(){
	if (isSuspended){
		pThread->ResumeThread();
		isSuspended = false;
	}
}

void GLHelper::Start(){
	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter(&liStart);

	isLooping = true;
	pThread = AfxBeginThread(glmain, (LPVOID)this);
}

void GLHelper::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height){
	this->width = width;
	this->height = height;
}

void GLHelper::Stop(){
	isLooping = false;
}

bool SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_DRAW_TO_BITMAP |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER |
		//必须启用双缓存，否则直接错误
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

//全局变量
GLHelper *glhp; //thi global GLHelper instance pointer

int width, height;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Shader ourShader;
GLuint VBOs[1], VAOs[1], EBOs[3];
GLsizei multi_counts[100];
GLvoid* multi_indices[100];

GLfloat lastX, lastY;
bool mouseLeft;
bool mouseRight;
bool keys[256];

bool prepare_context(){
	if (SetWindowPixelFormat(glhp->hDC) == FALSE)
		return 0;
	if (CreateViewGLContext(glhp->hDC, &glhp->GLContext) == FALSE)
		return 0;

	glewExperimental = GL_TRUE;
	glewInit();

	//width = glhp->width;
	//height = glhp->height;
	//glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

	glPolygonOffset(1, 1);
	glEnable(GL_POLYGON_OFFSET_FILL);

	return 1;
}

bool init_resource(){

	ourShader.Prepare("shader.v.glsl", "shader.f.glsl");

	struct point{
		GLfloat x;
		GLfloat y;
	};

	point vertices[101][101];
	for (int i = 0; i < 101; i++){
		for (int j = 0; j < 101; j++){
			vertices[i][j].x = (j - 50) / 50.0f;
			vertices[i][j].y = (i - 50) / 50.0f;
		}
	}

	GLushort indices[100 * 2 * 101];
	int i = 0;
	for (int y = 0; y < 100; y++){
		for (int x = 0; x < 101; x++){
			indices[i++] = (y + 1) * 101 + x;
			indices[i++] = y * 101 + x;
		}
	}

	for (int i = 0; i < 100; i++){
		multi_counts[i] = 2 * 101;
		multi_indices[i] = BOFF(i * 2 * 101 * sizeof(GLushort));
	}

	GLushort h_indx[100 * 2 * 101];
	i = 0;
	for (int y = 0; y < 101; y++){
		for (int x = 0; x < 100; x++){
			h_indx[i++] = y * 101 + x;
			h_indx[i++] = y * 101 + x + 1;
		}
	}

	GLushort v_indx[100 * 2 * 101];
	i = 0;
	for (int x = 0; x < 101; x++){
		for (int y = 0; y < 100; y++){
			v_indx[i++] = y * 101 + x;
			v_indx[i++] = (y + 1) * 101 + x;
		}
	}

	glGenVertexArrays(1, VAOs);
	glGenBuffers(1, VBOs);
	glGenBuffers(3, EBOs);

	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(h_indx), h_indx, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(v_indx), v_indx, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // Unbind VAO

	return true;
}

void do_movement(){
	// Camera controls
	if (keys['W'])
		camera.ProcessKeyboard(FORWARD, glhp->diffTime);
	if (keys['S'])
		camera.ProcessKeyboard(BACKWARD, glhp->diffTime);
	if (keys['A'])
		camera.ProcessKeyboard(LEFT, glhp->diffTime);
	if (keys['D'])
		camera.ProcessKeyboard(RIGHT, glhp->diffTime);
}

void logic(){
	// Create camera transformation
	glm::mat4 mvp;
	//glm::mat4 view;
	//glm::mat4 projection;
	glhp->view = camera.GetViewMatrix();
	glhp->projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 1000.0f);
	mvp = glhp->projection * glhp->view; //！！一定要记得变换的顺序！！

	ourShader.Use();

	glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "mvp"),
		1, GL_FALSE, glm::value_ptr(mvp));

	glUniform1f(glGetUniformLocation(ourShader.Program, "time"), glhp->Timef());

	glPointSize(10.0f);
	//glPolygonMode(GL_FRONT, GL_LINE);
}

void render(){
	// Render
	// Set Viewport
	if (width != glhp->width || height != glhp->height){
		width = glhp->width;
		height = glhp->height;
		glViewport(0, 0, width, height);
		camera.ViewportHeight = height;
	}
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Draw the triangle
	ourShader.Use();

	// Draw container
	glBindVertexArray(VAOs[0]);

	//glUniform1i(glGetUniformLocation(ourShader.Program, "isLine"), 0);
	//for (int i = 0; i < 100; i++){
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	//	glDrawElements(GL_TRIANGLE_STRIP, 202, GL_UNSIGNED_SHORT, (GLvoid*)(i * 202 * sizeof(GLushort)));
	//}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glMultiDrawElements(GL_TRIANGLE_STRIP, multi_counts, GL_UNSIGNED_SHORT, multi_indices, 100);

	//glUniform1i(glGetUniformLocation(ourShader.Program, "isLine"), 1);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	//glDrawElements(GL_LINES, 2 * 100 * 101, GL_UNSIGNED_SHORT, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
	//glDrawElements(GL_LINES, 2 * 100 * 101, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// Swap the screen buffers
	SwapBuffers(glhp->hDC);
}

void mainloop(){
	// Game loop
	while (glhp->isLooping){
		glhp->Tick();
		Sleep(1);

		while (!glhp->events.empty()){
			std::stringstream ss(glhp->events.front());

			std::string func;
			ss >> func;
			if (func == "key"){
				int key, action;
				ss >> key >> action;
				callback_key(key, action);
			}
			if (func == "mousemove"){
				int xpos, ypos;
				ss >> xpos >> ypos;
				callback_mousemove(xpos, ypos);
			}
			if (func == "mousebutton"){
				int button, action, xpos, ypos;
				ss >> button >> action >> xpos >> ypos;
				callback_mousebutton(button, action, xpos, ypos);
			}
			if (func == "mouseleave"){
				callback_mouseleave();
			}
			if (func == "scroll"){
				double xoffset, yoffset;
				ss >> xoffset >> yoffset;
				callback_scroll(xoffset, yoffset);
			}

			glhp->events.pop();
		}
		do_movement();
		logic();
		render();
	}
}

void free_resource(){
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, VAOs);
	glDeleteBuffers(1, VBOs);
	// Terminate rendering context, clearing any resources allocated by RC.
	Destroy(&glhp->GLContext);
}

UINT glmain(LPVOID pParam)
{
	glhp = (GLHelper*)pParam;

	prepare_context();

	if (!init_resource())
		return EXIT_FAILURE;

	mainloop();

	free_resource();
	return EXIT_SUCCESS;
}

void callback_key(int key, int action){
	if (key == VK_ESCAPE && action == GLKEYACT_PRESS)
		glhp->Stop();

	if (action == GLKEYACT_PRESS){
		keys[key] = true;
	}
	else if (action == GLKEYACT_RELEASE){
		keys[key] = false;
	}
}

void callback_mousemove(int xpos, int ypos){

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	// Reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (mouseLeft) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	if (mouseRight) {
		camera.ProcessMouseDragging(xoffset, yoffset);
	}

}

void callback_mousebutton(int button, int action, int xpos, int ypos){
	if (button == GLKEY_MOUSE_LEFT){
		if (action == GLKEYACT_PRESS){
			mouseLeft = true;
			lastX = xpos;
			lastY = ypos;
		}
		else if (action == GLKEYACT_RELEASE) {
			mouseLeft = false;
			//firstMouse = true;
		}
	}
	else if (button == GLKEY_MOUSE_RIGHT){
		if (action == GLKEYACT_PRESS){
			mouseRight = true;
			lastX = xpos;
			lastY = ypos;
		}
		else if (action == GLKEYACT_RELEASE) {
			mouseRight = false;
			//firstMouse = true;
		}
	}
	else if (button == GLKEY_MOUSE_MID && action == GLKEYACT_PRESS){
		GLfloat depth;
		glReadPixels(xpos, height - ypos - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

		glm::vec4 viewport = glm::vec4(0, 0, width, height);
		glm::vec3 wincoord = glm::vec3(xpos, height - ypos - 1, depth);
		glm::vec3 objcoord = glm::unProject(wincoord, glhp->view, glhp->projection, viewport);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), 3 * sizeof(GLfloat), glm::value_ptr(objcoord));
	}
}

void callback_mouseleave(){
	mouseLeft = false;
	mouseRight = false;
}

void callback_scroll(double xoffset, double yoffset){
	camera.ProcessMouseScroll(yoffset);
}
