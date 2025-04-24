/**
 * Declaration des sprites
 */
sprite_t sprites[SPRITES_ALLOCATED] = {
    {
        // Tileset for the background - test d'affichage
        .type = SPRITES_TYPE_BACKGROUND1,
        .actif = 1,
        .sprite = 1,
        .first_tile = 256, // 208 tiles
        .palette = 3,
        .width = 32,
        .height = 32,
        .picture_width = 20,
        .picture_height = 20,
        .offset_x = 0,
        .offset_y = 0,
        .first_tile_with_offset_y = 0, // offset_y * picture_width
        .x = 0,
        .y = 0,
        .sens = 1,
        .anim_frame_frequence = 4,
        .anim_frame_cpt = 0,
        .display = 1,
        .original_height = 32,
        .scrolling_actif = 1,
        .cache_pos_y_niveau0 = 0,
        .cache_width_pixels = 960, // Not used
    },
    {
        // Hero de Karim
        .type = SPRITES_TYPE_STANDARD,
        .actif = 1,
        .sprite = 33,
        .first_tile = 656,
        .palette = 2,
        .width = 4,
        .height = 4,
        .picture_width = 4,
        .picture_height = 4,
        .offset_x = 0,
        .offset_y = 0,
        .first_tile_with_offset_y = 0, // offset_y * picture_width
        .x = 90,
        .y = 28,
        .sens = 1,
        .anim_frame_frequence = 4,
        .anim_frame_cpt = 0,
        .display = 1,
        .original_height = 4,
        .scrolling_actif = 1,
        .cache_pos_y_niveau0 = 0,
        .cache_width_pixels = 64,
    },
};

/**
 * Affichage du sprite demandé
 */
void sprite_init(u16 sprite_indice)
{
    u16 first_tile = 0;
    *REG_VRAMMOD = 1;
    char str[10];

    if (sprites[sprite_indice].actif == 1){

        if (sprites[sprite_indice].display == 0 )
        {
            sprites[sprite_indice].height = 0;
        }

        if (sprites[sprite_indice].display == 1)
        {
            sprites[sprite_indice].height = sprites[sprite_indice].original_height;
        }

        // On prend en compte l'offset pour se positionner sur la portion de l'image complete
        first_tile = sprites[sprite_indice].first_tile + sprites[sprite_indice].offset_x;
        first_tile += sprites[sprite_indice].first_tile_with_offset_y;

        if (sprites[sprite_indice].type == SPRITES_TYPE_STANDARD)
        {
            for (u16 s = 0; s < sprites[sprite_indice].width; s++)
            {
                *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + s) << 6);
                for (u16 v = 0; v < sprites[sprite_indice].height; v++)
                {
                    *REG_VRAMRW = first_tile + s + (v * sprites[sprite_indice].picture_width);
                    *REG_VRAMRW = (sprites[sprite_indice].palette << 8);
                }
            }
        }
        else if (sprites[sprite_indice].type == SPRITES_TYPE_BACKGROUND1){

            // On initialise les tiles depuis le TMX tmx_mario
            for (u16 s = 0; s < 20; s++)
            {
                *REG_VRAMMOD = 1;
                *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + s) << 6);
                for (u16 v = sprites[sprite_indice].height; v > 0; v--)
                {
                    u16 new_v = MAP_HEIGHT_TILES - v;
                    *REG_VRAMRW = first_tile + tmx_mario[new_v][s] - 1;
                    *REG_VRAMRW = (sprites[sprite_indice].palette << 8);
                    //*REG_VRAMRW = (palette_tmp << 8);
                }
            }
        }

        u16 pos_y_niveau0 = ((240 - (sprites[sprite_indice].height << 4)) * -1) + sprites[sprite_indice].y;

        *REG_VRAMMOD = 0x200;
        *REG_VRAMADDR = ADDR_SCB2 + sprites[sprite_indice].sprite;
        *REG_VRAMRW = 0xFFF;
        *REG_VRAMRW = (pos_y_niveau0 << 7) + sprites[sprite_indice].height;
        *REG_VRAMRW = (sprites[sprite_indice].x << 7);

        // --- On chaine l'ensemble des sprites
        for (u16 v = 1; v < sprites[sprite_indice].width; v++)
        {
            *REG_VRAMADDR = ADDR_SCB2 + sprites[sprite_indice].sprite + v;
            *REG_VRAMRW = 0xFFF;
            *REG_VRAMRW = 1 << 6; // sticky
        }
    }
}

