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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <string.h>
#include <list>
#include <sys/time.h>
#include <sys/stat.h>
#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif

/* ******************************************** *
     Here are the compile-time options !! ;)
 * ******************************************** */
#define DATADIR "data/"
#define WITH_SOUND
// #define WITH_WIIUSE

#define VERSION "0.12-CVS"
#define SAVEGAMEVERSION 2

#ifdef WITH_WIIUSE
  #include <wiiuse.h>
  #define MAX_WIIMOTES	1
#endif
#ifdef WITH_SOUND
  #define MIX_CHANNELS 16
  #include <SDL/SDL_mixer.h>
#endif

#include "declerations.h"

#define PI 3.14159265
#define RAD 6.28318531
#define BALL_MAX_DEGREE 2.7925268 //160 degrees
#define BALL_MIN_DEGREE 0.174532925 //10 degrees

#define FALSE 0
#define TRUE 1

//#define debugBall 1
//     #define DEBUG_DRAW_BALL_QUAD

#define PO_GRAVITY    0
#define PO_BIGBALL    1
#define PO_NORMALBALL 2
#define PO_SMALLBALL  3
#define PO_EXPLOSIVE  4
#define PO_GLUE 5
#define PO_MULTIBALL 6
#define PO_GROWPADDLE 7
#define PO_SHRINKPADDLE 8
#define PO_AIM 9
#define PO_GUN 10
#define PO_THRU 11
#define PO_LASER 12
#define PO_LIFE 13
#define PO_DIE 14
#define PO_DROP 15
#define PO_DETONATE 16
#define PO_EXPLOSIVE_GROW 17
#define PO_EASYBRICK 18
#define PO_NEXTLEVEL 19
#define PO_AIMHELP 20

//"Max powerups"
#define MAXPOTEXTURES 21

#define EASY 0
#define NORMAL 1

//Sound effects
#define SND_START 0
#define SND_BALL_HIT_BORDER 1
#define SND_BALL_HIT_PADDLE 2
#define SND_NORM_BRICK_BREAK 3
#define SND_EXPL_BRICK_BREAK 4
#define SND_GLASS_BRICK_HIT 5
#define SND_GLASS_BRICK_BREAK 6
#define SND_CEMENT_BRICK_HIT 7
#define SND_PO_HIT_BORDER 8
#define SND_GOOD_PO_HIT_PADDLE 9
#define SND_EVIL_PO_HIT_PADDLE 10
#define SND_SHOT 11
#define SND_DIE 12
#define SND_NEXTLEVEL 13
#define SND_GAMEOVER 14
#define SND_MENUCLICK 15
#define SND_SCORE_TICK 16
#define SND_DOOM_BRICK_BREAK 17
#define SND_GLUE_BALL_HIT_PADDLE 18
#define SND_INVISIBLE_BRICK_APPEAR 19
#define SND_HIGHSCORE 20
#define SND_BUY_POWERUP 21
#define SNDSAMPLES 22


using namespace std;

void writeSettings();
void initScreen();
void initNewGame();
void pauseGame();
void resumeGame();
float rndflt(float total, float negative);
class ball;
class paddle_class;
float bounceOffAngle(GLfloat width, GLfloat posx, GLfloat hitx);

bool writeTxt(TTF_Font *font, SDL_Color textColor,const char text[], GLuint texture, bool center);

TTF_Font *fonts[4] = {NULL,NULL,NULL,NULL};

int globalTicks;
float globalMilliTicks;
int nonpausingGlobalTicks;
float nonpausingGlobalMilliTicks;
int globalTicksSinceLastDraw;
float globalMilliTicksSinceLastDraw;

struct pos {
  GLfloat x;
  GLfloat y;
};


struct difficultyStruct {
  GLfloat ballspeed[2];
  GLfloat maxballspeed[2];
  GLfloat hitbrickinc[2];
  GLfloat hitpaddleinc[2];
  GLfloat slowdown[2];
  GLfloat speedup[2];
};

struct difficultyStruct static_difficulty, difficulty;

struct settings {
  bool cfgRes[2];
  int resx;
  int resy;
  int fps;
  bool fullscreen;
  bool showBg;
  bool sound;
  bool stereo;
  bool eyeCandy;
  bool particleCollide;
  //Add to menu:
  SDLKey keyLeft, keyRight, keyShoot, keyNextPo, keyPrevPo, keyBuyPo;
  float controlAccel;
  float controlStartSpeed;
  float controlMaxSpeed;
  bool joyEnabled, joyIsDigital;
  int JoyCalMin, JoyCalMax, JoyCalHighJitter, JoyCalLowJitter;
};

struct scrollInfoScruct {
  bool direction[4]; //0 right, 1 left, 2 up, 3 down
  unsigned int speed[4]; //right, left, up, down, in ms/ticks.
  unsigned int lastTick[4]; // what was the time last they moved
};

struct privFileStruct {
  string programRoot;
  string settingsFile;
  string saveGameFile;
  string highScoreFile;
  string screenshotDir;
};
struct privFileStruct privFile;

struct vars {
  bool titleScreenShow;
  int frame;
  int halfresx;
  int halfresy;
  GLfloat glunits_per_xpixel, glunits_per_ypixel;
  bool paused;
  int menu;
  int menuItem;
  bool menuPressed;
  int menuNumItems;
  int menuJoyCalStage;
  bool quit;
  bool wiiConnect;

  int numlevels;
  bool transition_half_done;
  bool clearScreen;
  bool idiotlock; //transition
  bool bricksHit; //tells the mainloop if it should copy the updated array of brick status.

  GLfloat averageBallSpeed;
  int showHighScores;
  bool enterSaveGameName;
  bool startedPlaying;

  int transiteffectnum;

  struct scrollInfoScruct scrollInfo;
};

//Ting der har med spillogik at gøre
struct gameVars {
  bool shopNextItem, shopPrevItem, shopBuyItem; //When set to 1 shop goes next or prev
  int deadTime; //I hvor mange millisekunder har bolden intet rørt
  bool nextlevel;
  bool gameOver;
  bool newLife;
  bool newLevel; //Start en ny level
  int bricksleft; //hvor mange brikker er der tilbage
};
struct gameVars gVar;

struct player_struct {
  int coins;
  int multiply;
  bool powerup[MAXPOTEXTURES];
  bool explodePaddle; //This lock makes the paddle explode and it won't come back until newlife.
  int level;
  int lives;
  int difficulty;
  int score;
};

int listSaveGames(string slotName[]);
void loadGame(int slot);
void saveGame(int slot, string name);

struct settings setting;
struct player_struct player;
struct vars var;

typedef GLfloat texPos[8];
#ifndef uint // WIN32
typedef unsigned int uint;
#endif

struct texProp {
  GLuint texture; //Den GLtexture der er loaded
  GLfloat xoffset;// Hvor stort er springet mellem hver subframe
  GLfloat yoffset; //
  int cols,rows; //hvor mange rækker og kolonner er der i denne textur
  int ticks;
  uint frames; //This many frames in each se
  bool bidir; //Går Looper den fra 0 -> X - 0 eller fra 0 -> X -> 0
  bool playing;

  bool padding; //Bit of a nasty hack, but if a texture is padded with 1 pixel around each frame, this have to be set to 1
  float pxw, pxh; //pixels width, and height
};

class textureClass {
  private:
    float age; //Hvor gammel er den frame vi er ved?
    bool dir; //hvis dette er en animation der går frem og tilbage hvilken retning

    uint lastframe; //check om det er den samme frame som sidst, så vi kan vide om vis skal opdatere cords

  public:
    uint frame; //hvilken frame er vi nået til i texturen (den er public så vi kan lave offset)
    bool playing; //spiller vi?
    bool firstFrame; //If this is the first frame
    texPos pos; //Kordinater for den frame på texturen der er nu
    texProp prop; //Properties for den textur som dette objekt har

    textureClass() {
      age=10000;
      firstFrame=1;
      lastframe=1000;
      frame=1;
      dir=0;
    }

  void play()
  {
    int col=0,row=0;
    if(prop.playing)
    {
      //Skal vi skifte frame?
      age += globalTicksSinceLastDraw;
      if(age >= prop.ticks) //Denne frame har været vist længe nok
      {
        age=0.0;
        if(!dir)
        {
          if(frame == prop.frames)
          {
            if(prop.bidir)
            {
              dir=1;
            } else {
              frame=1;
            }
          } else {
            frame++;
          }
        }

        if(dir) {
          if(frame == 1)
          {
            dir=0;
            frame=2;
          } else {
            frame--;
          }
        }
      }
    }

    uint f=0;
    if(frame != lastframe || firstFrame)
    {
      lastframe=frame;
      firstFrame=0;


      //hvor mange kolonner er der på en række

      for(row=0; row < prop.rows; row++)
      {
        for(col=0; col < prop.cols; col ++)
        {
          f++;
          if(f == frame)
          {
            //Øverst Venstre
            pos[0] =  (prop.xoffset*(float)col); //0.0;
            pos[1] =  (prop.yoffset*(float)row);//0.0;

            //Øverst højre
            pos[2] = (prop.xoffset*(float)col) + prop.xoffset;
            pos[3] = (prop.yoffset*(float)row);//0.0;

            //Nederst højre
            pos[4] =  (prop.xoffset*(float)col) + prop.xoffset; // 1
            pos[5] =  (prop.yoffset*(float)row) + prop.yoffset; // 1

            //Nederst venstre
            pos[6] = (prop.xoffset*(float)col);//0.0;
            pos[7] = (prop.yoffset*(float)row) + prop.yoffset; //1

            if(prop.padding)
            {
              pos[0] += 1.0 / prop.pxw;
              pos[1] += 1.0 / prop.pxh;

              pos[2] -= 1.0 / prop.pxw;
              pos[3] += 1.0 / prop.pxh;

              pos[4] -= 1.0 / prop.pxw;
              pos[5] -= 1.0 / prop.pxh;

              pos[6] += 1.0 / prop.pxw;
              pos[7] -= 1.0 / prop.pxh;
            }
          }
        }
      }

    }
  }

};

class textureManager {

  public:

    bool load(string file, textureClass & tex)
    {
      SDL_Surface *temp = NULL;
      GLint maxTexSize;
      GLuint glFormat = GL_RGBA;

      if(file.substr(file.length()-3,3).compare("jpg") == 0)
      {
        glFormat = GL_RGB;
      }

      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

      temp = IMG_Load(file.data());

      if(temp == NULL)
      {
        cout << "Texture manager ERROR: " << file << " : "<< SDL_GetError() << endl;
        SDL_FreeSurface( temp );
        return(FALSE);
      }



      //Hvis større end tilladt:
      if(temp->w > maxTexSize)
      {
        cout << "Texture manager ERROR: '" << file << "' texturesize too large." << endl;
        //TODO:
        //Resize surface.
      }


      glGenTextures(1, &tex.prop.texture);
      glBindTexture(GL_TEXTURE_2D, tex.prop.texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

      glTexImage2D(GL_TEXTURE_2D, 0, glFormat, temp->w, temp->h, 0, glFormat, GL_UNSIGNED_BYTE, temp->pixels);

      tex.prop.pxw = temp->w;
      tex.prop.pxh = temp->h;
      SDL_FreeSurface( temp );

      return(TRUE);
    }
};

#include "sound.cpp"
soundClass soundMan; //Public object so all objects can use it

#include "menu.cpp"

#include "scoreboard.cpp"


class object {
  public:
    GLfloat color[3];
    GLfloat opacity;
    GLfloat posx, posy;
    GLfloat width,height;
    GLuint dl; //opengl display list
    bool active;
    bool collide;
    bool reflect;

    textureClass tex;

    object()
    {
      color[0]=1.0f;
      color[1]=1.0f;
      color[2]=1.0f;
      opacity=1.0f;
    }
};

class paddle_class : public object {
  private:
    GLfloat growspeed;
    GLfloat destwidth;
    GLfloat aspect; //så meget stiger højden i forhold til bredden
    bool growing;


  public:
     bool dead;
    textureClass *layerTex;
    paddle_class ()
    {
      init();
      growing=0;
      growspeed=0.05f;
      aspect = 0.2f;
    }

    void init()
    {
      posy = -1.15;
      width=0.1;
      height=0.02;
      dead=0;
    }

    void grow(GLfloat width)
    {
      destwidth = width;
      growing = 1;
    }

    void draw()
    {

      if(!dead)
      {
        if(player.powerup[PO_DIE])
        {
          player.powerup[PO_DIE]=0;
          dead=1;
          width=0.0;
          height=0.0;
        }

        if(growing)
        {
          GLfloat ix = growspeed * globalTicksSinceLastDraw;

          width += ix;

          if(width >= destwidth)
          {
            width = destwidth;
            height = aspect*destwidth;
            growing=0;
          }
        }

        glLoadIdentity();
        glTranslatef( posx, posy, -3.0 );

        tex.play();
        glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
        glBegin( GL_QUADS );
            glTexCoord2f(tex.pos[0], tex.pos[1]); glVertex3f(-width,height, 0.0f );
            glTexCoord2f(tex.pos[2], tex.pos[3]); glVertex3f( width,height, 0.0f );
            glTexCoord2f(tex.pos[4], tex.pos[5]); glVertex3f( width,-height, 0.0f );
            glTexCoord2f(tex.pos[6], tex.pos[7]); glVertex3f(-width,-height, 0.0f );
        glEnd( );

        //Hvis glue?
        if(player.powerup[PO_GLUE])
        {
          glBindTexture(GL_TEXTURE_2D, layerTex[0].prop.texture);
          glBegin( GL_QUADS );
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, height, 0.0f );
            glTexCoord2f(1.0f, 0.0f); glVertex3f( width, height, 0.0f );
            glTexCoord2f(1.0f, 0.99f); glVertex3f( width,-height, 0.0f );
            glTexCoord2f(0.0f, 0.99f); glVertex3f(-width,-height, 0.0f );
          glEnd( );
        }

        //Hvis gun
        if(player.powerup[PO_GUN])
        {
          layerTex[1].play();
          glBindTexture(GL_TEXTURE_2D, layerTex[1].prop.texture);
          glBegin( GL_QUADS );
            glTexCoord2f(layerTex[1].pos[0], layerTex[1].pos[1]); glVertex3f(-width, height*4, 0.0f );
            glTexCoord2f(layerTex[1].pos[2], layerTex[1].pos[3]); glVertex3f( width, height*4, 0.0f );
            glTexCoord2f(layerTex[1].pos[4], layerTex[1].pos[5]-0.01); glVertex3f( width,height, 0.0f );
            glTexCoord2f(layerTex[1].pos[6], layerTex[1].pos[7]-0.01); glVertex3f(-width,height, 0.0f );
          glEnd( );
        }
      }
    }
};

//nasty fix to a problem
int nbrick[23][26];
int updated_nbrick[23][26];

class brick;
void makeExplosive(brick & b);

textureClass * texExplosiveBrick; //NOTE:Ugly
class brick : public object {

  public:
  int score; //Hvor meget gir den
  bool destroytowin; // Skal den smadres for at man kan vinde?
  char powerup;
  char type;
  GLfloat fade; //hvor meget brik
  GLfloat fadespeed;
  GLfloat zoomspeed;
  GLfloat zoom;
  bool isdyingnormally;
  bool isexploding; //springer den i luften
  int row; //what row is this brick in
  int bricknum; //brick in this row
  int hitsLeft; //Hvor mange gange skal denne brik rammes før den dør?
  bool justBecomeExplosive; //If this brick just become a explosive one.


  bool n(int dir)
  {
    switch(dir)
    {
      case 0: //Er der en brik til venstre for dig?
        if(bricknum > 0)
        {
          if(nbrick[row][bricknum-1] != -1)
            return(1);
        }
        break;
      case 1: //Er der en brik til højre for dig?
        if(bricknum < 25) //26
        {
          if(nbrick[row][bricknum+1] != -1)
            return(1);
        }
        break;
      case 2: //Er der en brik Ovenpå dig
        if(row > 0)
        {
          if(nbrick[row-1][bricknum] != -1)
            return(1);
        }
        break;
      case 3: //Er der en brik nedenunder dig
        if(row < 22) //23
        {
          if(nbrick[row+1][bricknum] != -1)
            return(1);
        }
        break;
    }

    return(0);
  }


  void hit(effectManager & fxMan, pos poSpawnPos, pos poSpawnVel, bool ballHitMe);

