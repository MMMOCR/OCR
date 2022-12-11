//
// Created by rigole on 01/11/22.
//

#ifndef SYPBC_COLORS_H
#define SYPBC_COLORS_H

struct {
    unsigned char r, g, b, a;
} typedef rgba;

rgba color_rgba(char r, char g, char b, char a);
rgba color_rgb(char r, char g, char b);

#endif //SYPBC_COLORS_H
