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
    float4 color        : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    float4 viewPosition  = mul(worldPosition, view);
    output.clipPosition  = mul(viewPosition, projection);
    output.color         = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_Target0
{
    return float4(saturate(input.color.rgb * baseColor.rgb + emissiveColor.rgb), input.color.a * baseColor.a);
}