  void draw(brick bricks[], effectManager & fxMan)
  {

    if(isdyingnormally)
    {
      fade -= fadespeed * globalMilliTicksSinceLastDraw;
      opacity = fade;
      zoom -= zoomspeed * globalMilliTicksSinceLastDraw;

      if(fade < 0.0)
        active=0;
    }

    if(isexploding && !var.paused)
    {
      fade -= 7.0 * globalMilliTicksSinceLastDraw;
      opacity = fade;
      if(fade<0.0)
      {
        active=0;

        pos spos,svel;
        spos.x=posx;
        spos.y=posy;


        if(bricknum > 0)
        {
          if(nbrick[row][bricknum-1] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row][bricknum-1]].hit(fxMan,spos,svel,0);
          }
        }


        if(bricknum < 25)
        {
          if(nbrick[row][bricknum+1] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row][bricknum+1]].hit(fxMan,spos,svel,0);
          }
        }

        if(row > 0)
        {
          if(nbrick[row-1][bricknum] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row-1][bricknum]].hit(fxMan,spos,svel,0);
          }
        }

        if(row < 22)
        {
          if(nbrick[row+1][bricknum] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row+1][bricknum]].hit(fxMan,spos,svel,0);
          }
        }

        if(row > 0 && bricknum > 0)
        {
          if(nbrick[row-1][bricknum-1] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row-1][bricknum-1]].hit(fxMan,spos,svel,0);
          }
        }
        if(row > 0 && bricknum < 25)
        {
          if(nbrick[row-1][bricknum+1] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row-1][bricknum+1]].hit(fxMan,spos,svel,0);
          }
        }

        if(row < 22 && bricknum > 0)
        {
          if(nbrick[row+1][bricknum-1] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row+1][bricknum-1]].hit(fxMan,spos,svel,0);
          }
        }

        if(row < 22 && bricknum < 25)
        {
          if(nbrick[row+1][bricknum+1] != -1)
          {
            svel.x=rndflt(2,0)/3.0;
            svel.y=rndflt(2,0)/3.0;
            bricks[nbrick[row+1][bricknum+1]].hit(fxMan,spos,svel,0);
          }
        }


      }

    }

    tex.play();

    glColor4f( color[0], color[1], color[2], opacity );

    glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
    glBegin( GL_QUADS );
      glTexCoord2f(tex.pos[0],tex.pos[1]);glVertex3f( posx + (-0.0616*zoom), posy + (0.035*zoom), 0.00 ); // øverst venst
      glTexCoord2f(tex.pos[2],tex.pos[3]);glVertex3f( posx + ( 0.0616*zoom), posy + (0.035*zoom), 0.00 ); // øverst højre
      glTexCoord2f(tex.pos[4],tex.pos[5]);glVertex3f( posx + ( 0.0616*zoom), posy + (-0.035*zoom), 0.00 ); // nederst højre
      glTexCoord2f(tex.pos[6],tex.pos[7]);glVertex3f( posx + (-0.0616*zoom), posy + (-0.035*zoom), 0.00 ); // nederst venstre
    glEnd( );
  }

  void growExplosive(brick bricks[])
  {
    if(type!='B' || justBecomeExplosive)
    {
      return;
    }

    if(bricknum > 0)
    {
      if(nbrick[row][bricknum-1] != -1)
      {
         makeExplosive(bricks[nbrick[row][bricknum-1]]);
      }
    }

    if(bricknum < 25)
    {
      if(nbrick[row][bricknum+1] != -1)
      {
         makeExplosive(bricks[nbrick[row][bricknum+1]]);
      }
    }

    if(row > 0)
    {
      if(nbrick[row-1][bricknum] != -1)
      {
        makeExplosive(bricks[nbrick[row-1][bricknum]]);
      }
    }

    if(row < 22)
    {
      if(nbrick[row+1][bricknum] != -1)
      {
         makeExplosive(bricks[nbrick[row+1][bricknum]]);
      }
    }

    if(row > 0 && bricknum > 0)
    {
      if(nbrick[row-1][bricknum-1] != -1)
      {
         makeExplosive(bricks[nbrick[row-1][bricknum-1]]);
      }
    }
    if(row > 0 && bricknum < 25)
    {
      if(nbrick[row-1][bricknum+1] != -1)
      {
         makeExplosive(bricks[nbrick[row-1][bricknum+1]]);
      }
    }

    if(row < 22 && bricknum > 0)
    {
      if(nbrick[row+1][bricknum-1] != -1)
      {
         makeExplosive(bricks[nbrick[row+1][bricknum-1]]);
      }
    }

    if(row < 22 && bricknum < 25)
    {
      if(nbrick[row+1][bricknum+1] != -1)
      {
         makeExplosive(bricks[nbrick[row+1][bricknum+1]]);
      }
    }
  }

  void breakable()
  {
    if(type == '3')
    {
     score=300;
     hitsLeft=1;
     type='1'; //hehe..
     tex.frame=2;
     tex.play();
    } else if(type=='4')
    {
      hitsLeft=1;
      tex.frame=2;
      tex.play();
    } else if(type=='9')
    {
      hitsLeft=1;
      tex.frame=3;
      tex.play();
    }
  }
};

void makeExplosive(brick & b)
{
  if(b.type != 'B')
  {
    b.type='B';
    b.tex=*texExplosiveBrick;
    b.color[0]=1.0;
    b.color[1]=1.0;
    b.color[2]=1.0;
    b.opacity=1.0;
    b.justBecomeExplosive=1;
  }
}

#include "loadlevel.cpp"

class moving_object : public object {
  public:
    GLfloat xvel,yvel,velocity;

    moving_object () {
      xvel=0.0;
      yvel=0.0;

    }

};

#include "effects.cpp"
#include "background.cpp"
void spawnpowerup(char powerup, pos a, pos b);

class bulletsClass {
  private:
    moving_object bullets[16];
  public:
    int active;

    bulletsClass(textureClass & texBullet)
    {
      int i;
      for(i=0; i < 16; i++)
      {
        bullets[i].active=0;
        bullets[i].tex = texBullet;
        bullets[i].width = 0.02;
        bullets[i].height = 0.02;
      }
    }

    void shoot(pos p)
    {
      int i;
      //Find ledig bullet
      for(i=0; i < 16; i++)
      {
        if(!bullets[i].active)
        {
          soundMan.add(SND_SHOT, p.x);
          bullets[i].active=1;
          bullets[i].posx = p.x;
          bullets[i].posy = p.y;
          bullets[i].xvel =0;
          bullets[i].yvel =1.0;
          break;
        }
      }
    }

    void move()
    {
      int i;
      for(i=0; i < 16; i++)
      {
        if(bullets[i].active)
        {
          //Flyt
          bullets[i].posy += bullets[i].yvel * globalMilliTicks;
        }
      }
    }

    void draw()
    {
      int i;
      glColor4f(1,1,1,1);
      for(i=0; i < 16; i++)
      {
        if(bullets[i].active)
        {
          //draw

          bullets[i].tex.play();

          glLoadIdentity();
          glTranslatef( bullets[i].posx, bullets[i].posy, -3.0 );

          glBindTexture(GL_TEXTURE_2D, bullets[i].tex.prop.texture);
          glBegin( GL_QUADS );
          glTexCoord2f(bullets[i].tex.pos[0],bullets[i].tex.pos[1]); glVertex3f( -bullets[i].width, bullets[i].height, 0.0 );
          glTexCoord2f(bullets[i].tex.pos[2],bullets[i].tex.pos[3]); glVertex3f(  bullets[i].width, bullets[i].height, 0.0 );
          glTexCoord2f(bullets[i].tex.pos[4],bullets[i].tex.pos[5]); glVertex3f(  bullets[i].width,-bullets[i].height, 0.0 );
          glTexCoord2f(bullets[i].tex.pos[6],bullets[i].tex.pos[7]); glVertex3f( -bullets[i].width,-bullets[i].height, 0.0 );
          glEnd( );
        }
      }
    }

    void clear()
    {
      int i;
      for(i=0; i < 16; i++)
      {
        bullets[i].active=0;
      }
    }

    void coldet(brick & b, effectManager & fxMan)
    {
      int i;
      bool hit;
      pos v,p;
      v.x=0;
      v.y=bullets[0].xvel;

      for(i=0; i < 16; i++)
      {
        if(bullets[i].active)
        {
          hit=0;

          //y
          if(bullets[i].posy+bullets[i].height/10.0 > b.posy-b.height && bullets[i].posy+bullets[i].height/10.0 < b.posy+b.height)
          {
            p.x = b.posx;
            p.y = b.posy;
            //Venstre side:
            if(bullets[i].posx > b.posx-b.width && bullets[i].posx < b.posx+b.width)
            {
              hit=1;
            }

            if(hit)
            {
              b.hit(fxMan, p, v,1);

              if(!player.powerup[PO_THRU])
              {
                bullets[i].active=0;
              }

              p.x = bullets[i].posx;
              p.y = bullets[i].posy;

              if(setting.eyeCandy)
              {
                fxMan.set(FX_VAR_TYPE, FX_SPARKS);
                fxMan.set(FX_VAR_COLDET,1);
                fxMan.set(FX_VAR_LIFE, 1000);
                fxMan.set(FX_VAR_NUM, 16);
                fxMan.set(FX_VAR_SIZE, 0.015f);
                fxMan.set(FX_VAR_SPEED, 0.4f);
                fxMan.set(FX_VAR_GRAVITY, 1.0f);

                fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
                fxMan.spawn(p);
                fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
                fxMan.spawn(p);
                fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
                fxMan.spawn(p);
                fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
                fxMan.spawn(p);
              }
            }
          } else if(bullets[i].posy > 1.6)
          {
            bullets[i].active=0;
          }
        }
      }
    }
};

void brick::hit(effectManager & fxMan, pos poSpawnPos, pos poSpawnVel, bool ballHitMe)
{
  pos p,s;

  if(type!='3' || player.powerup[PO_THRU])
    hitsLeft--;

  //We don't want to play a sound if this brick is not an explosive, and was hit by an explosion
  if(ballHitMe || type=='B')
  {
    if(type=='3') //cement
    {
      soundMan.add(SND_CEMENT_BRICK_HIT, posx);
    } else if(type=='4' || type=='9') //glass or invisible
    {

      if(hitsLeft == 2)
      {
        soundMan.add(SND_INVISIBLE_BRICK_APPEAR, posx);
      } else if(hitsLeft == 1)
      {
        soundMan.add(SND_GLASS_BRICK_HIT, posx);
      } else {
        soundMan.add(SND_GLASS_BRICK_BREAK, posx);
      }
    } else if(type=='B') //explosive
    {
      soundMan.add(SND_EXPL_BRICK_BREAK, posx);
    } else if(type=='C') //Doom brick
    {
      soundMan.add(SND_DOOM_BRICK_BREAK, posx);
    } else { //All the other bricks
      soundMan.add(SND_NORM_BRICK_BREAK, posx);
    }
  }


  if(type != '3' || player.powerup[PO_THRU])
  {
    //Brick was hit, dont do anything
    if(isdyingnormally || isexploding)
    {
      return;
    }
    player.score += (brick::score*player.multiply)* var.averageBallSpeed; //Speed bonus

    if(hitsLeft < 1 || type == 'B') //Hvis brikken er explosiv kan den ikke have nogle hits tilbage
    {
      collide=0;

      updated_nbrick[row][bricknum]=-1;
      var.bricksHit = 1;

      gVar.deadTime=0;

      spawnpowerup(powerup, poSpawnPos, poSpawnVel);
      powerup='0';

      if(setting.eyeCandy)
      {
        p.x=posx;
        p.y=posy;
        s.x=width*2;
        s.y=height*2;

        fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
        fxMan.set(FX_VAR_COLDET, 1);
        fxMan.set(FX_VAR_LIFE, 1000);
        fxMan.set(FX_VAR_NUM, 20);
        fxMan.set(FX_VAR_SIZE, 0.03f);
        fxMan.set(FX_VAR_SPEED, 0.6f);
        fxMan.set(FX_VAR_GRAVITY, 0.7f);

        fxMan.set(FX_VAR_RECTANGLE, s);

        fxMan.set(FX_VAR_COLOR, color[0],color[1],color[2]);
        fxMan.spawn(p);
      }

      if(type=='B')
      {
        isexploding=1;

        if(setting.eyeCandy)
        {
          p.x = posx;
          p.y = posy;
          fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
          fxMan.set(FX_VAR_COLDET,1);
          fxMan.set(FX_VAR_LIFE, 1200);
          fxMan.set(FX_VAR_NUM, 10);
          fxMan.set(FX_VAR_SIZE, 0.08f);
          fxMan.set(FX_VAR_SPEED, 0.4f);
          fxMan.set(FX_VAR_GRAVITY, -1.3f);

          fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
          fxMan.spawn(p);
          fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
          fxMan.spawn(p);
          fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
          fxMan.spawn(p);
          fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
          fxMan.spawn(p);
        }
      } else {
        isdyingnormally=1;
      }
    } else { //No hits left
      tex.frame++;
      tex.play();
    } //Hits left
  }
}

glAnnounceTextClass announce;
//Slet mig måske
float abs2(float x)
{
      if (x<0) {return -x;}
      return x;
}
int LinesCross(float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3, GLfloat *linx, GLfloat *liny)
{
	float d=(x1-x0)*(y3-y2)-(y1-y0)*(x3-x2);
	if (abs2(d)<0.001f) {return -1;}
	float AB=((y0-y2)*(x3-x2)-(x0-x2)*(y3-y2))/d;
	if (AB>0.0 && AB<1.0)
	{
		float CD=((y0-y2)*(x1-x0)-(x0-x2)*(y1-y0))/d;
		if (CD>0.0 && CD<1.0)
        {
			*linx=x0+AB*(x1-x0);
			*liny=y0+AB*(y1-y0);
			return 1;
        }
    }
	return 0;
}

//Leaves an trail of the ball
class tracer {
  private:
    GLfloat x[100], y[100], r[100], g[100], b[100], a[100],s[100], cr,cg,cb,;
    bool active[100];
    int color;
    GLfloat lastX, lastY; //Last position where we spawned one

  public:
    GLfloat height, width;
    GLuint texture;

    int len;
    void draw()
    {
      int i;
      for(i=0; i < len; i++)
      {

        if(active[i])
        {
          a[i] -= 4.0*globalMilliTicksSinceLastDraw;
          s[i] += 4.0*globalMilliTicksSinceLastDraw;
          if(a[i] < 0.0)
            active[i]=0;

          glBindTexture( GL_TEXTURE_2D, texture);
          glLoadIdentity();
          glTranslatef(x[i],y[i],-3.0);
          glColor4f(r[i], g[i], b[i], a[i]);
          glBegin( GL_QUADS );
            glTexCoord2f(0.0, 0.0);glVertex3f( -width*s[i], height*s[i], 0.00 ); // øverst venst
            glTexCoord2f(0.0, 1.0);glVertex3f(  width*s[i], height*s[i], 0.00 ); // øverst højre
            glTexCoord2f(1.0, 1.0);glVertex3f(  width*s[i],-height*s[i], 0.00 ); // nederst højre
            glTexCoord2f(1.0, 0.0);glVertex3f( -width*s[i],-height*s[i], 0.00 ); // nederst venstre
          glEnd( );
        }
      }
    }

    void colorRotate(bool explosive, GLfloat c[])
    {
      color++;
      if(color > 5)
        color=0;

      if(!explosive)
      {
        cr=c[0];
        cg=c[1];
        cb=c[2];
      } else {
        cr=1.0;
        cg=0.6;
        cb=0.0;
      }
    }

    tracer()
    {
      len=100;
      lastX=0;
      lastY=0;
      cr=1;
      cg=0;
      cb=0;
      height = 0.01;
      width = 0.01;
      int i;
      for(i=0; i < 100; i++)
      {
        active[i]=0;
      }
    }

    void update(GLfloat nx, GLfloat ny)
    {
      //If long enough away
      GLfloat dist = sqrt( pow(nx-lastX, 2) + pow(ny-lastY, 2) );
      if(dist > 0.01)
      {
        lastX = nx;
        lastY = ny;
        //find a non-active trail-part
        int i;
        for(i=0; i < len; i++)
        {
          if(!active[i])
          {
            active[i]=1;
            a[i]=1.0; //tweak me
            x[i]=nx;
            y[i]=ny;
            s[i]=1.0;
            r[i]=cr;
            g[i]=cg;
            b[i]=cb;
            break;
          }
        }
      }
    }


};

class ball : public moving_object {
  private:
  GLfloat rad;
  bool growing,shrinking;
  GLfloat destwidth, growspeed;

  public:
  tracer tail;
  int died; //When did this ball get gravity?

  bool gravity; //did we get gravity
  bool explosive; //Makes brick explosive (to get a explosion effect) and explode it

  bool glued; //Sidder vi fast på padden for øjeblikket?
  GLfloat gluedX;
  //Variabler med forududregnede værdier
  GLfloat bsin[32], bcos[32];

  bool aimdir;
  textureClass fireTex, texTail;

  GLfloat lastX,lastY;
  ball() {
    growing=0;
    growspeed=0.1;
    width=0.0;
    height=0.0;
    glued=0;
    posx=0.0f;
    posy=0.0f;
    aimdir=0;
  }

  void hit(pos p, GLfloat c[])
  {
    if(gravity)
    {
      yvel /= 1.4;
      xvel /= 1.01;
      posy = p.y+height;
      if(globalTicks-died > 6000)
      {
        active=0;
      }
    }

      if(setting.eyeCandy)
        tail.colorRotate(explosive, c);
  }

