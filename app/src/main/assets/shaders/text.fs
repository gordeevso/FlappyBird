precision mediump float;

varying vec2 TexCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    gl_FragColor = vec4(textColor, 1.0) * vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).r);
}