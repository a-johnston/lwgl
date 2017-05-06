attribute vec4 position;
attribute vec4 normal;

varying vec4 color;
varying vec4 norm;

void main()
{
    norm = normal;
    color = position * 0.4 + vec4(0.5, 0.5, 0.5, 0.6);
    gl_Position = position;
}
