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

class powerupDescriptionClass :  public moving_object {
  public:
    powerupDescriptionClass();
    textureClass *tex;
    void draw();
    string name;
    string description;
};

powerupDescriptionClass::powerupDescriptionClass()
{
   width=0.055;
   height=0.055;
}

void powerupDescriptionClass::draw()
{
      tex->play();
      glBindTexture( GL_TEXTURE_2D, tex->prop.texture);
      glColor4f(tex->prop.glTexColorInfo[0],tex->prop.glTexColorInfo[1],tex->prop.glTexColorInfo[2],tex->prop.glTexColorInfo[3]);
      glBegin( GL_QUADS );
        glTexCoord2f(tex->pos[0],tex->pos[1]);glVertex3f( -width+posx, height+posy, 0.00 ); // øverst venst
        glTexCoord2f(tex->pos[2],tex->pos[3]);glVertex3f(  width+posx, height+posy, 0.00 ); // øverst højre
        glTexCoord2f(tex->pos[4],tex->pos[5]);glVertex3f(  width+posx,-height+posy, 0.00 ); // nederst højre
        glTexCoord2f(tex->pos[6],tex->pos[7]);glVertex3f( -width+posx,-height+posy, 0.00 ); // nederst venstre
      glEnd( );
      
      glColor4f(1.0, 1.0, 1.0, 1.0);
      //Write her
      glText->write(name, FONT_INTRODESCRIPTION, 0, 1.0, posx+width, posy+(glText->getHeight(FONT_INTRODESCRIPTION)/2.0));
      glText->write(description, FONT_INTRODESCRIPTION, 0, 1.0, posx+width, posy-(glText->getHeight(FONT_INTRODESCRIPTION)/2.0));
}

class titleScreenClass {
  private:
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    textureManager texMgr;
    textureClass texTitle;
    textureClass *texPowerups;
    GLuint glTitleList;
    float rot;
    bool rotDir;
    powerupDescriptionClass powerUp[MAXPOTEXTURES];
    int numHighScores; //Number of highscores to show in the intro
    struct pos runnerPos;
    menuClass *menu; //Here is the highscore text
    int runnerTime;
    float runnerVelX,runnerVelY;
    int hilight;
    bool hilightDir;
    int hilightTime;
    void readDescriptions(powerupDescriptionClass po[]);
  public:
    titleScreenClass(effectManager *m, textureClass tp[], menuClass *me);
    void draw(int * frameAge, int * maxFrameAge);
};

titleScreenClass::titleScreenClass(effectManager *m, textureClass tp[], menuClass *me)
{
  menu = me;
  numHighScores=7;
  texPowerups = tp;
  fxMan = m;
  ticksSinceLastSpawn=100;
  texMgr.load(useTheme("/gfx/title/title.png",setting.gfxTheme), texTitle);
  glTitleList = glGenLists(1);
  glNewList(glTitleList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, texTitle.prop.texture);
    glBegin( GL_QUADS );
      for(int i=0; i < 32; i++)
      {
        glColor4f(1,1,1,0.1);
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.2,1.15, 0.005*i );
        glTexCoord2f(1.0, 0.0); glVertex3f( 1.2,1.15, 0.005*i );
        glColor4f(0,0,1,0.00);
        glTexCoord2f(1.0, 1.0); glVertex3f( 1.2, 0.75, 0.005*i );
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.2, 0.75, 0.005*i );
      }
    glEnd( );
  glEndList();


  for(int ii = 0; ii < 3; ii++)
  {
    for(int i=0; i < 7; i++)
    {
      powerUp[i+(7*ii)].tex = &texPowerups[i+(7*ii)];
      powerUp[i+(7*ii)].posx = -1.5 + (0.8*ii);
      powerUp[i+(7*ii)].posy = -0.35 - (0.135*i);
    }
  }
  readDescriptions(powerUp);


  runnerPos.x=0.0;
  runnerPos.y=0.66;
  runnerVelX = rndflt(2,1)+2;
  runnerVelY = rndflt(2,1)+2;
  runnerTime=0;
  
  hilight=0;
  hilightDir=1;
  hilightTime=0;
}

int delta(int a, int b)
{
  if(a >= b)
  {
    return(a-b);
  } else {
    return(b-a);
  }
}

