#version 100

precision mediump float;

uniform vec4 uColor;
uniform sampler2D uTexture;

varying vec2 vTex;

void main() {
	vec4 tex = texture2D(uTexture, vTex);
	gl_FragColor = tex * uColor;
}
