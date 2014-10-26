#version 100

precision mediump float;

uniform mat4 uProj;
uniform mat4 uModel;
uniform vec2 uPos;

attribute vec2 aPos;

varying vec2 vPos;

void main() {
	gl_Position = uProj * uModel * vec4(aPos-uPos, 0, 1);
	vPos = aPos;
}
