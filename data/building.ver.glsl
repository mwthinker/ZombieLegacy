#version 100

precision highp float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform vec2 uGlobalCenterPos;
uniform vec2 uGlobalHumanPos;

attribute vec3 aVer;
attribute vec2 aTex;

varying vec2 vTex;

void main() {
	vec2 delta = vec2(0,0);//(aVer.z * 0.05) * (aVer.xy - uGlobalHumanPos);
	gl_Position = uProj * uModel * vec4(delta + aVer.xy - uGlobalCenterPos, 0, 1);
	vTex = aTex;
}
