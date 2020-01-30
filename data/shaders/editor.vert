#version 450 core
layout(location = 0) in vec2 in_vert_pos;
layout(location = 1) in vec2 in_position;
layout(location = 2) in vec2 in_scale;
layout(location = 3) in float in_angle;
layout(location = 4) in vec4 in_color;

out vec4 v_color;




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



 void main() {

	mat4 model = createScale(in_scale) * createRotationZ(in_angle) * createTranslation(in_position);
	v_color = in_color;
	gl_Position = vec4(in_vert_pos, 0.0f, 1.0f) * model * projection;
}