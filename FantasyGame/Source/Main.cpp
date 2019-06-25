#include <vld.h>

#include <Engine/Application.h>
#include <Core/LogFile.h>

int main()
{
	Application app;
	bool success = app.Run();

	return success ? 0 : 1;
}