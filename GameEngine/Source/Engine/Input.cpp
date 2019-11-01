#include <Engine/Input.h>
#include <Engine/Window.h>

#include <Scene/Scene.h>

#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input* input = (Input*)glfwGetWindowUserPointer(window);
	input->OnKeyEvent((Input::Keyboard)key, (Input::Action)action);
}

void OnMouseMove(GLFWwindow* window, double x, double y)
{
	Input* input = (Input*)glfwGetWindowUserPointer(window);
	input->OnMouseMove((float)x, (float)y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	Input* input = (Input*)glfwGetWindowUserPointer(window);
	input->OnMouseButton((Input::Mouse)button, (Input::Action)action);
}

void OnMouseScroll(GLFWwindow* window, double x, double y)
{
	Input* input = (Input*)glfwGetWindowUserPointer(window);
	input->OnMouseScroll((float)x, (float)y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Input::Init(Window* window)
{
	GLFWwindow* win = (GLFWwindow*)window->mWindow;
	mWindow = win;

	glfwSetKeyCallback(win, ::OnKeyEvent);
	glfwSetCursorPosCallback(win, ::OnMouseMove);
	glfwSetMouseButtonCallback(win, ::OnMouseButton);
	glfwSetScrollCallback(win, ::OnMouseScroll);

	glfwSetWindowUserPointer(win, this);
}

///////////////////////////////////////////////////////////////////////////////

void Input::SetScene(Scene* scene)
{
	mScene = scene;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Input::SetCursorMode(CursorMode mode)
{
	glfwSetInputMode((GLFWwindow*)mWindow, GLFW_CURSOR, mode);
}

///////////////////////////////////////////////////////////////////////////////

bool Input::KeyPressed(Keyboard key) const
{
	return glfwGetKey((GLFWwindow*)mWindow, key) == Press;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Input::OnKeyEvent(Keyboard key, Action action)
{
	E_KeyEvent event(key, action);
	mScene->OnKeyEvent(event);
}

void Input::OnMouseMove(float x, float y)
{
	E_MouseMove event(x, y);
	mScene->OnMouseMove(event);
}

void Input::OnMouseButton(Mouse button, Action action)
{
	E_MouseButton event(button, action);
	mScene->OnMouseButton(event);
}

void Input::OnMouseScroll(float x, float y)
{
	E_MouseScroll event(x, y);
	mScene->OnMouseScroll(event);
}

///////////////////////////////////////////////////////////////////////////////