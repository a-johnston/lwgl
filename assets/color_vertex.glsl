attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform mat4 model;
uniform mat4 mvp;

varying vec4 color;
varying vec4 norm;
varying vec2 texCoord;

void main()
{
    norm = model * vec4(normal, 1.0);
    color = vec4(position * 0.4 + vec3(0.5, 0.5, 0.5), 1.0);
    texCoord = uv;
    gl_Position = mvp * vec4(position, 1.0);
}
