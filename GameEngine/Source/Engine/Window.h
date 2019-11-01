#ifndef WINDOW_H
#define WINDOW_H

#include <Core/DataTypes.h>
#include <Math/Vector2.h>

///////////////////////////////////////////////////////////////////////////////

class Input;

class Window
{
	friend Input;

public:
	Window();

	/* Create window */
	bool Create(Uint32 w, Uint32 h, const char* title, bool fullscreen = false);
	/* Cleanup resources */
	void CleanUp();

	/* Poll input events */
	void PollEvents();
	/* Display rendered frame (swap back buffer) */
	void Display();

	/* Returns if window is (or should be) open */
	bool IsOpen() const;
	/* Queue window close */
	void Close();

	/* Set window resolution */
	void SetResolution(Uint32 w, Uint32 h);
	/* Set window title */
	void SetTitle(const char* title);

	/* Get window resolution */
	const Vector2u& GetResolution() const;
	/* Get window title */
	const char* GetTitle() const;

private:
	/* GLFW window ptr */
	void* mWindow;

	/* Window resolution */
	Vector2u mResolution;
	/* Window title */
	const char* mTitle;
};

///////////////////////////////////////////////////////////////////////////////

#endif