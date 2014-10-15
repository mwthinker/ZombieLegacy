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
	mat3 rot = mat3(cos(uAngle), sin(uAngle), 0,
					-sin(uAngle), cos(uAngle), 0,
					0, 0, 1);
	
	mat3 tra = mat3(1, 0, 0,
					0, 1, 0,					
					uPos.x - uCenterPos.x, uPos.y - uCenterPos.y, 1);
	
	vec3 pos = tra * rot * vec3(aVer, 1);
	
	
	gl_Position = uProj * uModel * vec4(pos, 1);
	vTex = aTex;
}
