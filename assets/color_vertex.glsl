attribute vec3 position;
attribute vec3 normal;

uniform mat4 mvp;

varying vec4 color;
varying vec4 norm;

void main()
{
    norm = vec4(normal, 1.0);
    color = vec4(position * 0.4 + vec3(0.5, 0.5, 0.5), 1.0);
    gl_Position = mvp * vec4(position, 1.0);
}
