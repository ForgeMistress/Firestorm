////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  /!\ main /!\
//
//  You know what this is.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <GLFW/glfw3.h>

static void GLFW_ErrorCallback(int error, const char* description)
{
	ELF_LOG_ERROR("[GLFW] Error Encountered:", error, ":", description);
}

static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(int ac, char** av)
{
	ELF_LOG(DEBUG, "[MAIN] Starting!");
	glfwSetErrorCallback(GLFW_ErrorCallback);
	ELF_LOG(DEBUG, "[MAIN] Set GLFW callback.");

	GLFWwindow* window;
	if(!glfwInit())
	{
		ELF_LOG_ERROR("[GLFW] Could not initialize GLFW.");
		return 1;
	}

	window = glfwCreateWindow(800, 600, "Elflord", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
		ELF_LOG_ERROR("[GLFW] Could not initialize window.");
		return 1;
	}

	/*glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, GLFW_KeyCallback);

	bool running = true;
	while(running)
	{
		int width = 800;
		int height = 640;

		glfwGetFramebufferSize(window, &width, &height);

		glfwWaitEvents();
		if(glfwWindowShouldClose(window))
		{
			running = false;
		}
	}

	ELF_LOG_DEBUG("[MAIN] Closing Elflord.");*/
	return 0;
}