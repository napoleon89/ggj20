#version 450 core
in vec4 v_color;




layout(std140, binding = 0) uniform PerScene {
	mat4 projection;
};

mat4 createTranslation(vec2 pos) {
	mat4 result = mat4(
		1.0, 0.0, 0.0, pos.x,
		0.0, 1.0, 0.0, pos.y,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);

	return result;
}

mat4 createRotationZ(float angle) {
	mat4 result = mat4(
		cos(angle), -sin(angle), 0.0, 0.0,
		sin(angle), cos(angle), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	return result;
}

mat4 createScale(vec2 scale) {
	mat4 result = mat4(
		scale.x, 0.0, 0.0, 0.0,
		0.0, scale.y, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	return result;
}



 out vec4 out_color; 
void main() {

	out_color = v_color;
}