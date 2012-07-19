matrix viewprojection;
matrix world;

float spec_pow[128];

sampler2D tex1 : register( s0 );
SamplerState sampler1
{

Filter = MIN_MAG_MIP_LINEAR;
AddressU = Clamp;
AddressV = Clamp;
AddressW = Wrap;
//ComparisonFunc = NEVER;
//MinLOD = 0;
//MaxLOD = MAX;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	uint page : BLENDINDICES0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	uint page : BLENDINDICES0;
};

PS_INPUT VS( VS_INPUT input )
{
	//PS_INPUT output = (PS_INPUT)0;
	//return input;
    PS_INPUT output = (PS_INPUT)0;
	
    output.pos = mul(  float4(input.pos,1), world );
    output.pos = mul( output.pos, viewprojection );
	
	//output.Pos.x -= output.Pos.z;
    output.uv = input.uv;
	output.color = input.color;
	output.page = input.page;
    return output;
}


PS_INPUT SpectrumVS( VS_INPUT input )
{
	//PS_INPUT output = (PS_INPUT)0;
	//return input;
    PS_INPUT output = (PS_INPUT)0;
	input.pos.y = 50-spec_pow[input.page]*50;//50-(spec_pow[input.page]*50);
    output.pos = mul(  float4(input.pos,1), world );
    output.pos = mul( output.pos, viewprojection );
	
	//output.Pos.x -= output.Pos.z;
    output.uv = input.uv;
	output.color = input.color;
	output.page = input.page;
    return output;
}


PS_INPUT OscilloscopeVS( VS_INPUT input )
{
	//PS_INPUT output = (PS_INPUT)0;
	//return input;
    PS_INPUT output = (PS_INPUT)0;
	input.pos.y = 50-spec_pow[input.page]*50;//50-(spec_pow[input.page]*50);
    output.pos = mul(  float4(input.pos,1), world );
    output.pos = mul( output.pos, viewprojection );
	
	//output.Pos.x -= output.Pos.z;
    output.uv = input.uv;
	output.color = input.color;
	output.page = input.page;
    return output;
}


float4 PS( PS_INPUT input,uniform bool tex_enabled) : SV_Target
{
	if (tex_enabled == true) {
		return input.color * tex2D(tex1,input.uv);
	}
	else
	{
		return input.color;// * tex1.Sample(sampler1, input.uv);// * tex2D(tex,input.uv);
	}
	/*float4 Out    = (float4) 0; 

    Out        = input.color * tex1.Sample(samLinear2, input.uv);
    float fIntensity = Out.r*0.30f + Out.g*0.59f + Out.b*0.11f;
    Out        = float4(fIntensity,fIntensity,fIntensity,1.f);

    return Out;*/

}



technique t1
{
    pass p1
    {
        //SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetVertexShader( CompileShader( vs_3_0, VS() ) );
        SetPixelShader( CompileShader( ps_3_0, PS(true) ) );
    }

    pass p2
    {
        //SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetVertexShader( CompileShader( vs_3_0, VS() ) );
        SetPixelShader( CompileShader( ps_3_0, PS(false) ) );
    }

    pass spec
    {
        //SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetVertexShader( CompileShader( vs_3_0, SpectrumVS() ) );
        SetPixelShader( CompileShader( ps_3_0, PS(false) ) );
    }

    pass osc
    {
        //SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetVertexShader( CompileShader( vs_3_0, OscilloscopeVS() ) );
        SetPixelShader( CompileShader( ps_3_0, PS(false) ) );
    }
}