void titleScreenClass::draw(int * frameAge, int * maxFrameAge)
{
  pos p,s;
  if(*frameAge >= *maxFrameAge)
  {
          soundMan.play();

    if(var.clearScreen)
    {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glEnable(GL_TEXTURE_2D);
    
    ticksSinceLastSpawn +=globalTicksSinceLastDraw;
    if(ticksSinceLastSpawn > 125)
    {
      s.x = 3.25;
      s.y = 0.525;
      p.x = 0;
      p.y = 1;

      ticksSinceLastSpawn=0;
      fxMan->set(FX_VAR_TYPE, FX_PARTICLEFIELD);
      fxMan->set(FX_VAR_COLDET, 0);
      fxMan->set(FX_VAR_LIFE, 1500);
      fxMan->set(FX_VAR_NUM, 50);
      fxMan->set(FX_VAR_SIZE, 0.01f);
      fxMan->set(FX_VAR_SPEED, 0.0f);
      fxMan->set(FX_VAR_GRAVITY, 0.0f);
      fxMan->set(FX_VAR_RECTANGLE, s);
      fxMan->set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f);
      fxMan->spawn(p);
    }
    fxMan->draw();

    int i;
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);

    for(i=0; i < MAXPOTEXTURES; i++)
    {
      powerUp[i].draw();
    }
    
    runnerTime+=globalTicksSinceLastDraw;
    if(runnerTime>10)
    {
      fxMan->set(FX_VAR_TYPE, FX_SPARKS);
      fxMan->set(FX_VAR_COLDET,0);
      fxMan->set(FX_VAR_LIFE, 250);
      fxMan->set(FX_VAR_NUM, 2);
      fxMan->set(FX_VAR_SIZE, 0.09f);
      fxMan->set(FX_VAR_SPEED, 0.5f);
      fxMan->set(FX_VAR_GRAVITY, -0.4f);
  
      fxMan->set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
      fxMan->spawn(runnerPos);
      fxMan->set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
      fxMan->spawn(runnerPos);
      fxMan->set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
      fxMan->spawn(runnerPos);
      fxMan->set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
      fxMan->spawn(runnerPos);
      
      runnerPos.x += runnerVelX*(runnerTime/1000.0);
      runnerPos.y += runnerVelY*(runnerTime/1000.0);
      if(runnerPos.x > 1.64 && runnerVelX > 0)
      {
        runnerVelX *= -1;
      }
      if(runnerPos.x < -1.64 && runnerVelX < 0)
      {
        runnerVelX *= -1;
      }
      
      if(runnerPos.y > 1.24 && runnerVelY > 0)
      {
        runnerVelY *= -1;
      }
      
      if(runnerPos.y < -1.24 && runnerVelY < 0)
      {
        runnerVelY *= -1;
      }
      runnerTime=0;
    }
   
  hilightTime += globalTicksSinceLastDraw;
  if(hilightTime > 50)
  {
    if(hilightDir)
    {
      hilight++;
      if(hilight == numHighScores*3)//-1)
      {
        hilight= numHighScores-1;
        hilightDir=0;
      }
    } else {
      hilight--;
      if(hilight == -numHighScores*2)
      {
        hilightDir=1;
        hilight=0;
      }
    }
    hilightTime=0;
  }
   
    glTranslatef(0.0, 0.59, 0.0);
    float a;
    for(i=0; i < numHighScores; i++)
    {
      if((hilightDir && i < hilight+1) || (!hilightDir && i > hilight-1))
      {
        a=1.0-((1.0/(float)(numHighScores*2))*(delta(hilight,i)));
        glColor4f(1,1,1,a);
        glText->write(menu->highScores[i], FONT_INTROHIGHSCORE, 1, 1.0, 0.0, 0.0);
      }
      glTranslatef(0.0,-glText->getHeight(FONT_INTROHIGHSCORE),0.0);
   }
   
   //Draw the info string with website and version
   glColor4f(1,1,0,1);
   glText->write("Visit http://sdl-ball.sf.net/ for info, help and updates!", FONT_INTROHIGHSCORE, 1, 1.0, 0,0);
   
    if(!rotDir)
    {
      rot += 0.01 * globalTicksSinceLastDraw;
      if(rot > 40)
      {
        rotDir=1;
      }
    } else {
      rot -= 0.01 * globalTicksSinceLastDraw;
      if(rot < -40)
      {
        rotDir=0;
      }
    }
    glLoadIdentity();
    
    glTranslatef(0.0,0.0,-3.0);
    
    glRotatef(20, 1,0,0);
    glRotatef(rot, 0,1,0);
    glCallList(glTitleList);
    
    SDL_GL_SwapBuffers( );
    
    globalTicksSinceLastDraw=0;
    globalMilliTicksSinceLastDraw=0;
    *frameAge = 0;
    
  }
}


void titleScreenClass::readDescriptions(powerupDescriptionClass po[])
{
  ifstream f;
  string line;
  int p=0;
  bool flip=0;
  
  f.open( useTheme("/powerupdescriptions.txt", setting.gfxTheme).data() );
  if(f.is_open())
  {
    while(!f.eof())
    {
      getline(f, line);
      if(!flip)
      {
        flip=1;
        po[p].name=line;
      } else {
        flip=0;
        po[p].description=line;
      p++;
      }
      if(p == MAXPOTEXTURES)
        break;
    }
    f.close();
  } else {
    cout << "Could not open powerupdescriptions"<<endl;
  }
}