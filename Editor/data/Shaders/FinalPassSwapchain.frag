#version 450 core

layout(location = 0)out vec4 frag_colour;

in vec2 v_TexCoord;

layout(binding = 0)uniform sampler2D u_SceneTexture;

void main() {
    frag_colour = texture(u_SceneTexture, v_TexCoord);
}
