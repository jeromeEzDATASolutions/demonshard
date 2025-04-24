/*
 * Copyright (c) 2020 Damien Ciabrini
 * This file is part of ngdevkit
 *
 * ngdevkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * ngdevkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Additional resources for sprites
 * https://wiki.neogeodev.org/index.php?title=Sprites
 */
#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>

#define CROM_TILE_OFFSET 256

// Pas de déplacement en pixels
#define PAS 2

#define SPRITES_ALLOCATED 3
#define SPRITES_TYPE_STANDARD 1
#define SPRITES_TYPE_BACKGROUND1 2 // TMX Mario

// Largeur d'une tile en pixels
#define TILE_SIZE 16

// Taille de l'écran visible
#define SCREEN_WIDTH 20
#define SCREEN_HEIGHT 15

#include "tile_layers.c"

#define MAP_WIDTH_PIXEL (MAP_WIDTH_TILES * 16) - (SCREEN_WIDTH * 16)    // 960 - une largeur d'écran
#define MAP_HEIGHT_PIXEL (MAP_HEIGHT_TILES * 16) - (SCREEN_HEIGHT * 16) // 960 - une hauteur d'écran

#include "palette.c"
#include "sprite.h"
#include "sprite.c"

int main(void) {

    char str[10];

    typedef struct{
        s16 x, y; // Position de la caméra
    } Camera;

    // Position of the camera in pixels, start bottom left
    Camera camera;
    camera.x = 0;
    camera.y = 0;

    // Position of the camera in tiles
    Camera camera_tiles;
    camera_tiles.x = 0;

    ng_cls();
    init_palette();

    sprite_init(0);
    //sprite_init(1);

    for (;;)
    {
        // Get controles from joystick 1
        u8 u = (bios_p1current & CNT_UP);
        u8 d = (bios_p1current & CNT_DOWN);
        u8 l = (bios_p1current & CNT_LEFT);
        u8 r = (bios_p1current & CNT_RIGHT);
        u8 b1 = (bios_p1current & 16);
        u8 b2 = (bios_p1current & 32);

        if (u)
        {
            if (camera.y < MAP_HEIGHT_PIXEL+16)
            {
                sprites[0].y -= PAS;
                sprite_update(0);
                camera.y += PAS;
                camera_tiles.y = camera.y >> 4;
                u16 row_to_update = 0;
                u16 tmp = 0;

                // Update first tiles of each sprite
                if (camera_tiles.y >= 0)
                {
                    if (camera_tiles.y >= 0 && camera_tiles.y < 16)
                    {
                        row_to_update = 32 - (camera_tiles.y + 15);
                        for (u16 i = 0; i < sprites[0].width; i++){
                            sprite_change_tile_in_a_colonne(0, i, row_to_update, tmx_mario[(MAP_HEIGHT_TILES - 32) + row_to_update][i]);
                        }
                    }
                    else if (camera_tiles.y >= 16 && camera_tiles.y < 59){

                        if (camera_tiles.y < 48){
                            row_to_update = 32 - (camera_tiles.y - 15);
                        }
                        else if (camera_tiles.y < 100) // ???????
                        {
                            row_to_update = 64 - (camera_tiles.y - 15);
                        }

                        for (u16 i = 0; i < sprites[0].width; i++)
                        {
                            sprite_change_tile_in_a_colonne(0, i, row_to_update, tmx_mario[(MAP_HEIGHT_TILES - 32) - (camera_tiles.y - 15)][i]);
                        }

                        /*snprintf(str, 10, "Row : %3d", row_to_update);
                        ng_text(2, 3, 0, str);
                        snprintf(str, 10, "Cty : %3d", camera_tiles.y);
                        ng_text(2, 5, 0, str);*/
                    }
                }
            }
        }

        if (d)
        {
            if (camera.y > 0)
            {
                sprites[0].y += PAS;
                sprite_update_y(0);
                camera.y -= PAS;
                camera_tiles.y = camera.y >> 4;
            }
        }

        if (l)
        {
            if ( camera.x > 0 )
            {
                sprites[0].x += PAS;
                sprite_update_x(0);
                camera.x -= PAS;
                camera_tiles.x = camera.x >> 4;
                //sprite_update_tiles_left_from_one_sprite(0, camera_tiles.x);
            }
        }

        if (r)
        {
            if (camera.x < MAP_WIDTH_PIXEL)
            {
                sprites[0].x -= PAS;
                sprite_update_x(0);
                camera.x += PAS;
                camera_tiles.x = camera.x >> 4;
                sprite_update_tiles_right_from_one_sprite(0, camera_tiles.x);
            }
        }

        ng_wait_vblank();
    }

    return 0;
}