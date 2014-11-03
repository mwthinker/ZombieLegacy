#version 100

precision highp float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform vec2 uGlobalCenterPos;

attribute vec2 aVer;

varying vec2 vVer;

void main() {
	gl_Position = uProj * uModel * vec4(aVer-uGlobalCenterPos, 0, 1);
	vVer = aVer;
}
