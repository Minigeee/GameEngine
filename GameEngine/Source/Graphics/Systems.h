#ifndef GRAPHICS_SYSTEMS_H
#define GRAPHICS_SYSTEMS_H

#include <Scene/GameSystem.h>

#include <Scene/Components.h>
#include <Graphics/Components.h>

///////////////////////////////////////////////////////////////////////////////

class TransformMatrixSystem : public GameSystem
{
	TYPE_INFO(TransformMatrixSystem);

	REQUIRES_COMPONENTS(
		TransformComponent,
		RenderComponent
	);

	REQUIRES_TAGS(
		"Dynamic"
	);

public:
	TransformMatrixSystem();
	~TransformMatrixSystem();

	void Execute(const TransformComponent& t, RenderComponent& r);
};

///////////////////////////////////////////////////////////////////////////////

#endif