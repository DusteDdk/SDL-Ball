/* ************************************************************************* *
    OldSkoolGravityGame (OSGG) Lunar Lander-like game for linux.
    Copyright (C) 2008 Jimmy Christensen ( dusted at dusted dot dk )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */

#include <iostream>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>


#ifndef DATADIR
    #define DATADIR "./"
#endif

#define FONT_DEFAULT 0
#define FONT_NUM 1

using namespace std;

struct glCharInfo_struct {
  GLfloat Xa,Ya,Xb,Yb, width;
};

struct glFontInfo_struct {
  GLuint tex;
  GLfloat height;
  struct glCharInfo_struct ch[255];
};

class glTextClass {
  private:
    void genFontTex(string TTFfontName, int fontSize, int font);
    struct glFontInfo_struct fontInfo[FONT_NUM];

  public:
    GLfloat getHeight(int font);
    void write(string text, int font, bool center, GLfloat scale, GLfloat x, GLfloat y);
    glTextClass();
    ~glTextClass();
};