  void move()
  {
    bool col=0;
    //vi laver lige den her coldet her...
    if(posx+width > 1.6 && xvel > 0.0)
    {
      col=1;
      xvel *= -1;
    } else if(posx-width < -1.6 && xvel < 0.0)
    {
      col=1;
      xvel *= -1;
    } else if(posy+width > 1.25 && yvel > 0.0)
    {
      col=1;
      yvel *= -1;
    } else if(posy-width < -1.24)
    {
      active=0;
    }

    if(col)
    {
      soundMan.add(SND_BALL_HIT_BORDER, posx);
    }

    if(gravity)
    {
      if(yvel != 0)
        yvel -= 0.6*globalMilliTicks;
    }

    posx +=xvel*globalMilliTicks;

    if(!glued)
    {
      posy +=yvel*globalMilliTicks;
    } else {
      gVar.deadTime = 0;
    }

    if(setting.eyeCandy)
      tail.update(posx,posy);

  }

  void draw(paddle_class &paddle)
  {

    GLfloat newsize;

    if(setting.eyeCandy)
      tail.draw();

    if(growing)
    {
      newsize = growspeed * globalMilliTicksSinceLastDraw;
      width += newsize;
      height += newsize;

      if(width >= destwidth)
      {
        width=destwidth;
        height=destwidth;

        growing=0;
      }

      tail.width = width;
      tail.height = height;

    } else if(shrinking)
    {

      newsize = growspeed * globalMilliTicksSinceLastDraw;
      width -= newsize;
      height -= newsize;
      if(width <= destwidth)
      {
        width=destwidth;
        height=destwidth;

        shrinking=0;
      }

      tail.width = width;
      tail.height = height;

    }

    if(glued && player.powerup[PO_LASER])
    {
      if(player.powerup[PO_AIM])
      {
        if(aimdir==0)
        {
          rad -= 1.2*globalMilliTicksSinceLastDraw;

          if(rad < BALL_MIN_DEGREE)
            aimdir=1;
        } else {
          rad += 1.2*globalMilliTicksSinceLastDraw;
          if(rad  > BALL_MAX_DEGREE+BALL_MIN_DEGREE)
            aimdir=0;
        }
        setangle(rad);
      } else {
        getRad();
      }

      GLfloat bxb = cos(rad)*0.5, byb = sin(rad)*0.5;

      glLoadIdentity();
      glTranslatef( posx, posy, -3.0);
      glDisable(GL_TEXTURE_2D);
        glLineWidth ( 1.0 );
        glEnable( GL_LINE_SMOOTH );
        glBegin( GL_LINES );
          glColor4f(rndflt(2,0), rndflt(1,0), 0.0, 0.0);
          glVertex3f( 0.0, 0.0, 0.0 );
          glColor4f(rndflt(2,0), 0.0, 0.0, 1.0);
          glVertex3f( bxb, byb, 0.0 );
        glEnd();

        glPointSize( 5.0f );
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glEnable(GL_POINT_SMOOTH);
        glBegin( GL_POINTS );
          glVertex3f(bxb, byb, 0.0);
        glEnd( );


    }

    if(!glued && player.powerup[PO_AIMHELP])
    {
      //Use line intersect to determine if this ball will collide with the paddle

        getRad();
        GLfloat p[4], b[4], o[2]; //Paddle line, ball line, bounceoff endpoint
        p[0] = paddle.posx - paddle.width;
        p[1] = paddle.posx + paddle.width;

        p[2] = paddle.posy + paddle.height + height;
        p[3] = paddle.posy + paddle.height + height;

        b[0] = posx;
        b[1] = posx + (cos(rad) * 3.0);
        b[2] = posy;
        b[3] = posy + (sin(rad) * 3.0);

        GLfloat cx,cy, R;
        if(LinesCross(p[0], p[2], p[1],p[3], b[0], b[2], b[1], b[3], &cx, &cy))
        {
          R = bounceOffAngle(paddle.width, paddle.posx, cx);
          o[0] = cx+(cos(R)*2.0);
          o[1] = cy+(sin(R)*2.0);
          glLoadIdentity();
          glTranslatef( 0.0, 0.0, -3.0);
          glDisable(GL_TEXTURE_2D);
          glLineWidth ( 2.0 );
          glEnable( GL_LINE_SMOOTH );
          glBegin( GL_LINE_STRIP );
            //Line from ball to paddle
            glColor4f(1.0, 0.0, 0.0, 0.0);
            glVertex3f( b[0], b[2], 0.0 );
            glColor4f(1.0, 1.0, 0.0, 1.0);
            glVertex3f( cx, cy, 0.0 );
            //Bounce off line.
            glColor4f(1.0, 0.0, 0.0, 0.0);
            glVertex3f( o[0], o[1], 0.0 );
          glEnd( );

        }

    }

    glLoadIdentity();
    glTranslatef( posx, posy, -3.0);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    if(explosive)
    {
      fireTex.play();
      glBindTexture(GL_TEXTURE_2D, fireTex.prop.texture);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBegin( GL_QUADS );
        glTexCoord2f(fireTex.pos[0],fireTex.pos[1]); glVertex3f( -width, height, 0.0 );
        glTexCoord2f(fireTex.pos[2],fireTex.pos[3]); glVertex3f(  width, height, 0.0 );
        glTexCoord2f(fireTex.pos[4],fireTex.pos[5]); glVertex3f(  width,-height, 0.0 );
        glTexCoord2f(fireTex.pos[6],fireTex.pos[7]); glVertex3f( -width,-height, 0.0 );
      glEnd( );
    } else {
      tex.play();
      glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBegin( GL_QUADS );
        glTexCoord2f(tex.pos[0],tex.pos[1]); glVertex3f( -width, height, 0.0 );
        glTexCoord2f(tex.pos[2],tex.pos[3]); glVertex3f(  width, height, 0.0 );
        glTexCoord2f(tex.pos[4],tex.pos[5]); glVertex3f(  width,-height, 0.0 );
        glTexCoord2f(tex.pos[6],tex.pos[7]); glVertex3f( -width,-height, 0.0 );
      glEnd( );
    }

    #ifdef DEBUG_DRAW_BALL_QUAD
      glLoadIdentity();
      glTranslatef(posx, posy, -3.0);
      glDisable( GL_TEXTURE_2D );
      glColor4f(1.0,1.0,1.0,1.0);
      glBegin( GL_LINES );
        glVertex3f( -width, height, 0);
        glVertex3f( width, height, 0);

        glVertex3f( -width, -height, 0);
        glVertex3f( width, -height,0);

        glVertex3f( -width, height, 0);
        glVertex3f( -width, -height, 0);

        glVertex3f( width, height, 0);
        glVertex3f( width, -height, 0 );

      glEnd();
      glEnable(GL_TEXTURE_2D);
      #endif

  }

  GLfloat getRad()
  {
    rad = atan2(yvel,xvel);
    return(rad);
  }

  void setangle(GLfloat o)
  {
     if(o < BALL_MIN_DEGREE)
     {
       o=BALL_MIN_DEGREE;
     }

     if( o > BALL_MAX_DEGREE + BALL_MIN_DEGREE)
     {
       o=BALL_MAX_DEGREE + BALL_MIN_DEGREE;
     }

    rad=o;
    xvel = velocity * cos(rad);
    yvel = velocity * sin(rad);
  }

  void setspeed(GLfloat v)
  {
    if(v > difficulty.maxballspeed[player.difficulty])
    {
      velocity = difficulty.maxballspeed[player.difficulty];
    } else {
      velocity = v;
    }

    getRad();
    xvel = velocity * cos(rad);
    yvel = velocity * sin(rad);
  }

  void setSize(GLfloat s)
  {
    float rad;

    if(s > width)
      growing=1;
      else if(s < width)
      shrinking=1;

    destwidth=s;

    int i=0;

    //opdater points
    for(rad=0.0; rad < 6.3; rad +=0.2)
    {
      if(i < 32)
      {
        bsin[i] = sin(rad)*s;
        bcos[i] = cos(rad)*s;
      }
      i++;
    }
  }

};

void coldet(brick & br, ball & ba, pos & p, effectManager & fxMan);
void padcoldet(ball & b, paddle_class & p, pos & po);

#define MAXBALLS 16
class ballManager {

  public:
    int activeBalls;
    ball b[MAXBALLS];
    textureClass tex[3];

    void initBalls()
    {
      activeBalls=0;
      clear();
    }

    ballManager(textureClass btex[])
    {
      int i;
      tex[0] = btex[0];
      tex[1] = btex[1];
      tex[2] = btex[2];

      tex[0].prop.ticks = 1000;
      tex[0].prop.cols = 1;
      tex[0].prop.rows = 1;
      tex[0].prop.xoffset = 1.0;
      tex[0].prop.yoffset = 1.0;
      tex[0].prop.frames = 1;
      tex[0].prop.bidir = 0;
      tex[0].prop.playing = 0;
      tex[0].prop.padding=1;

      tex[1].prop.ticks = 25;
      tex[1].prop.cols = 4;
      tex[1].prop.rows = 4;
      tex[1].prop.xoffset = 0.25;
      tex[1].prop.yoffset = 0.25;
      tex[1].prop.frames = 16;
      tex[1].prop.bidir = 1;
      tex[1].prop.playing = 1;
      tex[1].prop.padding=1;

      for(i=0; i < MAXBALLS; i++)
      {
        b[i].tex=tex[0];
        b[i].fireTex=tex[1];
        b[i].texTail=tex[2];
        b[i].tail.texture = tex[2].prop.texture;
      }

      initBalls();
    }

    void getSpeed()
    {
      int i;
      var.averageBallSpeed = 0.0;
      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          var.averageBallSpeed += b[i].velocity;
        }
      }
      var.averageBallSpeed /= activeBalls;
    }

    //klon alle aktive bolde
    void multiply()
    {
      pos op;
      int a=0,c=0;
      int i;
        //How many balls are active?
        for(i=0; i < MAXBALLS; i++)
        {
          if(b[i].active)
          {
            a++;
          }
        }

        for(i=0; i < MAXBALLS; i++)
        {
          if(b[i].active && c != a)
          {
            c++;
            op.y = b[i].posy;
            op.x = b[i].posx;
            spawn(op,0,0.0f,b[i].velocity, rndflt( BALL_MAX_DEGREE+BALL_MIN_DEGREE,0));
          }
        }

    }

    void unglue()
    {
      int i;
      for(i=0; i < MAXBALLS; i++)
      {
        b[i].glued=0;
      }
    }

    void spawn(pos p, bool glued, GLfloat gx ,GLfloat speed, GLfloat angle)
    {
      int i;
      for(i=0; i < MAXBALLS; i++)
      {
        if(!b[i].active)
        {
          activeBalls++;
          b[i].tex = tex[0];
          b[i].fireTex = tex[1];
          b[i].texTail = tex[2];
          b[i].glued=glued;

          b[i].width=0.0;
          b[i].height=0.0;
          b[i].gluedX=gx;
          b[i].active=1;
          b[i].collide=1;
          b[i].reflect=1;
          b[i].lastX = p.x;
          b[i].lastY = p.y;
          b[i].posx = p.x;
          b[i].posy = p.y;
          b[i].gravity=0;
          b[i].explosive=0;
          b[i].setspeed(speed);
          b[i].setangle(angle);
          b[i].setSize(0.025);

          //Find ud af om den skal "arve" powerups
          if(player.difficulty < 3)
          {
            b[i].explosive = player.powerup[PO_EXPLOSIVE];

            if(player.powerup[PO_SMALLBALL])
            {
              powerup(PO_SMALLBALL);
            }

            if(player.powerup[PO_BIGBALL])
            {
              powerup(PO_BIGBALL);
            }

          }
          getSpeed();
          break;
        }
      }


    }

    void clear()
    {
      int i;
      activeBalls=0;
        for(i=0; i < MAXBALLS; i++)
        {
           b[i].active=0;
        }
        getSpeed();
    }

    void move()
    {
      int a=0;
      int i;

      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          b[i].move();
          a++;
        }

      }
      activeBalls=a;
    }

    void draw(paddle_class &paddle)
    {
      int i;

      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          b[i].draw(paddle);
        }
      }

    }

    void bcoldet(brick & bri,effectManager & fxMan)
    {
      int i;
      pos p;
      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          p.x=100;
          coldet(bri, b[i], p, fxMan);
          if(p.x < 50) //we totally hit?? :P
          {

            getSpeed();

            if(setting.eyeCandy)
            {
              if(!b[i].gravity)
              {
                //spawn partikler
                fxMan.set(FX_VAR_TYPE, FX_SPARKS);
                fxMan.set(FX_VAR_COLDET,1);

                fxMan.set(FX_VAR_SPEED, 1.0f);

                fxMan.set(FX_VAR_LIFE, 1500);
                fxMan.set(FX_VAR_NUM, 16);
                fxMan.set(FX_VAR_SIZE, 0.015f);

                fxMan.set(FX_VAR_COLOR, 1.0,1.0,0.8);

                fxMan.spawn(p);
              }
            }
          }
        }
      }
    }

    int pcoldet(paddle_class & paddle,effectManager & fxMan)
    {
      int i, hits=0;
      pos p;
      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          if(b[i].glued)
            b[i].posx=paddle.posx+paddle.width-b[i].gluedX;

          p.x=100;
          padcoldet(b[i], paddle, p);
          if(p.x < 50)
          {
            hits++;
            getSpeed();

            if(b[i].gravity)
            {
              player.score += 1000*player.multiply;
            } else {

              if(player.powerup[PO_GLUE])
              {
                soundMan.add(SND_GLUE_BALL_HIT_PADDLE, p.x);
              } else {
                soundMan.add(SND_BALL_HIT_PADDLE, p.x);
              }
            }

            if(setting.eyeCandy)
            {
              //spawn partikler
              fxMan.set(FX_VAR_TYPE, FX_SPARKS);
              fxMan.set(FX_VAR_LIFE, 2000);
              fxMan.set(FX_VAR_GRAVITY, 0.6f);
              fxMan.set(FX_VAR_NUM, 16);
              fxMan.set(FX_VAR_COLDET,1);
              fxMan.set(FX_VAR_SIZE, 0.01f);
              fxMan.set(FX_VAR_COLOR, 1.0,1.0,0.8);
              p.y = paddle.posy+paddle.height;
              fxMan.set(FX_VAR_SPEED, 0.5f);
              fxMan.spawn(p);

              if(b[i].gravity)
              {
                p.y = paddle.posy+paddle.height;

                fxMan.set(FX_VAR_LIFE, 1000);
                fxMan.set(FX_VAR_NUM, 32);

                fxMan.set(FX_VAR_COLOR, 1.0,1.0,0.7);


                fxMan.set(FX_VAR_SPEED, 0.6f);
                fxMan.spawn(p);

                fxMan.set(FX_VAR_LIFE, 1500);
                fxMan.set(FX_VAR_NUM, 32);
                fxMan.set(FX_VAR_COLOR, 1.0,0.0,0.0);
                fxMan.spawn(p);

                fxMan.set(FX_VAR_SPEED, 0.7f);
                fxMan.set(FX_VAR_LIFE, 1500);
                fxMan.set(FX_VAR_NUM, 32);
                fxMan.set(FX_VAR_COLOR, 1.0,1.0,1.0);
                fxMan.spawn(p);

                b[i].active=0;
              }
            } //eyecandy
          } // if col
        } //if active
      } //for loop
      return(hits);
    } //pcoldet

    void updatelast()
    {
      int i;
      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          b[i].lastX=b[i].posx;
          b[i].lastY=b[i].posy;
        }
      }
    }

    void powerup(int powerup)
    {
      int i;
      for(i=0; i < MAXBALLS; i++)
      {
        if(b[i].active)
        {
          switch(powerup)
          {
            case PO_GRAVITY: //gravity
              b[i].gravity=1;
              b[i].died=globalTicks;
              break;
            case PO_BIGBALL: //big balls
              b[i].setSize(0.04);
              //Sløv bolden ned 10 eller 20%
              b[i].setspeed( b[i].velocity - ((b[i].velocity/100.f)*difficulty.slowdown[player.difficulty]) );
              break;
            case PO_SMALLBALL: //small balls
              b[i].setSize(0.015);
              //speed bolden op
              b[i].setspeed( b[i].velocity + ((b[i].velocity/100.f)*difficulty.speedup[player.difficulty]) );
              break;
            case PO_NORMALBALL: // normal balls
              b[i].setSize(0.025);
              b[i].setspeed(difficulty.ballspeed[player.difficulty]);
              break;
            case PO_EXPLOSIVE: //exploderer brikker
              b[i].explosive=1;
              b[i].tail.colorRotate(TRUE, 0 );
            break;
          }
        }
      }
    }
};

float bounceOffAngle(GLfloat width, GLfloat posx, GLfloat hitx)
{
  return ( (BALL_MAX_DEGREE/(width*2.0))*(posx+width-hitx) + BALL_MIN_DEGREE );
}


class powerupClass : public moving_object {
  public:
    int score;
    int type;
    int level, maxlevel;
    GLfloat gravity;

    powerupClass()
    {
      posx=0.0;
      posy=0.0;
      xvel=0.0;
      yvel=0.0;
      width=0.055;
      height=0.055;
    }

