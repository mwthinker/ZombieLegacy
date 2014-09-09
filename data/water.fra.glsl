#version 100

precision mediump float;

uniform float uTime;

varying vec4 vPos;

const float PI = 3.14159265359;

float waveHeight(float amplitude, vec2 crestDir, float waveLength, float speed, vec2 pos, float time) {
	float w = 2.0 * PI / waveLength;
	float phase = 2.0 * PI * speed / waveLength;
	return 2.0*amplitude*pow(sin(dot(crestDir, pos)*w + phase * time + 1.0) / 2.0, 3.0);
}

float circleWaveHeight(float amplitude, vec2 center, float waveLength, float speed, vec2 pos, float time) {
	float len = length(center-pos);
	if (len > 10.0) {
		//amplitude = amplitude*10.0/len;
	}
	return waveHeight(amplitude, normalize(pos - center), waveLength, speed, pos, time);
}

void main() {
	float h1 = circleWaveHeight(1.0, vec2(100.0,100.0), 30.0, 11.0, vPos.xy, uTime);
	float h2 = circleWaveHeight(1.0, vec2(-100.0,0.0), 80.0, 4.0, vPos.xy, uTime);
	float h3 = waveHeight(4.0, vec2(1,0), 150.0, 15.0, vPos.xy, uTime);
	float h = h1*0.3 + h2*0.3 + h3*0.3;
	gl_FragColor = vec4(0, 0, 0, h + 0.5);
}
