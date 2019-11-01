#ifndef SKY_BOX_H
#define SKY_BOX_H

#include <Math/Vector3.h>
#include <Math/Matrix4.h>

///////////////////////////////////////////////////////////////////////////////

class Shader;
class VertexArray;
class VertexBuffer;

class Skybox
{
public:
	/* Skybox color options */
	struct SkyboxColor
	{
		SkyboxColor() = default;
		SkyboxColor(const Vector3f& color, float h);

		/* The color */
		Vector3f mColor;
		/* Height of color (1.0f highest, 0.0f lowest) */
		float mHeight;
	};

	/* Gradient properties */
	struct SkyboxGrad
	{
		SkyboxGrad(float steepness = 1.0f, float mid = 0.5f);

		/* How sudden the change from one color to the other is */
		float mSteepness;
		/* Location where there is an equal mix between both colors */
		float mMidpoint;
	};

public:
	Skybox();
	~Skybox();

	/* Set skybox color (Make sure 0 has lowest height, and 2 has highest height) */
	void SetColor(const SkyboxColor& color, Uint32 index);
	/* Set gradient properties */
	void SetGradient(const SkyboxGrad& grad, Uint32 index);

	/* Render skybox */
	void Render(const Matrix4f& projView);

private:
	/* Initialize skybox */
	void Init();

private:
	/* Shader used to render skybox */
	Shader* mShader;
	/* Vertex array to render skybox */
	VertexArray* mVertexArray;
	/* Vertex buffer to store data */
	VertexBuffer* mVertexBuffer;

	/* Skybox colors */
	SkyboxColor mColors[3];
	/* Skybox gradients */
	SkyboxGrad mGradients[2];
};

///////////////////////////////////////////////////////////////////////////////

#endif