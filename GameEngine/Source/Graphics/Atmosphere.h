#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include <Math/Vector3.h>

#include <Graphics/RenderPass.h>

///////////////////////////////////////////////////////////////////////////////

class Scene;

class Atmosphere : public LightingPass
{
public:
	Atmosphere(Scene* scene);
	~Atmosphere();

	/* Do precalculations */
	void Init() override;

	/* Render as lighting effect */
	void RenderSetup(FrameBuffer* gbuffer) override;

	/* Set uniforms needed for atmosphere shaders */
	void SetUniforms(Shader* shader);

	/* Get transmittance buffer */
	FrameBuffer* GetTransmittanceBuffer() const;
	/* Get scattering buffer */
	FrameBuffer* GetScatteringBuffer() const;
	/* Get irradiance buffer */
	FrameBuffer* GetIrradianceBuffer() const;
	/* Bind transmittance texture and set uniform */
	void BindTransmittance(Shader* shader, Uint32 slot);
	/* Bind scattering texture and set uniform */
	void BindScattering(Shader* shader, Uint32 slot);
	/* Bind irradiance texture and set uniform */
	void BindIrradiance(Shader* shader, Uint32 slot);

public:
	/* Intensity of sunlight */
	float mSolarIntensity;
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

	/* Base altitude (km) */
	float mBaseHeight;
	/* Distance multiplier */
	float mDistScale;

	/* Transmittance texture sizes */
	int mTransmittanceTexture_W;
	int mTransmittanceTexture_H;

	/* Scattering texture sizes */
	int mScatteringTexture_R;
	int mScatteringTexture_Mu;
	int mScatteringTexture_MuS;
	int mScatteringTexture_Nu;

	/* Irradiance texture sizes */
	int mIrradianceTexture_W;
	int mIrradianceTexture_H;

private:
	/* Save atmospheric model */
	bool SaveModel(const char* fname);

private:
	/* Precomputed transmittance table */
	FrameBuffer* mTransmittanceBuffer;
	/* Precomputed scattering table */
	FrameBuffer* mScatteringBuffer;
	/* Precomputed irradiance table */
	FrameBuffer* mIrradianceBuffer;
	/* Flag so only initialized once */
	bool mInitialized;

	/* Size of sun computed from angular radius */
	Vector2f mSunSize;
};

///////////////////////////////////////////////////////////////////////////////

#endif