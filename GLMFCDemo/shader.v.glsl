#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

out vec3 ourColor;
uniform mat4 mvp;
uniform float time;
uniform int isLine; 

vec3 hue2rgb(float h){
	float s = 1.0, v = 1.0, f, p, q, t;

	f = fract(h / 60);
	p = v * (1-s);
	q = v * (1 - f*s);
	t = v * (1 - (1-f)*s);

	int hi = int(h) / 60 % 6;
	if (hi == 0) return vec3(v, t, p);
	if (hi == 1) return vec3(q, v, p);
	if (hi == 2) return vec3(p, v, t);
	if (hi == 3) return vec3(p, q, v);
	if (hi == 4) return vec3(t, p, v);
	if (hi == 5) return vec3(v, p, q);

	return (1, 1, 1);
}

vec3 heightcolor(float h){
	//only accept from 0.0f to 1.0f
	if (h < 0 || h > 1) return vec3(0, 0, 0);
	return hue2rgb((1 - h) * 270);
}

void main()
{
	float x = pos.x, y = pos.y;
	float t = 3.5 * sqrt(x*x+y*y);
	float z = (1 - t * t) * exp(-0.5*t*t);
	float h = z*0.6 + 0.4;
	
	gl_Position = mvp * vec4(pos, z, 1.0f);

//	全是白线效果不好，先留着
//	if (isLine == 1){
//		ourColor = vec3(1, 1, 1);
//	}
//	else{
//		ourColor = heightcolor(h) * 0.8;
//	}
	
	ourColor = heightcolor(h) * 0.8;
}