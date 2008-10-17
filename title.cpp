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
    textureClass *text;
    void draw();
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
      glColor4f(1.0, 1.0, 1.0, 1.0);
      glBegin( GL_QUADS );
        glTexCoord2f(tex->pos[0],tex->pos[1]);glVertex3f( -width+posx, height+posy, 0.00 ); // øverst venst
        glTexCoord2f(tex->pos[2],tex->pos[3]);glVertex3f(  width+posx, height+posy, 0.00 ); // øverst højre
        glTexCoord2f(tex->pos[4],tex->pos[5]);glVertex3f(  width+posx,-height+posy, 0.00 ); // nederst højre
        glTexCoord2f(tex->pos[6],tex->pos[7]);glVertex3f( -width+posx,-height+posy, 0.00 ); // nederst venstre
      glEnd( );
      
      text->play();
      glBindTexture( GL_TEXTURE_2D, text->prop.texture);
      glBegin( GL_QUADS );//0.58 x 0.11
        glTexCoord2f(text->pos[0],text->pos[1]);glVertex3f(  width+posx, height+posy, 0.00 );
        glTexCoord2f(text->pos[2],text->pos[3]);glVertex3f(  0.58+width+posx, height+posy, 0.00 );
        glTexCoord2f(text->pos[4],text->pos[5]);glVertex3f(  0.58+width+posx,-height+posy, 0.00 );
        glTexCoord2f(text->pos[6],text->pos[7]);glVertex3f( width+posx,-height+posy, 0.00 );
      glEnd( );

}

class titleScreenClass {
  private:
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    textureManager texMgr;
    textureClass texTitle;
    textureClass *texPowerups;
    GLuint glTitleList;
    textureClass texText[MAXPOTEXTURES];
    float rot;
    bool rotDir;
    powerupDescriptionClass powerUp[MAXPOTEXTURES];
    GLuint *texHighScore; //Pointer to the texture array in menuClass, where they are updated.
    int *numHighScores; //Pointer to the number of highscores in menuClass.
    struct pos runnerPos;
    int runnerTime;
    float runnerVelX,runnerVelY;
    int hilight;
    bool hilightDir;
    int hilightTime;
  public:
    titleScreenClass(effectManager *m, textureClass tp[], menuClass *me);
    void draw(int * frameAge, int * maxFrameAge);
};