void sprite_change_tile_in_a_colonne(u16 sprite_indice, u16 indice, u16 VRAMSpriteDrawStartIndex, u16 new_tile)
{
    *REG_VRAMMOD = 1;
    *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + indice) << 6) + (VRAMSpriteDrawStartIndex << 1);
    *REG_VRAMRW = 256 + new_tile - 1;
    *REG_VRAMRW = (sprites[sprite_indice].palette << 8);
}

/**
 * Permet d'actualiser à partir du TMX une colonne spécifique d'un sprite
 * et surtout hide tous les autres sprites après
 */
void sprite_update_tiles_left_from_one_sprite(u16 sprite_indice, u16 indice)
{
    u16 first_tile = 0;
    u16 tmp;

    tmp = indice;
    if (tmp >= 32)
    {
        tmp -= 32;
    }
    if (tmp >= 64)
    {
        tmp -= 64;
    }

    // On prend en compte l'offset pour se positionner sur la portion de l'image complete
    first_tile = sprites[sprite_indice].first_tile + sprites[sprite_indice].offset_x;
    first_tile += sprites[sprite_indice].first_tile_with_offset_y;

    *REG_VRAMMOD = 1;
    *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + tmp) << 6);
    for (u16 v = sprites[sprite_indice].height; v > 0; v--)
    {
        u16 new_v = MAP_HEIGHT_TILES - v;
        *REG_VRAMRW = first_tile + tmx_mario[new_v][indice] - 1;
        *REG_VRAMRW = (sprites[sprite_indice].palette << 8);
    }
}

void change_colonne(u16 sprite_indice, u16 sprite_to_change, u16 first_tile, u16 x)
{
    *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + sprite_to_change) << 6);
    for (u16 v = sprites[sprite_indice].height; v > 0; v--)
    {
        u16 new_v = MAP_HEIGHT_TILES - v;
        *REG_VRAMRW = first_tile + tmx_mario[new_v][x] - 1;
        *REG_VRAMRW = (sprites[sprite_indice].palette << 8);
    }
}

/**
 * Permet d'actualiser à partir du TMX une colonne spécifique d'un sprite
 * et surtout hide tous les autres sprites après
 */
void sprite_update_tiles_right_from_one_sprite(u16 sprite, u16 indice)
{
    char str[10];
    u16 first_tile = 0;
    u16 sprite_to_change = 0;

    sprite_to_change = indice + 20;

    snprintf(str, 10, "ind : %3d", indice);
    ng_text(2, 3, 0, str);
    //snprintf(str, 10, "x : %3d", x);
    //ng_text(2, 5, 0, str);

    // On prend en compte l'offset pour se positionner sur la portion de l'image complete
    first_tile = sprites[sprite].first_tile + sprites[sprite].offset_x;
    first_tile += sprites[sprite].first_tile_with_offset_y;

    *REG_VRAMMOD = 1;

    if (indice >= 0 && indice < 12)
    {
        change_colonne(sprite, sprite_to_change, first_tile, indice + 20);
    }
    else if (indice >= 12 && indice < 44)
    {
        change_colonne(sprite, sprite_to_change - 32, first_tile, indice + 20);
    }
    else if (indice >= 44 && indice < 76)
    {
        change_colonne(sprite, sprite_to_change - 64, first_tile, indice + 20);
    }
    else if (indice >= 76 && indice < 108)
    {
        change_colonne(sprite, sprite_to_change - 96, first_tile, indice + 20);
    }
}

/**
 * Hide a sprite
 * indice commence à 0 à partir du sprite passé en paramètre
 */
void sprite_hide(u16 sprite_indice, u16 indice)
{
    *REG_VRAMADDR = ADDR_SCB3 + sprites[sprite_indice].sprite + indice;
    *REG_VRAMRW = 0;
}

/**
 * Display a sprite
 * indice comment à 0 à partir du sprite passé en paramètre
 */
void sprite_display(u16 sprite_indice, u16 indice)
{
    *REG_VRAMADDR = ADDR_SCB3 + sprites[sprite_indice].sprite + indice;
    *REG_VRAMRW = (240 << 7);
}

void sprite_update_tiles(u16 sprite_indice)
{
    u16 first_tile = 0;
    *REG_VRAMMOD = 1;

    if (sprites[sprite_indice].actif == 1){

        // On prend en compte l'offset pour se positionner sur la portion de l'image complete
        first_tile = sprites[sprite_indice].first_tile + sprites[sprite_indice].offset_x;
        first_tile += sprites[sprite_indice].first_tile_with_offset_y;

        for (u16 s = 0; s < sprites[sprite_indice].width; s++)
        {
            *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + s) << 6);
            for (u16 v = 0; v < sprites[sprite_indice].height; v++)
            {
                *REG_VRAMRW = first_tile + s + (v * sprites[sprite_indice].picture_width);
                *REG_VRAMRW = (sprites[sprite_indice].palette << 8);
            }
        }
    }
}

