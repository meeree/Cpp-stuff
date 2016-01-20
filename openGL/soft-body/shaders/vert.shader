#version 150

in vec3 vert_color;
out vec4 frag_color;

in vec4 position;
uniform mat4 PVM;

void main() {
   gl_Position = PVM * position;
   frag_color = vec4(vert_color.xyz, 1);
}
