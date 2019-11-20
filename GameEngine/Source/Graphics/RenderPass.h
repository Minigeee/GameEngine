#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <Math/Vector4.h>
#include <Math/Plane.h>

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

	/* Create framebuffer as render pass target */
	void CreateTarget(
		Texture::Format fmt = Texture::Rgb,
		Image::DataType dtype = Image::Ushort);

	/* Set lighting pass object */
	void SetLightingPass(LightingPass* pass);
	/* Set render pass plane */
	void SetPlane(const Plane& plane);
	/* Set clipping enabled */
	void SetClippingEnabled(bool enabled);

	/* Get render pass type */
	Type GetType() const;
	/* Get render pass target buffer */
	FrameBuffer* GetTarget() const;
	/* Get lighting pass object */
	LightingPass* GetLightingPass() const;
	/* Get plane */
	const Plane& GetPlane() const;
	/* Returns true if clipping is enabled */
	bool IsClippingEnabled() const;

private:
	/* Render pass type */
	Type mType;
	/* Target framebuffer */
	FrameBuffer* mTarget;
	/* Lighting pass */
	LightingPass* mLightingPass;

	/* Plane used for various purposes */
	Plane mPlane;

	/* True if clipping is enabled */
	bool mIsClippingEnabled;
};

///////////////////////////////////////////////////////////////////////////////

#endif