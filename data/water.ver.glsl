#version 100

precision mediump float;

uniform mat4 uMat;

attribute vec4 aPos;

varying vec4 vPos;

void main() {
	gl_Position = uMat * aPos;
	vPos = aPos;
}
