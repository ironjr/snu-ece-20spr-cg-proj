#ifndef GRAPHICS_SETTINGS_H
#define GRAPHICS_SETTINGS_H


namespace engine
{
// Defaults for graphics settings.
constexpr unsigned int DEFAULT_GRAPHICS_SHADOW_WIDTH = 2048;
constexpr unsigned int DEFAULT_GRAPHICS_SHADOW_HEIGHT = 2048;


struct GraphicsSettings
{
    // Graphics settings.
    unsigned int shadowWidth = DEFAULT_GRAPHICS_SHADOW_WIDTH;
    unsigned int shadowHeight = DEFAULT_GRAPHICS_SHADOW_HEIGHT;
    bool useLighting = true;
    bool useNormalMap = true;
    bool useSpecularMap = true;
    bool useShadow = true;
};
}

#endif
