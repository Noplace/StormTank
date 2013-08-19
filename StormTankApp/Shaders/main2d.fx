matrix viewprojection;
matrix world;

float spec_pow[128];

//sampler2D tex1 : register( s0 );
texture tex1;
sampler s0: register(s0) = sampler_state  {
    Texture = (tex1);
};

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
	
    output.pos = mul(  float4(input.pos,0), world );
    output.pos = mul( output.pos, viewprojection );
	
	//output.Pos.x -= output.Pos.z;
    output.uv = input.uv;
	output.color = input.color;
	output.page = input.page;
    return output;
}

PS_INPUT VS_3D( VS_INPUT input )
{
	//PS_INPUT output = (PS_INPUT)0;
	//return input;
    PS_INPUT output = (PS_INPUT)0;
	
    output.pos = mul(  float4(input.pos,0), world );
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
		return input.color * tex2D(s0,input.uv);
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

float pixelWidth = 0.0015625;
float pw = 0.0015625;
float ph = 0.00208333333333333333333333333333;
float PixelKernel[13] =
{
    -6,
    -5,
    -4,
    -3,
    -2,
    -1,
     0,
     1,
     2,
     3,
     4,
     5,
     6,
};
static const float BlurWeights[13] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};


float4 PSBlur( PS_INPUT input) : SV_Target
{
	
	float4 Color = 0;
	float2 Samp = input.uv;
	Color += tex2D(s0, Samp);
	Color += tex2D(s0, float2(input.uv.x-pw,input.uv.y-ph))*0.4;
	Color += tex2D(s0, float2(input.uv.x-pw,input.uv.y+ph))*0.3;
	Color += tex2D(s0, float2(input.uv.x+pw,input.uv.y-ph))*0.2;
	Color += tex2D(s0, float2(input.uv.x+pw,input.uv.y+ph))*0.1;

	Color /= 4;
	Color *= 0.88;
  /*float4 Color = 0;
  float2 Samp = input.uv;

    for (int i = 0; i < 13; i++) {
        Samp.x = input.uv.x + PixelKernel[i] * (pixelWidth/1.5);
        Color += tex2D(s0, Samp.xy) * BlurWeights[i];
    }
   
   for (int i = 0; i < 13; i++) {
        Samp.y = input.uv.y + PixelKernel[i] * pixelWidth;
        Color += tex2D(s0, Samp.xy) * BlurWeights[i];
   }

   Color.rgb *= round(Color.rgb)/1.25;
   Color.rgb *= (Color.r + Color.g + Color.b)/3;
   Color *= 0.70;
   */
    return Color;

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
		/*AlphaBlendEnable = TRUE;
		ColorArg1[0] = TEXTURE;
		ColorArg2[0] = DIFFUSE;  
		ColorOp[0] = MODULATE;
		AlphaArg1[0] = TEXTURE;
		AlphaArg2[0] = DIFFUSE;
		AlphaOp[0] = MODULATE;
		ZEnable = TRUE;
		CullMode = NONE;
		Lighting = false;*/
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

    pass blur
    {
        //SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetVertexShader( CompileShader( vs_3_0, VS() ) );
        SetPixelShader( CompileShader( ps_3_0, PSBlur() ) );
    }
}
