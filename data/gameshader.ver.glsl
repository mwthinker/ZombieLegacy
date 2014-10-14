#version 100

precision mediump float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform vec2 uCenterPos;
uniform vec2 uPos;
uniform float uAngle;

attribute vec2 aVer;
attribute vec2 aTex;

varying vec2 vTex;

void main() {
	// Column-major order.
	mat2 rotation = mat2(cos(uAngle), sin(uAngle),
						-sin(uAngle), cos(uAngle));
	
	vec2 pos = uPos + rotation * aVer - uCenterPos;
	
	gl_Position = uProj * uModel * vec4(aPos, 0, 1);
	vTex = aTex;
}
