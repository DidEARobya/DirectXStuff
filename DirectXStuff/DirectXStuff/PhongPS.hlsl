cbuffer LightCBuffer
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColour;
    float diffuseIntensity;
    float attenuationConst;
    float attenuationLin;
    float attenuationQuad;
};
cbuffer ObjectCBuffer
{
    float3 materialColour;
    float specularIntensity;
    float specularPower;
};
float4 main(float3 cameraPos : Position, float3 n : Normal) : SV_Target
{
	//Fragment to light vector data
    const float3 vToL = lightPos - cameraPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	//Diffuse attenuation
    const float attenuation = 1.0f / (attenuationConst + attenuationLin * distToL + attenuationQuad * (distToL * distToL));
	//Diffuse intensity
    const float3 diffuse = diffuseColour * diffuseIntensity * attenuation * max(0.0f, dot(dirToL, n));

    //Reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    //Calculate specular intensity based on angle between viewing vector and reflection vector
    const float3 specular = attenuation * (diffuseColour * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(cameraPos))), specularPower);
    
    return float4(saturate((diffuse + ambient + specular) * materialColour), 1.0f);
}