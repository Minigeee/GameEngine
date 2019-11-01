#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <Math/Vector4.h>

///////////////////////////////////////////////////////////////////////////////

class FrameBuffer;
class Shader;
class Scene;

/* Defines the lighting function at end of deferred render pass */
class LightingPass
{
public:
	LightingPass(Scene* scene);
	virtual ~LightingPass();

	/* Bind shader, set uniforms */
	virtual void Render(FrameBuffer* gbuffer) = 0;

protected:
	/* Shader used to render lighting */
	Shader* mShader;
	/* Access to scene's lights */
	Scene* mScene;
};

///////////////////////////////////////////////////////////////////////////////

/* Default lighting pass */
class DefaultLighting : public LightingPass
{
public:
	DefaultLighting(Scene* scene);
	~DefaultLighting();

	void Render(FrameBuffer* gbuffer) override;
};

///////////////////////////////////////////////////////////////////////////////

/* Defines render pass options */
class RenderPass
{
public:
	enum Type
	{
		None = 0,
		Normal = 1 << 0,
		Reflect = 1 << 1,
		Refract = 1 << 2,
		Shadow = 1 << 3
	};

public:
	RenderPass(Type type, bool create_fb = true);
	~RenderPass();

	/* Set lighting pass object */
	void SetLightingPass(LightingPass* pass);
	/* Free output buffer */
	void FreeOutput();

	/* Get render pass type */
	Type GetType() const;
	/* Get render pass output buffer */
	FrameBuffer* GetOutput() const;
	/* Get lighting pass object */
	LightingPass* GetLightingPass() const;

private:
	/* Render pass type */
	Type mType;
	/* Ouptput framebuffer */
	FrameBuffer* mOutput;
	/* Lighting pass */
	LightingPass* mLightingPass;

	/* Reflection plane */
	Vector4f mReflectionPlane;
};

///////////////////////////////////////////////////////////////////////////////

#endif