    void move()
    {

      //grav
      yvel -= gravity*globalMilliTicks;
      //cout << yvel << endl;
      posx +=xvel*globalMilliTicks;
      posy +=yvel*globalMilliTicks;
    }


    bool coldet(paddle_class & p, effectManager & fxMan, ballManager & bMan)
    {
      bool col=0;
      if(posx+width > 1.6 && xvel > 0.0)
      {
        col=1;
        xvel *= -1;
      } else if(posx-width < -1.6 && xvel < 0.0)
      {
        col=1;
        xvel *= -1;
      } else if(posy+width > 1.25 && yvel > 0.0)
      {
        col=1;
        yvel *= -1;
      } else if(posy-width < -1.24)
      {
        active=0;
      }

      if(col)
      {
        soundMan.add(SND_PO_HIT_BORDER, posx);
      }

      //idiotisk lavet...

      bool ycol=0;
      bool xcol=0;
      pos fxpos, fxSize;

      //En side
      if(posx+width > p.posx-p.width &&  posx+width < p.posx+p.width)
      {
        xcol=1;
      }

      if(posx-width > p.posx-p.width &&  posx-width < p.posx+p.width)
      {
        xcol=1;
      }

      if(posy-height < p.posy+p.height  &&  posy-height > p.posy-p.height)
      {
        ycol=1;
      }

      if(posy+height < p.posy+p.height  &&  posy+height > p.posy-p.height)
      {
        ycol=1;
      }

      if(xcol && ycol)
      {
        if(setting.eyeCandy)
        {
          fxpos.x = posx;
          fxpos.y = posy;
          fxSize.x=width;
          fxSize.y=height;

          fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);
          fxMan.set(FX_VAR_COLDET,1);
          fxMan.set(FX_VAR_SPEED, yvel/1.5f);
          fxMan.set(FX_VAR_LIFE, 1500);
          fxMan.set(FX_VAR_GRAVITY, 0.7f);
          fxMan.set(FX_VAR_NUM, 20);
          fxMan.set(FX_VAR_SIZE, 0.03f);

          fxMan.set(FX_VAR_RECTANGLE, fxSize);

          fxMan.set(FX_VAR_COLOR, color[0], color[1], color[2]);
          fxMan.spawn(fxpos);
        }
        active=0;

        //Score
        player.score += score*player.multiply;
        //Apply powerup:
        switch(type)
        {
          case PO_GLUE:
            player.coins += 150;
            player.powerup[PO_GLUE] = 1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_BIGBALL:
            player.coins += 30;
            bMan.powerup(PO_BIGBALL);
            player.powerup[PO_BIGBALL]=1;
            player.powerup[PO_NORMALBALL]=0;
            player.powerup[PO_SMALLBALL]=0;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_NORMALBALL:
            player.coins += 50;
            bMan.powerup(PO_NORMALBALL);
            player.powerup[PO_NORMALBALL]=1;
            player.powerup[PO_BIGBALL]=0;
            player.powerup[PO_SMALLBALL]=0;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_SMALLBALL:
            player.coins += 10;
            bMan.powerup(PO_SMALLBALL);
            player.powerup[PO_SMALLBALL]=1;
            player.powerup[PO_BIGBALL]=0;
            player.powerup[PO_NORMALBALL]=0;
            soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
            break;
          case PO_GRAVITY:
            player.coins += 5;
            bMan.powerup(PO_GRAVITY);
            soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
            break;
          case PO_MULTIBALL:
            player.coins += 100;
            bMan.multiply();
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_AIM:
            player.coins += 50;
            if(player.difficulty==0)
            {
              player.powerup[PO_GLUE]=1;
            }
            if(!player.powerup[PO_AIM])
            {
              player.powerup[PO_AIM]=1;
              player.powerup[PO_LASER]=1;
            } else {
              player.powerup[PO_GLUE]=1;
            }
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_GROWPADDLE:
            player.coins += 100;
            if(p.width < 0.4)
              p.grow(p.width+0.03);
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_SHRINKPADDLE:
            player.coins += 10;
            if(p.width > 0.02) p.grow(p.width-0.02);
            soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
            break;
          case PO_EXPLOSIVE:
            player.coins += 150;
            bMan.powerup(PO_EXPLOSIVE);
            player.powerup[PO_EXPLOSIVE]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_GUN:
            player.coins += 200;
            player.powerup[PO_GUN]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_THRU:
            player.coins += 300;
            player.powerup[PO_THRU]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_LASER:
            player.coins += 40;
            player.powerup[PO_LASER]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_LIFE:
            player.coins += 400;
            player.lives++;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_DIE:
            player.coins += 1;
            player.explodePaddle=1;
            player.powerup[PO_DIE]=1;
            //NOTE: no sound here, SND_DIE is played when paddle dissapers
            break;
          case PO_DROP:
            player.coins += 1;
            player.powerup[PO_DROP]=1;
            soundMan.add(SND_EVIL_PO_HIT_PADDLE, posx);
            break;
          case PO_DETONATE:
            player.coins += 200;
            player.powerup[PO_DETONATE]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_EXPLOSIVE_GROW:
            player.coins += 100;
            player.powerup[PO_EXPLOSIVE_GROW]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_EASYBRICK:
            player.coins += 90;
            player.powerup[PO_EASYBRICK]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
          case PO_NEXTLEVEL:
            player.coins += 100;
            player.powerup[PO_NEXTLEVEL]=1;
            //NOTE: no sound here, SND_NEXTLEVEL is played when changing level
            break;
          case PO_AIMHELP:
            player.coins += 50;
            player.powerup[PO_AIMHELP]=1;
            soundMan.add(SND_GOOD_PO_HIT_PADDLE, posx);
            break;
        }
        return(1);
      }

      return(0);
    }

    void die(effectManager & fxMan)
    {
      active=0;
      if(setting.eyeCandy)
      {
        struct pos p;
        p.x = posx;
        p.y = posy;
        fxMan.set(FX_VAR_TYPE, FX_SPARKS);
        fxMan.set(FX_VAR_COLDET,1);
        fxMan.set(FX_VAR_LIFE, 1000);
        fxMan.set(FX_VAR_NUM, 16);

        fxMan.set(FX_VAR_SPEED, 0.8f);
        fxMan.set(FX_VAR_GRAVITY, 0.6f);
        fxMan.set(FX_VAR_SIZE, 0.025f);
        fxMan.set(FX_VAR_COLOR, color[0], color[1], color[2]);
        fxMan.spawn(p);

        fxMan.set(FX_VAR_SPEED, 0.4f);
        fxMan.set(FX_VAR_SIZE, 0.05f);
        fxMan.set(FX_VAR_GRAVITY, -1.0f);
        fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
        fxMan.spawn(p);
        fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
        fxMan.spawn(p);
        fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
        fxMan.spawn(p);
        fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
        fxMan.spawn(p);

      }
    }

    void draw()
    {
      tex.play();

      glBindTexture( GL_TEXTURE_2D, tex.prop.texture);
      glColor4f(1.0, 1.0, 1.0, 1.0);
      glLoadIdentity();
      glTranslatef( posx, posy, -3.0f);
      glBegin( GL_QUADS );
        glTexCoord2f(tex.pos[0],tex.pos[1]);glVertex3f( -width, height, 0.00 ); // øverst venst
        glTexCoord2f(tex.pos[2],tex.pos[3]);glVertex3f(  width, height, 0.00 ); // øverst højre
        glTexCoord2f(tex.pos[4],tex.pos[5]);glVertex3f(  width,-height, 0.00 ); // nederst højre
        glTexCoord2f(tex.pos[6],tex.pos[7]);glVertex3f( -width,-height, 0.00 ); // nederst venstre
      glEnd( );
    }
};

#define MAXPOWERUPS 64
class powerupManager {
  private:
  int i;
  powerupClass p[MAXPOWERUPS];
  textureClass* tex;
  public:

    void init(textureClass texPowerup[])
    {
      //Sætter textures op for de forskellige powerups

      //Glue
      texPowerup[PO_GLUE].prop.frames = 6;
      texPowerup[PO_GLUE].prop.ticks = 100;
      texPowerup[PO_GLUE].prop.rows= 4;
      texPowerup[PO_GLUE].prop.cols= 2  ;
      texPowerup[PO_GLUE].prop.xoffset=0.5;
      texPowerup[PO_GLUE].prop.yoffset=1.0/4.0;
      texPowerup[PO_GLUE].prop.playing=1;
      texPowerup[PO_GLUE].prop.padding=1;
      texPowerup[PO_GLUE].prop.bidir=0;
      
      //gravity
      texPowerup[PO_GRAVITY].prop.frames = 1;
      texPowerup[PO_GRAVITY].prop.ticks = 1000;
      texPowerup[PO_GRAVITY].prop.rows=1;
      texPowerup[PO_GRAVITY].prop.cols=1;
      texPowerup[PO_GRAVITY].prop.xoffset=1;
      texPowerup[PO_GRAVITY].prop.yoffset=1;
      texPowerup[PO_GRAVITY].prop.bidir=0;
      texPowerup[PO_GRAVITY].prop.playing=0;
      texPowerup[PO_GRAVITY].prop.padding=1;

      //multiball
      texPowerup[PO_MULTIBALL].prop.frames = 1;
      texPowerup[PO_MULTIBALL].prop.ticks = 1000;
      texPowerup[PO_MULTIBALL].prop.rows= 1;
      texPowerup[PO_MULTIBALL].prop.cols= 1;
      texPowerup[PO_MULTIBALL].prop.xoffset=1.0;
      texPowerup[PO_MULTIBALL].prop.yoffset=1.0;
      texPowerup[PO_MULTIBALL].prop.bidir=0;
      texPowerup[PO_MULTIBALL].prop.playing=0;
      texPowerup[PO_MULTIBALL].prop.padding=1;

      //bigball
      texPowerup[PO_BIGBALL].prop.frames = 1;
      texPowerup[PO_BIGBALL].prop.ticks = 1000;
      texPowerup[PO_BIGBALL].prop.rows= 1;
      texPowerup[PO_BIGBALL].prop.cols= 1;
      texPowerup[PO_BIGBALL].prop.xoffset=1;
      texPowerup[PO_BIGBALL].prop.yoffset=1;
      texPowerup[PO_BIGBALL].prop.bidir=0;
      texPowerup[PO_BIGBALL].prop.playing=0;
      texPowerup[PO_BIGBALL].prop.padding=0;


      //normalball
      texPowerup[PO_NORMALBALL].prop.frames = 1;
      texPowerup[PO_NORMALBALL].prop.ticks = 1000;
      texPowerup[PO_NORMALBALL].prop.rows= 1;
      texPowerup[PO_NORMALBALL].prop.cols= 1;
      texPowerup[PO_NORMALBALL].prop.xoffset=1;
      texPowerup[PO_NORMALBALL].prop.yoffset=1;
      texPowerup[PO_NORMALBALL].prop.bidir=0;
      texPowerup[PO_NORMALBALL].prop.playing=0;
      texPowerup[PO_NORMALBALL].prop.padding=1;

      //smallball
      texPowerup[PO_SMALLBALL].prop.frames = 1;
      texPowerup[PO_SMALLBALL].prop.ticks = 1000;
      texPowerup[PO_SMALLBALL].prop.rows= 1;
      texPowerup[PO_SMALLBALL].prop.cols= 1;
      texPowerup[PO_SMALLBALL].prop.xoffset=1;
      texPowerup[PO_SMALLBALL].prop.yoffset=1;
      texPowerup[PO_SMALLBALL].prop.bidir=0;
      texPowerup[PO_SMALLBALL].prop.playing=0;
      texPowerup[PO_SMALLBALL].prop.padding=1;

      //Aim
      texPowerup[PO_AIM].prop.frames = 8;
      texPowerup[PO_AIM].prop.ticks = 100;
      texPowerup[PO_AIM].prop.rows= 4;
      texPowerup[PO_AIM].prop.cols= 2;
      texPowerup[PO_AIM].prop.xoffset=0.5;
      texPowerup[PO_AIM].prop.yoffset=1.0/4.0;
      texPowerup[PO_AIM].prop.bidir=1;
      texPowerup[PO_AIM].prop.playing=1;
      texPowerup[PO_AIM].prop.padding=1;

      //Growpaddle
      texPowerup[PO_GROWPADDLE].prop.frames = 1;
      texPowerup[PO_GROWPADDLE].prop.ticks = 1000;
      texPowerup[PO_GROWPADDLE].prop.rows= 1;
      texPowerup[PO_GROWPADDLE].prop.cols= 1;
      texPowerup[PO_GROWPADDLE].prop.xoffset=1;
      texPowerup[PO_GROWPADDLE].prop.yoffset=1;
      texPowerup[PO_GROWPADDLE].prop.bidir=0;
      texPowerup[PO_GROWPADDLE].prop.playing=0;
      texPowerup[PO_GROWPADDLE].prop.padding=1;

      //Shrinkpaddle
      texPowerup[PO_SHRINKPADDLE].prop.frames = 1;
      texPowerup[PO_SHRINKPADDLE].prop.ticks = 1000;
      texPowerup[PO_SHRINKPADDLE].prop.rows= 1;
      texPowerup[PO_SHRINKPADDLE].prop.cols= 1;
      texPowerup[PO_SHRINKPADDLE].prop.xoffset=1;
      texPowerup[PO_SHRINKPADDLE].prop.yoffset=1;
      texPowerup[PO_SHRINKPADDLE].prop.bidir=0;
      texPowerup[PO_SHRINKPADDLE].prop.playing=0;
      texPowerup[PO_SHRINKPADDLE].prop.padding=1;

      //Explosive
      texPowerup[PO_EXPLOSIVE].prop.frames = 1;
      texPowerup[PO_EXPLOSIVE].prop.ticks = 1000;
      texPowerup[PO_EXPLOSIVE].prop.rows= 1;
      texPowerup[PO_EXPLOSIVE].prop.cols= 1;
      texPowerup[PO_EXPLOSIVE].prop.xoffset=1;
      texPowerup[PO_EXPLOSIVE].prop.yoffset=1;
      texPowerup[PO_EXPLOSIVE].prop.bidir=0;
      texPowerup[PO_EXPLOSIVE].prop.playing=0;
      texPowerup[PO_EXPLOSIVE].prop.padding=1;

      //Gun
      texPowerup[PO_GUN].prop.frames = 3;
      texPowerup[PO_GUN].prop.ticks = 150;
      texPowerup[PO_GUN].prop.rows= 2;
      texPowerup[PO_GUN].prop.cols= 2;
      texPowerup[PO_GUN].prop.xoffset=0.5;
      texPowerup[PO_GUN].prop.yoffset=0.5;
      texPowerup[PO_GUN].prop.bidir=0;
      texPowerup[PO_GUN].prop.playing=1;
      texPowerup[PO_GUN].prop.padding=1;

      //Thru
      texPowerup[PO_THRU].prop.frames = 1;
      texPowerup[PO_THRU].prop.ticks = 1000;
      texPowerup[PO_THRU].prop.rows= 1;
      texPowerup[PO_THRU].prop.cols= 1;
      texPowerup[PO_THRU].prop.xoffset=1;
      texPowerup[PO_THRU].prop.yoffset=1;
      texPowerup[PO_THRU].prop.bidir=0;
      texPowerup[PO_THRU].prop.playing=0;
      texPowerup[PO_THRU].prop.padding=1;

      //Laser
      texPowerup[PO_LASER].prop.frames = 1;
      texPowerup[PO_LASER].prop.ticks = 1000;
      texPowerup[PO_LASER].prop.rows= 1;
      texPowerup[PO_LASER].prop.cols= 1;
      texPowerup[PO_LASER].prop.xoffset=1;
      texPowerup[PO_LASER].prop.yoffset=1;
      texPowerup[PO_LASER].prop.bidir=0;
      texPowerup[PO_LASER].prop.playing=0;

      //Teh extra life
      texPowerup[PO_LIFE].prop.frames = 1;
      texPowerup[PO_LIFE].prop.ticks = 1000;
      texPowerup[PO_LIFE].prop.rows= 1;
      texPowerup[PO_LIFE].prop.cols= 1;
      texPowerup[PO_LIFE].prop.xoffset=1;
      texPowerup[PO_LIFE].prop.yoffset=1;
      texPowerup[PO_LIFE].prop.bidir=0;
      texPowerup[PO_LIFE].prop.playing=0;
      texPowerup[PO_LIFE].prop.padding=1;

      //Die powerup
      texPowerup[PO_DIE].prop.frames = 1;
      texPowerup[PO_DIE].prop.ticks = 1000;
      texPowerup[PO_DIE].prop.rows= 1;
      texPowerup[PO_DIE].prop.cols= 1;
      texPowerup[PO_DIE].prop.xoffset=1;
      texPowerup[PO_DIE].prop.yoffset=1;
      texPowerup[PO_DIE].prop.bidir=0;
      texPowerup[PO_DIE].prop.playing=0;
      texPowerup[PO_DIE].prop.padding=1;

      //PO_DROP
      texPowerup[PO_DROP].prop.frames = 1;
      texPowerup[PO_DROP].prop.ticks = 1000;
      texPowerup[PO_DROP].prop.rows= 1;
      texPowerup[PO_DROP].prop.cols= 1;
      texPowerup[PO_DROP].prop.xoffset=1;
      texPowerup[PO_DROP].prop.yoffset=1;
      texPowerup[PO_DROP].prop.bidir=0;
      texPowerup[PO_DROP].prop.playing=0;
      texPowerup[PO_DROP].prop.padding=1;

      //PO_DETONATE
      texPowerup[PO_DETONATE].prop.frames = 1;
      texPowerup[PO_DETONATE].prop.ticks = 1000;
      texPowerup[PO_DETONATE].prop.rows= 1;
      texPowerup[PO_DETONATE].prop.cols= 1;
      texPowerup[PO_DETONATE].prop.xoffset=1;
      texPowerup[PO_DETONATE].prop.yoffset=1;
      texPowerup[PO_DETONATE].prop.bidir=0;
      texPowerup[PO_DETONATE].prop.playing=0;
      texPowerup[PO_DETONATE].prop.padding=1;

      //PO_EXPLOSIVE_GROW
      texPowerup[PO_EXPLOSIVE_GROW].prop.frames = 3;
      texPowerup[PO_EXPLOSIVE_GROW].prop.ticks = 300;
      texPowerup[PO_EXPLOSIVE_GROW].prop.rows= 2;
      texPowerup[PO_EXPLOSIVE_GROW].prop.cols= 2;
      texPowerup[PO_EXPLOSIVE_GROW].prop.xoffset=0.5;
      texPowerup[PO_EXPLOSIVE_GROW].prop.yoffset=0.5;
      texPowerup[PO_EXPLOSIVE_GROW].prop.bidir=0;
      texPowerup[PO_EXPLOSIVE_GROW].prop.playing=1;
      texPowerup[PO_EXPLOSIVE_GROW].prop.padding=1;

      //PO_EASYBRICK
      texPowerup[PO_EASYBRICK].prop.frames = 1;
      texPowerup[PO_EASYBRICK].prop.ticks = 1000;
      texPowerup[PO_EASYBRICK].prop.rows= 1;
      texPowerup[PO_EASYBRICK].prop.cols= 1;
      texPowerup[PO_EASYBRICK].prop.xoffset=1;
      texPowerup[PO_EASYBRICK].prop.yoffset=1;
      texPowerup[PO_EASYBRICK].prop.bidir=0;
      texPowerup[PO_EASYBRICK].prop.playing=0;
      texPowerup[PO_EASYBRICK].prop.padding=1;

      //PO_NEXTLEVEL
      texPowerup[PO_NEXTLEVEL].prop.frames = 1;
      texPowerup[PO_NEXTLEVEL].prop.ticks = 1000;
      texPowerup[PO_NEXTLEVEL].prop.rows= 1;
      texPowerup[PO_NEXTLEVEL].prop.cols= 1;
      texPowerup[PO_NEXTLEVEL].prop.xoffset=1;
      texPowerup[PO_NEXTLEVEL].prop.yoffset=1;
      texPowerup[PO_NEXTLEVEL].prop.bidir=0;
      texPowerup[PO_NEXTLEVEL].prop.playing=0;
      texPowerup[PO_NEXTLEVEL].prop.padding=1;

      //PO_AIMHELP
      texPowerup[PO_AIMHELP].prop.frames = 6;
      texPowerup[PO_AIMHELP].prop.ticks = 100;
      texPowerup[PO_AIMHELP].prop.rows= 3;
      texPowerup[PO_AIMHELP].prop.cols= 2;
      texPowerup[PO_AIMHELP].prop.xoffset=0.5;
      texPowerup[PO_AIMHELP].prop.yoffset=1.0/4.0;
      texPowerup[PO_AIMHELP].prop.bidir=1;
      texPowerup[PO_AIMHELP].prop.playing=1;
      texPowerup[PO_AIMHELP].prop.padding=1;



      tex = new textureClass[40];
      tex = texPowerup;
    }
    powerupManager()
    {
      clear();
    }

