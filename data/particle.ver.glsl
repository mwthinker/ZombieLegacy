#version 100

precision mediump float;

uniform mat4 uMat;

attribute vec2 aPos;
attribute vec2 aTex;
attribute float aAngle;

varying vec2 vTex;

void main() {
	// Column-major order.
	mat2 rotation = mat2(cos(aAngle), sin(aAngle),
						-sin(aAngle), cos(aAngle));
	
	gl_Position = uMat * vec4(aPos, 0, 1);
	vTex = aTex;
}
