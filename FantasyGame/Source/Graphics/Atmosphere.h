#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include <Math/Vector3.h>

#include <Graphics/PostProcess.h>

///////////////////////////////////////////////////////////////////////////////

class Scene;

class Atmosphere : public PostProcess::Effect
{
public:
	Atmosphere();
	~Atmosphere();

	/* Do precalculations */
	void Init(Scene* scene);

	/* Render as post processing effect */
	void Render(VertexArray* vao) override;

	/* Set uniforms needed for atmosphere shaders */
	void SetUniforms(Shader* shader);

	/* Get transmittance buffer */
	FrameBuffer* GetTransmittanceBuffer() const;
	/* Get scattering buffer */
	FrameBuffer* GetScatteringBuffer() const;
	/* Bind transmittance texture and set uniform */
	void BindTransmittance(Shader* shader, Uint32 slot);
	/* Bind scattering texture and set uniform */
	void BindScattering(Shader* shader, Uint32 slot);

public:
	/* Color of sunlight */
	Vector3f mSolarIrradiance;
	/* Radius of sun in radians (angle from viewer) */
	float mSunAngularRadius;
	/* Altitude of top of atmosphere */
	float mTopRadius;
	/* Altitude of bottom of atmosphere (ground) */
	float mBotRadius;
	/* Scale height for rayleigh scattering */
	float mScaleHeight_R;
	/* Scale height for mie scattering */
	float mScaleHeight_M;
	/* Rayleigh scattering factor */
	Vector3f mScattering_R;
	/* Mie scattering factor */
	Vector3f mScattering_M;
	/* G-constant for Mie phase function */
	float mMiePhase_G;

	/* Transmittance texture sizes */
	int mTransmittanceTexture_W;
	int mTransmittanceTexture_H;

	/* Scattering texture sizes */
	int mScatteringTexture_R;
	int mScatteringTexture_Mu;
	int mScatteringTexture_MuS;
	int mScatteringTexture_Nu;

private:
	/* Save atmospheric model */
	bool SaveModel(const char* fname);

private:
	/* Precomputed transmittance table */
	FrameBuffer* mTransmittanceBuffer;
	/* Precomputed scattering table */
	FrameBuffer* mScatteringBuffer;
	/* Flag so only initialized once */
	bool mInitialized;

	/* Scene pointer */
	Scene* mScene;

	/* Size of sun computed from angular radius */
	Vector2f mSunSize;
};

///////////////////////////////////////////////////////////////////////////////

#endif