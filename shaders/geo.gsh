#version 410 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 100) out;
uniform int processGeo = 0;

in PVA
{
	vec3 ecPosition;
	vec3 ecUnitNormal;
	vec2 textureCoords; // (s,t)
} pvaIn[];

out PVA
{
	vec3 ecPosition;
	vec3 ecUnitNormal;
	vec2 textureCoords; // (s,t)
} pvaOut;


void outputAttributesFor(int i)
{
	pvaOut.ecPosition = pvaIn[i].ecPosition;
	pvaOut.ecUnitNormal = pvaIn[i].ecUnitNormal;
	pvaOut.textureCoords = pvaIn[i].textureCoords;
}

void process(vec4 v0, vec4 v1, vec4 v2)
{
	vec4 center = (v0 + v1 + v2) / 3.0;
	gl_Position = v1;
	outputAttributesFor(0);
	EmitVertex();
	gl_Position = v2;
	outputAttributesFor(1);
	EmitVertex();
	gl_Position = v0;
	outputAttributesFor(2);
	EmitVertex();

	EndPrimitive();
}

void passShape()
{
  gl_Position = gl_in[0].gl_Position;
  outputAttributesFor(0);
  EmitVertex();
  gl_Position = gl_in[1].gl_Position;
  outputAttributesFor(1);
  EmitVertex();
  gl_Position = gl_in[2].gl_Position;
  outputAttributesFor(2);
  EmitVertex();

  EndPrimitive();
}


void main()
{
  if(processGeo == 1)
process(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
  else
    passShape();
}
