/**
 * Declaration des sprites
 */
sprite_t sprites[SPRITES_ALLOCATED] = {
    {
        // Tileset for the background - test d'affichage
        .type = 1,
        .actif = 1,
        .sprite = 1,
        .first_tile = 256, // 208 tiles
        .palette = 1,
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
        .cache_width_pixels = 960,
        .tmx = {},
    },
};

void sprite_init_tmx(u16 sprite_indice)
{
    for (u16 j = 0; j < 45; j++)
    {
        for (u16 i = 0; i < 60; i++)
        {
            sprites[sprite_indice].tmx[j][i] = tmx_decor[j][i];
        }
    }
}

/**
 * Affichage du sprite demandé
 */
void sprite_init(u16 sprite_indice)
{
    u16 first_tile = 0;
    *REG_VRAMMOD = 1;
    char str[10];

    if (sprites[sprite_indice].actif == 1){

        if (sprites[sprite_indice].display == 0 ) {
            sprites[sprite_indice].height = 0;
        }

        if (sprites[sprite_indice].display == 1){
            sprites[sprite_indice].height = sprites[sprite_indice].original_height;
        }

        if (sprites[sprite_indice].type == 0){

            // On initialise les tiles depuis l'image

            // On prend en compte l'offset pour se positionner sur la portion de l'image complete
            first_tile = sprites[sprite_indice].first_tile + sprites[sprite_indice].offset_x;
            first_tile += sprites[sprite_indice].first_tile_with_offset_y;

            // snprintf(str, 10, "W %3d", sprites[sprite_indice].picture_width);
            // ng_text(2, 3, 0, str);

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
        else if (sprites[sprite_indice].type == 1){

            // On prend en compte l'offset pour se positionner sur la portion de l'image complete
            first_tile = sprites[sprite_indice].first_tile + sprites[sprite_indice].offset_x;
            first_tile += sprites[sprite_indice].first_tile_with_offset_y;

            // On initialise les tiles depuis le TMX : 60 colonnes et 45 lignes
            for (u16 s = 0; s < sprites[sprite_indice].width; s++){
                
                *REG_VRAMMOD = 1;
                *REG_VRAMADDR = ADDR_SCB1 + ((sprites[sprite_indice].sprite + s) << 6);
                for (u16 v = sprites[sprite_indice].height; v > 0; v--)
                {
                    // Recherche de la bonne palette
                    /*for (u16 k = 0; k < sizeof(palettes_offset); k++)
                    {
                        if (plane->tmx[v][s] < palettes_offset[k][0])
                        {
                            palette_tmp = palettes_offset[k][1];
                            break;
                        }
                    }*/

                    u16 new_v = 45 - v;

                    *REG_VRAMRW = first_tile + sprites[sprite_indice].tmx[new_v][s] - 1;
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
