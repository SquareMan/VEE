struct VtxPushConstants {
    float4x4 m;
    float4x4 proj;
}

[vk::push_constant]
uniform VtxPushConstants mats;

struct VtxInput {
    float2 position;
    float3 color;
    float2 uv;
}

struct VtxOut {
    float4 position : SV_Position;
    float4 color;
    float2 uv;
}

[shader("vertex")]
VtxOut vertexMain(VtxInput input) {
    VtxOut output;
    let pos = float4(input.position.xy, 0, 1);

    output.position = mul(mul(mats.proj, mats.m), pos);
    output.color = float4(input.color.xyz, 1);
    output.uv = input.uv;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VtxOut input, uniform Sampler2D tex) : SV_Target {
    return tex.Sample(input.uv);
}