titleScreenClass::titleScreenClass(effectManager *m, textureClass tp[], menuClass *me)
{
  texHighScore = me->titleHighscoreTex;
  numHighScores = &me->numHighScores;
  texPowerups = tp;
  fxMan = m;
  ticksSinceLastSpawn=100;
  texMgr.load(useTheme("/gfx/title/title.png",setting.gfxTheme), texTitle);
  glTitleList = glGenLists(1);

  glNewList(glTitleList, GL_COMPILE);
    for(int i=0; i < 32; i++)
    {
      glBindTexture(GL_TEXTURE_2D, texTitle.prop.texture);
      glBegin( GL_QUADS );
        glColor4f(1,1,1,0.1);
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.2,1.15, 0.005*i );
        glTexCoord2f(1.0, 0.0); glVertex3f( 1.2,1.15, 0.005*i );
        glColor4f(0,0,1,0.00);
        glTexCoord2f(1.0, 1.0); glVertex3f( 1.2, 0.75, 0.005*i );
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.2, 0.75, 0.005*i );
      glEnd( );
    }
  glEndList();

  texMgr.load(useTheme("/gfx/title/glue.png",setting.gfxTheme), texText[PO_GLUE]);
  texMgr.load(useTheme("/gfx/title/gravity.png",setting.gfxTheme), texText[PO_GRAVITY]);
  texMgr.load(useTheme("/gfx/title/multiball.png",setting.gfxTheme), texText[PO_MULTIBALL]);
  texMgr.load(useTheme("/gfx/title/bigball.png",setting.gfxTheme), texText[PO_BIGBALL]);
  texMgr.load(useTheme("/gfx/title/normalball.png",setting.gfxTheme), texText[PO_NORMALBALL]);
  texMgr.load(useTheme("/gfx/title/smallball.png",setting.gfxTheme), texText[PO_SMALLBALL]);
  texMgr.load(useTheme("/gfx/title/aim.png",setting.gfxTheme), texText[PO_AIM]);
  texMgr.load(useTheme("/gfx/title/explosive.png",setting.gfxTheme), texText[PO_EXPLOSIVE]);
  texMgr.load(useTheme("/gfx/title/gun.png",setting.gfxTheme), texText[PO_GUN]);
  texMgr.load(useTheme("/gfx/title/go-thru.png",setting.gfxTheme), texText[PO_THRU]);
  texMgr.load(useTheme("/gfx/title/laser.png",setting.gfxTheme), texText[PO_LASER]);
  texMgr.load(useTheme("/gfx/title/life.png",setting.gfxTheme), texText[PO_LIFE]);
  texMgr.load(useTheme("/gfx/title/die.png",setting.gfxTheme), texText[PO_DIE]);
  texMgr.load(useTheme("/gfx/title/drop.png",setting.gfxTheme), texText[PO_DROP]);
  texMgr.load(useTheme("/gfx/title/detonate.png",setting.gfxTheme), texText[PO_DETONATE]);
  texMgr.load(useTheme("/gfx/title/explosive-grow.png",setting.gfxTheme), texText[PO_EXPLOSIVE_GROW]);
  texMgr.load(useTheme("/gfx/title/easybrick.png",setting.gfxTheme), texText[PO_EASYBRICK]);
  texMgr.load(useTheme("/gfx/title/nextlevel.png",setting.gfxTheme), texText[PO_NEXTLEVEL]);
  texMgr.load(useTheme("/gfx/title/aimhelp.png",setting.gfxTheme), texText[PO_AIMHELP]);
  texMgr.load(useTheme("/gfx/title/growbat.png",setting.gfxTheme), texText[PO_GROWPADDLE]);
  texMgr.load(useTheme("/gfx/title/shrinkbat.png",setting.gfxTheme), texText[PO_SHRINKPADDLE]);

  for(int ii = 0; ii < 3; ii++)
  {
    for(int i=0; i < 7; i++)
    {
      texText[i+(7*ii)].prop.ticks = 1000;
      texText[i+(7*ii)].prop.cols = 1;
      texText[i+(7*ii)].prop.rows = 1;
      texText[i+(7*ii)].prop.xoffset = 1;
      texText[i+(7*ii)].prop.yoffset = 1;
      texText[i+(7*ii)].prop.frames = 1;
      texText[i+(7*ii)].prop.bidir=0;
      texText[i+(7*ii)].prop.playing = 0;
      texText[i+(7*ii)].prop.padding = 0;
      
      powerUp[i+(7*ii)].tex = &texPowerups[i+(7*ii)];
      powerUp[i+(7*ii)].text = &texText[i+(7*ii)];
      powerUp[i+(7*ii)].posx = -1.5 + (0.7*ii);
      powerUp[i+(7*ii)].posy = -0.35 - (0.13*i);
    }
  }

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
      if(hilight == *numHighScores*3)//-1)
      {
        hilight=*numHighScores-1;
        hilightDir=0;
      }
    } else {
      hilight--;
      if(hilight == -*numHighScores*2)
      {
        hilightDir=1;
        hilight=0;
      }
    }
    hilightTime=0;
  }
   
    glTranslatef(0.0, 0.59, 0.0);
    float a;
    for(i=0; i < *numHighScores; i++)
    {
      if(hilightDir && i < hilight+1 || !hilightDir && i > hilight-1)
      {
        a=1.0-((1.0/(float)(*numHighScores*2))*(delta(hilight,i)));
        glBindTexture(GL_TEXTURE_2D, texHighScore[i]);
        glColor4f(1.0,1.0,1.0, a  );
        glBegin( GL_QUADS );
          glTexCoord2f(-0.0f,0); glVertex3f(  -1.27, 0.16, 0.0 );
          glTexCoord2f(1.0f,0);glVertex3f(   1.27, 0.16, 0.0 );
          glTexCoord2f(1.0f,0.110001);glVertex3f(   1.27,-0.16, 0.0 );
          glTexCoord2f(0.0f,0.110001); glVertex3f(  -1.27,-0.16, 0.0 );
        glEnd( );
      }
      glTranslatef(0.0,-0.1,0.0);
   }
   
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
