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

class titleScreenClass {
  private:
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    textureManager texMgr;
    textureClass texTitle;
  public:
    titleScreenClass(effectManager *m);
    void draw(int * frameAge);
};

titleScreenClass::titleScreenClass(effectManager *m)
{
  fxMan = m;
  ticksSinceLastSpawn=100;
  texMgr.load(DATADIR"/gfx/title.png", texTitle);
  
 
}

void titleScreenClass::draw(int * frameAge)
{
  pos p,s;
  if(*frameAge > 8)
  {
    if(var.clearScreen)
    {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
          glDisable(GL_TEXTURE_2D);

        
    glBegin( GL_QUADS );
      glColor3d(0,0,0);
      glVertex3f(-1.66,  0.0, 0.0);
      glVertex3f( 1.66,  0.0, 0.0);
      glColor3d(0,0,0.1);
      glVertex3f( 1.66, -1.25, 0.0);
      glVertex3f(-1.66, -1.25, 0.0);
    glEnd();
        
    

    ticksSinceLastSpawn +=nonpausingGlobalTicks;
    if(ticksSinceLastSpawn > 10)
    {
      s.x = 3.2;
      s.y = 0.6;
      p.x = 0;
      p.y = 1;

      ticksSinceLastSpawn=0;
      fxMan->set(FX_VAR_TYPE, FX_PARTICLEFIELD);
      fxMan->set(FX_VAR_COLDET, 0);
      fxMan->set(FX_VAR_LIFE, 1000);
      fxMan->set(FX_VAR_NUM, 100);
      fxMan->set(FX_VAR_SIZE, 0.02f);
      fxMan->set(FX_VAR_SPEED, 0.5f);
      fxMan->set(FX_VAR_GRAVITY, 0.0f);
      fxMan->set(FX_VAR_RECTANGLE, s);
      fxMan->set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f);
      fxMan->spawn(p);
    }
    glEnable(GL_TEXTURE_2D);
        
    fxMan->draw();
    
    glColor4f(1.0,1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, texTitle.prop.texture);
    glBegin( GL_QUADS );
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.2,1.2, 0.0f );
        glTexCoord2f(1.0, 0.0); glVertex3f( 1.2,1.2, 0.0f );
        glColor4f(1.0,1.0,1.0,0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f( 1.2,0.8, 0.0f );
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.2,0.8, 0.0f );
    glEnd( );

    SDL_GL_SwapBuffers( );
    
    globalTicksSinceLastDraw=0;
    globalMilliTicksSinceLastDraw=0;
    *frameAge = 0;
  }
}
