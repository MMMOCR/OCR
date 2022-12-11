//
// Created by rigole on 01/11/22.
//

#include "colors.h"

#define WHITE color_rgb(255, 255, 255)

rgba
color_rgba(char r, char g, char b, char a)
{
    rgba col = {r, g, b , a};
    return col;
}

rgba
color_rgb(char r, char g, char b)
{
    rgba col = {r, g, b, 255};
    return col;
}