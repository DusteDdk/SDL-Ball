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


#include "text.hpp"

GLfloat glTextClass::getHeight(int font)
{
  return(fontInfo[font].height*2.0);
}

glTextClass::glTextClass()
{
  TTF_Init();
  //Parse font-description file
  string line,set,val,tempName;

  genFontTex(DATADIR"Bandal.ttf", 60, FONT_DEFAULT);

  TTF_Quit();
}

glTextClass::~glTextClass()
{
    for(int i=0; i < FONT_NUM; i++)
    {
        glDeleteTextures(1, &fontInfo[i].tex);
    }
}

void glTextClass::genFontTex(string TTFfontName, int fontSize, int font)
{
  TTF_Font *ttfFont = NULL;
  SDL_Surface *c, *t=NULL;
  Uint32 rmask, gmask, bmask, amask;
  char tempChar[2] = { 0,0 };
  int sX=0,sY=0; //Size of the rendered character
  SDL_Rect src={0,0,0,0},dst={0,0,0,0};
  SDL_Color white = { 255,255,255 };

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
  #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
  #endif

  ttfFont = TTF_OpenFont( TTFfontName.data(), fontSize );

  if(!ttfFont)
  {
    cout << "Could not load font" << endl;
  }

  t = SDL_CreateRGBSurface(0, 512, 512, 32, rmask,gmask,bmask,amask);

    dst.x=1;
    dst.y=1;

  fontInfo[font].height=0.0;
  for(int i=32; i < 128; i++)
  {
    tempChar[0] = (char)i;

    //Render to surface
    c = TTF_RenderText_Blended(ttfFont, tempChar, white);
    SDL_SetAlpha(c, 0, 0);
    TTF_SizeUTF8(ttfFont, tempChar, &sX, &sY);

    src.x=0;
    src.y=0;
    src.w=sX;
    src.h=sY;


    if(dst.x + sX > 512)
    {
      dst.x=1;
      dst.y += sY+2;
    }

    fontInfo[font].ch[i].Xa = ( 1.0 / ( 512.0 / (float)dst.x ) );
    fontInfo[font].ch[i].Xb = ( 1.0 / ( 512.0 / ((float)dst.x+sX) ) );
    fontInfo[font].ch[i].Ya = ( 1.0 / ( 512.0 / (float)dst.y ) );
    fontInfo[font].ch[i].Yb = ( 1.0 / ( 512.0 / ((float)dst.y+sY) ) );
    fontInfo[font].ch[i].width = sX/800.0;

    if(sY/800.0 > fontInfo[font].height)
    {
      fontInfo[font].height = sY/800.0;
    }

    //blit
    dst.w=sX;
    dst.h=sY;
    SDL_BlitSurface(c,&src,t,&dst);

    dst.x += sX+2; // Waste some space 1 px padding around each char

    SDL_FreeSurface(c); //Free character-surface
  }

  glGenTextures(1, &fontInfo[font].tex); //Generate a gltexture for this font

 glEnable( GL_TEXTURE_2D );
 glBindTexture(GL_TEXTURE_2D, fontInfo[font].tex);

    
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);


  TTF_CloseFont(ttfFont); //Free the font
  SDL_FreeSurface(t); //Free text-surface
}

void glTextClass::write(string text, int font,bool center, GLfloat scale, GLfloat x, GLfloat y)
{
  int c;
  GLfloat sX,sY,posX=0;

  //We need to find out half of the string width in order to center
  if(center)
  {
    for(unsigned int i=0; i < text.length(); i++)
    {
      c = (unsigned int)text[i];
      sX = fontInfo[font].ch[c].width*scale;
      posX += sX;
    }
    posX *= -1;
  }
  posX += x;

  glEnable( GL_TEXTURE_2D );
  glBindTexture(GL_TEXTURE_2D, fontInfo[font].tex);


  //Draw the quads
  for(unsigned int i=0; i < text.length(); i++)
  {
    c = (unsigned int)text[i];
    sX = fontInfo[font].ch[c].width*scale;
    sY = fontInfo[font].height*scale;
    posX += sX;

    glBegin(GL_QUADS);
      glTexCoord2f( fontInfo[font].ch[c].Xa,fontInfo[font].ch[c].Ya ); glVertex3f(-sX+posX, sY+y,0);
      glTexCoord2f( fontInfo[font].ch[c].Xb,fontInfo[font].ch[c].Ya ); glVertex3f( sX+posX, sY+y,0);
      glTexCoord2f( fontInfo[font].ch[c].Xb,fontInfo[font].ch[c].Yb ); glVertex3f( sX+posX,-sY+y,0);
      glTexCoord2f( fontInfo[font].ch[c].Xa,fontInfo[font].ch[c].Yb ); glVertex3f(-sX+posX,-sY+y,0);
    glEnd( );
    posX += sX;
  }
  glDisable( GL_TEXTURE_2D );
}
