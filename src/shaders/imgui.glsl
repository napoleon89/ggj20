VertIn {
	vec2 in_pos;
	vec2 in_uv;
	vec4 in_color;
}

Vary {
	vec4 frag_color;
	vec2 frag_uv;
}

layout(std140, binding = 0) uniform PerScene {
	mat4 projection;
};


void vert() {
	frag_color = in_color;
	frag_uv = in_uv; 
	gl_Position = projection * vec4(in_pos, 0.0f, 1.0f);
}

uniform sampler2D u_texture;

void frag() {
	out_color = frag_color * texture(u_texture, frag_uv); 
	// out_color = frag_color;	
}