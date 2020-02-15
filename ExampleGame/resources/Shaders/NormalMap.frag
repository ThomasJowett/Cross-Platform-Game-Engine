#version 330 core

in VertexData{
vec3 posW;
vec2 texCoord;
vec3 normal;
} FragmentIn;

out vec4 FragColour;