#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"
// Following raycasting tutorial from https://lodev.org/cgtutor/raycasting.html

#define map_width 24
#define map_height 24

#define screen_width 640
#define screen_height 480

#define floor_tex_num 3
#define ceil_tex_num 6

#define sprite_total 19

//note, each sprite is 64px width and height, offset by 16px tall, 1px wide

typedef struct
{
    Vector2 pos;
    int tex_num;
} sprite_t;

float z_buf[screen_width] = {0};
int sprite_order[sprite_total] = {0};
float sprite_dist[sprite_total] = {0};

void sort_sprites(int order[], float dist[], int amount);
void draw_floor_and_ceil(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[11], Image *image_buffer);
void draw_walls(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[11], Image *image_buffer);
void draw_sprites(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[11], Image *image_buffer);

int world_map[map_width][map_height] =
    {
        {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
        {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
        {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
        {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
        {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
        {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
        {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
        {7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
        {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
        {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
        {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
        {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
        {7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
        {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
        {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
        {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
        {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
        {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
        {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
        {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
        {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
        {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}};

sprite_t sprites[sprite_total] =
    {
        {{20.5f, 11.5f}, 10}, // green light in front of playerstart
                              // green lights in every room
        {{18.5f, 4.5f}, 10},
        {{10.0f, 4.5f}, 10},
        {{10.0f, 12.5f}, 10},
        {{3.5f, 6.5f}, 10},
        {{3.5f, 20.5f}, 10},
        {{3.5f, 14.5f}, 10},
        {{14.5f, 20.5f}, 10},

        // row of pillars in front of wall: fisheye test
        {{18.5f, 10.5f}, 9},
        {{18.5f, 11.5f}, 9},
        {{18.5f, 12.5f}, 9},

        // some barrels around the map
        {{21.5f, 1.5f}, 8},
        {{15.5f, 1.5f}, 8},
        {{16.0f, 1.8f}, 8},
        {{16.2f, 1.2f}, 8},
        {{3.5f, 2.5f}, 8},
        {{9.5f, 15.5f}, 8},
        {{10.0f, 15.1f}, 8},
        {{10.5f, 15.8f}, 8},
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

    Image tex[11] = {0};
    // "../../" needed because of default windows build path for quick testing
    tex[0] = LoadImage("../../res/tex/eagle.png");
    tex[1] = LoadImage("../../res/tex/redbrick.png");
    tex[2] = LoadImage("../../res/tex/purplestone.png");
    tex[3] = LoadImage("../../res/tex/greystone.png");
    tex[4] = LoadImage("../../res/tex/bluestone.png");
    tex[5] = LoadImage("../../res/tex/mossy.png");
    tex[6] = LoadImage("../../res/tex/wood.png");
    tex[7] = LoadImage("../../res/tex/colorstone.png");

    tex[8] = LoadImage("../../res/tex/barrel.png");
    tex[9] = LoadImage("../../res/tex/pillar.png");
    tex[10] = LoadImage("../../res/tex/greenlight.png");

    Texture2D weapon = LoadTexture("../../res/tex/weapons.png");
    int weapon_width = 64;
    int weapon_height = 64;
    Rectangle weapon_src = (Rectangle){.x = 1.f, .y = (float)(16 * 2 + weapon_height), (float)weapon_width, (float)weapon_height};
    Rectangle weapon_dst = (Rectangle){.x = screen_width / 2.0f, .y = screen_height - (weapon_height * 2.0f), .height = weapon_height * 4.0f, .width = weapon_width * 4.0f};
    Vector2 weapon_origin = (Vector2){.x = weapon_width * 2.0f, .y = weapon_height * 2.0f};

    Shader purple_discard = LoadShader(NULL, "../../shaders/purple_discard.fs");

    Image image_buffer = GenImageColor(GetScreenWidth(), GetScreenHeight(), BLACK);
    Texture display_texture = LoadTextureFromImage(image_buffer);
#if 0
    Image clear_buffer = GenImageColor(GetScreenWidth(), GetScreenHeight(), GRAY);
#endif

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
        draw_sprites(pos, dir, cam_plane, tex, &image_buffer);
        UpdateTexture(display_texture, image_buffer.data);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        DrawTexture(display_texture, 0, 0, RAYWHITE);

        // draw weapon over scene and discard all transparent values, weapon_src will be updated to match the correct frame
        BeginShaderMode(purple_discard);
        DrawTexturePro(weapon, weapon_src, weapon_dst, weapon_origin, 0.0f, WHITE);
        EndShaderMode();

        EndDrawing();
// not clearing the buffer because we're always writing over every pixel
#if 0
        ImageDraw(&image_buffer, clear_buffer, (Rectangle){.x = 0, .y = 0, .height = screen_height, .width = screen_width}, (Rectangle){.x = 0, .y = 0, .height = screen_height, .width = screen_width}, RAYWHITE);
#endif
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
void draw_floor_and_ceil(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[11], Image *image_buffer)
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

void draw_walls(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[11], Image *image_buffer)
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
        if (draw_start < 0)
            draw_start = 0;
        int draw_end = line_height / 2 + screen_height / 2;
        if (draw_end >= screen_height)
            draw_end = screen_height - 1;

        int tex_num = world_map[map_x][map_y] - 1;

        float wall_x;
        if (side == 0)
            wall_x = pos.y + perp_wall_dist * ray_dir.y;
        else
            wall_x = pos.x + perp_wall_dist * ray_dir.x;
        wall_x -= (float)floor(wall_x);

        int tex_x = (int)(wall_x * (float)tex[tex_num].width);
        if (side == 0 && ray_dir.x > 0)
            tex_x = tex[tex_num].width - tex_x - 1;
        if (side == 1 && ray_dir.y < 0)
            tex_x = tex[tex_num].width - tex_x - 1;

        float step = 1.0f * tex[tex_num].height / line_height;
        float tex_pos = (draw_start - screen_height / 2 + line_height / 2) * step;
        for (int y = draw_start; y < draw_end; y++)
        {
            int tex_y = (int)tex_pos & (tex[tex_num].height - 1);
            tex_pos += step;
            Color color = GetImageColor(tex[tex_num], tex_x, tex_y);
            if (side == 1)
                color = (Color){.a = color.a, .r = (color.r >> 1) & 8355711, .g = (color.g >> 1) & 8355711, .b = (color.b >> 1) & 8355711};
            ImageDrawPixel(image_buffer, x, y, color);
        }
        z_buf[x] = perp_wall_dist; // used for sprite drawing
    }
}

void draw_sprites(Vector2 pos, Vector2 dir, Vector2 cam_plane, Image tex[11], Image *image_buffer)
{
    for (int i = 0; i < sprite_total; i++)
    {
        sprite_order[i] = i;
        sprite_dist[i] = ((pos.x - sprites[i].pos.x) * (pos.x - sprites[i].pos.x) + (pos.y - sprites[i].pos.y) * (pos.y - sprites[i].pos.y));
    }
    sort_sprites(sprite_order, sprite_dist, sprite_total);

    for (int i = 0; i < sprite_total; i++)
    {
        Vector2 sprite_pos = (Vector2){.x = sprites[sprite_order[i]].pos.x - pos.x,
                                       .y = sprites[sprite_order[i]].pos.y - pos.y};
        int sprite_num = sprites[sprite_order[i]].tex_num;

        // transform sprite with inverse camera matrix, currently not using raylib functions
        float inverse_det = 1.f / (cam_plane.x * dir.y - dir.x * cam_plane.y);
        Vector2 transform = (Vector2) { .x = inverse_det * (dir.y * sprite_pos.x - dir.x * sprite_pos.y),
                                        .y = inverse_det * (-cam_plane.y * sprite_pos.x + cam_plane.x * sprite_pos.y)};
        int sprite_screen_x = (int)((screen_width / 2) * (1 + transform.x / transform.y));
        // calc height of target sprite
        int sprite_height = abs((int)(screen_height / transform.y));

        // calc lowest and highest pixels to fill
        int draw_start_y = -sprite_height / 2 + screen_height / 2;
        if (draw_start_y < 0) draw_start_y = 0;
        int draw_end_y = sprite_height / 2 + screen_height / 2;
        if (draw_end_y >= screen_height) draw_end_y = screen_height - 1;

        // calc width of target sprite
        int sprite_width = abs((int)(screen_height / transform.y));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if (draw_start_x < 0) draw_start_x = 0;
        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if (draw_end_x >= screen_width) draw_end_x = screen_width - 1;

        // loop through vertical stripes of sprite on screen
        for (int stripe = draw_start_x; stripe < draw_end_x; stripe++)
        {
            int tex_x = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * tex[sprite_num].width / sprite_width) / 256;

            // if sprite is in front of camera plane, on screen (left), on screen (right), zbuffer
            if (transform.y > 0 && stripe > 0 && stripe < screen_width && transform.y < z_buf[stripe])
            for (int y = draw_start_y; y < draw_end_y; y++)
            {
                int d = y * 256 - screen_height * 128 + sprite_height * 128;
                int tex_y = ((d * tex[sprite_num].height) / sprite_height) / 256;
                Color color = GetImageColor(tex[sprite_num], tex_x, tex_y);
                if (!(color.r == 0 && color.g == 0 && color.b == 0))
                ImageDrawPixel(image_buffer, stripe, y, color);
            }
        }
    }
}
// used only for sorting
typedef struct {
    float dist;
    int order;
} sprite_pair_t;

// compares distances between pairs first, then order
int cmp_func (const void *a, const void *b)
{
    sprite_pair_t arg1 = *(sprite_pair_t*)a;
    sprite_pair_t arg2 = *(sprite_pair_t*)b;
    if (arg1.dist < arg2.dist) return -1;
    if (arg1.dist > arg2.dist) return 1;
    if (arg1.order < arg2.order) return -1;
    if (arg1.order > arg2.order) return 1;
    return 0;
}

void sort_sprites(int order[], float dist[], int amount)
{
    sprite_pair_t *sprites = calloc(amount, sizeof *sprites);
    for (int i = 0; i < amount; i++) {
        sprites[i].dist = dist[i];
        sprites[i].order = order[i];
    }
    qsort(sprites, amount, sizeof *sprites, cmp_func);
    for (int i = 0; i < amount; i++) {
        dist[i] = sprites[amount - i - 1].dist;
        order[i] = sprites[amount - i - 1].order;
    }
    free(sprites);
}
