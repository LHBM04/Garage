cbuffer CameraConstants : register(b0, space1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float3             cameraLightDirection;
    float              cameraLightIntensity;
    float4             cameraLightColor;
    float4             cameraAmbientLightColor;
};

cbuffer ObjectConstants : register(b1, space1)
{
    row_major float4x4 world;
};

cbuffer FragmentCameraConstants : register(b0, space3)
{
    row_major float4x4 fragmentView;
    row_major float4x4 fragmentProjection;
    float3             lightDirection;
    float              lightIntensity;
    float4             lightColor;
    float4             ambientLightColor;
};

cbuffer MaterialConstants : register(b2, space3)
{
    float4 baseColor;
    float4 emissiveColor;
    float  metallic;
    float  roughness;
    uint   textureMask;
    float  padding;
    float4 specularColor;
};

struct VSInput
{
    float3 position : TEXCOORD0;
    float3 normal   : TEXCOORD1;
    float4 color    : TEXCOORD2;
    float2 texCoord : TEXCOORD3;
};

struct PSInput
{
    float4 clipPosition : SV_Position;
    float3 worldNormal  : TEXCOORD0;
    float4 color        : TEXCOORD1;
    float3 localNormal  : TEXCOORD2;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    float4 viewPosition  = mul(worldPosition, view);
    output.clipPosition  = mul(viewPosition, projection);
    output.worldNormal   = mul(input.normal, (row_major float3x3)world);
    output.color         = input.color;
    output.localNormal   = input.normal;
    return output;
}

float4 PSMain(PSInput input) : SV_Target0
{
    float3 normal    = normalize(input.worldNormal);
    float3 toLight   = normalize(-lightDirection);
    float  diffuse   = saturate(dot(normal, toLight));
    float3 faceColor = saturate(normalize(input.localNormal) * 0.45f + 0.55f);
    float3 albedo    = faceColor * input.color.rgb * baseColor.rgb;
    float3 litColor  = albedo * (ambientLightColor.rgb + lightColor.rgb * (lightIntensity * diffuse));
    return float4(saturate(litColor + emissiveColor.rgb), input.color.a * baseColor.a);
}