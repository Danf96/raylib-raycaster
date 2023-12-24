#include <math.h>
#include <stdint.h>

#include "raylib.h"
#include "raymath.h"
// Following raycasting tutorial from https://lodev.org/cgtutor/raycasting.html
// https://github.com/raylib-extras/examples-c/tree/main/write_to_image
// can update the texture with UpdateTexture
#define map_width 24
#define map_height 24

#define screen_width 640
#define screen_height 480

#define floor_tex_num 3
#define ceil_tex_num 6

void draw_floor_and_ceil(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[8], Image *image_buffer);
void draw_walls(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[8], Image *image_buffer);

int world_map[map_width][map_height] =
{
  {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screen_width, screen_height, "raylib raycaster");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    
    Vector2 pos = (Vector2){.x = 22.f, .y = 12.f};
    Vector2 dir = (Vector2){.x = -1.f, .y = 0.f};
    Vector2 cam_plane = (Vector2){.x = 0.f, .y = 0.66f};
    
    Image tex[8] = {0};
    // "../../" needed because of default windows build path for quick testing
    tex[0] = LoadImage("../../res/tex/eagle.png");
    tex[1] = LoadImage("../../res/tex/redbrick.png");
    tex[2] = LoadImage("../../res/tex/purplestone.png");
    tex[3] = LoadImage("../../res/tex/greystone.png");
    tex[4] = LoadImage("../../res/tex/bluestone.png");
    tex[5] = LoadImage("../../res/tex/mossy.png");
    tex[6] = LoadImage("../../res/tex/wood.png");
    tex[7] = LoadImage("../../res/tex/colorstone.png");

    Image image_buffer = GenImageColor(GetScreenWidth(), GetScreenHeight(), BLACK);
    Texture display_texture = LoadTextureFromImage(image_buffer);

    // Image clear_buffer = GenImageColor(GetScreenWidth(), GetScreenHeight(), GRAY);

    
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        float frameTime = GetFrameTime();
        float move_speed = frameTime * 5.0f;
        float rot_speed = frameTime * 3.0f;
        if (IsKeyDown(KEY_UP))
        {
            if (world_map[(int)(pos.x + dir.x * move_speed)][(int)pos.y] == 0)
                pos.x += dir.x * move_speed;
            if (world_map[(int)pos.x][(int)(pos.y + dir.y * move_speed)] == 0)
                pos.y += dir.y * move_speed;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            if (world_map[(int)(pos.x - dir.x * move_speed)][(int)pos.y] == 0)
                pos.x -= dir.x * move_speed;
            if (world_map[(int)pos.x][(int)(pos.y + dir.y * move_speed)] == 0)
                pos.y -= dir.y * move_speed;
        }
        // look into raylib vec2 rotation functions
        if (IsKeyDown(KEY_RIGHT))
        {
            dir = Vector2Rotate(dir, -rot_speed);
            cam_plane = Vector2Rotate(cam_plane, -rot_speed);
        }
        if (IsKeyDown(KEY_LEFT))
        {
            // both camera direction and camera plane must be rotated
            dir = Vector2Rotate(dir, rot_speed);
            cam_plane = Vector2Rotate(cam_plane, rot_speed);
        }
        draw_floor_and_ceil(pos, dir, cam_plane, tex, &image_buffer);
        draw_walls(pos, dir, cam_plane, tex, &image_buffer);
        UpdateTexture(display_texture, image_buffer.data);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        DrawTexture(display_texture, 0, 0, RAYWHITE);

        EndDrawing();
        // not clearing the buffer because we're always writing over every pixel
        // ImageDraw(&image_buffer, clear_buffer, (Rectangle){.x = 0, .y = 0, .height = screen_height, .width = screen_width}, (Rectangle){.x = 0, .y = 0, .height = screen_height, .width = screen_width}, RAYWHITE);

        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
void draw_floor_and_ceil(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[8], Image *image_buffer)
{
    for (int y = 0; y < screen_height; y++)
    {
        // direction vectors for leftmost and rightmost rays
        Vector2 ray_left = (Vector2){.x = dir.x - cam_plane.x, .y = dir.y - cam_plane.y};
        Vector2 ray_right = (Vector2){.x = dir.x + cam_plane.x, .y = dir.y + cam_plane.y};

        // y pos compared to horizon
        int p = y - screen_height / 2;

        // horizontal distance from camera to floor
        float pos_z = 0.5 * screen_height;

        // distance from camera to floor for current row
        float row_dist = pos_z / p;

        Vector2 floor_step = (Vector2){.x = row_dist * (ray_right.x - ray_left.x) / screen_width, .y = row_dist * (ray_right.y - ray_left.y) / screen_width};

        Vector2 floor = (Vector2){.x = pos.x + row_dist * ray_left.x, .y = pos.y + row_dist * ray_left.y};

        for (int x = 0; x < screen_width; x++)
        {
            int cell_x = (int)floor.x;
            int cell_y = (int)floor.y;

            int t_x = (int)(tex[floor_tex_num].width * (floor.x - cell_x)) & (tex[floor_tex_num].width - 1);
            int t_y = (int)(tex[floor_tex_num].height * (floor.y - cell_y)) & (tex[floor_tex_num].height - 1);

            floor = Vector2Add(floor, floor_step);
            // floor
            Color color = GetImageColor(tex[floor_tex_num], t_x, t_y);
            color = (Color){.a = color.a, .r = (color.r >> 1) & 8355711, .g = (color.g >> 1) & 8355711, .b = (color.b >> 1) & 8355711};
            ImageDrawPixel(image_buffer, x, y, color);

            // ceiling
            color = GetImageColor(tex[ceil_tex_num], t_x, t_y);
            color = (Color){.a = color.a, .r = (color.r >> 1) & 8355711, .g = (color.g >> 1) & 8355711, .b = (color.b >> 1) & 8355711};
            ImageDrawPixel(image_buffer, x, screen_height - y - 1, color);
        }

    }
    
}

void draw_walls(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[8], Image *image_buffer)
{
    for (int x = 0; x < screen_width; x++)
    {
        // calc ray pos and direction (switch to raylib structs)
        float camera_x = 2 * x / (float)screen_width - 1;
        Vector2 ray_dir = (Vector2){.x = dir.x + cam_plane.x * camera_x, .y = dir.y + cam_plane.y * camera_x};

        // which cell we're in
        int map_x = (int)pos.x;
        int map_y = (int)pos.y;

        // length of ray from current pos to next x or y side

        Vector2 side_dist = (Vector2){0};

        // length of ray from one x or y-side to next x or y-side
        Vector2 delta_dist = (Vector2){.x = (float)fabs(1.0 / ray_dir.x), .y = (float)fabs(1.0 / ray_dir.y)};

        float perp_wall_dist;

        int step_x;
        int step_y;

        bool hit = false;
        int side;

        if (ray_dir.x < 0)
        {
            step_x = -1;
            side_dist.x = (pos.x - map_x) * delta_dist.x;
        }
        else
        {
            step_x = 1;
            side_dist.x = (map_x + 1.0f - pos.x) * delta_dist.x;
        }
        if (ray_dir.y < 0)
        {
            step_y = -1;
            side_dist.y = (pos.y - map_y) * delta_dist.y;
        }
        else
        {
            step_y = 1;
            side_dist.y = (map_y + 1.0f - pos.y) * delta_dist.y;
        }
        while (!hit)
        {
            if (side_dist.x < side_dist.y)
            {
                side_dist.x += delta_dist.x;
                map_x += step_x;
                side = 0;
            }
            else
            {
                side_dist.y += delta_dist.y;
                map_y += step_y;
                side = 1;
            }
            if (world_map[map_x][map_y] > 0)
                hit = true;
        }
        if (side == 0)
            perp_wall_dist = (side_dist.x - delta_dist.x);
        else
            perp_wall_dist = (side_dist.y - delta_dist.y);

        int line_height = (int)(screen_height / perp_wall_dist);
        int draw_start = -line_height / 2 + screen_height / 2;
        if (draw_start < 0) draw_start = 0;
        int draw_end = line_height / 2 + screen_height / 2;
        if (draw_end >= screen_height) draw_end = screen_height - 1;

        int tex_num = world_map[map_x][map_y] - 1;

        float wall_x;
        if (side == 0) wall_x = pos.y + perp_wall_dist * ray_dir.y;
        else wall_x = pos.x + perp_wall_dist * ray_dir.x;
        wall_x -= (float)floor(wall_x);

        int tex_x = (int)(wall_x * (float)tex[tex_num].width);
        if (side == 0 && ray_dir.x > 0) tex_x = tex[tex_num].width - tex_x - 1;
        if (side == 1 && ray_dir.y < 0) tex_x = tex[tex_num].width - tex_x - 1;

        float step = 1.0f * tex[tex_num].height / line_height;
        float tex_pos = (draw_start - screen_height / 2 + line_height / 2) * step;
        for (int y = draw_start; y < draw_end; y++)
        {
            int tex_y = (int)tex_pos & (tex[tex_num].height - 1);
            tex_pos += step;
            Color color = GetImageColor(tex[tex_num], tex_x, tex_y); 
            if(side == 1) color = (Color){.a = color.a, .r = (color.r >> 1) & 8355711, .g = (color.g >> 1) & 8355711, .b = (color.b >> 1) & 8355711};
            ImageDrawPixel(image_buffer, x, y, color);
        }
    }
}