#version 330 core

///////////////////////////////////////////////////////////////////////////////

uniform float mTopRadius;
uniform float mBotRadius;
// Radius of sun in radians (Based on view point)
uniform float mSunAngularRadius;

// Rayleigh density
uniform float mHr;
// Mie density
uniform float mHm;
// Rayleigh extinction factor
uniform vec3 mBr;
// Mie extinction factor
uniform vec3 mBm;

// Transmitance precalculation texture
uniform sampler2D mTransmittanceTexture;

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

float ClampRadius(float r)
{
    return clamp(r, mBotRadius, mTopRadius);
}

float SafeSqrt(float a)
{
  return sqrt(max(a, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////

float GetTexCoordFromUnitRange(float x, int texture_size)
{
  return 0.5 / texture_size + x * (1.0 - 1.0 / texture_size);
}

float GetUnitRangeFromTexCoord(float u, int texture_size)
{
  return (u - 0.5 / texture_size) / (1.0 - 1.0 / texture_size);
}

// Return distance to top of atmosphere
float DistToTop(float r, float mu)
{
    float discriminant = r * r * (mu * mu - 1.0f) + mTopRadius * mTopRadius;
    return ClampDistance(-r * mu + SafeSqrt(discriminant));
}

// Return distance to bottom of atmosphere
float DistToBot(float r, float mu)
{
    float discriminant = r * r * (mu * mu - 1.0) + mBotRadius * mBotRadius;
    return ClampDistance(-r * mu - SafeSqrt(discriminant));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
vec3 CalcTransmittanceRMu(float r, float mu)
{
    return
        exp(-(
            mBr * GetDensityAlongPath(mHr, r, mu) +
            mBm * GetDensityAlongPath(mHm, r, mu)
        ));
}

vec2 GetTransmittanceUV(float r, float mu)
{
    // Distance to top atmosphere boundary for a horizontal ray at ground level.
    float H = sqrt(mTopRadius * mTopRadius - mBotRadius * mBotRadius);
    // Distance to the horizon.
    float rho = SafeSqrt(r * r - mBotRadius * mBotRadius);

    // Distance to the top atmosphere boundary for the ray (r,mu), and its minimum
    // and maximum values over all mu - obtained for (r,1) and (r,mu_horizon).
    float d = DistToTop(r, mu);
    float d_min = mTopRadius - r;
    float d_max = rho + H;
    float x_mu = (d - d_min) / (d_max - d_min);
    float x_r = rho / H;

    return vec2(GetTexCoordFromUnitRange(x_mu, 1024),
                GetTexCoordFromUnitRange(x_r, 1024));
}

// Get altitude and mu from UV
void GetTransmittanceRMu(vec2 uv, out float r, out float mu)
{
  float x_r = GetUnitRangeFromTexCoord(uv.x, 1024);

  float x_mu = GetUnitRangeFromTexCoord(uv.y, 1024);
  // Distance to top atmosphere boundary for a horizontal ray at ground level.
  float H = sqrt(mTopRadius * mTopRadius - mBotRadius * mBotRadius);
  // Distance to the horizon, from which we can compute r:
  float rho = H * x_r;
  r = sqrt(rho * rho + mBotRadius * mBotRadius);

  // Distance to the top atmosphere boundary for the ray (r,mu), and its minimum
  // and maximum values over all mu - obtained for (r,1) and (r,mu_horizon) -
  // from which we can recover mu:
  float d_min = mTopRadius - r;
  float d_max = rho + H;
  float d = d_min + x_mu * (d_max - d_min);
  mu = d == 0.0f ? 1.0f : (H * H - rho * rho - d * d) / (2.0f * r * d);
  mu = ClampCosine(mu);
}

// Calc transmittance from UV
vec3 CalcTransmittanceUV(vec2 tex_coord)
{
    float r, mu;
    GetTransmittanceRMu(tex_coord, r, mu);
    return CalcTransmittanceRMu(r, mu);
}

///////////////////////////////////////////////////////////////////////////////

// Get transmittance to top of atmosphere (assumes that ray does not intersect ground)
vec3 TransmittanceToTop(float r, float mu)
{
    vec2 uv = GetTransmittanceUV(r, mu);
    return texture(mTransmittanceTexture, uv).rgb;
}

// Get transmittance along path of certain length
vec3 Transmittance(float r, float mu, float d, bool intersects_ground)
{
    float r_d = ClampRadius(sqrt(d * d + 2.0f * r * mu * d + r * r));
    float mu_d = ClampCosine((r * mu + d) / r_d);

    if (intersects_ground)
    {
        return min(
            TransmittanceToTop(r_d, -mu_d) /
            TransmittanceToTop(r, -mu),
            vec3(1.0f)
        );
    }
    else
    {
        return min(
            TransmittanceToTop(r, mu) /
            TransmittanceToTop(r_d, mu_d),
            vec3(1.0f)
        );
    }
}

// Get tranmittance to sun given cosine of sun-zenith angle
vec3 TransmittanceToSun(float r, float mu_s)
{
    // Angles to horizon
    float sin_theta_h = mBotRadius / r;
    float cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h, 0.0));

    return TransmittanceToTop(r, mu_s) *
        smoothstep(
            -sin_theta_h * mSunAngularRadius,
            sin_theta_h * mSunAngularRadius,
            mu_s - cos_theta_h
        );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////