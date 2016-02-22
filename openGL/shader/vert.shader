#version 150

out vec4 frag_color;

in vec4 position;
uniform mat4 PVM;

void main() {
   gl_Position = PVM * position;
   frag_color = vec4(0, 0, 0, 1);
}
