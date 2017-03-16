attribute vec4 position;
attribute vec4 normal;

uniform mat4 mvp;
uniform mat4 model;

varying vec4 color;
varying vec4 norm;

void main()
{
    norm = model * normal;
    color = position * 0.4 + vec4(0.5, 0.5, 0.5, 0.6);
    gl_Position = mvp * position;
}
