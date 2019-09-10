#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

///////////////////////////////////////////////////////////////////////////////

class FrameBuffer;

class Atmosphere
{
public:
	Atmosphere();
	~Atmosphere();

	/* Do precalculations */
	void Init();

public:
	/* Precomputed transmittance table */
	FrameBuffer* mTransmittanceBuffer;
	/* Precomputed scattering table */
	FrameBuffer* mScatteringBuffer;
	/* Flag so only initialized once */
	bool mInitialized;

	/* Transmittance texture sizes */
	int mTransmittanceTexture_W;
	int mTransmittanceTexture_H;

	/* Scattering texture sizes */
	int mScatteringTexture_R;
	int mScatteringTexture_Mu;
	int mScatteringTexture_MuS;
	int mScatteringTexture_Nu;
};

///////////////////////////////////////////////////////////////////////////////

#endif