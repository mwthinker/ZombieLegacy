#version 100

precision highp float;

uniform sampler2D uTexture;

varying vec2 vTex;

void main() {
	gl_FragColor = texture2D(uTexture, vTex);
}
