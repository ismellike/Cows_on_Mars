#version 410 core

// phong.fsh - a fragment shader that implements a Phong Lighting model.

in PVA
{
	vec3 ecPosition;
	vec3 ecUnitNormal;
	vec2 textureCoords;
} pvaIn;

// For lighting model:
uniform mat4 ec_lds; // so projection type and hence vHat can be determined

// Phong material properties (RGB reflectances);
uniform vec3 ka = vec3(0.8, 0.0, 0.0); // default: darkish red
uniform vec3 kd = vec3(0.8, 0.0, 0.0); // default: darkish red
uniform vec3 ks = vec3(.8, 0, 0);
uniform float m = 1;

const int MAX_NUM_LIGHTS = 4;
uniform int actualNumLights = 0;
uniform vec3 lightStrength[MAX_NUM_LIGHTS];
uniform vec3 globalAmbient;
uniform vec4 ecLightPosition[MAX_NUM_LIGHTS];


//PROJECT 4 VARIABLES
uniform float alpha = 1;
uniform int sceneHasTranslucentObjects = 0;
uniform int drawingOpaqueObjects = 1;
uniform sampler2D textureMap;
uniform int usingTexture = 0;

out vec4 fragmentColor;

float attenuation(int i, vec3 Q)
{
	float k0 = 25, k1 = 0, k2 = .002;
	float d = distance(ecLightPosition[i].xyz, Q);
	return k0/(k1 * d + k2 * pow(d, 2));
}

vec4 evaluateLightingModel()
{
	vec3 vHat;
	vec3 liHat;
	vec3 riHat;
	vec3 nHat = pvaIn.ecUnitNormal;
	vec3 Q = pvaIn.ecPosition;
	vec3 ambient = ka * globalAmbient;
	vec3 notAmbient = vec3(0,0,0);

	if(ec_lds[3][3] == 0) //perspective
	{
		vHat = normalize(-Q);
	}
	else //otherwise
	{
		float dx = -ec_lds[2][0]/ec_lds[0][0];
		float dy = -ec_lds[2][1]/ec_lds[1][1];
		vHat = normalize(vec3(dx, dy, 1));
	}

	if(dot(vHat, nHat) < 0) //correct unit normal
		nHat *= -1;

	for(int i = 0; i < actualNumLights; i++)
	{
		vec3 diffuse = vec3(0,0,0);
		vec3 specular = vec3(0,0,0);
		float fiQ = 1;

		if(ecLightPosition[i].w == 0)
		{
			liHat = normalize(ecLightPosition[i].xyz);
		}
		else
		{
			liHat = normalize(ecLightPosition[i].xyz - Q);
			fiQ = attenuation(i, Q);
		}

		if(dot(liHat, nHat) > 0)
		{
				diffuse = kd * dot(liHat, nHat);

				riHat = normalize(reflect(-liHat, nHat));
				if(dot(riHat, vHat) > 0)
					specular = ks * pow(dot(riHat, vHat), m);
		}

		notAmbient += fiQ * lightStrength[i] * (diffuse + specular);
	}

	return vec4(ambient + notAmbient, alpha);
}

void main ()
{
	vec4 color = evaluateLightingModel();
	vec4 textureColor = vec4(1,1,1,1);

	if(usingTexture == 1)
		textureColor = texture(textureMap, pvaIn.textureCoords);
	color = textureColor * color;

		if(sceneHasTranslucentObjects == 1)
		{
			if (drawingOpaqueObjects == 1)
			{
		      if (color.a < 1.0)
					{
		          discard;
					}
		      else
					{
		          fragmentColor = color;
					}
			}
			else if (color.a < 1)
			{
			    fragmentColor = color;
			}
			else
			{
		   discard;
			 }
		 }
		 else
		 {
		 		fragmentColor = color;
				}
}
