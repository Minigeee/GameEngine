#version 330 core

///////////////////////////////////////////////////////////////////////////////

uniform float mTopRadius;
uniform float mBotRadius;

// Sun light at top of atmosphere
uniform vec3 mSolarIrradiance;
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
// Mie phase function G variable
uniform float mMiePhaseG;

uniform int TRANSMITTANCE_TEXTURE_WIDTH = 256;
uniform int TRANSMITTANCE_TEXTURE_HEIGHT = 64;

uniform int SCATTERING_TEXTURE_R_SIZE = 32;
uniform int SCATTERING_TEXTURE_MU_SIZE = 128;
uniform int SCATTERING_TEXTURE_MU_S_SIZE = 32;
uniform int SCATTERING_TEXTURE_NU_SIZE = 8;

// Transmittance precalculation texture
uniform sampler2D mTransmittanceTexture;
// Scattering precalculation texture
uniform sampler3D mScatteringTexture;

const float PI = 3.14159265358979323846;

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

// Return distance to nearest atmosphere boundary
float DistToNearest(float r, float mu, bool intersects_ground)
{
    if (intersects_ground)
        return DistToBot(r, mu);
    else
        return DistToTop(r, mu);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Get density along path (optical depth)
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

    return vec2(GetTexCoordFromUnitRange(x_mu, TRANSMITTANCE_TEXTURE_WIDTH),
                GetTexCoordFromUnitRange(x_r, TRANSMITTANCE_TEXTURE_HEIGHT));
}

