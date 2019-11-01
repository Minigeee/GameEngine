#include <vld.h>

#include <Engine/Application.h>

#include <cstdlib>
#include <ctime>

int main()
{
	srand(time(NULL));

	Application app;
	bool success = app.Run();

	return success ? 0 : 1;
}