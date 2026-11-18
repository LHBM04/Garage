cbuffer CameraConstants : register(b0, space1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float3             cameraLightDirection;
    float              cameraLightIntensity;
    float4             cameraLightColor;
    float4             cameraAmbientLightColor;
};

cbuffer SkyboxConstants : register(b2, space3)
{
    float4 color;
};

TextureCube<float4> skyboxTexture : register(t0, space2);
SamplerState        linearClamp   : register(s0, space2);

struct VSOutput
{
    float4 position  : SV_Position;
    float3 direction : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    const float2 ndc = float2((vertexID == 1u) ? 3.0f : -1.0f, (vertexID == 2u) ? -3.0f : 1.0f);
    float3 viewDirection = float3(0.0f, 0.0f, 1.0f);
    if (projection[2][3] != 0.0f)
    {
        viewDirection.xy = ndc / float2(projection[0][0], projection[1][1]);
    }

    VSOutput output;
    output.position  = float4(ndc, 1.0f, 1.0f);
    output.direction = mul(viewDirection, transpose((float3x3)view));
    return output;
}

float4 PSMain(VSOutput input) : SV_Target0
{
    return float4(skyboxTexture.Sample(linearClamp, input.direction).rgb, 1.0f) * color;
}
