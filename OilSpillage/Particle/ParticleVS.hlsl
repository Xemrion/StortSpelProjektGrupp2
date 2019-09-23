struct VS_IN
{
	float4 Pos : SV_POSITION;// pos=xyz, size=w
	float4 color: COLOR;
	float4 config: CONFIG;
	float4 speed: SPEED;

};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 color: COLOR;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = input.Pos;
	output.color = input.color;

	return output;
}