void sprite_update_x(u16 sprite_indice)
{
    if (sprites[sprite_indice].actif == 1){
        *REG_VRAMMOD = 0x200;
        *REG_VRAMADDR = ADDR_SCB4 + sprites[sprite_indice].sprite;
        *REG_VRAMRW = (sprites[sprite_indice].x << 7);
    }
}

void sprite_update_y(u16 sprite_indice)
{
    if (sprites[sprite_indice].actif == 1)
    {
        s16 pos_y_niveau0 = 0;

        // On prend en compte la valeur du cache si renseignée
        if (sprites[sprite_indice].cache_pos_y_niveau0 != 0)
        {
            pos_y_niveau0 = sprites[sprite_indice].cache_pos_y_niveau0;
        }
        else
        {
            pos_y_niveau0 = ((240 - (sprites[sprite_indice].height << 4)) * -1) + sprites[sprite_indice].y;
        }

        *REG_VRAMMOD = 0x200;
        *REG_VRAMADDR = ADDR_SCB3 + sprites[sprite_indice].sprite;
        *REG_VRAMRW = (pos_y_niveau0 << 7) + sprites[sprite_indice].height; // Position Y
    }
}

void sprite_update_xy(u16 sprite_indice)
{
    s16 pos_y_niveau0 = 0;

    if (sprites[sprite_indice].actif == 1){

        // On prend en compte la valeur du cache si renseignée
        if (sprites[sprite_indice].cache_pos_y_niveau0 != 0)
        {
            pos_y_niveau0 = sprites[sprite_indice].cache_pos_y_niveau0;
        }
        else
        {
            pos_y_niveau0 = ((240 - (sprites[sprite_indice].height << 4)) * -1) + sprites[sprite_indice].y;
        }

        *REG_VRAMMOD = 0x200;
        *REG_VRAMADDR = ADDR_SCB2 + sprites[sprite_indice].sprite;
        *REG_VRAMRW = 0xFFF;
        *REG_VRAMRW = (pos_y_niveau0 << 7) + sprites[sprite_indice].height; // Position Y
        *REG_VRAMRW = (sprites[sprite_indice].x << 7);                      // Position X
    }
}

// ecart = nombre de tiles pour chaque étape de l'animation. Dans le cas d'Arthur, c'est 2
void sprite_rollup(u16 sprite_indice, u16 ecart)
{
    if (sprites[sprite_indice].actif == 1){
        if (sprites[sprite_indice].width < sprites[sprite_indice].picture_width )
        {
            if (sprites[sprite_indice].anim_frame_cpt == 0)
            {
                sprites[sprite_indice].offset_x += ecart;
                // snprintf(str, 10, "F %3d", sprites[sprite_indice].offset_x); ng_text(2, 3, 0, str);
                // snprintf(str, 10, "F %3d", sprites[sprite_indice].picture_width); ng_text(2, 5, 0, str);

                if (sprites[sprite_indice].offset_x >= sprites[sprite_indice].picture_width){
                    // snprintf(str, 10, "F %3d", sprites[sprite_indice].offset_x); ng_text(2, 3, 0, str);
                    sprites[sprite_indice].offset_x = 0;
                }

                sprite_update_tiles(sprite_indice);
            }

            sprite_update_tiles(sprite_indice);
        }
        sprites[sprite_indice].anim_frame_cpt++;
        if (sprites[sprite_indice].anim_frame_cpt == sprites[sprite_indice].anim_frame_frequence)
        {
            sprites[sprite_indice].anim_frame_cpt = 0;
        }
    }
}

void sprite_update(u16 sprite_indice)
{
    if (sprites[sprite_indice].actif == 1){
        // << 4 = * 16
        u16 pos_y_niveau0 = ((240 - (sprites[sprite_indice].height << 4)) * -1) + sprites[sprite_indice].y;

        *REG_VRAMMOD = 0x200;
        *REG_VRAMADDR = ADDR_SCB2 + sprites[sprite_indice].sprite;
        *REG_VRAMRW = 0xFFF;
        *REG_VRAMRW = (pos_y_niveau0 << 7) + sprites[sprite_indice].height;
        *REG_VRAMRW = (sprites[sprite_indice].x << 7);
    }
}
