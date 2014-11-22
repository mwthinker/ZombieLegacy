#version 100

precision highp float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform vec2 uGlobalCenterPos;
uniform vec2 uGlobalPos;
uniform float uLocalAngle;

attribute vec2 aVer;
attribute vec2 aTex;

varying vec2 vTex;

void main() {
	// Column-major order.
	mat3 rot = mat3(cos(uLocalAngle), sin(uLocalAngle), 0,
					-sin(uLocalAngle), cos(uLocalAngle), 0,
					0, 0, 1);
	
	mat3 tra = mat3(1, 0, 0,
					0, 1, 0,					
					uGlobalPos.x - uGlobalCenterPos.x, uGlobalPos.y - uGlobalCenterPos.y, 1);
	
	vec3 pos = tra * rot * vec3(aVer, 1);	
	
	gl_Position = uProj * uModel * vec4(pos.xy, 0, 1);
	vTex = aTex;
}
