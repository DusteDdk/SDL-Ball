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


class highScoreClass {
  private:
    string name;
    textureManager texMgr;
    textureClass tex;
  public:
  
    highScoreClass()
    {
      name="";
      texMgr.load(useTheme("/gfx/highscore/entername.png",setting.gfxTheme), tex);
    }
  
    void draw()
    {

      glLoadIdentity();
      glTranslatef(0,0,-3.0);
      glColor4f(1,1,1,1);
 
      glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.5f, 0.0f );
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 0.5f, 0.0f );
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,-0.5f, 0.0f );
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,-0.5f, 0.0f );
      glEnd( );      


      glColor4f(1,1,1,1);
      glText->write(name, FONT_MENUHIGHSCORE, 1, 2.0, 0.0, 0.0);
      
    }
    
    void type(SDL_Event e, menuClass &menu)
    {
      if(var.showHighScores == 1)
      {
        if(e.key.keysym.sym == SDLK_ESCAPE)
        {
          var.showHighScores = 0;
          initNewGame();
          menu.refreshHighScoreList();
          var.menu = 7;
          return;
        }
        
        if(e.key.keysym.sym != SDLK_RETURN)
        {
          if ( (e.key.keysym.unicode & 0xFF80) == 0 && e.key.keysym.unicode != 0) {
            if(e.key.keysym.sym == SDLK_BACKSPACE)
            {
              if(name.length() > 0)
                name.erase(name.length()-1);
            } else {
              name += e.key.keysym.unicode;
            }
          }
        } else {
          var.showHighScores = 0;
          ofstream hsList;
          hsList.open(privFile.highScoreFile.data(), ios::out | ios::app);
          hsList << "[" << player.level+1 << "]" << player.score << "|" << name << endl;
          hsList.close();
          initNewGame();
          menu.refreshHighScoreList();
          resumeGame();
          var.titleScreenShow=1;
        }
      }
    }

    bool isHighScore()
    {
      int n;
      int high=0;
      struct score * slist = sortScores(&n);
      for(int i = 0; i < n; i++)
      {
        if(player.score < slist[i].score)
        {
          high++;
        }
      }
      
      if(n>0)
        delete[] slist;
      
      if(high < 20)
      {
        return(1);
      }
      return(0);
    }
};
