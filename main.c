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
#define SPRITES_ALLOCATED 1

#include "tile_layers.c"

#include "sprite.h"
#include "palette.c"
#include "sprite.c"

int main(void) {

    char str[10];

    ng_cls();
    init_palette();

    sprite_init_tmx(0);
    sprite_init(0);

    for (;;)
    {
        // Get controles from joystick 1
        u8 u = (bios_p1current & CNT_UP);
        u8 d = (bios_p1current & CNT_DOWN);
        u8 l = (bios_p1current & CNT_LEFT);
        u8 r = (bios_p1current & CNT_RIGHT);
        u8 b1 = (bios_p1current & 16);
        u8 b2 = (bios_p1current & 32);

        if (bios_p1current & CNT_UP)
        {
            sprites[0].y -= 1;
            sprite_update(0);
        }
        else if (bios_p1current & CNT_DOWN)
        {
            sprites[0].y += 1;
            sprite_update_y(0);
        }
        else if (bios_p1current & CNT_LEFT)
        {
            sprites[0].x += 1;
            sprite_update_x(0);
        }
        else if (bios_p1current & CNT_RIGHT)
        {
            sprites[0].x -= 1;
            sprite_update_x(0);
        }

        snprintf(str, 10, "T : %3d", sprites[0].tmx[0][0]);
        ng_text(2, 3, 0, str);

        ng_wait_vblank();
    }

    return 0;
}