#include <Graphics/Material.h>
#include <Graphics/Shader.h>

#include <Graphics/Texture.h>

///////////////////////////////////////////////////////////////////////////////

Material::Material() :
	mDiffuse		(1.0f),
	mSpecular		(0.5f),
	mSpecFactor		(16.0f),

	mShader			(0)
{

}

///////////////////////////////////////////////////////////////////////////////

void Material::Use()
{
	mShader->SetUniform("material.diffuse", mDiffuse);
	mShader->SetUniform("material.specular", mSpecular);
	mShader->SetUniform("material.specFactor", mSpecFactor);

	// Apply all textures
	for (Uint32 i = 0; i < mTextures.Size(); ++i)
	{
		mTextures[i].mTexture->Bind(i);
		mShader->SetUniform(mTextures[i].mUniform, (int)i);
	}
}

///////////////////////////////////////////////////////////////////////////////

void Material::AddTexture(Texture* texture, const char* uniform)
{
	if (!mTextures.Capacity())
		mTextures.Resize(4);

	mTextures.Push(MaterialTexture{ texture, uniform });
}

///////////////////////////////////////////////////////////////////////////////