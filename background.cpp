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


class backgroundClass {
  private:
  GLfloat r[4], g[4], b[4], a;

  textureClass tex;
  bool dir;
  bool firstTexture;
  public:

    
    backgroundClass()
    {
      firstTexture=1;
    }
    
    void init(textureManager & texMgr)
    {
      bool l;
      //Så vi ikke kommer til at slette texture 0 første gang funktionen bliver kaldt
      if(!firstTexture)
        glDeleteTextures(1, &tex.prop.texture);
      
      float t = 0.25 * ((float)player.level+1);

      if(t <= 1.0)
        l=texMgr.load(useTheme("/gfx/bg/1.jpg",setting.gfxTheme), tex);
      else if(t <= 2.0)
        l=texMgr.load(useTheme("/gfx/bg/2.jpg",setting.gfxTheme), tex);
      else if(t <= 3.0)
        l=texMgr.load(useTheme("/gfx/bg/3.jpg",setting.gfxTheme), tex);
      else if(t <= 4.0)
        l=texMgr.load(useTheme("/gfx/bg/4.jpg",setting.gfxTheme), tex);
      else if(t <= 5.0)
        l=texMgr.load(useTheme("/gfx/bg/5.jpg",setting.gfxTheme), tex);
      else if(t <= 6.0)
        l=texMgr.load(useTheme("/gfx/bg/6.jpg",setting.gfxTheme), tex);
      else if(t <= 7.0)
        l=texMgr.load(useTheme("/gfx/bg/7.jpg",setting.gfxTheme), tex);
      else if(t <= 8.0)
        l=texMgr.load(useTheme("/gfx/bg/8.jpg",setting.gfxTheme), tex);
      else if(t <= 9.0)
        l=texMgr.load(useTheme("/gfx/bg/9.jpg",setting.gfxTheme), tex);
      else if(t <= 10.0)
        l=texMgr.load(useTheme("/gfx/bg/10.jpg",setting.gfxTheme), tex);
      else if(t <= 11.0)
        l=texMgr.load(useTheme("/gfx/bg/11.jpg",setting.gfxTheme), tex);
      else if(t <= 12.0)
        l=texMgr.load(useTheme("/gfx/bg/12.jpg",setting.gfxTheme), tex);
      else if(t <= 13.0)
        l=texMgr.load(useTheme("/gfx/bg/13.jpg",setting.gfxTheme), tex);
      else if(t <= 14.0)
        l=texMgr.load(useTheme("/gfx/bg/14.jpg",setting.gfxTheme), tex);
      else if(t <= 15.0)
        l=texMgr.load(useTheme("/gfx/bg/15.jpg",setting.gfxTheme), tex);
      else if(t <= 16.0)
        l=texMgr.load(useTheme("/gfx/bg/16.jpg",setting.gfxTheme), tex);
      else if(t <= 17.0)
        l=texMgr.load(useTheme("/gfx/bg/17.jpg",setting.gfxTheme), tex);
      else if(t <= 18.0)
        l=texMgr.load(useTheme("/gfx/bg/18.jpg",setting.gfxTheme), tex);
      else if(t <= 19.0)
        l=texMgr.load(useTheme("/gfx/bg/19.jpg",setting.gfxTheme), tex);
      else if(t <= 20.0)
        l=texMgr.load(useTheme("/gfx/bg/20.jpg",setting.gfxTheme), tex);
      else if(t <= 21.0)
        l=texMgr.load(useTheme("/gfx/bg/21.jpg",setting.gfxTheme), tex);
      else if(t <= 22.0)
        l=texMgr.load(useTheme("/gfx/bg/22.jpg",setting.gfxTheme), tex);
      else if(t <= 23.0)
        l=texMgr.load(useTheme("/gfx/bg/23.jpg",setting.gfxTheme), tex);
      else if(t <= 24.0)
        l=texMgr.load(useTheme("/gfx/bg/24.jpg",setting.gfxTheme), tex);
      else if(t <= 25.0)
        l=texMgr.load(useTheme("/gfx/bg/25.jpg",setting.gfxTheme), tex);
      else
        l=texMgr.load(useTheme("/gfx/bg/final.jpg",setting.gfxTheme), tex);
      
      if(!l)
      {
        setting.showBg=0;
        cout << "Backgrounds disabled." << endl;
      }

      firstTexture=0;

      for(int i = 0; i  < 4; i++)
      {
        r[i] = rndflt(1,0);
        g[i] = rndflt(1,0);
        b[i] = rndflt(1,0);
      }
      a = 1;

    }

    void draw()
    {

      glLoadIdentity();
      glTranslatef( 0, 0, -3.0 );
  
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
      glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
      
      glBegin( GL_QUADS );
        glColor4f(r[0],g[0],b[0],a); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.66, 1.25, 0.0f );
        glColor4f(r[1],g[1],b[1],a); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.66, 1.25, 0.0f );
        glColor4f(r[2],g[2],b[2],a); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.66,-1.25, 0.0f );
        glColor4f(r[3],g[3],b[3],a); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.66,-1.25, 0.0f );
      glEnd( );
    }
};