// Get altitude and mu from UV
void GetTransmittanceRMu(vec2 uv, out float r, out float mu)
{
    float x_mu = GetUnitRangeFromTexCoord(uv.x, TRANSMITTANCE_TEXTURE_WIDTH);
    float x_r = GetUnitRangeFromTexCoord(uv.y, TRANSMITTANCE_TEXTURE_HEIGHT);

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

// Get transmittance along path of certain float
vec3 Transmittance(float r, float mu, float d)
{
    float r_d = ClampRadius(sqrt(d * d + 2.0f * r * mu * d + r * r));
    float mu_d = ClampCosine((r * mu + d) / r_d);

    if (mu < 0.0f)
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Calculate effect of inscattering of single ray from sun
void CalcIntegrand(
    float r, float mu, float mu_s, float nu, float d,
    out vec3 rayleigh, out vec3 mie)
{
    float r_d = ClampRadius(sqrt(d * d + 2.0f * r * mu * d + r * r));
    float mu_s_d = ClampCosine((r * mu_s + d * nu) / r_d);

    // Transmittance from sun to P to camera
    vec3 transmittance =
        Transmittance(r, mu, d) *
        TransmittanceToSun(r_d, mu_s_d);

    rayleigh = exp(-(r_d - mBotRadius) / mHr) * transmittance;
    mie = exp(-(r_d - mBotRadius) / mHm) * transmittance;
}

// Calculate effect of single scattering along path given angle of sun-zenith and view-sun
void CalcSingleScattering(
    float r, float mu, float mu_s, float nu,
    out vec3 rayleigh, out vec3 mie)
{
    const int SAMPLE_COUNT = 50;

    // float of single integration step
    float rho = SafeSqrt(r * r - mBotRadius * mBotRadius);
    bool intersects_ground = mu < -rho / r;
    float dx = DistToNearest(r, mu, intersects_ground) / SAMPLE_COUNT;

    // Calculate first step
    vec3 rayleigh_i, mie_i;
    CalcIntegrand(
        r, mu, mu_s, nu, 0.0f, rayleigh_i, mie_i
    );

    // Integrate
    for (int i = 1; i <= SAMPLE_COUNT; ++i)
    {
        float x_i = i * dx;

        // Calc scattering effect at current step
        vec3 rayleigh_j, mie_j;
        CalcIntegrand(
            r, mu, mu_s, nu, x_i, rayleigh_j, mie_j
        );

        // TRS
        rayleigh += (rayleigh_i + rayleigh_j) * dx * 0.5f;
        mie += (mie_i + mie_j) * dx * 0.5f;

        rayleigh_i = rayleigh_j;
        mie_i = mie_j;
    }

    rayleigh = rayleigh * mSolarIrradiance * mBr;
    mie = mie * mSolarIrradiance * mBm;
}

///////////////////////////////////////////////////////////////////////////////

vec4 GetScatteringUV4(
    float r, float mu, float mu_s, float nu)
{
    // Distance to top atmosphere boundary for a horizontal ray at ground level.
    float H = sqrt(mTopRadius * mTopRadius -
        mBotRadius * mBotRadius);
    // Distance to the horizon.
    float rho =
        SafeSqrt(r * r - mBotRadius * mBotRadius);
    float u_r = GetTexCoordFromUnitRange(rho / H, SCATTERING_TEXTURE_R_SIZE);

    // Mu
    float u_mu = mu * 0.5f + 0.5f;

    float d = DistToTop(mBotRadius, mu_s);
    float d_min = mTopRadius - mBotRadius;
    float d_max = H;
    float a = (d - d_min) / (d_max - d_min);
    float A =
        -2.0 * -0.2 * mBotRadius / (d_max - d_min);
    float u_mu_s = GetTexCoordFromUnitRange(
        max(1.0 - a / A, 0.0) / (1.0 + a), SCATTERING_TEXTURE_MU_S_SIZE);

    float u_nu = (nu + 1.0) / 2.0;
    return vec4(u_nu, u_mu_s, u_mu, u_r);
}

void GetScatteringRMuMuSNu(
    vec4 uvwz, out float r, out float mu, out float mu_s, out float nu)
{
    // Distance to top atmosphere boundary for a horizontal ray at ground level.
    float H = sqrt(mTopRadius * mTopRadius -
        mBotRadius * mBotRadius);
    // Distance to the horizon.
    float rho =
        H * GetUnitRangeFromTexCoord(uvwz.w, SCATTERING_TEXTURE_R_SIZE);
    r = sqrt(rho * rho + mBotRadius * mBotRadius);

    // Mu
    mu = uvwz.z * 2.0f - 1.0f;

    float x_mu_s =
        GetUnitRangeFromTexCoord(uvwz.y, SCATTERING_TEXTURE_MU_S_SIZE);
    float d_min = mTopRadius - mBotRadius;
    float d_max = H;
    float A =
        -2.0 * -0.2 * mBotRadius / (d_max - d_min);
    float a = (A - x_mu_s * A) / (1.0 + x_mu_s * A);
    float d = d_min + min(a, A) * (d_max - d_min);
    mu_s = d == 0.0 ? float(1.0) :
        ClampCosine((H * H - d * d) / (2.0 * mBotRadius * d));

    nu = ClampCosine(uvwz.x * 2.0 - 1.0);
}

// Get values from UV3
void GetScatteringRMuMuSNuFromUV3(
    vec3 uv3, out float r, out float mu, out float mu_s, out float nu)
{
    float x_scaled = uv3.x * SCATTERING_TEXTURE_NU_SIZE;
    float u_nu = floor(x_scaled);
    float u_mu_s = x_scaled - u_nu;
    u_nu /= SCATTERING_TEXTURE_NU_SIZE;

    // Get values
    vec4 uv4 = vec4(u_nu, u_mu_s, uv3.y, uv3.z);
    GetScatteringRMuMuSNu(uv4, r, mu, mu_s, nu);

    // Clamp nu
    nu = clamp(nu, mu * mu_s - sqrt((1.0f - mu * mu) * (1.0f - mu_s * mu_s)),
        mu * mu_s + sqrt((1.0f - mu * mu) * (1.0f - mu_s * mu_s)));
}

// Calculate single scattering from uv
void CalcScatteringUV3(vec3 uv3, out vec3 rayleigh, out vec3 mie)
{
    float r, mu, mu_s, nu;

    GetScatteringRMuMuSNuFromUV3(uv3, r, mu, mu_s, nu);
    CalcSingleScattering(r, mu, mu_s, nu, rayleigh, mie);
    // CalcIntegrand(r, mu, mu_s, nu, 50.0f, rayleigh, mie);
}

///////////////////////////////////////////////////////////////////////////////

// Get Mie scattering from combined scattering
vec3 GetMieFromCombined(vec4 scattering)
{
    if (scattering.r == 0.0f)
        return vec3(0.0f);

    return scattering.rgb * scattering.a / scattering.r *
        (mBr.r / mBm.r) * (mBm / mBr);
}

void Scattering(
    float r, float mu, float mu_s, float nu,
    out vec3 rayleigh, out vec3 mie)
{
    vec4 uv4 = GetScatteringUV4(r, mu, mu_s, nu);
    float x_scaled = uv4.x * SCATTERING_TEXTURE_NU_SIZE;
    float u_x = floor(x_scaled);
    float factor = x_scaled - u_x;

    vec3 uv1 = vec3(
        (u_x + uv4.y) / SCATTERING_TEXTURE_NU_SIZE,
        uv4.z, uv4.w
    );
    vec3 uv2 = vec3(
        (u_x + uv4.y + 1.0f) / SCATTERING_TEXTURE_NU_SIZE,
        uv4.z, uv4.w
    );

    vec4 combined =
        texture(mScatteringTexture, uv1) * (1.0f - factor) +
        texture(mScatteringTexture, uv2) * factor;

    rayleigh = combined.rgb;
    mie = GetMieFromCombined(combined);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float RayleighPhaseFunction(float nu)
{
  float k = 3.0f / (16.0f * PI);
  return k * (1.0f + nu * nu);
}

float MiePhaseFunction(float g, float nu)
{
    float k = 3.0f / (8.0f * PI) * (1.0f - g * g) / (2.0f + g * g);
    return k * (1.0f + nu * nu) / pow(1.0f + g * g - 2.0f * g * nu, 1.5f);
}

///////////////////////////////////////////////////////////////////////////////

// Returns color of sky in direction and transmittance of path
vec3 GetSkyRadiance(float r, vec3 dir, vec3 sun_dir, out vec3 transmittance)
{
    vec3 pos = vec3(0, r, 0);
    float mu = dot(pos, dir) / r;
    float mu_s = dot(pos, sun_dir) / r;
    float nu = dot(dir, sun_dir);

    // Calc transmittance (Since this is for sky, doesn't calculate transmittance for ground intersections)
    transmittance = TransmittanceToTop(r, mu);

    // Calc scattering
    vec3 scattering, mie;
    Scattering(r, mu, mu_s, nu, scattering, mie);

    return
        scattering * RayleighPhaseFunction(nu) +
        mie * MiePhaseFunction(mMiePhaseG, nu);
}

///////////////////////////////////////////////////////////////////////////////