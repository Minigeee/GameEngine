#ifndef POST_PROCESS_H
#define POST_PROCESS_H

///////////////////////////////////////////////////////////////////////////////

class FrameBuffer;
class Shader;
class VertexArray;
class VertexBuffer;

class PostProcess
{
public:
	PostProcess();
	~PostProcess();

	/* Enable post processing system */
	void Enable();

	/* Render post process effects */
	void Render(FrameBuffer* fb);

	/* Returns if system is enabled */
	bool IsEnabled() const;
	/* Get framebuffer */
	FrameBuffer* GetFrameBuffer() const;

private:
	/* Framebuffer used as input */
	FrameBuffer* mFrameBuffer;
	/* Shader used to render effects */
	Shader* mShader;
	/* Vertex array to render quad */
	VertexArray* mVertexArray;
	/* Vertex buffer to store data */
	VertexBuffer* mVertexBuffer;

	/* Is system enabled */
	bool mIsEnabled;
};

///////////////////////////////////////////////////////////////////////////////

#endif