    void clear()
    {
      for(i=0; i < MAXPOWERUPS; i++)
      {
        p[i].active=0;
      }
    }

    void die(effectManager & fxMan)
    {
      for(i=0; i < MAXPOWERUPS; i++)
      {
        if(p[i].active)
        {
          p[i].die(fxMan);
        }
      }
    }

    void spawn(pos spawnpos, pos velocity, int type)
    {
      for(i=0; i < MAXPOWERUPS; i++)
      {
        if(!p[i].active)
        {
          p[i].gravity = 0.7;
          p[i].type = type;
          p[i].posx = spawnpos.x;
          p[i].posy = spawnpos.y;
          p[i].xvel = velocity.x*-1;
          p[i].yvel = velocity.y*-1;
          p[i].active=1;

          //Give texture that this type has.
          p[i].tex = tex[type];

          //Set colors and score
          if(type==PO_GLUE)
          {
            p[i].score = 500;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_GRAVITY)
          {
            p[i].score = -600;
            p[i].color[0]=1.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_MULTIBALL)
          {
            p[i].score = 500;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;

          }

          if(type==PO_BIGBALL)
          {
            p[i].score = 300;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_NORMALBALL)
          {
            p[i].score = 400;
            p[i].color[0]=0.5f;
            p[i].color[1]=0.5f;
            p[i].color[2]=0.5f;
          }

          if(type==PO_SMALLBALL)
          {
            p[i].score = 100;
            p[i].color[0]=1.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_AIM)
          {
            p[i].score = 1600;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_GROWPADDLE)
          {
            p[i].score = 500;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_SHRINKPADDLE)
          {
            p[i].score = -1000;
            p[i].color[0]=1.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_EXPLOSIVE)
          {
            p[i].score = 1400;
            p[i].color[0]=0.0f;
            p[i].color[1]=1.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_GUN)
          {
            p[i].score = 1800;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_THRU)
          {

            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_LASER)
          {
            p[i].score = 500;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_LIFE)
          {
            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_DIE)
          {
            p[i].score = -1000;
            p[i].color[0]=1.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_DROP)
          {
            p[i].score = -1000;
            p[i].color[0]=1.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_DETONATE)
          {
            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=1.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_EXPLOSIVE_GROW)
          {
            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=1.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_EASYBRICK)
          {
            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=1.0f;
            p[i].color[2]=0.0f;
          }

          if(type==PO_NEXTLEVEL)
          {
            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=0.0f;
            p[i].color[2]=1.0f;
          }

          if(type==PO_AIMHELP)
          {
            p[i].score = 1000;
            p[i].color[0]=0.0f;
            p[i].color[1]=1.0f;
            p[i].color[2]=0.0f;
          }

          break; //Whats this doing?
        }
      }
    }

    int coldet(paddle_class & paddle, effectManager & fxMan, ballManager & bMan)
    {
      int hits=0;
      for(i=0; i < MAXPOWERUPS; i++)
      {
        if(p[i].active)
        {
          if(p[i].coldet(paddle, fxMan, bMan))
          {
            hits++;
          }
        }
      }
      return(hits);
    }

    void move()
    {
      for(i=0; i < MAXPOWERUPS; i++)
      {
        if(p[i].active)
        {
          p[i].move();
        }
      }
    }

    void draw()
    {
      for(i=0; i < MAXPOWERUPS; i++)
      {
        if(p[i].active)
        {
          p[i].draw();
        }
      }
    }
};

powerupManager pMan;

void spawnpowerup(char powerup, pos a, pos b)
{

  if(powerup == '1')
  {
    pMan.spawn(a,b,PO_GROWPADDLE);
  }

  if(powerup == '2')
  {
    pMan.spawn(a,b,PO_SHRINKPADDLE);
  }

  if(powerup == '3')
  {
    pMan.spawn(a,b,PO_DIE);
  }

  if(powerup == '4')
  {
    pMan.spawn(a,b,PO_GLUE);
  }

  if(powerup == 'A')
  {
    pMan.spawn(a,b,PO_EASYBRICK);
  }

  if(powerup == 'B')
  {
    pMan.spawn(a,b,PO_EXPLOSIVE);
  }

  if(powerup == 'C')
  {
    pMan.spawn(a,b,PO_NEXTLEVEL);
  }

  if(powerup == 'D')
  {
    pMan.spawn(a,b,PO_AIMHELP);
  }

  if(powerup == 'E')
  {
    pMan.spawn(a,b,PO_GRAVITY);
  }

  if(powerup == '5')
  {
    pMan.spawn(a,b,PO_MULTIBALL);
  }

  if(powerup == '6')
  {
    pMan.spawn(a,b,PO_THRU);
  }

  if(powerup == '7')
  {
    pMan.spawn(a,b,PO_DROP);
  }

  if(powerup == '8')
  {
    pMan.spawn(a,b,PO_DETONATE);
  }

  if(powerup == '9')
  {
    pMan.spawn(a,b,PO_EXPLOSIVE_GROW);
  }

  if(powerup == 'F')
  {
    pMan.spawn(a,b,PO_BIGBALL);
  }

  if(powerup == 'G')
  {
    pMan.spawn(a,b,PO_NORMALBALL);
  }

  if(powerup == 'H')
  {
    pMan.spawn(a,b,PO_SMALLBALL);
  }

  if(powerup == 'I')
  {
    pMan.spawn(a,b,PO_AIM);
  }

  if(powerup == 'P')
  {
    pMan.spawn(a,b,PO_GUN);
  }

  if(powerup == 'R')
  {
    pMan.spawn(a,b,PO_LASER);
  }

  if(powerup == 'O')
  {
    pMan.spawn(a,b,PO_LIFE);
  }
}

SDL_Surface *screen = NULL;

/* function to reset our viewport after a window resize */
void resizeWindow( int width, int height )
{
    /* Height / width ration */
    GLfloat ratio;

    /* Protect against a divide by zero */
    if ( height == 0 )
      height = 1;

    ratio = ( GLfloat )width / ( GLfloat )height;
    var.glunits_per_xpixel = (2.485281374*ratio) / setting.resx;
    var.glunits_per_ypixel = 2.485281374 / setting.resy;


    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 10.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity();
}

void initGL() {

//     printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
   // printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
//     printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
    //printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
  //  glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
     glDepthFunc( GL_LEQUAL );

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

bool writeTxt(TTF_Font *font, SDL_Color textColor,const char text[], GLuint texture, bool center)
{
  SDL_Surface *temp,*tempb;

  Uint32 rmask, gmask, bmask, amask;
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

  if(font == NULL)
  {
    cout << SDL_GetError() << endl;;
    SDL_FreeSurface(temp);
    SDL_FreeSurface(tempb);
    return(FALSE);
  }

  temp = TTF_RenderText_Blended(font, text, textColor);
  SDL_SetAlpha(temp, 0, 0);

  tempb = SDL_CreateRGBSurface(0, 512, 512, 32, rmask,gmask,bmask,amask);

  int w,h;
  TTF_SizeUTF8(font,text, &w,&h);

  SDL_Rect src,dst;
  src.x=0;
  src.y=0;
  src.w=w;
  src.h=h;

  if(center)
  {
    dst.x=(512.0-w)/2.0;
  } else {
    dst.x=0;
  }
  dst.y=0;
  dst.w=w;
  dst.h=h;

  SDL_BlitSurface(temp, &src, tempb, &dst);
  glBindTexture(GL_TEXTURE_2D, texture);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tempb->w, tempb->h, GL_RGBA, GL_UNSIGNED_BYTE, tempb->pixels);

  SDL_FreeSurface(temp);
  SDL_FreeSurface(tempb);
  return(TRUE);
}

float rndflt(float total, float negative)
{

  return (rand()/(float(RAND_MAX)+1)*total)-negative;
}

void initScreen()
{
  int SDL_videomodeSettings = SDL_OPENGL|SDL_RESIZABLE;

  if(setting.fullscreen)
    SDL_videomodeSettings |= SDL_FULLSCREEN;

  screen = SDL_SetVideoMode(setting.resx,setting.resy,32, SDL_videomodeSettings);
  resizeWindow(setting.resx,setting.resy);

  if( screen == NULL )
  {
    cout << "Error:" << SDL_GetError() << endl;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  var.halfresx = setting.resx /2;
  var.halfresy = setting.resy / 2;

}

void resetPlayerPowerups()
{
  player.powerup[PO_GRAVITY] = 0;

  player.powerup[PO_BIGBALL] = 0;
  player.powerup[PO_NORMALBALL] = 0;
  player.powerup[PO_SMALLBALL] = 0;


  player.powerup[PO_EXPLOSIVE] = 0;

  player.powerup[PO_GLUE] = 0;
  player.powerup[PO_MULTIBALL] = 0;

  player.powerup[PO_GROWPADDLE] = 0;
  player.powerup[PO_SHRINKPADDLE] = 0;


  player.powerup[PO_AIM] = 0;
  player.powerup[PO_LASER] = 0;

  player.powerup[PO_GUN] = 0;
  player.powerup[PO_THRU] = 0;
  player.powerup[PO_DROP] = 0;
  player.powerup[PO_DETONATE] = 0;

  player.powerup[PO_AIMHELP] = 0;

}

void initNewGame()
{
  player.level=0;
  player.score=0;
  gVar.deadTime=0;

  gVar.newLevel=1;
  gVar.newLife=1;

  player.multiply = 1;

  player.lives=5;
  player.coins = 600;
  //Easy skal ikke clears af powerups
  if(player.difficulty > 0)
  {
    player.coins = 0;
    player.lives -= 2;
  }

  resetPlayerPowerups();
}

void pauseGame()
{
  var.paused=1;
#ifndef WIN32
  SDL_WM_GrabInput(SDL_GRAB_OFF);
#endif
  SDL_ShowCursor(SDL_ENABLE);
}

void resumeGame()
{
#ifndef WIN32
  SDL_WM_GrabInput(SDL_GRAB_ON);
#endif
  SDL_ShowCursor(SDL_DISABLE);
  var.paused=0;
  var.menu=0;
}

void mkDLscene(GLuint *dl,textureClass tex)
{
  //Scenen
  *dl = glGenLists(1);
  glNewList(*dl,GL_COMPILE);
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -3.0 );
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
    glBegin( GL_POINTS );
      glVertex3f( -1.60, 1.25, 0.0 );
    glEnd( );
    glBegin( GL_QUADS );
      //venstre kant
      glTexCoord2f(0.0f,0.0f);glVertex3f( -1.66, 1.25, 0.0 );
      glTexCoord2f(1.0f,0.0f);glVertex3f( -1.60, 1.25, 0.0 );
      glTexCoord2f(1.0f,-1.0f);glVertex3f( -1.60,-1.25, 0.0 );
      glTexCoord2f(0.0f,-1.0f);glVertex3f( -1.66,-1.25, 0.0 );
      //højre kant
      glTexCoord2f(0.0f,0.0f);glVertex3f( 1.66, 1.25, 0.0 );
      glTexCoord2f(1.0f,0.0f);glVertex3f( 1.60, 1.25, 0.0 );
      glTexCoord2f(1.0f,-1.0f);glVertex3f( 1.60,-1.25, 0.0 );
      glTexCoord2f(0.0f,-1.0f);glVertex3f( 1.66,-1.25, 0.0 );
    glEnd( );
  glEndList();
}

