#ifndef RENDERER_H
#define RENDERER_H

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "fz_std\\external\\stb_image.h"

// Fonts
#define FONT_PROGGY_CLEAN_PATH S("\\assets\\fonts\\ProggyClean.ttf")
#define FONT_INCONSOLATA_PATH  S("\\assets\\fonts\\Inconsolata.ttf")

///////////////////////////////////////////////////////
// @Section: Screenspace primitives

// Vertex Shaders
#define V_SS_PRIMITIVE_PATH S("\\src\\shaders\\v_ss_primitive.glsl")
#define V_SS_TEXT_PATH      S("\\src\\shaders\\v_ss_text.glsl")
#define V_SS_LINE_PATH      S("\\src\\shaders\\v_ss_line.glsl")
#define V_WS_PRIMITIVE_PATH S("\\src\\shaders\\v_ws_primitive.glsl")
#define V_WS_LINE_PATH      S("\\src\\shaders\\v_ws_line.glsl")

// Fragment Shaders
#define F_TEXTURE_PATH S("\\src\\shaders\\f_texture.glsl")
#define F_TEXT_PATH    S("\\src\\shaders\\f_text.glsl")
#define F_COLOR_PATH    S("\\src\\shaders\\f_color.glsl")

///////////////////////////////////////////////////////
// @Section: Forward declare
typedef struct Texture_Info Texture_Info;
typedef struct Primitive2D Primitive2D;
typedef struct Line2D Line2D;
typedef struct Primitive3D Primitive3D;
typedef struct Line3D Line3D;
typedef struct Glyph Glyph;
typedef struct Font Font;
typedef struct Render_Batch Render_Batch;
typedef struct Renderer Renderer;

///////////////////////////////////////////////////////
// @Section: Texture
struct Texture_Info
{
  u32 handle;
  u32 index;
  u32 width;
  u32 height;
};

///////////////////////////////////////////////////////
// @Section: Screenspace primitives]
struct Primitive2D
{
  Vec2f32 position;
  Vec2f32 scale;
  Vec2f32 uv_min;
  Vec2f32 uv_max;
  Vec4f32 color;
  u32 texture_id;
};
global Vec2f32 unit_2d_triangle[6] = {
  { -0.5f, -0.5f }, {  0.5f, -0.5f },
  { -0.5f,  0.5f }
};
global Vec2f32 unit_2d_quad[6] = {
  { -0.5f, -0.5f }, {  0.5f, -0.5f },
  {  0.5f,  0.5f }, { -0.5f, -0.5f },
  {  0.5f,  0.5f }, { -0.5f,  0.5f }
};

struct Line2D
{
  Vec2f32 p0;
  Vec2f32 p1;
  Vec4f32 color;
};

///////////////////////////////////////////////////////
// @Section: Worldspace primitives
struct Primitive3D
{
  Transform3f32 transform;
  Vec2f32 uv_min;
  Vec2f32 uv_max;
  Vec3f32 normal;
  Vec4f32 color;
  u32 texture_id;
};

// Triangles
global Vec3f32 unit_3d_triangle[3] = {
  { -0.5f, -0.5f, 0.0f }, 
  {  0.5f, -0.5f, 0.0f },
  { -0.5f,  0.5f, 0.0f }
};

// Quad
global Vec3f32 unit_3d_quad[6] = {
  {  0.5f,  0.5f, 0.0f }, { -0.5f,  0.5f, 0.0f },
  { -0.5f, -0.5f, 0.0f }, {  0.5f, -0.5f, 0.0f },
  {  0.5f,  0.5f, 0.0f }, { -0.5f, -0.5f, 0.0f }
};

struct Line3D
{
  Vec3f32 p0;
  Vec3f32 p1;
  Vec4f32 color;
};

///////////////////////////////////////////////////////
// @Section: Fonts
struct Glyph
{
  Vec2f32 uv_min; /* Top left texture coordinate in atlas */
  Vec2f32 uv_max; /* Bottom right texture coordinate in atlas */
  Vec2f32 size; /* Pixel dimensions */
  Vec2f32 offset; /* Pixel offset from the baseline cursor position to where the glyph should be drawn. X is horizontal offset from cursor, Y is vertical offset from baseline (negative Y means above baseline) */
  f32     advance; /* How far - horizontally - to move the cursor after drawing this glyph */
};

struct Font
{
  #define FONT_LOAD_SIZE 96
  #define MAX_FONT_GLYPHS 95
  Glyph glyphs[MAX_FONT_GLYPHS];
  u32   texture_id; /* Opengl texture handle for the atlas */
  u32   texture_index; /* Index into renderer's texture array */
  f32   height; /* Pixel size requested when loading the font */
  f32   line_height; /* Total pixels between baselines when stacking text. line_height = ascent + |descent| + line_gap */
  f32   ascent; /* Pixels from baseline to highest any glyph can reach */
  f32   descent; /* Pixels from baseline to lowest any glyph can reach */
  f32   line_gap; /* Additional spacing between lines */
};

