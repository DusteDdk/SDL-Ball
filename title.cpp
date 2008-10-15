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
    textureClass texText[MAXPOTEXTURES];
    float rot;
    bool rotDir;
    powerupDescriptionClass powerUp[MAXPOTEXTURES];
  public:
    titleScreenClass(effectManager *m, textureClass tp[]);
    void draw(int * frameAge);
};

titleScreenClass::titleScreenClass(effectManager *m, textureClass tp[])
{
  texPowerups = tp;
  fxMan = m;
  ticksSinceLastSpawn=100;
  texMgr.load(DATADIR"/gfx/title/title.png", texTitle);
  
  texMgr.load(DATADIR"/gfx/title/glue.png", texText[PO_GLUE]);
  texMgr.load(DATADIR"/gfx/title/gravity.png", texText[PO_GRAVITY]);
  texMgr.load(DATADIR"/gfx/title/multiball.png", texText[PO_MULTIBALL]);
  texMgr.load(DATADIR"/gfx/title/bigball.png", texText[PO_BIGBALL]);
  texMgr.load(DATADIR"/gfx/title/normalball.png", texText[PO_NORMALBALL]);
  texMgr.load(DATADIR"/gfx/title/smallball.png", texText[PO_SMALLBALL]);
  texMgr.load(DATADIR"/gfx/title/aim.png", texText[PO_AIM]);
  texMgr.load(DATADIR"/gfx/title/explosive.png", texText[PO_EXPLOSIVE]);
  texMgr.load(DATADIR"/gfx/title/gun.png", texText[PO_GUN]);
  texMgr.load(DATADIR"/gfx/title/go-thru.png", texText[PO_THRU]);
  texMgr.load(DATADIR"/gfx/title/laser.png", texText[PO_LASER]);
  texMgr.load(DATADIR"/gfx/title/life.png", texText[PO_LIFE]);
  texMgr.load(DATADIR"/gfx/title/die.png", texText[PO_DIE]);
  texMgr.load(DATADIR"/gfx/title/drop.png", texText[PO_DROP]);
  texMgr.load(DATADIR"/gfx/title/detonate.png", texText[PO_DETONATE]);
  texMgr.load(DATADIR"/gfx/title/explosive-grow.png", texText[PO_EXPLOSIVE_GROW]);
  texMgr.load(DATADIR"/gfx/title/easybrick.png", texText[PO_EASYBRICK]);
  texMgr.load(DATADIR"/gfx/title/nextlevel.png", texText[PO_NEXTLEVEL]);
  texMgr.load(DATADIR"/gfx/title/aimhelp.png", texText[PO_AIMHELP]);
  texMgr.load(DATADIR"/gfx/title/growbat.png", texText[PO_GROWPADDLE]);
  texMgr.load(DATADIR"/gfx/title/shrinkbat.png", texText[PO_SHRINKPADDLE]);

  int i,ii;
  for(ii = 0; ii < 3; ii++)
  {
    for(i=0; i < 7; i++)
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

  

}

void titleScreenClass::draw(int * frameAge)
{
  pos p,s;
  if(*frameAge > 8)
  {
          soundMan.play();

    if(var.clearScreen)
    {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glEnable(GL_TEXTURE_2D);
    ticksSinceLastSpawn +=nonpausingGlobalTicks;
    if(ticksSinceLastSpawn > 15)
    {
      s.x = 3.2;
      s.y = 0.5;
      p.x = 0;
      p.y = 0;

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

    int i;
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);

    for(i=0; i < MAXPOTEXTURES; i++)
    {
      powerUp[i].draw();
    }
    
    if(!rotDir)
    {
      rot += 0.05 * nonpausingGlobalTicks;
      if(rot > 45)
      {
        rotDir=1;
      }
    } else {
      rot -= 0.05 * nonpausingGlobalTicks;
      if(rot < -45)
      {
        rotDir=0;
      }
    }
    fxMan->draw();
    glLoadIdentity();
    glTranslatef(0.0,0.0,-3.0);
    glRotatef(rot, 0,1,0);
    

    for(i=0; i < 32; i++)
    {
      glBindTexture(GL_TEXTURE_2D, texTitle.prop.texture);
      glBegin( GL_QUADS );
          glColor4f(1,1,1,0.1);
          glTexCoord2f(0.0, 0.0); glVertex3f(-1.2,0.2, 0.005*i );
          glTexCoord2f(1.0, 0.0); glVertex3f( 1.2,0.2, 0.005*i );
          glColor4f(0,0,1,0.00);
          glTexCoord2f(1.0, 1.0); glVertex3f( 1.2,-0.2, 0.005*i );
          glTexCoord2f(0.0, 1.0); glVertex3f(-1.2,-0.2, 0.005*i );
      glEnd( );
    }
    
    

    SDL_GL_SwapBuffers( );
    
    globalTicksSinceLastDraw=0;
    globalMilliTicksSinceLastDraw=0;
    *frameAge = 0;
    
  }
}
