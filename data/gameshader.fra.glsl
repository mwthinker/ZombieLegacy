#version 100

precision mediump float;

uniform vec4 uColor;
uniform float uIsTexture;

uniform sampler2D uTexture;

varying vec2 vTex;

void main() {
	if (uIsTexture > 0.5) {
		gl_FragColor = texture2D(uTexture, vTex) * uColor;
	} else {
		gl_FragColor = uColor;
	}
}