///////////////////////////////////////////////////////
// @Section: Instanced Target
typedef enum
{
  Render_Batch_None,

  Render_Batch_SS_Line,
  Render_Batch_SS_triangle,
  Render_Batch_SS_quad,
  Render_Batch_SS_text,

  Render_Batch_WS_Triangle,
  Render_Batch_WS_Quad,
  Render_Batch_WS_text,
  Render_Batch_WS_line,

  Render_Batch_Count,
} Render_Batch_Kind;

struct Render_Batch
{
  Render_Batch_Kind kind;

  u32 pipeline;

  // Buffers
  u32 vao;
  u32 unit_vbo;
  u32 instance_vbo;

  // Uniforms
  u32 u_screen_size_location;
  u32 u_view_location;
  u32 u_projection_location;

  // Instanced data
  void* data;   
  u32   stride; /* Size of data type used in data */
  u32   Max;    /* Max allocated instances */
  u32   count;  /* Current instances count */
};

///////////////////////////////////////////////////////
// @Section: Renderer
struct Renderer
{
  Arena* arena;

  struct
  {
    u32 v_ss_line;
    u32 v_ss_quad;
    u32 v_ss_text;
    u32 v_ws_quad;
    u32 v_ws_line;
    u32 f_line;
    u32 f_texture;
    u32 f_text;
  } shaders;

  Render_Batch* batches[Render_Batch_Count];

  // Textures
  u32* textures;
  u32 texture_count;
  u32 texture_max;

  // Fonts
  Font* fonts;
  u32 fonts_count;
  u32 fonts_max;
};

global Renderer g_renderer;

function void r_init();
function void r_render(Mat4f32 view, Mat4f32 projection);

function void   _r_draw_2d_primitive(Render_Batch* render_batch, Vec2f32 position, Vec2f32 scale, Vec2f32 uv_min, Vec2f32 uv_max, Vec4f32 color, u32 texture_id);
function void    r_draw_2d_triangle(Vec2f32 position, Vec2f32 scale, Vec2f32 uv_min, Vec2f32 uv_max, Vec4f32 color, u32 texture_id);
function void    r_draw_2d_quad_colored(Vec2f32 position, Vec2f32 scale, Vec4f32 color);
function void    r_draw_2d_quad(Vec2f32 position, Vec2f32 scale, Vec2f32 uv_min, Vec2f32 uv_max, Vec4f32 color, u32 texture_id);
function void    r_draw_2d_point(Vec2f32 position, Vec4f32 color);
function void    r_draw_2d_box(Vec2f32 p0, Vec2f32 p1, Vec4f32 color); /* Hollow quad. p0 is bottom left, p1 is top right */
function Vec2f32 r_draw_2d_text(Vec2f32 position, f32 pixel_height, Vec4f32 color, String8 text);
function Vec2f32 r_draw_2d_text_centered(Vec2f32 position, f32 pixel_height, Vec4f32 color, String8 text);
function void    r_draw_2d_line(Vec2f32 p0, Vec2f32 p1, Vec4f32 color);
function void    r_draw_2d_grid(Vec2f32 p0, Vec2f32 p1, u32 square_size_pixel, Vec4f32 color);

function void   _r_draw_3d_primitive(Render_Batch* render_batch, Transform3f32 transform, Vec2f32 uv_min, Vec2f32 uv_max, Vec4f32 color, u32 texture_id);
function void    r_draw_3d_triangle(Transform3f32 transform, Vec2f32 uv_min, Vec2f32 uv_max, Vec4f32 color, u32 texture_id);
function void    r_draw_3d_box(Vec3f32 translation, Quatf32 rotation, f32 size, Vec4f32 color);
function void    r_draw_3d_quad(Transform3f32 transform, Vec2f32 uv_min, Vec2f32 uv_max, Vec4f32 color, u32 texture_id);
function void    r_draw_3d_text(Transform3f32 transform, Vec4f32 color, f32 font_scale, String8 text);
function void    r_draw_3d_line(Vec3f32 p0, Vec3f32 p1, Vec4f32 color);
function void    r_draw_3d_arrow(Vec3f32 start, Vec3f32 end, Vec4f32 color);
function void    r_draw_3d_grid(Vec3f32 center, Vec3f32 normal, Vec3f32 forward, u32 square_size, u32 square_per_side, Vec4f32 color);

function Texture_Info  r_load_texture(String8 path);
function Texture_Info  r_create_color_texture(Vec4f32 color);
function void          r_create_fallback_texture();
function void          r_load_font(String8 relative_path);
function Render_Batch* r_new_render_batch(Arena* arena, Render_Batch_Kind kind, u32 max_instances);

function Vec2f32 r_get_2d_text_dimensions(String8 text, f32 pixel_height);
function void    r_toggle_wireframe();
function void    r_toggle_facecull();
function u32     r_compile_shader(String8 relative_path, GLenum shader_type);

#endif // RENDERER_H