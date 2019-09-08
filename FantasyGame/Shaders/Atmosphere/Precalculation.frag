#version 330 core

///////////////////////////////////////////////////////////////////////////////

uniform float mTopRadius;
uniform float mBotRadius;

// Rayleigh density
uniform float mHr;
// Mie density
uniform float mHm;
// Rayleigh extinction factor
uniform vec3 mBr;
// Mie extinction factor
uniform vec3 mBm;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float ClampCosine(float mu)
{
  return clamp(mu, -1.0f, 1.0f);
}

float ClampDistance(float d)
{
  return max(d, 0.0f);
}

float SafeSqrt(float a)
{
  return sqrt(max(a, 0.0f));
}

float GetTextureCoordFromUnitRange(float x, int texture_size)
{
  return 0.5 / texture_size + x * (1.0 - 1.0 / texture_size);
}

float GetUnitRangeFromTextureCoord(float u, int texture_size)
{
  return (u - 0.5 / texture_size) / (1.0 - 1.0 / texture_size);
}

///////////////////////////////////////////////////////////////////////////////

// Return distance to top of atmosphere
float DistToTop(float r, float mu)
{
    float discriminant = r * r * (mu * mu - 1.0f) +
        mTopRadius * mTopRadius;
    return ClampDistance(-r * mu + SafeSqrt(discriminant));
}

// Get density along path
float GetDensityAlongPath(float H, float r, float mu)
{
    const int NUM_SAMPLES = 500;

    // Integration step float
    float dx = DistToTop(r, mu) / NUM_SAMPLES;

    // Density value
    float density = 0.0f;
    // Initial density
    float y_j = exp(-(r - mBotRadius) / H);

    for (int i = 1; i < NUM_SAMPLES; ++i)
    {
        // Position along path
        float x_i = i * dx;
        // Altitude along path
        float r_i = sqrt(x_i * x_i + 2.0f * r * mu * x_i + r * r);

        // Density at point on path
        float y_i = exp(-(r_i - mBotRadius) / H);
        // Trapezoidal Reimann sum
        density += (y_j + y_i) * 0.5f * dx;

        y_j = y_i;
    }

    return density;
}

// Calculate transmittance along path
vec3 CalcTransmittance(float r, float mu)
{
    return
        exp(-(
            mBr * GetDensityAlongPath(mHr, r, mu) +
            mBm * GetDensityAlongPath(mHm, r, mu)
        ));
}

vec2 GetRMuFromTransmittanceTextureUv(vec2 uv)
{
  float x_r = uv.x;
  float x_mu = uv.y;
  // Distance to top atmosphere boundary for a horizontal ray at ground level.
  float H = sqrt(mTopRadius * mTopRadius - mBotRadius * mBotRadius);
  // Distance to the horizon, from which we can compute r:
  float rho = H * x_r;
  float r = sqrt(rho * rho + mBotRadius * mBotRadius);
  // Distance to the top atmosphere boundary for the ray (r,mu), and its minimum
  // and maximum values over all mu - obtained for (r,1) and (r,mu_horizon) -
  // from which we can recover mu:
  float d_min = mTopRadius - r;
  float d_max = rho + H;
  float d = d_min + x_mu * (d_max - d_min);
  float mu = d == 0.0f ? 1.0f : (H * H - rho * rho - d * d) / (2.0f * r * d);
  mu = ClampCosine(mu);
  
  return vec2(r, mu);
}

vec3 TransmittanceFromTexCoord(vec2 tex_coord)
{
    vec2 rmu = GetRMuFromTransmittanceTextureUv(tex_coord);
    return CalcTransmittance(rmu.x, rmu.y);
}

///////////////////////////////////////////////////////////////////////////////