#version 150

in vec4 position;
in vec4 color;
out vec4 color_from_vshader;
uniform mat4 PVM;

void main() {
   gl_Position = PVM * position;
   color_from_vshader = color;
}