void coldet(brick & br, ball &ba, pos & p, effectManager & fxMan)
{

  GLfloat x,y;
  int i=0;

  int points=0;
  GLfloat px=0,py=0;
  bool col=0;
  bool dirfound=0;
  GLfloat dist[4] = { 4.0, 4.0, 4.0, 4.0 }; //measure the distance from last pos to each possible impact, the shortest should be the right one

  //vi tager y først da der er mindst brikker
  if(ba.posy <= br.posy+br.height+ba.height && ba.posy >= br.posy-br.height-ba.height)
  {
    //cout << " y " << endl;
    if(ba.posx >= br.posx-br.width-ba.width && ba.posx <= br.posx+br.width+ba.width)
    {
      //cout << " x " << endl;
      for(i=0; i < 32; i++) // 32 punkter præcis
      {

        x = ba.bsin[i];
        y = ba.bcos[i];

        if(ba.posx+x >= br.posx-br.width  &&  ba.posx+x <= br.posx+br.width)
        {
          if(ba.posy+y <= br.posy+br.height  && ba.posy+y >= br.posy-br.height)
          {
            //Vi har helt sikkert ramt
            points++;
            px += x;
            py += y;
            col=1;
          } //y
        } //x
      } //32 punkters for loop

      if(col)
      {

        px /= (float)points;
        py /= (float)points;

        if(ba.lastX-px <= br.posx-br.width && !br.n(0)) //
        {
          dirfound=1;
       //    cout << "På venstre"<<endl;
          dist[0] = sqrt( pow( br.posx-br.width - (ba.lastX+px), 2) + pow( (ba.posy+py) - (ba.lastY+py), 2) );
        }

        if(ba.lastX-px >= br.posx+br.width  && !br.n(1))
        {
          dirfound=1;
          // cout << "På højre"<<endl;
          dist[1] = sqrt( pow( br.posx+br.width - (ba.lastX+px), 2) + pow( (ba.posy+py) - (ba.lastY+py), 2) );
        }

        if(ba.lastY-py <= br.posy-br.height && !br.n(3))
        {
          dirfound=1;
         // cout << "På bunden" << endl;
          dist[2] = sqrt( pow( (ba.posx+px) - (ba.lastX+px), 2) + pow( br.posy-br.height - (ba.lastY+py), 2) );

        }

        if(ba.lastY-py >= br.posy+br.height && !br.n(2)) // &&
        {
          dirfound=1;
          // cout << "På toppen"<< endl;
          dist[3] = sqrt( pow( (ba.posx+px) - (ba.lastX+px), 2) + pow( br.posy+br.height - (ba.lastY+py), 2) );
        }


        //Was hit on left
        if(dist[0] < dist[1] && dist[0] < dist[2] && dist[0] < dist[3])
        {
          ba.posx = br.posx - br.width - ba.width;
          if(ba.xvel > 0.0 && !player.powerup[PO_THRU])
            ba.xvel *=-1;
        }

        //Was hit on right
        if(dist[1] < dist[0] && dist[1] < dist[2] && dist[1] < dist[3])
        {
          ba.posx = br.posx + br.width + ba.width;
          if(ba.xvel < 0 && !player.powerup[PO_THRU])
            ba.xvel *=-1;
        }

        //Was hit on bottom
        if(dist[2] < dist[0] && dist[2] < dist[1] && dist[2] < dist[3])
        {
          ba.posy = br.posy - br.height - ba.height;
          if(ba.yvel > 0 && !player.powerup[PO_THRU])
            ba.yvel *=-1;
        }

        //Was hit on top
        if(dist[3] < dist[0] && dist[3] < dist[1] && dist[3] < dist[2])
        {
          ba.posy = br.posy + br.height + ba.height;
          if(ba.yvel < 0 && !player.powerup[PO_THRU])
            ba.yvel *=-1;
        }

        //Setup vars for spawning powerups
        pos a,b;
        a.x = br.posx;
        a.y = br.posy;

        //Hastigheden en powerup blier sendt afsted med
        if(player.difficulty == EASY)
        {
          b.x = ba.xvel/2.0;
          b.y = ba.yvel/2.0;
        } else if(player.difficulty == NORMAL)
        {
          b.x = ba.xvel/1.5;
          b.y = ba.yvel/1.5;
        }

        if(dirfound)
        {
          if(ba.explosive)
          {
            br.type='B';
          }

          p.x = ba.posx+px;
          p.y = ba.posy+py;

          ba.hit(p, br.color);

          if(!player.powerup[PO_THRU])
          {
            ba.setspeed(ba.velocity + difficulty.hitbrickinc[player.difficulty]);
          }
        } else {
          cout << "Collision detection error: Dont know where the ball hit." <<endl;
        }
        br.hit(fxMan, a,b,1);
      } //collision
    } //x boxcol
  } //y boxcol

}


void padcoldet(ball & b, paddle_class & p, pos & po)
{
  int i,points=0;
  GLfloat x,y,px=0,py=0;
  bool col=0;
  //Er bolden tæt nok på?

  if(b.posy < (p.posy+p.height)+b.height && b.posy > p.posy-p.height)
  {

    if(b.posx > p.posx-(p.width*2.0)-b.width && b.posx < p.posx+(p.width*2.0)+b.width)
    {
      for(i=0; i < 32; i++)
      {
        x = b.bsin[i];
        y = b.bcos[i];

        //Find de punkter der er inden i padden
        if(b.posx+x > p.posx-p.width  &&  b.posx+x < p.posx+p.width)
        {
          if(b.posy+y < p.posy+p.height  &&  b.posy+y > p.posy-p.height)
          {
            col=1;

            px +=x;
            py +=y;
            points++;
          }
        }
      } //For loop

      if(col)
      {
        col=0;
        gVar.deadTime=0;
        px /= (float)points;
        py /= (float)points;

        px = b.posx+px;


        //Ved at reagere herinde fungerer yvel som en switch, så det kun sker een gang ;)
        if(b.yvel < 0)
        {
          b.posy=p.posy+p.height+b.height; //løft op over pad

          //Only decrease speed if the player does not have the go-thru powerup
          if(!player.powerup[PO_THRU])
          {
            b.setspeed(b.velocity + difficulty.hitpaddleinc[player.difficulty]);
          }

          b.setangle(bounceOffAngle(p.width, p.posx, b.posx));
          if(player.powerup[PO_GLUE])
          {
            b.gluedX = p.posx+p.width - px;
            b.glued=1;
          }

          po.x = px;
          po.y = py;
        }

      }

    }

  }
}

#include "highscores.cpp"

struct shopItemStruct {
  int price;
  char type;
};

class hudClass {
  private:
  textureClass texBall;
  
  //For the powerup "shop"
  textureClass *texPowerup; //Pointer to array of powerup textures
  int shopItemSelected;
  #define NUMITEMSFORSALE 10
  struct shopItemStruct item[NUMITEMSFORSALE];
  bool shopItemBlocked[NUMITEMSFORSALE]; //One can only buy each powerup one time each life/level
  
  public:
  hudClass(textureClass texB, textureClass texPo[])
  {
    texPowerup = texPo;
    texBall=texB;
    texBall.prop.ticks = 1000;
    texBall.prop.cols = 1;
    texBall.prop.rows = 1;
    texBall.prop.xoffset = 1;
    texBall.prop.yoffset = 1;
    texBall.prop.frames = 1;
    texBall.prop.bidir = 0;
    texBall.prop.playing = 0;
    texBall.prop.padding=1;

    item[0].type = PO_NORMALBALL;
    item[0].price = 400;
    item[1].type = PO_BIGBALL;
    item[1].price = 500;
    item[2].type = PO_AIMHELP;
    item[2].price = 600;
    item[3].type = PO_GROWPADDLE;
    item[3].price = 600;
    item[4].type = PO_MULTIBALL;
    item[4].price = 700;
    item[5].type = PO_GLUE;
    item[5].price = 700;
    item[6].type = PO_EXPLOSIVE;
    item[6].price = 800;
    item[7].type = PO_THRU;
    item[7].price = 1000;
    item[8].type = PO_EXPLOSIVE_GROW;
    item[8].price = 1500;
    item[9].type = PO_LIFE;
    item[9].price = 3000;
    
    shopItemSelected=0;
  }

  void draw()
  {
    int i;
    //Draw lives left.
    glLoadIdentity();
    glTranslatef(0,0,-3.0);
    glColor4f( 1,1,1,1);

    glBindTexture(GL_TEXTURE_2D, texBall.prop.texture);
    texBall.play();
    glBegin( GL_QUADS );
    for(i=0; i < player.lives; i++)
    {
       glTexCoord2f(texBall.pos[0],texBall.pos[1]); glVertex3f(1.55-(0.05*i), 1.2, 0.0);
       glTexCoord2f(texBall.pos[2],texBall.pos[3]); glVertex3f(1.5 -(0.05*i), 1.2, 0.0);
       glTexCoord2f(texBall.pos[4],texBall.pos[5]); glVertex3f(1.5 -(0.05*i),1.15, 0.0);
       glTexCoord2f(texBall.pos[6],texBall.pos[7]); glVertex3f(1.55 -(0.05*i), 1.15, 0.0);
    }
    glEnd( );

    //Draw the "shop"
    //First, find out how many items the player can afford, so we can center them
    int canAfford=0;
    for(i=0; i < NUMITEMSFORSALE; i++)
    {
      if(item[i].price <= player.coins)
      {
        canAfford++;
      }
    }
    
    if(shopItemSelected > canAfford || shopItemSelected < 0)
    {
      shopItemSelected=canAfford-1;
    }
    
    GLfloat shopListStartX = -((0.11*canAfford)/2.0);
    if(gVar.shopNextItem)
    {
      gVar.shopNextItem=0;
      shopItemSelected++;

      if(shopItemSelected > canAfford-1)
      {
        shopItemSelected=0;
      }
    } else if(gVar.shopPrevItem)
    {
      gVar.shopPrevItem=0;
      shopItemSelected--;

      if(shopItemSelected < 0)
      {
        shopItemSelected=canAfford-1;
      }
    } else if(gVar.shopBuyItem)
    {
      gVar.shopBuyItem=0;
      if(item[shopItemSelected].price <= player.coins && !shopItemBlocked[shopItemSelected])
      {
        struct pos a,b;
        a.x = shopListStartX + (0.11*shopItemSelected);
        a.y = 1.15;
        b.x = 0.0;
        b.y = 0.0;
        pMan.spawn(a,b,item[shopItemSelected].type);
        player.coins -= item[shopItemSelected].price;
        shopItemBlocked[shopItemSelected]=1;
        gVar.shopNextItem=1;
        soundMan.add(SND_BUY_POWERUP, 0.0);
      }
    }
    
    glTranslatef( shopListStartX, 1.15, 0.0f);
    for(i=0; i < canAfford; i++)
    {
      if(i==shopItemSelected)
      {
        if(shopItemBlocked[i])
        {
          glColor4f(1.0, 0.0, 0.0, 1.0);
        } else {
          glColor4f(1.0, 1.0, 1.0, 1.0);
        }
      } else {
        if(shopItemBlocked[i])
        {
          glColor4f(1.0, 0.0, 0.0, 0.4);
        } else {
          glColor4f(1.0, 1.0, 1.0, 0.4);
        }
      }
      texPowerup[item[i].type].play();
      glBindTexture( GL_TEXTURE_2D, texPowerup[item[i].type].prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(texPowerup[item[i].type].pos[0],texPowerup[item[i].type].pos[1]);glVertex3f( -0.055, 0.055, 0.00 );
        glTexCoord2f(texPowerup[item[i].type].pos[2],texPowerup[item[i].type].pos[3]);glVertex3f(  0.055, 0.055, 0.00 ); 
        glTexCoord2f(texPowerup[item[i].type].pos[4],texPowerup[item[i].type].pos[5]);glVertex3f(  0.055,-0.055, 0.00 ); 
        glTexCoord2f(texPowerup[item[i].type].pos[6],texPowerup[item[i].type].pos[7]);glVertex3f( -0.055,-0.055, 0.00 ); 
      glEnd( );
      glTranslatef( 0.11, 0.0, 0.0f);
    }
    
  }

  void clearShop()
  {
    for(int i=0; i < NUMITEMSFORSALE; i++)
    {
      shopItemBlocked[i]=0;
    }
  }
};

void writeSettings()
{
  ofstream conf;
  conf.open(privFile.settingsFile.data(),ios::out | ios::trunc); //homeDirFiles.settingsFile.data()

  if(conf.is_open())
  {
    conf << "clearscreen="<<var.clearScreen<<endl;
    conf << "eyecandy="<<setting.eyeCandy<<endl;
    conf << "resx="<<setting.resx<<endl;
    conf << "resy="<<setting.resy<<endl;
    conf << "showbg="<<setting.showBg<<endl;
    conf << "fullscreen="<<setting.fullscreen<<endl;
    conf << "particlecollide="<<setting.particleCollide<<endl;
    conf << "sound="<<setting.sound<<endl;
    conf << "stereo="<<setting.stereo<<endl;
    conf << "controlmaxspeed="<<setting.controlMaxSpeed<<endl;
    conf << "controlaccel="<<setting.controlAccel<<endl;
    conf << "controlstartspeed="<<setting.controlStartSpeed<<endl;
    conf << "rightkey="<<setting.keyRight<<endl;
    conf << "leftkey="<<setting.keyLeft<<endl;
    conf << "nextkey="<<setting.keyNextPo<<endl;
    conf << "buykey="<<setting.keyBuyPo<<endl;
    conf << "prevkey="<<setting.keyPrevPo<<endl;
    conf << "shootkey="<<setting.keyShoot<<endl;
    conf << "joyenabled="<<setting.joyEnabled<<endl;
    conf << "joyisdigital="<<setting.joyIsDigital<<endl;
    conf << "joycalhighjitter="<<setting.JoyCalHighJitter<<endl;
    conf << "joycallowjitter="<<setting.JoyCalLowJitter<<endl;
    conf << "joycalmax="<<setting.JoyCalMax<<endl;
    conf << "joycalmin="<<setting.JoyCalMin<<endl;
    conf.close();
  } else {
    cout << "Could not open ' ' for writing." << endl;
  }
}

class speedometerClass {
  public:

    void draw()
    {
      //GLfloat y = -1.24 + difficulty.maxballspeed[player.difficulty]/2.44*var.averageBallSpeed;

      GLfloat y = 2.48/ (difficulty.maxballspeed[player.difficulty] - difficulty.ballspeed[player.difficulty]) * (var.averageBallSpeed - difficulty.ballspeed[player.difficulty] );


      glDisable(GL_TEXTURE_2D);

      glLoadIdentity();
      glTranslatef(1.61, -1.24,-3.0);
      glBegin( GL_QUADS );
        glColor4f(0,1,0,1);
        glVertex3f( 0, y, 0);
        glVertex3f( 0.03, y, 0);
        glColor4f(1,1,1,1);
        glVertex3f( 0.03,0,0 );
        glVertex3f( 0, 0 ,0);
      glEnd( );
      glEnable(GL_TEXTURE_2D);
    }
};

struct savedGame {
  char name[32];
  struct player_struct player;
};

//Savegame files now consist of a int long header with a version number.
void saveGame(int slot, string name) {
  fstream file;
  struct savedGame game;
  strcpy(game.name, name.data() );
  game.player = player;
  file.open(privFile.saveGameFile.data(), ios::out | ios::in | ios::binary);
  if(!file.is_open())
  {
    cout << "Could not open '"<<privFile.saveGameFile<<"' for Read+Write." << endl;
  }
    
  //move to the slot, mind the header
  file.seekp( (sizeof(int))+((sizeof(savedGame)*slot)) );

  file.write((char *)(&game), sizeof(savedGame));
  file.close();
}

void clearSaveGames()
{
  fstream file;
  file.open(privFile.saveGameFile.data(), ios::out | ios::in | ios::binary);
  if(!file.is_open())
  {
    cout << "Could not open '"<<privFile.saveGameFile<<"' for Read+Write." << endl;
  }
  //Write the header
  const int sgHead = SAVEGAMEVERSION; //Savegame file version
  file.write( (char *)(&sgHead), sizeof(int));
  file.close();

  
  saveGame(0, "Empty Slot");
  saveGame(1, "Empty Slot");
  saveGame(2, "Empty Slot");
  saveGame(3, "Empty Slot");
  saveGame(4, "Empty Slot");
  saveGame(5, "Empty Slot");
}

void loadGame(int slot)
{
  fstream file;
  struct savedGame game;
  file.open(privFile.saveGameFile.data(), ios::in | ios::binary);
  if(!file.is_open())
    cout << "Could not open '" << privFile.saveGameFile << "' for Reading." << endl;

  //first, move to the slot
  file.seekg( sizeof(int)+(sizeof(savedGame)*slot) );
  file.read((char *)(&game), sizeof(savedGame));
  file.close();

   if(game.player.level != 0) //Only change level if level is not 0 (level one cannot be saved, and is used for the empty slots)
   {
    game.player.level--; //subtract one, because when the savegame is loaded, nextlevel is used to apply changes. nextlevel will add one to the level.
    player = game.player;
    gVar.nextlevel=1;
    var.paused=1;
   }
}

int listSaveGames(string slotName[])
{
  fstream file;
  struct savedGame slot[6];
  int i=0;

  file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
  if(!file.is_open())
  {
    cout << "Creating savegame slots in '"<<privFile.saveGameFile<<"'."<<endl;
    file.open(privFile.saveGameFile.data(), ios::out | ios::binary);
    if(!file.is_open())
    {
      cout << "Do not have permissions to write '" << privFile.saveGameFile << "'" <<endl;
      return(0);
    }
    file.close();

    clearSaveGames();
    file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
    if(!file.is_open())
    {
      cout << "Could not write template."<<endl;
      return(0);
    }
  }
  
    //First we check if this is the right version
  int sgHead=0x00; //Invalid version
  
  file.read((char *)(&sgHead), sizeof(int));
  if(sgHead!=SAVEGAMEVERSION)
  {
    cout << "Savegame format error, is v" << sgHead << " should be v'"  << SAVEGAMEVERSION << "'." << endl;
    cout << "Overriding old savegames..." << endl;
    file.close();
    clearSaveGames();

    file.open(privFile.saveGameFile.data() , ios::in | ios::binary);
    file.seekp(sizeof(int));
  }

  while(1)
  {
    if(i == 6)
    {
      break;
    }

    file.read((char *)(&slot[i]), sizeof(savedGame));
    if( file.eof())
    {
      break;
    }

      slotName[i] = slot[i].name;
    i++;
  }
  file.close();


  return(i);
}

void detonateExplosives(brick bricks[], effectManager & fxMan)
{
  struct pos p,v;
  for(int i=0; i < 598; i++)
  {
    if(bricks[i].active && bricks[i].type=='B')
    {
      p.x = bricks[i].posx;
      p.y = bricks[i].posy;
      v.x = 0.0;
      v.y = 0.0;
      bricks[i].hit(fxMan, p, v,0);
    }
  }
}

void moveBoard(brick bricks[], int dir)
{
  int i;

  switch(dir)
  {
    case 0:    //Move to the right
    for(i=0; i < 598; i++)
    {
      if(bricks[i].active)
      {
        bricks[i].posx += bricks[i].width*2;
        if(bricks[i].posx > 1.60)
        {
          bricks[i].posx=-1.6+bricks[i].width;
        }
      }
    }
    break;

    case 1: // move to the left
    for(i=0; i < 598; i++)
    {
      if(bricks[i].active)
      {
        bricks[i].posx -= bricks[i].width*2;
        if(bricks[i].posx < -1.60)
        {
          bricks[i].posx= 1.6-bricks[i].width;
        }
      }
    }
    break;

    case 2: // move up
    for(i=0; i < 598; i++)
    {
      if(bricks[i].active)
      {
        bricks[i].posy += bricks[i].height*2;
        if(bricks[i].posy > 1.15-bricks[i].height)
        {
         //Destroy brick, and subtract score
          bricks[i].active=0;
          updated_nbrick[bricks[i].row][bricks[i].bricknum]=-1;
          var.bricksHit=1;
          player.score -= bricks[i].score;
          gVar.deadTime=0;
        }
      }
    }
    break;

    case 3: // move down
    for(i=0; i < 598; i++)
    {
      if(bricks[i].active)
      {
        bricks[i].posy -= bricks[i].height*2;
        if(bricks[i].posy < -1.00-bricks[i].height)
        {
          //Destroy brick, and subtract score
          bricks[i].active=0;
          updated_nbrick[bricks[i].row][bricks[i].bricknum]=-1;
          var.bricksHit=1;
          player.score -= bricks[i].score;
          gVar.deadTime=0;
        }
      }
    }
    break;
  }
}

void explosiveGrow(brick bricks[])
{
  for(int i=0; i < 598; i++)
  {
    if(bricks[i].active)
    {
      bricks[i].growExplosive(bricks);
    }
  }

  for(int i=0; i < 598; i++)
  {
    bricks[i].justBecomeExplosive=0;
  }
}

void easyBrick(brick bricks[])
{
  for(int i=0; i < 598; i++)
  {
    if(bricks[i].active)
    {
      bricks[i].breakable();
    }
  }
}

bool checkDir(string & dir)
{
  struct stat st;

  if( stat(dir.data(), &st) != 0)
  {
    cout << "Directory '" << dir << "' does not exist, ";
#ifdef WIN32
    if(CreateDirectory(dir.data(), NULL) != 0)
#else
    if(mkdir(dir.data(), S_IRWXU | S_IRWXG) !=0)
#endif
    {
      cout << "could not create it." << endl;
      return(0);
    }
    cout << "created it." << endl;
  }
  return(1);
}

#include "input.cpp"
#include "title.cpp"
bool screenShot()
{
  FILE *fscreen;

  char cName[256];
  int i = 0;
  bool found=0;
  while(!found)
  {
    sprintf(cName, "%s/sdl-ball_%i.tga",privFile.screenshotDir.data(),i);
    fscreen = fopen(cName,"rb");
    if(fscreen==NULL)
      found=1;
    else
      fclose(fscreen);
      i++;
  }
  int nS = setting.resx * setting.resy * 3;
  GLubyte *px = new GLubyte[nS];
  if(px == NULL)
  {
    cout << "Alloc err, screenshot failed." <<endl;
    return 0;
  }
  fscreen = fopen(cName,"wb");

  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glReadPixels(0, 0, setting.resx, setting.resy, GL_BGR, GL_UNSIGNED_BYTE, px);

  unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
  unsigned char header[6] = {setting.resx%256,setting.resx/256,setting.resy%256,setting.resy/256,24,0};
  fwrite(TGAheader, sizeof(unsigned char), 12, fscreen);
  fwrite(header, sizeof(unsigned char), 6, fscreen);

  fwrite(px, sizeof(GLubyte), nS, fscreen);
  fclose(fscreen);
  delete [] px;
  cout << "Wrote screenshot to '" << cName << "'" <<endl;
  return 1;


}

int main (int argc, char *argv[]) {
  var.quit=0;
  var.clearScreen=1;
  var.titleScreenShow=1;
  setting.eyeCandy = 1;
  setting.showBg = 0;
  setting.particleCollide=0;
  setting.fullscreen = 1;
  setting.sound = 1;
  setting.stereo=1;
  //Defaults for keyboard/joystick control
  setting.keyLeft = (SDLKey)276;
  setting.keyRight= (SDLKey)275;
  setting.keyShoot= (SDLKey)273;
  setting.keyNextPo=(SDLKey)SDLK_v;
  setting.keyBuyPo =(SDLKey)SDLK_b;
  setting.keyPrevPo=(SDLKey)SDLK_n;
  setting.controlAccel = 7;
  setting.controlStartSpeed = 1.0;
  setting.controlMaxSpeed = 5;
  setting.joyEnabled = 1;
  setting.joyIsDigital = 1;
  //Default calibaration.
  setting.JoyCalMin=-32767;
  setting.JoyCalMax=32767;
  setting.JoyCalLowJitter=-20;
  setting.JoyCalHighJitter=20;

  GLfloat mousex,mousey;

  static_difficulty.ballspeed[EASY] = 0.7f;
  static_difficulty.ballspeed[NORMAL] = 1.3f;

  static_difficulty.maxballspeed[EASY] = 1.5f;
  static_difficulty.maxballspeed[NORMAL] = 2.3f;

  static_difficulty.hitbrickinc[EASY] = 0.0025;
  static_difficulty.hitbrickinc[NORMAL] = 0.0035;

  static_difficulty.hitpaddleinc[EASY] = -0.001;
  static_difficulty.hitpaddleinc[NORMAL] = -0.0005;

  //Percentage
  static_difficulty.slowdown[EASY] = 20.0f;
  static_difficulty.slowdown[NORMAL] = 10.0f;
  static_difficulty.speedup[EASY] = 10.0f;
  static_difficulty.speedup[NORMAL] = 20.0f;

  difficulty = static_difficulty;

  cout << "SDL-Ball v "VERSION << endl;

#ifdef WIN32
  privFile.programRoot = ""; // default to ./ on win32
#else
  privFile.programRoot = getenv("HOME");
#endif

  //Default places if it can't place files another place.
  privFile.saveGameFile = "./savegames.sav";
  privFile.settingsFile ="./settings.cfg";
  privFile.highScoreFile = "./highscores.txt";
  privFile.screenshotDir = "./";

  if(privFile.programRoot.length()==0)
  {
    cout << "Could not locate home directory defaulting to ./" << endl;
  } else {
     privFile.programRoot.append("/.config");

    if( checkDir(privFile.programRoot))
    {
      privFile.programRoot.append("/sdl-ball");
      if( checkDir(privFile.programRoot) )
      {
        privFile.saveGameFile = privFile.programRoot;
        privFile.settingsFile = privFile.programRoot;
        privFile.highScoreFile = privFile.programRoot;
        privFile.saveGameFile.append("/savegame.sav");
        privFile.settingsFile.append("/settings.cfg");
        privFile.highScoreFile.append("/highscores.txt");

        privFile.screenshotDir = privFile.programRoot;
        privFile.screenshotDir.append("/screenshots");
        if( !checkDir(privFile.screenshotDir) )
        {
          cout << "Screenshots are saved in current directory." << endl;
          privFile.screenshotDir = "./";
        }

      }
    }
  }
  srand((unsigned)time(0));

  //Load settings
  ifstream conf;
  string line,set,val;
  conf.open( privFile.settingsFile.data() );
  if(conf.is_open())
  {
    while(!conf.eof())
    {
      getline(conf, line);
      if(line.find('=') != string::npos)
      {
        set=line.substr(0,line.find('='));
        val=line.substr(line.find('=')+1);

        if(set=="clearscreen")
        {
          var.clearScreen=atoi(val.data());
        } else if(set=="eyecandy")
        {
          setting.eyeCandy=atoi(val.data());
        } else if(set=="resx")
        {
          setting.cfgRes[0]=1;
          setting.resx=atoi(val.data());
        } else if(set=="resy")
        {
          setting.cfgRes[1]=1;
          setting.resy=atoi(val.data());
        } else if(set=="showbg")
        {
          setting.showBg=atoi(val.data());
        } else if(set=="fullscreen")
        {
          setting.fullscreen=atoi(val.data());
        } else if(set=="particlecollide")
        {
          setting.particleCollide=atoi(val.data());
        } else if(set=="sound")
        {
          setting.sound=atoi(val.data());
        } else if(set=="stereo")
        {
          setting.stereo=atoi(val.data());
        } else if(set=="controlmaxspeed")
        {
          setting.controlMaxSpeed = atof(val.data());
        } else if(set=="controlaccel")
        {
          setting.controlAccel = atof(val.data());
        } else if(set=="controlstartspeed")
        {
          setting.controlStartSpeed = atof(val.data());
        } else if(set=="rightkey")
        {
          setting.keyRight = (SDLKey)atoi(val.data());
        } else if(set=="leftkey")
        {
          setting.keyLeft = (SDLKey)atoi(val.data());
        } else if(set=="shootkey")
        {
          setting.keyShoot = (SDLKey)atoi(val.data());
        } else if(set=="nextkey")
        {
          setting.keyNextPo = (SDLKey)atoi(val.data());
        } else if(set=="buykey")
        {
          setting.keyBuyPo = (SDLKey)atoi(val.data());
        } else if(set=="prevkey")
        {
          setting.keyPrevPo = (SDLKey)atoi(val.data());
        } else if(set=="joyenabled")
        {
          setting.joyEnabled = atoi(val.data());
        } else if(set=="joyisdigital")
        {
          setting.joyIsDigital = atoi(val.data());
        } else if(set=="joycalhighjitter")
        {
          setting.JoyCalHighJitter = atoi(val.data());
        } else if(set=="joycallowjitter")
        {
          setting.JoyCalLowJitter = atoi(val.data());
        } else if(set=="joycalmax")
        {
          setting.JoyCalMax = atoi(val.data());
        } else if(set=="joycalmin")
        {
          setting.JoyCalMin = atoi(val.data());
        }
        else
        {
          cout << "I did not understand '"<<set<<"' in settings.cfg" << endl;
        }
      }
    }
    conf.close();
  } else {
    cout << "No config file found, using default settings." << endl;
  }

  //Initialize SDL
  #ifdef WITH_SOUND
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) <0 )
  #else
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK) <0 )
  #endif
  {
    printf("\nError: Unable to initialize SDL:%s\n", SDL_GetError());
  }

  //Save current resolution so it can be restored at exit
   int oldResX = SDL_GetVideoInfo()->current_w;
   int oldResY = SDL_GetVideoInfo()->current_h;
   int oldColorDepth = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

   if(!setting.cfgRes[0] || !setting.cfgRes[1])
   {
    setting.resx = oldResX;
    setting.resy = oldResY;
   }

   SDL_Event sdlevent;

#ifndef WIN32
   SDL_WM_GrabInput(SDL_GRAB_ON);
#endif
   SDL_ShowCursor(SDL_DISABLE);

   SDL_EnableUNICODE (1);

  initScreen();

  initGL();

  soundMan.init();
  TTF_Init();
  fonts[0] = TTF_OpenFont( DATADIR"/font/ack.ttf", 50 );
  fonts[3] = TTF_OpenFont( DATADIR"/font/ack.ttf", 25);
  fonts[1] = TTF_OpenFont( DATADIR"/font/acidic.ttf", 60 );
  fonts[2] = TTF_OpenFont( DATADIR"/font/acidic.ttf", 80 );

  SDL_WM_SetCaption("SDL-Ball", "SDL-Ball");
  SDL_WarpMouse(var.halfresx, var.halfresy);

  textureManager texMgr; // textur manager

  textureClass texPaddleBase;
  textureClass texPaddleLayers[2];
  textureClass texBall[3];
  textureClass texLvl[6];
  texExplosiveBrick = &texLvl[0]; //Pointer to explosive texture..

  textureClass texBorder;
  textureClass texPowerup[MAXPOTEXTURES];
  textureClass texBullet;
  textureClass texParticle;

  texPaddleBase.prop.ticks= 100;
  texPaddleBase.prop.cols = 1;
  texPaddleBase.prop.rows = 1;
  texPaddleBase.prop.xoffset=1.0;
  texPaddleBase.prop.yoffset=1.0;
  texPaddleBase.prop.frames=1;
  texPaddleBase.prop.bidir=0;
  texPaddleBase.prop.playing=0;
  texPaddleBase.prop.padding=1;

  texPaddleLayers[1].prop.ticks = 250;
  texPaddleLayers[1].prop.cols = 2;
  texPaddleLayers[1].prop.rows = 3;
  texPaddleLayers[1].prop.xoffset = 0.5;
  texPaddleLayers[1].prop.yoffset = 0.25;
  texPaddleLayers[1].prop.frames = 6;
  texPaddleLayers[1].prop.bidir=0;
  texPaddleLayers[1].prop.playing = 1;
  texPaddleLayers[1].prop.padding = 1;

  texBullet.prop.ticks = 100;
  texBullet.prop.cols = 2;
  texBullet.prop.rows = 4;
  texBullet.prop.xoffset = 0.5;
  texBullet.prop.yoffset = 0.25;
  texBullet.prop.frames = 8;
  texBullet.prop.bidir = 1;
  texBullet.prop.playing = 1;

  texMgr.load(DATADIR"/gfx/paddle/base.png", texPaddleBase);
  texMgr.load(DATADIR"/gfx/paddle/glue.png", texPaddleLayers[0]);
  texMgr.load(DATADIR"/gfx/paddle/gun.png", texPaddleLayers[1]);


  texMgr.load(DATADIR"/gfx/ball/test.png", texBall[0]);
  texMgr.load(DATADIR"/gfx/ball/fireball.png", texBall[1]);
  texMgr.load(DATADIR"/gfx/ball/tail.png", texBall[2]);

  texMgr.load(DATADIR"/gfx/brick/explosive.png", texLvl[0]);
  texMgr.load(DATADIR"/gfx/brick/base.png", texLvl[1]);
  texMgr.load(DATADIR"/gfx/brick/cement.png", texLvl[2]);
  texMgr.load(DATADIR"/gfx/brick/doom.png", texLvl[3]);
  texMgr.load(DATADIR"/gfx/brick/glass.png", texLvl[4]);
  texMgr.load(DATADIR"/gfx/brick/invisible.png", texLvl[5]);


  texMgr.load(DATADIR"/gfx/border.png", texBorder);

  texMgr.load(DATADIR"/gfx/powerup/glue.png", texPowerup[PO_GLUE]);
  texMgr.load(DATADIR"/gfx/powerup/gravity.png", texPowerup[PO_GRAVITY]);
  texMgr.load(DATADIR"/gfx/powerup/multiball.png", texPowerup[PO_MULTIBALL]);
  texMgr.load(DATADIR"/gfx/powerup/bigball.png", texPowerup[PO_BIGBALL]);
  texMgr.load(DATADIR"/gfx/powerup/normalball.png", texPowerup[PO_NORMALBALL]);
  texMgr.load(DATADIR"/gfx/powerup/smallball.png", texPowerup[PO_SMALLBALL]);
  texMgr.load(DATADIR"/gfx/powerup/aim.png", texPowerup[PO_AIM]);
  texMgr.load(DATADIR"/gfx/powerup/explosive.png", texPowerup[PO_EXPLOSIVE]);
  texMgr.load(DATADIR"/gfx/powerup/gun.png", texPowerup[PO_GUN]);
  texMgr.load(DATADIR"/gfx/powerup/go-thru.png", texPowerup[PO_THRU]);
  texMgr.load(DATADIR"/gfx/powerup/laser.png", texPowerup[PO_LASER]);
  texMgr.load(DATADIR"/gfx/powerup/life.png", texPowerup[PO_LIFE]);
  texMgr.load(DATADIR"/gfx/powerup/die.png", texPowerup[PO_DIE]);
  texMgr.load(DATADIR"/gfx/powerup/drop.png", texPowerup[PO_DROP]);
  texMgr.load(DATADIR"/gfx/powerup/detonate.png", texPowerup[PO_DETONATE]);
  texMgr.load(DATADIR"/gfx/powerup/explosive-grow.png", texPowerup[PO_EXPLOSIVE_GROW]);
  texMgr.load(DATADIR"/gfx/powerup/easybrick.png", texPowerup[PO_EASYBRICK]);
  texMgr.load(DATADIR"/gfx/powerup/nextlevel.png", texPowerup[PO_NEXTLEVEL]);
  texMgr.load(DATADIR"/gfx/powerup/aimhelp.png", texPowerup[PO_AIMHELP]);

  pMan.init(texPowerup);

  texMgr.load(DATADIR"/gfx/powerup/bullet.png", texBullet);


  texMgr.load(DATADIR"/gfx/powerup/growbat.png", texPowerup[PO_GROWPADDLE]);
  texMgr.load(DATADIR"/gfx/powerup/shrinkbat.png", texPowerup[PO_SHRINKPADDLE]);

  texMgr.load(DATADIR"/gfx/particle.png", texParticle);

  GLuint sceneDL;

  mkDLscene(&sceneDL, texBorder);

  brick bricks[598];

  string levelfile = DATADIR"/levels.txt";



  int i=0; //bruges i for loop xD
  glScoreBoard scoreboard;
  menuClass menu;


  paddle_class paddle;

  paddle.tex = texPaddleBase;
  paddle.layerTex = texPaddleLayers;

  struct pos p; //kordinater for hvor den stødte sammen

  effectManager fxMan;
  fxMan.set(FX_VAR_TEXTURE, texParticle);
  fxMan.set(FX_VAR_GRAVITY, 0.6f);
  
  titleScreenClass titleScreen(&fxMan, texPowerup, &menu);

  ballManager bMan(texBall);
  player.difficulty = NORMAL;
  initNewGame();
  paddle.posy = -1.15;

  highScoreClass hKeeper;
  backgroundClass bg;

  bulletsClass bullet(texBullet);

  speedometerClass speedo;

  hudClass hud(texBall[0], texPowerup); //This is GOING to be containing the "hud" (score, borders, lives left, level, speedometer)

  var.transiteffectnum=-1;

  int lastTick = SDL_GetTicks();
  int nonpausingLastTick = lastTick;
  char txt[256];
  int frameAge=0; //How long have the current frame been shown
  int maxFrameAge=8; //When frame has been displayed this long

