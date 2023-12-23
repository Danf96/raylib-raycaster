#include <math.h>
#include "raylib.h"
#include "raymath.h"
// Following raycasting tutorial from https://lodev.org/cgtutor/raycasting.html

#define mapWidth 24
#define mapHeight 24

void draw_walls(Vector2 pos, Vector2 dir, Vector2 cam_plane);

int world_map[mapWidth][mapHeight] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    Vector2 pos = (Vector2){.x = 22.f, .y = 12.f};
    Vector2 dir = (Vector2){.x = -1.f, .y = 0.f};
    Vector2 cam_plane = (Vector2){.x = 0.f, .y = 0.66f};

    InitWindow(640, 480, "raylib raycaster");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
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
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);
        draw_walls(pos, dir, cam_plane);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void draw_walls(Vector2 pos, Vector2 dir, Vector2 cam_plane)
{
    for (int x = 0; x < GetScreenWidth(); x++)
    {
        // calc ray pos and direction (switch to raylib structs)
        float camera_x = 2 * x / (float)GetScreenWidth() - 1;
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

        int line_height = (int)(GetScreenHeight() / perp_wall_dist);
        int draw_start = -line_height / 2 + GetScreenHeight() / 2;
        if (draw_start < 0)
            draw_start = 0;

        Color color;
        switch (world_map[map_x][map_y])
        {
        case 1:
            color = RED;
            break;
        case 2:
            color = GREEN;
            break;
        case 3:
            color = BLUE;
            break;
        case 4:
            color = WHITE;
            break;
        default:
            color = YELLOW;
        }

        if (side == 1)
        {
            color = (Color){.a = color.a, .r = color.r / 2, .g = color.g / 2, .b = color.b / 2};
        }
        DrawRectangle(x, draw_start, 1, line_height, color);
    }
}