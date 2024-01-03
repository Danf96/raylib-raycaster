#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec3 discardColor = vec3(152/255.0, 0.0, 136/255.0);
    vec4 texelColor = texture(texture0, fragTexCoord);
    if (texelColor.rgb == discardColor) discard;
    finalColor = texelColor * fragColor * colDiffuse;
}