//   #define performanceTimer

  #ifdef performanceTimer
  struct timeval timeStart,timeStop;
  int renderTime;
  #endif

  controllerClass control(&paddle, &bullet, &bMan);


//   announce.write("SDL-BALL",2000, fonts[0]);
  soundMan.add(SND_START,0);

  while(!var.quit)
  {
    #ifdef performanceTimer
    gettimeofday(&timeStart, NULL);
    #endif

    nonpausingGlobalTicks = SDL_GetTicks() - nonpausingLastTick;
    frameAge += nonpausingGlobalTicks;

    nonpausingGlobalMilliTicks = nonpausingGlobalTicks/1000.0;
    nonpausingLastTick = SDL_GetTicks();

    if(var.paused)
      lastTick = SDL_GetTicks();

    globalTicks = SDL_GetTicks() - lastTick;
    globalMilliTicks = globalTicks/1000.0;
    lastTick = SDL_GetTicks();

    globalTicksSinceLastDraw += nonpausingGlobalTicks;
    globalMilliTicksSinceLastDraw += nonpausingGlobalMilliTicks;

    gVar.deadTime += globalTicks;

    //really ugly... but easy
    if(!var.titleScreenShow)
    {

      if(gVar.deadTime > 20000)
      {
        gVar.deadTime=0;
        bMan.powerup(PO_EXPLOSIVE); //give the balls explosive ability, in order to blow up cement block and get on with the game
      }

      if(bMan.activeBalls == 0 && !gVar.newLevel) //check kun om vi er døde hvis vi faktisk er kommet igang med at spille
      {
        player.lives--;
        if(player.lives >= 1)
        {
          resetPlayerPowerups();
          gVar.newLife=1;
          if(!paddle.dead)
            player.explodePaddle=1;
          pMan.die(fxMan);
        } else if(!gVar.gameOver) {
          gVar.gameOver=1;
          pauseGame();
          if( hKeeper.isHighScore() )
          {
            announce.write("Highscore!", 3000,fonts[0]);
            var.showHighScores=1;
            soundMan.add(SND_HIGHSCORE, 0);
          } else {
            announce.write("GameOver!", 3000,fonts[0]);
            soundMan.add(SND_GAMEOVER, 0);
            initNewGame();
            var.titleScreenShow=1;
          }
        }
      }

      if(gVar.nextlevel)
      {
        if(var.transiteffectnum == -1)
        {

          announce.write("Well Done!", 1000, fonts[2]);
          soundMan.add(SND_NEXTLEVEL, 0);

          if(bMan.activeBalls > 1)
          {
            sprintf(txt, "Bonus: %i", bMan.activeBalls*150);
            player.score += (bMan.activeBalls*150)*player.multiply;
            announce.write(txt, 2000, fonts[2]);
          }

          fxMan.set(FX_VAR_TYPE, FX_TRANSIT);
          fxMan.set(FX_VAR_LIFE, 1600);
          fxMan.set(FX_VAR_COLOR, 0.0,0.0,0.0);
          p.x = 0.0;
          p.y = 0.0;

            //Kør en transition effekt
          var.transiteffectnum = fxMan.spawn(p);

          var.idiotlock = 0;

        } else {
          if(var.transition_half_done)
          {

            if(!var.idiotlock)
            {
              var.idiotlock=1;

              //If player completed all levels, restart the game with higher multiplier
              player.level++;
              if(player.level == var.numlevels)
              {
                player.multiply += player.multiply*3;
                player.level=0;
                announce.write("Finished!",3500,fonts[2]);
              }

              sprintf(txt, "Level %i",player.level+1); //+1 fordi levels starter fra 0
              announce.write(txt,1500,fonts[2]);

              //check om vi skal fjerne powerups
              if(player.difficulty > EASY)
              {
                resetPlayerPowerups();
              }
              gVar.newLevel = 1;
              var.paused=0;
            }

          }

          if(!fxMan.isActive(var.transiteffectnum))
          {
            var.transiteffectnum = -1; //nulstil så den er klar igen
            gVar.nextlevel = 0;
            var.paused = 0;
            var.idiotlock=0;
          }
        }
      }

      if(gVar.newLevel)
      {
        var.bricksHit = 1;
        gVar.newLevel=0;
        loadlevel(levelfile, bricks,player.level);
        initlevels(bricks,texLvl);
        gVar.gameOver=0;
        gVar.newLife=1;
        pMan.clear();
        bullet.clear();
        paddle.posx = 0.0;
        var.startedPlaying=0;
        bg.init(texMgr);
      }

      if(gVar.newLife)
      {
        gVar.newLife=0;
        paddle.init();
        hud.clearShop();
        p.x=paddle.posx;

        p.y=paddle.posy+paddle.height+0.025;

        bMan.clear();
        bMan.spawn(p,1,paddle.width,difficulty.ballspeed[player.difficulty],1.57100000f); //Not exactly 90 degree, so the ball will always fall a bit to the side
      }

      if(frameAge >= maxFrameAge)
      {

        if(var.clearScreen)
        {
          glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        //Update player score
        scoreboard.update(player.score);

        //background
        if(setting.showBg)
          bg.draw();
        //borders
        glCallList(sceneDL);

        for(i=0; i < 4; i++)
        {
          if(var.scrollInfo.direction[i])
          {
            if( (SDL_GetTicks() - var.scrollInfo.lastTick[i] ) > var.scrollInfo.speed[i] )
            {
              var.scrollInfo.lastTick[i]=SDL_GetTicks();
              moveBoard(bricks, i);
            }
          }
        }

        if(gVar.bricksleft==1)
        {
          player.powerup[PO_AIMHELP]=1;
        }
      }

      bullet.move();

      gVar.bricksleft=0;

      //Update nbrick here
      if(var.bricksHit)
      {
        memcpy(nbrick, updated_nbrick, sizeof(updated_nbrick));
        var.bricksHit = 0;
      }

      for(i=0; i <598; i++)
      {
        if(bricks[i].active)
        {
          if(bricks[i].destroytowin)
          {
            gVar.bricksleft++;
          }

          if(bricks[i].collide)
          {
            bMan.bcoldet(bricks[i], fxMan);
            //bullets
            if(player.powerup[PO_GUN])
            {
              bullet.coldet(bricks[i], fxMan);
            }

            //check kollision på effekterne
            if(setting.particleCollide && setting.eyeCandy && frameAge >= maxFrameAge)
              fxMan.coldet(bricks[i]);
          }
          if(frameAge >= maxFrameAge)
              bricks[i].draw(bricks,fxMan);
        } //aktiv brik
      } // for loop

      //Collission between paddle and balls
      if( bMan.pcoldet(paddle, fxMan) )
      {
        if(player.powerup[PO_DROP])
        {
          moveBoard(bricks, 3);
        }
      }

        bMan.move();

      if(setting.particleCollide && setting.eyeCandy && frameAge >= maxFrameAge)
        fxMan.pcoldet(paddle);

      pMan.move();
      if(pMan.coldet(paddle, fxMan, bMan))
      {
        if(player.powerup[PO_DETONATE])
        {
          player.powerup[PO_DETONATE]=0;
          detonateExplosives(bricks, fxMan);
        }

        if(player.powerup[PO_EASYBRICK])
        {
          player.powerup[PO_EASYBRICK]=0;
          easyBrick(bricks);
        }

        if(player.powerup[PO_NEXTLEVEL])
        {
          player.powerup[PO_NEXTLEVEL]=0;
          gVar.nextlevel=1;
        }

        if(player.powerup[PO_EXPLOSIVE_GROW])
        {
          player.powerup[PO_EXPLOSIVE_GROW]=0;
          explosiveGrow(bricks);
        }
      }

      if(frameAge >= maxFrameAge)
      {

        soundMan.play();

        if(player.explodePaddle)
        {
          player.explodePaddle=0;
          soundMan.add(SND_DIE,0);
          if(setting.eyeCandy)
          {
            fxMan.set(FX_VAR_TYPE, FX_PARTICLEFIELD);

            p.x=paddle.width*2;
            p.y=paddle.height*2;
            fxMan.set(FX_VAR_RECTANGLE, p);

            p.x=paddle.posx;
            p.y=paddle.posy;

            fxMan.set(FX_VAR_LIFE, 2000);
            fxMan.set(FX_VAR_NUM, 20);
            fxMan.set(FX_VAR_SIZE, 0.025f);
            fxMan.set(FX_VAR_SPEED, 0.35f);
            fxMan.set(FX_VAR_GRAVITY, -0.7f);
            fxMan.set(FX_VAR_COLOR, 1.0f, 0.7f, 0.0f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 0.8f, 0.0f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 0.9f, 0.0f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 0.0f);
            fxMan.spawn(p);

            fxMan.set(FX_VAR_NUM, 32);
            fxMan.set(FX_VAR_SIZE, 0.05f);
            fxMan.set(FX_VAR_LIFE, 1500);
            fxMan.set(FX_VAR_SPEED, 0.7f);
            fxMan.set(FX_VAR_GRAVITY, 0.0f);

            fxMan.set(FX_VAR_COLOR, 0.5f, 0.5f, 0.5f);
            fxMan.spawn(p);
            fxMan.set(FX_VAR_COLOR, 1.0f, 1.0f, 1.0f);
            fxMan.spawn(p);
          }
        }

        bMan.updatelast();
        glColor3d(255,255,255);
        bullet.draw();
        paddle.draw();

        pMan.draw();
        bMan.draw(paddle);
        scoreboard.draw();
        speedo.draw();
        hud.draw();
        fxMan.draw();

        if(var.showHighScores)
          hKeeper.draw();

        if(var.menu>0)
        {
          if(var.menu==10 || var.menu==11)
          {
            control.calibrate();
          }
          menu.doMenu();
        }

        announce.draw();

        SDL_GL_SwapBuffers( );

        frameAge = 0;

        globalTicksSinceLastDraw=0;
        globalMilliTicksSinceLastDraw=0;
      #ifdef performanceTimer
          cout << "FrameAge:";
      } else {
        cout << "LoopAge:";
      }
      gettimeofday(&timeStop, NULL);
      renderTime = timeStop.tv_usec - timeStart.tv_usec;
      cout << renderTime << endl;
      #else
      }
    #endif


      if(!gVar.bricksleft)
      {
        gVar.nextlevel=1;
        var.paused=1;
      }
    } else {
    //Show the title screen
      titleScreen.draw(&frameAge, &maxFrameAge);
    }


    control.get(); //Check for keypresses and joystick events
    while (SDL_PollEvent(&sdlevent) )
    {
      if( sdlevent.type == SDL_KEYDOWN ) {

        if(var.showHighScores)
        {
          hKeeper.type(sdlevent, menu);
        } else if(var.enterSaveGameName)
        {
          menu.enterSaveGameName(sdlevent);
        } else {
          if( sdlevent.key.keysym.sym==SDLK_p)
          {
            if(var.paused)
              resumeGame();
            else
              pauseGame();
          }

          if( sdlevent.key.keysym.sym == SDLK_q )
          {
            var.quit=1;
          } else if( sdlevent.key.keysym.sym == SDLK_s )
          {
            screenShot();
          } else if( sdlevent.key.keysym.sym == setting.keyNextPo)
          {
            gVar.shopPrevItem=1;
          } else if( sdlevent.key.keysym.sym == setting.keyBuyPo)
          {
            gVar.shopBuyItem=1;
          } else if( sdlevent.key.keysym.sym == setting.keyPrevPo)
          {
            gVar.shopNextItem=1;
          }
            
          #ifdef WITH_WIIUSE
          if( sdlevent.key.keysym.sym == SDLK_w )
          {
            var.titleScreenShow=0;
            pauseGame();
            var.menu=11;
            var.menuJoyCalStage=-1;
          }
          #endif
        }

        //Toggle menu
        if( sdlevent.key.keysym.sym == SDLK_ESCAPE)
        {
          if(var.titleScreenShow)
            var.titleScreenShow=0;
          switch(var.menu)
          {
            case 0:
              var.menu=1;
              pauseGame();
              break;
            case 1:
              resumeGame();
              break;
            default:
              var.menu=1;
              break;
          }
        } else if( sdlevent.key.keysym.sym == SDLK_F1 )
        {
          if(!var.titleScreenShow)
          {
            var.titleScreenShow=1;
          } else {
            var.titleScreenShow=0;
          }
        }
#ifndef WIN32
        else if( sdlevent.key.keysym.sym == SDLK_F11 )
        {
          if(setting.fullscreen)
            setting.fullscreen=0;
          else
            setting.fullscreen=1;

          initScreen();
        }
#endif
      }

      if( sdlevent.type == SDL_MOUSEMOTION )
      {
        mousex = (sdlevent.motion.x - var.halfresx) * var.glunits_per_xpixel;
        mousey = (sdlevent.motion.y - var.halfresy) * var.glunits_per_ypixel * -1;

        if(var.menu)
        {
          if(mousex > -0.5 && mousex < 0.5 && mousey < (-0.78)+(0.07) && mousey > (-0.78)-(0.07) )
            var.menuItem = 1;
          else if(mousex > -0.5 && mousex < 0.5 && mousey < (-0.56)+(0.07) && mousey > (-0.56)-(0.07) )
            var.menuItem = 2;
          else if(mousex > -0.5 && mousex < 0.5 && mousey < (-0.34)+(0.07) && mousey > (-0.34)-(0.07) )
            var.menuItem = 3;
          else if(mousex > -0.5 && mousex < 0.5 && mousey < (-0.12)+(0.07) && mousey > (-0.12)-(0.07) )
            var.menuItem = 4;
          else if(mousex > -0.5 && mousex < 0.5 && mousey < (0.1)+(0.07) && mousey > (0.1)-(0.07) )
            var.menuItem = 5;
          else if(mousex > -0.5 && mousex < 0.5 && mousey < (0.32)+(0.07) && mousey > (0.32)-(0.07) )
            var.menuItem = 6;
          else if(mousex > -0.5 && mousex < 0.5 && mousey < (0.54)+(0.07) && mousey > (0.54)-(0.07) )
            var.menuItem = 7;
          else
           var.menuItem = 0;
        } else {
          control.movePaddle(mousex);
        }
      } else if( sdlevent.type == SDL_MOUSEBUTTONDOWN )
      {
        if(sdlevent.button.button == SDL_BUTTON_LEFT )
        {
          if(var.menu)
          {
            var.menuPressed=1;
            if(var.menuItem > 0)
              soundMan.add(SND_MENUCLICK, 0);
          }

          control.btnPress();
        } else if(sdlevent.button.button == SDL_BUTTON_RIGHT )
        {
          gVar.shopBuyItem=1;
        } else if(sdlevent.button.button == 4)
        {
          gVar.shopPrevItem=1;
        } else if(sdlevent.button.button == 5)
        {
          gVar.shopNextItem=1;
        }
      }
      if( sdlevent.type == SDL_QUIT ) {
        var.quit = 1;
      } else if( sdlevent.type == SDL_VIDEORESIZE )
      {
        setting.resx = sdlevent.resize.w;
        setting.resy = sdlevent.resize.h;
        initScreen();
      }
    }
#ifdef WIN32
	Sleep( 1 );
#else
    usleep( 1000 );
#endif
  }

#ifndef WIN32
  if(setting.fullscreen)
    SDL_SetVideoMode(oldResX,oldResY,oldColorDepth, SDL_OPENGL);
#endif

#ifndef WIN32
  SDL_WM_GrabInput(SDL_GRAB_OFF);
#endif
  SDL_ShowCursor(SDL_ENABLE);
  SDL_FreeSurface(screen);
  SDL_Quit();
  cout << "Thank you for playing sdl-ball ;)" << endl;
  return EXIT_SUCCESS;
}
