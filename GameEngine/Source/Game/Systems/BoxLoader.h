#ifndef BOX_LOADER_H
#define BOX_LOADER_H

#include <Scene/ObjectLoader.h>

///////////////////////////////////////////////////////////////////////////////

class Renderer;
class Model;

class BoxLoader : public ObjectLoader
{
	TYPE_INFO(BoxLoader);

public:
	BoxLoader();
	~BoxLoader();

private:
	/* Called during inialization */
	void OnInit() override;
	/* Called when chunk enters range */
	void OnChunkEnter(ObjectChunk& chunk) override;
	/* Called when chunk leaves range */
	void OnChunkLeave(ObjectChunk& chunk) override;

};

///////////////////////////////////////////////////////////////////////////////

#endif