#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <Math/Vector4.h>

#include <Graphics/FrameBuffer.h>

///////////////////////////////////////////////////////////////////////////////

class Shader;
class Scene;

/* Defines the lighting function at end of deferred render pass */
class LightingPass
{
public:
	LightingPass(Scene* scene);
	virtual ~LightingPass();

	/* Bind shader, set uniforms */
	virtual void RenderSetup(FrameBuffer* gbuffer) = 0;

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

	void RenderSetup(FrameBuffer* gbuffer) override;
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
		Shadow = 1 << 3,

		All = Normal | Reflect | Refract | Shadow
	};

public:
	RenderPass();
	RenderPass(Type type);
	~RenderPass();

	/* Set lighting pass object */
	void SetLightingPass(LightingPass* pass);
	/* Create framebuffer as render pass target */
	void CreateTarget(
		Texture::Format fmt = Texture::Rgb,
		Image::DataType dtype = Image::Ushort);

	/* Get render pass type */
	Type GetType() const;
	/* Get render pass target buffer */
	FrameBuffer* GetTarget() const;
	/* Get lighting pass object */
	LightingPass* GetLightingPass() const;

private:
	/* Render pass type */
	Type mType;
	/* Target framebuffer */
	FrameBuffer* mTarget;
	/* Lighting pass */
	LightingPass* mLightingPass;

	/* Reflection plane */
	Vector4f mReflectionPlane;
};

///////////////////////////////////////////////////////////////////////////////

#endif