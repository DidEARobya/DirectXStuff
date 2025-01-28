cbuffer CBuffer
{
    float4 face_colours[8];
};

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
    return face_colours[(tid / 2) % 8];
}