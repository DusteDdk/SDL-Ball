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

struct glQuad {
  GLfloat x[2],y[2],z;
  int frame;
};

class glScoreBoard
{
  private:
  unsigned int lastScoreTick;
  
  uint tempScore;
  list<struct glQuad> glNum;
  int numQuads;

  textureManager texMgr;
  textureClass tex;
  uint score;
  public:
  

  glScoreBoard()
  {
    texMgr.load(DATADIR"/gfx/highscore/numbers.png", tex);
    tex.prop.ticks=1000;
    tex.prop.cols=4;
    tex.prop.rows=4;
    tex.prop.xoffset=0.25;
    tex.prop.yoffset=0.25;
    tex.prop.frames=16;
    tex.prop.playing=0;
    
    init();
  }
  
  void init()
  {
    tempScore=1;
    
    lastScoreTick = SDL_GetTicks();
  }
  
  void update(int point)
  {
    score=point;
  }
  
  void draw()
  {
    int dif=score - tempScore;
    
    int i,n;
    struct glQuad tempNum;

    if(tempScore != score )
    {
      if(lastScoreTick + 50 < SDL_GetTicks())
      {
//         soundMan.add(SND_SCORE_TICK, 0.0);
        tempScore+= (float)dif/5.0 +1; ;
        if(tempScore >= score)
          tempScore=score;
        lastScoreTick = SDL_GetTicks();

        n = tempScore;
        i=0;
        
        //This might be slow
        glNum.clear();
        
        do
        {
          i++;
          tempNum.frame=(n%10)+1;
          tempNum.x[0]= -1.50 -0.10;
          tempNum.x[1]= -1.50 +0.10;
          tempNum.y[0]= 1.15 +0.10;
          tempNum.y[1]= 1.15 -0.10;
          tempNum.z=-0.0;
  
          glNum.push_front(tempNum);
          n /= 10;
          i++;
        } while (n > 0);
      }
    }
    
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);

    glBindTexture(GL_TEXTURE_2D, tex.prop.texture);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
    glColor4f( 1,1,1,1 );
    i=0;
    for(list<struct glQuad>::iterator it=glNum.begin(); it != glNum.end(); ++it)
    {
      tex.frame=it->frame; //+1??
      tex.play();
      glBegin( GL_QUADS );
       glTexCoord2f(tex.pos[0],tex.pos[1]); glVertex3f(it->x[0]+ (i*0.20), it->y[0], it->z);
       glTexCoord2f(tex.pos[2],tex.pos[3]); glVertex3f(it->x[1]+ (i*0.20), it->y[0], it->z);
       glTexCoord2f(tex.pos[4],tex.pos[5]); glVertex3f(it->x[1]+ (i*0.20), it->y[1], it->z);
       glTexCoord2f(tex.pos[6],tex.pos[7]); glVertex3f(it->x[0]+ (i*0.20), it->y[1], it->z);
       i++;
      glEnd( );
    }

  }
};

