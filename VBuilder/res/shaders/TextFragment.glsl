#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec3 u_TextColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, v_TexCoord).r);
	o_Color = vec4(u_TextColor, 1.0) * sampled;
}