#include <Engine/Window.h>
#include <Core/LogFile.h>
#include <Graphics/OpenGL.h>

#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Window::Window() :
	mWindow			(0),
	mResolution		(0),
	mTitle			(0)
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Window::Create(Uint32 w, Uint32 h, const char* title, bool fullscreen)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
	GLFWwindow* window = glfwCreateWindow(w, h, title, monitor, NULL);
	if (!window)
	{
		LOG_ERROR << "Failed to create window\n";
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_ERROR << "Failed to initalize GLAD\n";
		return false;
	}


	mWindow = window;
	mResolution = Vector2u(w, h);
	mTitle = title;

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void Window::CleanUp()
{
	glfwTerminate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Window::PollEvents()
{
	glfwPollEvents();
}

///////////////////////////////////////////////////////////////////////////////

void Window::Display()
{
	glfwSwapBuffers((GLFWwindow*)mWindow);
}

///////////////////////////////////////////////////////////////////////////////

bool Window::IsOpen() const
{
	return !glfwWindowShouldClose((GLFWwindow*)mWindow);
}

void Window::Close()
{
	glfwSetWindowShouldClose((GLFWwindow*)mWindow, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Window::SetResolution(Uint32 w, Uint32 h)
{
	mResolution = Vector2u(w, h);
	glfwSetWindowSize((GLFWwindow*)mWindow, w, h);
}

void Window::SetTitle(const char* title)
{
	mTitle = title;
	glfwSetWindowTitle((GLFWwindow*)mWindow, title);
}

///////////////////////////////////////////////////////////////////////////////

const Vector2u& Window::GetResolution() const
{
	return mResolution;
}

const char* Window::GetTitle() const
{
	return mTitle;
}

///////////////////////////////////////////////////////////////////////////////