#version 450 core

in vec4 v_color;
in vec2 v_uv;

in flat uint v_texture_id;

uniform sampler2D u_textures[32];

out vec4 FragColor;

void main()
{
  // NOTE(fz): if statement in shaders hurts me, maybe we should just add a render batch for colored quads?
  if (v_texture_id == 0xFFFFFFFFu)
  {
    FragColor = v_color;
  }
  else
  {
    vec4 tex_color = texture(u_textures[v_texture_id], v_uv);
    FragColor = tex_color * v_color;
  }
}