varying vec4 color;
varying vec4 norm;

void main()
{
    float intensity = clamp(dot(normalize(norm), normalize(vec4(1, 1, 1, 1))), 0.0, 1.0);
    vec4 shaded_color = color * intensity;
    gl_FragColor = shaded_color + vec4(.1, .1, .1, 1.0);
}
