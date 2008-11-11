/* ************************************************************************* *
    SDL-Ball - DX-Ball/Breakout remake with openGL and SDL for Linux
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

#define FONT_MENU 0
#define FONT_ANNOUNCE_GOOD 1
#define FONT_ANNOUNCE_BAD 2
#define FONT_HIGHSCORE 3
#define FONT_MENUHIGHSCORE 4
#define FONT_INTROHIGHSCORE 5
#define FONT_INTRODESCRIPTION 6
#define FONT_NUM 7

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
};

GLfloat glTextClass::getHeight(int font)
{
  return(fontInfo[font].height*2.0);
}

glTextClass::glTextClass()
{
  TTF_Init();
  //Parse font-description file
  ifstream f;
  string line,set,val,tempName;
  
  f.open( useTheme("/font/fonts.txt",setting.gfxTheme).data() );
  if(f.is_open())
  {
    while(!f.eof())
    {
      getline(f, line);
      if(line.find('=') != string::npos)
      {
        set=line.substr(0,line.find('='));
        val=line.substr(line.find('=')+1);
        if(set=="menufile")
        {
          tempName=val;
        } else if(set=="menusize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_MENU);
        } else if(set=="announcegoodfile")
        {
          tempName=val;
        } else if(set=="announcegoodsize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_ANNOUNCE_GOOD);
        
        } else if(set=="announcebadfile")
        {
          tempName=val;
        } else if(set=="announcebadsize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_ANNOUNCE_BAD);
        } else if(set=="highscorefile")
        {
          tempName=val;
        } else if(set=="highscoresize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_HIGHSCORE);
        } else if(set=="menuhighscorefile")
        {
          tempName=val;
        } else if(set=="menuhighscoresize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_MENUHIGHSCORE);
        } else if(set=="introhighscorefile")
        {
          tempName=val;
        } else if(set=="introhighscoresize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_INTROHIGHSCORE);
        } else if(set=="introdescriptionfile")
        {
          tempName=val;
        } else if(set=="introdescriptionsize")
        {
          genFontTex(tempName, atoi(val.data()), FONT_INTRODESCRIPTION);
        }
      }
    }
    f.close();
  } else {
    cout << "Error: could not open font-description file.";
  }
  TTF_Quit();
}

void glTextClass::genFontTex(string TTFfontName, int fontSize, int font)
{
  TTF_Font *ttfFont = NULL;
  SDL_Surface *c, *t;
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
  
  ttfFont = TTF_OpenFont( useTheme(TTFfontName,setting.gfxTheme).data(), fontSize );
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
      if(dst.y > 512 && font != FONT_HIGHSCORE) //FONT_HIGHSCORE is always rendered too big for texture
      {
        cout << "Too many chars for tex ("<<i<<")"<<endl;
      }
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
 
  glBindTexture(GL_TEXTURE_2D, fontInfo[font].tex);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, t->w, t->h, GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);

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
}
