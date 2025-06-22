#version 450 core

layout(location = 0)out vec4 frag_colour;
layout(location = 1)out int entityId;

in vec2 v_TexCoord;

layout(binding = 0)uniform sampler2D u_SceneTexture;
layout(binding = 1)uniform isampler2D u_EntityId;

void main() {
    frag_colour = texture(u_SceneTexture, v_TexCoord);
    entityId = texture(u_EntityId, v_TexCoord).r;
}