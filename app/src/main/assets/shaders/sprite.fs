precision mediump float;
// fragment shader for sprite rendering
varying vec2 TexCoords;

uniform sampler2D sprite;
uniform vec3 spriteColor;

void main()
{
    gl_FragColor = vec4(spriteColor, 1.0) * texture2D(sprite, TexCoords);
}