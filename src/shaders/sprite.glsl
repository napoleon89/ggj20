VertIn {
	vec2 in_pos;
	vec2 in_tex;
}

Vary {
	vec2 frag_uv;
}

layout(std140, binding = 0) uniform PerScene {
	mat4 projection;
};

layout(std140, binding = 1) uniform PerObject {
	mat4 model;
	vec4 color;
	vec2 uv_offset;
	vec2 uv_scale;
};

void vert() {
	frag_uv = in_tex * uv_scale + uv_offset;
	gl_Position = vec4(in_pos, 0.0f, 1.0f) * model * projection;
} 

uniform sampler2D u_texture; 

void frag() {
	vec4 texel = texture(u_texture, frag_uv);
	if(texel.a == 0.0) discard;
	vec4 diffuse = color * texel;
	out_color = diffuse;
}
