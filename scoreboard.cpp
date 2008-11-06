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
// #include <list>


class glScoreBoard
{
  private:
  unsigned int lastScoreTick;
  uint tempScore;
  uint score;
  char tempText[255];
  
  int DEBUGCALL; //DEBUG_EVG
  
  public:
  bool DEBUG; //DEBUG_EVG
  

  glScoreBoard()
  {
    init();
  }
  
  void init()
  {
    DEBUG=0;//DEBUG_EVG
    DEBUGCALL=0;//DEBUG_EVG
    tempScore=1;
    score=0;
    lastScoreTick = SDL_GetTicks();
  }
  
  void update(int point)
  {
    score=point;
  }
  
  void draw()
  {
    int dif=score - tempScore;

    if(tempScore != score )
    {
      if(lastScoreTick + 50 < SDL_GetTicks())
      {
        tempScore+= (float)dif/7.0 +1; ;
        if(tempScore > score)
          tempScore=score;
        lastScoreTick = SDL_GetTicks();

        sprintf(tempText, "%i", tempScore);
      }
    }
    
    glLoadIdentity();
    glTranslatef(-1.55, 1.24-(glText->getHeight(FONT_HIGHSCORE)/2.0), -3.0);
        
    glColor4f(1.0,1.0,1.0,1.0);
    glText->write(tempText, FONT_HIGHSCORE, 0, 1.0, 0.0, 0.0);
    
    //The texture from glText is still bound DEBUG_EVG -->
    if(DEBUG)
    {
      DEBUG=0;
      GLint id;
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
      DEBUGCALL++;
      cout << "(" << DEBUGCALL <<")DebugTextureId:" << id << endl;
    }
    if(var.debugChars)
    {
      glLoadIdentity( );
      glTranslatef(0.0, 0.0, -3.0);
      glBegin( GL_QUADS );
        glTexCoord2f( 0.0,0.0 ); glVertex3f(-1, 1,0);
        glTexCoord2f( 1.0,0.0 ); glVertex3f( 1, 1,0);
        glTexCoord2f( 1.0,1.0 ); glVertex3f( 1,-1,0);
        glTexCoord2f( 0.0,1.0 ); glVertex3f(-1,-1,0);
      glEnd( );
    }
    //<<--- DEBUG_EVG
    
  }
};

