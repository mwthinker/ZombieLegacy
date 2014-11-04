#version 100

precision highp float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform vec2 uGlobalCenterPos;

attribute vec2 aVer;
attribute vec2 aTex;
attribute float aHeight;

varying vec2 vTex;

void main() {
	vec4 pos = uProj * uModel * vec4(aVer, 0, 1);
	
	pos += normalize(uGlobalCenterPos - pos.xy) * h;
	
	gl_Position = uProj * uModel * pos;
	vTex = aTex;
}
