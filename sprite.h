typedef struct _sprite_t
{
    u8 type;  // 1 = sprite standard, 2 = background from TMX
    u8 actif; // Désactivation complete du sprite

    u16 sprite;     // Id du first sprite 43
    u16 first_tile; // Find tile number with https://neospriteviewer.mattgreer.dev

    u16 palette;

    u16 width; // Width of the sprite
    u16 height; // Height of the sprite

    u16 picture_width; // Total Number of tiles de 16px de l'image PNG
    u16 picture_height; // Total Number of tiles de 16px de l'image PNG

    u16 offset_x; // Permet de spécifier la ligne dans l'image PNG. Par défaut 0
    u16 offset_y; // Permet de spécifier la colonne dans l'image PNG. Par défaut 0

    u16 first_tile_with_offset_y; // offset_y * picture_width

    s16 x; // Position X on the screen. 0 is left
    s16 y; // Position Y on the screen. 0 is bottom with this function

    u16 sens; // 0 left or 1 right
    //u8 state; // CACHEE

    // Variables dédiées à l'animation
    u16 anim_frame_frequence;
    u16 anim_frame_cpt;

    u8 display;

    u16 original_height; // Hauteur d'origine, permet de reset le field height quand le sprite a été réduit à 0 pour etre caché

    // Variable pour le scrolling
    u8 scrolling_actif; // Si scrolling alors le sprite va bouger avec

    // Variable pour le cache et éviter des calculs
    s16 cache_pos_y_niveau0; // Nombre signé
    u16 cache_width_pixels; // Largeur du sprite en pixels (utilisé pour cacher le sprite s'il est en dehors de l'écran)

    // Remplissaage à partir d'un TMX
    // u16 tmx[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];
    // tmx *StartTiles = tmx_mario[LevelTileStartIndexX][LevelTileStartIndexY]

} sprite_t;

extern sprite_t sprites[SPRITES_ALLOCATED];