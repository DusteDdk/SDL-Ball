/* ************************************************************************* *
    OldSkoolGravityGame (OSGG) Lunar Lander-like game for linux.
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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <vector>
#include <list>
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>

#ifndef WIN32
    #include <endian.h>
#else
    typedef unsigned int uint;
    #include <windows.h>
    #include <GL/glext.h>
#endif

// #define SHOWRENDER

#define VERSION "1.0SVN"

#ifdef SHOWRENDER
#include <sys/time.h>
#endif


#ifndef DATADIR
    #define DATADIR "./"
#endif

using namespace std;


bool soundOn;

#include "sound.hpp"
#include "text.hpp"
glTextClass* glText = NULL;
soundClass* soundMan = NULL;

#define THRUSTINCRATE 0.01
#define GRAVITY 0.002
#define TURNINCRATE 0.15

uint32_t swapbytes32(uint32_t value)
{
  uint32_t result;
  ((uint8_t*) &result)[0] = ((uint8_t*) &value)[3];
  ((uint8_t*) &result)[1] = ((uint8_t*) &value)[2];
  ((uint8_t*) &result)[2] = ((uint8_t*) &value)[1];
  ((uint8_t*) &result)[3] = ((uint8_t*) &value)[0];
  return result;
}

enum entTypes { entLine, entShip, entBase, entWp, entEnemy };
enum gameStateEnum { GameStateEditor, GameStatePlaying, GameStatePause, GameStateQuit, GameStateNewGame, GameStateGameOver, GameStateNextLevel };
uint gameState = GameStateEditor;


struct demoFrameStruct {
  Uint8 left,right,up,shoot;
};

struct pStruct {
  int x,y;
};

string levelFile;

struct glPstruct {
  GLfloat x, y;
};

struct structVert {
  GLfloat color[3];
  struct glPstruct p;
  int collision; // 0 = none, 1 = normal(damage), 2 = platform/Landing gear
};

typedef struct structVert vert;
typedef struct glPstruct gPs;
typedef struct pStruct pS;
typedef struct demoFrameStruct demoFrame;
vector<demoFrame>demoFrames;
struct structEnt {
  struct glPstruct p; //position
  GLfloat rotation; //In DEG
  int type; //type
  int id;
  bool base; // If it's a base, don't translate it every time..
};
typedef struct structEnt entity;

struct structDispInfo {
  gPs mousePos;
  bool mouseDrag; //Is user holding btn
  bool mouseRightDrag;

  bool fullScreen;

  pS dispSize; //Window size in pixels
  pS dispHalfSize;
  gPs glSceneSize;
  gPs camPos; //The is where the camera is.

  GLfloat glZoom;
  GLfloat glUnitsPrPixel;
  GLdouble aspect;

  pS screenRes; //The current resolution ) SDL_GetVideoInfo() );

};
struct structDispInfo dispInfo;

struct structGameRules {
  int startLevel; //what level to start in
  GLfloat maxLandingRotForce;
  GLfloat maxLandingYel;
  GLfloat maxLandingXvel;
  int fuelConsumptionThrust;
  int fuelConsumptionTurn;
  int fuelMaxFuel;
  int ammoMaxAmmo;
};

struct structGameRules gameRules;

struct gameInfoStruct {
  bool recording; //recording demo?
  bool playing; //playing a demo?
  int  demoFrame;

  GLfloat thrust;
  gPs velocity;
  GLfloat rotationForce;

  int fuel;
  int reloadTime; //After this amount of fames (60th's of 1 sec) player can shoot again
  int ammo;

  gPs radarDest; //Destination for package, points to the base where package should be delivered
  int destBase; // 0 = no package, id of base.
  int numBases; // Number of bases on map, set by loadMap
  int score;
  int numMissions; //number of finished missions
  int level;
  int lives;

  vector<vert> shipVerts;
  vector<vert> baseVerts;
  vector<vert> enemyVerts;

  vector<vert> shipStaticVerts;
  vector<vert> baseStaticVerts;
  vector<vert> enemyStaticVerts;

  vector<int> nextObjective; //Where to deliver to next.
  int currentObjective; //What right now?

};
struct gameInfoStruct gameInfo;

GLfloat abs2(GLfloat x)
{
      if (x<0) {return -x;}
      return x;
}

bool LinesCross(gPs aLineA, gPs aLineB, gPs bLineA, gPs bLineB)
{
        //First line, first point
        GLfloat x0 = aLineA.x;
        GLfloat y0 = aLineA.y;
        //First Line, second point
        GLfloat x1 = aLineB.x;
        GLfloat y1 = aLineB.y;
        //Second Line, First point
        GLfloat x2 = bLineA.x;
        GLfloat y2 = bLineA.y;
        //Second Line, Second point
        GLfloat x3 = bLineB.x;
        GLfloat y3 = bLineB.y;


	GLfloat d=(x1-x0)*(y3-y2)-(y1-y0)*(x3-x2);
	if (abs2(d)<0.001f) {return 0;}
	GLfloat AB=((y0-y2)*(x3-x2)-(x0-x2)*(y3-y2))/d;
	if (AB>0.0 && AB<1.0)
	{
		GLfloat CD=((y0-y2)*(x1-x0)-(x0-x2)*(y1-y0))/d;
		if (CD>0.0 && CD<1.0)
                {
                  return 1;
                }
        }

  return 0;
}

struct structSpark {
  gPs pa,pb, v; //Position and velocity
  GLfloat color[4];
  int life; //in frames
  int age; //in frames
};

class classSparkler
{
  private:
    list<struct structSpark> sparks;
  public:
    void spawn(gPs p, GLfloat rotation, int degree, GLfloat velocity, int num);
    void render();
};
classSparkler sparkler;

//Spawn sparks, used for the thruster and explotions
//gPs position, rotation (angle of emitting), degrees of freedom, gPs velocity, number of sparks, maxLife
void classSparkler::spawn(gPs p, GLfloat rotation, int degree, GLfloat velocity, int num)
{
  struct structSpark tS;
  for(int i = 0; i != num; i++)
  {
    GLfloat rotRad = ((rotation-(degree/2))+rand()%degree ) * 0.0174532925;
    tS.life = rand()%60;
    tS.age = 0;
    tS.color[0] = 1;
    tS.color[1] = float(rand()%1000) / 1000.0 ;
    tS.color[2] = 0;
    tS.color[3] = 1.0;
    tS.pa.x = p.x;
    tS.pa.y = p.y;
    int r=rand()%4;
    tS.pb.x = p.x + float(r) *cos( rotRad );
    tS.pb.y = p.y + float(r) *sin( rotRad );
    tS.v.x = velocity*cos( rotRad );
    tS.v.y = velocity*sin( rotRad );

    sparks.push_back(tS);
  }
}

void classSparkler::render()
{
  glBegin( GL_LINES );
    for(list <struct structSpark>::iterator it = sparks.begin(); it != sparks.end(); ++it)
    {
      //Move spark.
      it->pa.x += it->v.x;
      it->pa.y += it->v.y;
      it->pb.x += it->v.x;
      it->pb.y += it->v.y;

      //change color
      it->color[1] -= float(rand()%20/1000.0);
      it->color[3] = (1.0/it->life)*(it->life-it->age);

      glColor4f( it->color[0],it->color[1],it->color[2], it->color[3] );
      glVertex2f(it->pa.x - dispInfo.camPos.x, it->pa.y - dispInfo.camPos.y);
      glVertex2f(it->pb.x - dispInfo.camPos.x, it->pb.y - dispInfo.camPos.y);

      it->age++;
      if(it->age > it->life)
      {
        it = sparks.erase(it);
      }
    }
  glEnd( );
}

struct structShot {
  gPs pa,pb;
  gPs v;
  int age, life;
  bool fromShip; //if 1, wont collide with ship
};

class classBullets {
  private:
  vector<struct structShot> shots;
  public:
  void shoot(entity owner, gPs velocity);
  void render();
  bool col(vector<vert> target, bool isShip);

};

bool classBullets::col(vector<vert> target, bool isShip)
{
  gPs tLineA, tLineB;
  for(vector<struct structShot>::iterator it = shots.begin(); it != shots.end(); ++it)
  {
    if(it->fromShip != isShip)
    {
      for(int ii=0; ii < target.size(); ii++)
      {
        if(ii==0)
        {
          tLineB = target[ii].p;
        } else {
          tLineA = tLineB;
          tLineB = target[ii].p;

          if(LinesCross(tLineA, tLineB, it->pa, it->pb))
          {
            //Remove bullet.
            it = shots.erase(it);
            return(1);
          }
        }
      }
    }
  }
}

void classBullets::shoot(entity owner, gPs velocity)
{
  struct structShot ts;

  ts.pb = owner.p;
  ts.pa.x = owner.p.x + 6.0* cos( owner.rotation * 0.0174532925 );
  ts.pa.y = owner.p.y + 6.0* sin( owner.rotation * 0.0174532925 );

  ts.v.x = velocity.x + 1.0 * cos( owner.rotation * 0.0174532925 );
  ts.v.y = velocity.y + 1.0 * sin( owner.rotation * 0.0174532925 );

  ts.age = 0;
  ts.life = 40;

  if(owner.type == entShip)
  {
    ts.fromShip=1;
  } else {
    ts.fromShip=0;
  }

  shots.push_back(ts);
}

void classBullets::render()
{

  glBegin( GL_LINES );
  for(vector<struct structShot>::iterator it = shots.begin(); it != shots.end(); ++it)
  {
    it->pa.x += it->v.x;
    it->pa.y += it->v.y;
    it->pb.x += it->v.x;
    it->pb.y += it->v.y;

    glColor4f( 1.0, 0.5,0.0, 1.0 );
    glVertex2f(it->pa.x - dispInfo.camPos.x, it->pa.y - dispInfo.camPos.y);
    glVertex2f(it->pb.x - dispInfo.camPos.x, it->pb.y - dispInfo.camPos.y);

    it->age++;
    if(it->age > it->life)
    {
       it = shots.erase(it);
       if(shots.size() < 1)
        break;
    }
  }
  glEnd( );
}
class classBullets bullets;

void setSeneSize()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLfloat glSceneRes = dispInfo.glZoom;;
  dispInfo.glSceneSize.x = glSceneRes * dispInfo.aspect;
  dispInfo.glSceneSize.y = glSceneRes;
  dispInfo.glUnitsPrPixel = (glSceneRes / float(dispInfo.dispSize.y)) * 2.0;
  glOrtho( -dispInfo.glSceneSize.x, dispInfo.glSceneSize.x, -dispInfo.glSceneSize.y, dispInfo.glSceneSize.y, 0,1);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH );

  glLineWidth ( 1.0 );
}

void setRes(int x, int y)
{
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
  int more=0;
  dispInfo.fullScreen ? more=SDL_FULLSCREEN : more=0;

  /*SDL_Surface *screen =*/ SDL_SetVideoMode(x,y,32, SDL_OPENGL|SDL_RESIZABLE|more);
  glDisable(GL_DEPTH_TEST);

  dispInfo.aspect = float(x) / float(y);
  dispInfo.dispSize.x = x;
  dispInfo.dispSize.y = y;
  dispInfo.dispHalfSize.x = float(x)/2.0;
  dispInfo.dispHalfSize.y = float(y)/2.0;


  glViewport(0,0,x,y);


  glClearStencil(0); //stencil
  setSeneSize();

  glLineStipple (1, 0xCCCC);
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  if(glText != NULL)
    delete glText;

  glText = new glTextClass;
}

/*  It almost work, except when two verticies are both outside the screen, but the line between them crosses into the screen. */
/*
void isOnScreen(vector <vector<vert> >& polys, vector< vector<vert> >& onScreenPolys, gPs scrP, gPs scrN)
{
  bool polyNotEmpty, lastVis; //is this going to be visible? was the last one ?
  int numVert; //number of verticies, number of submitted (to the list to render)
  vector<vert> tPoly;
  onScreenPolys.clear();

  //Loop through polys.
  for(vector <vector <vert> >::iterator polyIt = polys.begin(); polyIt != polys.end(); ++polyIt)
  {
    numVert=0;
    lastVis=0;
    tPoly.clear();
    polyNotEmpty=0;
    for(vector <vert>::iterator vertIt = polyIt->begin(); vertIt != polyIt->end(); ++vertIt)
    {
      //Visible?
      if(vertIt->p.x > scrN.x && vertIt->p.x < scrP.x && vertIt->p.y < scrP.y && vertIt->p.y > scrN.y)
      {
        polyNotEmpty=1;
        //Was the one before me on ?
        if(numVert!=0 && lastVis==0)
        {
          //Put it on
          tPoly.push_back( polyIt->at(numVert-1) );
        }
        lastVis=1; //Telling it to the next.
        tPoly.push_back( *vertIt );
      } else {
        if(lastVis)
        {
          lastVis=0;
          tPoly.push_back( *vertIt );

          polyNotEmpty=0;
          onScreenPolys.push_back( tPoly );
          tPoly.clear();

        }
      }
        numVert++;
    }
    //That was one poly, check if any verts is there.
    if(polyNotEmpty)
    {
      onScreenPolys.push_back( tPoly );
    }
  }
}*/

void renderPolys(vector< vector<vert> > polys)
{
  int pc=0;
  int pp=0;
  for(vector <vector <vert> >::iterator it = polys.begin(); it != polys.end(); ++it)
  {
    glBegin( GL_LINE_STRIP );
    for(vector <vert>::iterator itt = it->begin(); itt != it->end(); ++itt)
    {
      if(itt->color[0] == -1.0)
      {
        glColor4f(0,0,0,0);
      } else {
        glColor4f(itt->color[0], itt->color[1], itt->color[2], 1.0);
      }

      glVertex2f(itt->p.x-dispInfo.camPos.x, itt->p.y-dispInfo.camPos.y);

      #ifdef SHOWRENDER
      pc++;
      #endif
    }
    glEnd();
    #ifdef SHOWRENDER
    pp++;
    #endif
  }

  //Draw verts if in editor
  if(gameState == GameStateEditor)
  {
    glPointSize( 3.0 );
    for(vector <vector <vert> >::iterator it = polys.begin(); it != polys.end(); ++it)
    {
      glColor4f(1,1,1,1);
      glBegin( GL_POINTS );
      for(vector <vert>::iterator itt = it->begin(); itt != it->end(); ++itt)
      {
        glVertex2f(itt->p.x-dispInfo.camPos.x, itt->p.y-dispInfo.camPos.y);
      }
      glEnd();
    }
  }

  #ifdef SHOWRENDER
  cout << "Rendered " << pc << " verts in " << pp << " polys." <<endl;
  #endif

}


//Move and rotate an entity according to ent.p and ent.rotation
void updateEntVerts(entity ent, vector<vert>& entverts, vector<vert> statV )
{
  entverts = statV;

  GLfloat tx, ty;

  for(vector<vert>::iterator i = entverts.begin(); i != entverts.end(); ++i)
  {
    tx = i->p.x * cos( (ent.rotation-90.0) * 0.0174532925 ) - ( i->p.y *sin( (ent.rotation-90.0) * 0.0174532925 )) ;
    ty = i->p.x * sin( (ent.rotation-90.0) * 0.0174532925 ) + ( i->p.y *cos( (ent.rotation-90.0) * 0.0174532925 ));
    i->p.x=tx;
    i->p.y=ty;
    i->p.x += ent.p.x;
    i->p.y += ent.p.y;
  }

}


#define PI 3.14159265
void renderEntities(vector<entity> ents)
{
  for(vector<entity>::iterator it = ents.begin(); it != ents.end(); ++it)
  {
    if(it->type==entShip)
    {
      updateEntVerts(*it, gameInfo.shipVerts, gameInfo.shipStaticVerts);

      if(gameState == GameStateEditor)
      {
        glColor3f(1,0,1);
        glBegin(GL_POINTS);
          glVertex2f(it->p.x - dispInfo.camPos.x, it->p.y - dispInfo.camPos.y);
        glEnd( );
        glPointSize(1.0);
      }

      if(gameInfo.thrust > 0)
      {
        gPs tP = it->p;
        tP.x += -1.3 * cos(it->rotation* 0.0174532925);
        tP.y += -1.3 * sin(it->rotation* 0.0174532925);

        sparkler.spawn(tP, it->rotation+180, 40, 0.7, 7);
      }


      glBegin(GL_LINE_STRIP);
      for(vector<vert>::iterator shipIt = gameInfo.shipVerts.begin(); shipIt != gameInfo.shipVerts.end(); ++shipIt)
      {
        glColor3f( shipIt->color[0],shipIt->color[1],shipIt->color[2] );
        glVertex2f( shipIt->p.x - dispInfo.camPos.x, shipIt->p.y - dispInfo.camPos.y );
      }
      glEnd( );


    }
    if(it->type==entBase)
    {

      updateEntVerts(*it, gameInfo.baseVerts, gameInfo.baseStaticVerts);

      if(gameState == GameStateEditor)
      {
        glPointSize(2.0);
        glColor3f(0,1,1);
        glBegin(GL_POINTS);
          glVertex2f(it->p.x - dispInfo.camPos.x, it->p.y - dispInfo.camPos.y);
        glEnd( );
        glPointSize(1.0);
      }

      glBegin(GL_LINE_STRIP);
      for(vector<vert>::iterator baseIt = gameInfo.baseVerts.begin(); baseIt != gameInfo.baseVerts.end(); ++baseIt)
      {
        glColor3f( baseIt->color[0],baseIt->color[1],baseIt->color[2] );
        glVertex2f( baseIt->p.x - dispInfo.camPos.x, baseIt->p.y - dispInfo.camPos.y );
      }
      glEnd( );
    }

    if(it->type==entEnemy)
    {

      updateEntVerts(*it, gameInfo.enemyVerts, gameInfo.enemyStaticVerts);

      if(gameState == GameStateEditor)
      {
        glPointSize(2.0);
        glColor3f(0,1,1);
        glBegin(GL_POINTS);
          glVertex2f(it->p.x - dispInfo.camPos.x, it->p.y - dispInfo.camPos.y);
        glEnd( );
        glPointSize(1.0);
      }

      glBegin(GL_LINE_STRIP);
      for(vector<vert>::iterator It = gameInfo.enemyVerts.begin(); It != gameInfo.enemyVerts.end(); ++It)
      {
        glColor3f( It->color[0],It->color[1],It->color[2] );
        glVertex2f( It->p.x - dispInfo.camPos.x, It->p.y - dispInfo.camPos.y );
      }
      glEnd( );
    }

  }
}

void drawGrid()
{
      //Draw a grid as editor background
      glColor3f(0.3,0.3,0.3);
      glBegin(GL_LINES);

        GLfloat x = int(dispInfo.camPos.x/5.0)*5 - dispInfo.glSceneSize.x;
        while(x < dispInfo.camPos.x + dispInfo.glSceneSize.x)
        {
          glVertex2f(x-dispInfo.camPos.x, -dispInfo.glSceneSize.y);
          glVertex2f(x-dispInfo.camPos.x,  dispInfo.glSceneSize.y);
          x += 5.0;
        }

        GLfloat y = int(dispInfo.camPos.y/5.0)*5 + dispInfo.glSceneSize.y;
        while(y > dispInfo.camPos.y -dispInfo.glSceneSize.y)
        {
          glVertex2f(-dispInfo.glSceneSize.x,  y-dispInfo.camPos.y);
          glVertex2f( dispInfo.glSceneSize.x,  y-dispInfo.camPos.y);
          y -= 5.0;
        }

      glEnd( );
}

void saveMap(vector< vector<vert> >verts, vector<entity> ents, string FileName)
{
  //Open file stream
  ofstream save;
  save.open( FileName.data() , ios::out | ios::trunc);

  if(!save.is_open())
  {
    cout << "File open err."<<endl;
    return;
  }
  //Save polys:
  for(vector< vector<vert> >::iterator it = verts.begin(); it != verts.end(); ++it)
  {
    save << "StartPoly" << endl;
    for(vector<vert>::iterator itt = it->begin(); itt != it->end(); ++itt)
    {
      save << "StartVert" << endl;
      save << itt->p.x << endl;
      save << itt->p.y << endl;
      save << itt->color[0] << endl;
      save << itt->color[1] << endl;
      save << itt->color[2] << endl;
      save << itt->collision << endl;
      save << "EndVert" << endl;
    }
    save << "EndPoly" << endl;
  }

  //Save ents
  for(vector<entity>::iterator it = ents.begin(); it != ents.end(); ++it)
  {
    {
      save << "StartEntity" << endl;
      save << it->p.x << endl;
      save << it->p.y << endl;
      save << it->type << endl;
      save << it->rotation << endl;
      save << it->id << endl;
      save << "EndEntity" << endl;
    }

  }


  //Save mission
  if(gameInfo.nextObjective.size() > 1)
  {
    save << "StartMission" << endl;
    for(int i = 0; i < gameInfo.nextObjective.size(); i++)
    {
      save << gameInfo.nextObjective[i] << endl;
    }
    save << "EndMission" << endl;
  }

  save.close();
}


void loadMap(vector< vector<vert> >& polys, vector<entity>& ents)
{
  polys.clear();
  ents.clear();
  string line;
  ifstream load;
  vector<vert> tvs; //temp to store the poly as we read it
  vert tv; //temp to store the vert as we read it
  entity te; //temp to store the entity while we read it
  gameInfo.numBases=0;
  gameInfo.nextObjective.clear();
  load.open(levelFile.data());

  if(!load.is_open())
  {
    cout << "could not load '" << levelFile << "'" << endl;
    return;
  }

  int parseState=0, dataNum=0;

  while(!load.eof())
  {
    getline(load, line);
    if(line == "StartPoly")
    {
      parseState=1;
    } else if(line=="EndPoly")
    {
      parseState=0;
      polys.push_back(tvs);
      tvs.clear();
    } else if(parseState==1)
    {
      if(line == "StartVert")
      {
        parseState=2;
        dataNum=0;
      }
    } else if(parseState==2)
    {
      if(line == "EndVert")
      {
        tvs.push_back(tv);
        parseState=1;
      } else {
        dataNum++;
        switch(dataNum)
        {
          case 1:
          tv.p.x = atof(line.data());
          break;
          case 2:
          tv.p.y = atof(line.data());
          case 3:
          tv.color[0] = atof(line.data());
          break;
          case 4:
          tv.color[1] = atof(line.data());
          break;
          case 5:
          tv.color[2] = atof(line.data());
          break;
          case 6:
          tv.collision = atoi(line.data());
          break;
          default:
          cout << "??" << line << endl;
          break;
        }
      }
    } else if(line =="StartEntity")
    {
      parseState=3;
      dataNum=0;
    } else if(parseState==3)
    {
      if(line=="EndEntity")
      {
        dataNum=0;
        ents.push_back(te);
        parseState=0;
      } else {
        dataNum++;
        switch(dataNum)
        {
          case 1:
            te.p.x = atof(line.data());
            break;
          case 2:
            te.p.y = atof(line.data());
            break;
          case 3:
            te.type = atoi(line.data());
            if(te.type==entBase)
            {
              gameInfo.numBases++;
            }
            break;
          case 4:
            te.rotation = atof(line.data());
            break;
          case 5:
            te.id = atoi(line.data());
            if(te.type==entBase && te.id != gameInfo.numBases)
            {
              cout << "error: base entity have id:" << te.id << " but numBases is:" << gameInfo.numBases << endl;
            }
            break;
          default:
            cout << ">?" << line << endl;
            break;
        }
      }
    } else if(line=="StartMission")
    {
      parseState=4;
    } else if(parseState==4)
    {
      if(line=="EndMission")
      {
        parseState=0;
      } else {
        gameInfo.nextObjective.push_back( atoi(line.data()) );
      }
    }
  }


}

void readEnt(string File, vector<vert>& verts)
{
  string line;
  ifstream f;
  vert tv; //temp to store the vert as we read it

  f.open(File.data());
  if(!f.is_open())
  {
    cout << "Failed to open file" << endl;
    return;
  }

  int parseState=0, dataNum=0;

  while(!f.eof())
  {
    getline(f, line);
    if(line == "StartVert")
    {
      parseState=1;
      dataNum=0;
    } else if(parseState==1)
    {
      if(line == "EndVert")
      {
        parseState=0;
        verts.push_back(tv);
      } else {
        dataNum++;
        switch(dataNum)
        {
          case 1:
          tv.p.x = atof(line.data());
          break;
          case 2:
          tv.p.y = atof(line.data());
          case 3:
          tv.color[0] = atof(line.data());
          break;
          case 4:
          tv.color[1] = atof(line.data());
          break;
          case 5:
          tv.color[2] = atof(line.data());
          break;
          case 6:
          tv.collision = atoi(line.data());
          break;
          default:
          cout << "??" << line << endl;
          break;
        }

      }
    }
  }

}

bool boxCol(gPs posa, gPs posb, GLfloat dist)
{
  if(posa.x < posb.x+dist && posa.x > posb.x-dist)
  {
    if(posa.y < posb.y+dist && posa.y > posb.y-dist)
    {
      return(1);
    }
  }
  return(0);
}

bool PolyCol( vector<vert> PolyA, vector<vert> PolyB  )
{
  gPs paLineA, paLineB, pbLineA, pbLineB;
  bool paFirst=1, pbFirst=1;
  int prevColType=0;
  int checks=0;

  for( vector<vert>::iterator paVertIt = PolyA.begin(); paVertIt != PolyA.end(); ++paVertIt)
  {
    if(paVertIt->collision)
    {
      if(paFirst || prevColType == 2) //if the last one was a 2, this one is the start of the next line to collision detect
      {
        paFirst=0;
        paLineB=paVertIt->p;
      } else {
        paLineA=paLineB;
        paLineB=paVertIt->p;


        pbFirst=1;
        //Great now we have a line, lets check it against every line in the other poly.
        for(vector<vert>::iterator pbVertIt = PolyB.begin(); pbVertIt != PolyB.end(); ++pbVertIt)
        {
          if(pbVertIt->collision)
          {
            if(pbFirst)
            {
              pbFirst=0;
              pbLineB=pbVertIt->p;
            } else {
              pbLineA=pbLineB;
              pbLineB=pbVertIt->p;

              //Here we have two lines. paLineA, paLineB, and pbLineA, pbLineB.
              checks++;
              if(LinesCross(paLineA, paLineB, pbLineA, pbLineB))
              {
                return(1);
              }

            }
          }
        }

      }
    }
    prevColType = paVertIt->collision;
  }
  #ifdef SHOWRENDER
  cout << "Did " << checks << " checks." << endl;
  #endif
  return(0);
}


bool landCol(vector<vert> baseVerts, vector<vert> shipVerts)
{

  gPs baseA, baseB;
  gPs shipA, shipB;
  int prevColType=0;

  //Find points on ship
  for(vector<vert>::iterator shipIt = shipVerts.begin(); shipIt != shipVerts.end(); ++shipIt)
  {
    if(shipIt->collision == 2)
    {
      if(prevColType == 0)
      {
        shipA = shipIt->p;
        prevColType++;
      } else {
        shipB = shipIt->p;
        prevColType++;
      }
    }
  }

  prevColType=0;
  for(vector<vert>::iterator baseIt = baseVerts.begin(); baseIt != baseVerts.end(); ++baseIt)
  {
    //Find the line to check with
    if(prevColType==2)
    {
      baseB = baseIt->p;
      if(shipA.x > baseA.x && shipA.x < baseB.x) //Is left side inside
      {
        if(shipB.x > baseA.x && shipB.x < baseB.x) // Is right side inside
        {
          if(shipA.y < baseA.y && shipB.y < baseB.y) //Is left side under the line
          {
            if(shipB.y < baseA.y && shipB.y < baseB.y) //Is right side under the line
            {
              if(shipA.y > baseA.y-1.0 && shipB.y > baseA.y-1.0) //Check that left side is above the height
              {
                //check velocity:
                //cout << "Impact:"<<endl<<"Xvel:" << gameInfo.velocity.x<<" Yvel: " << gameInfo.velocity.y << endl << "Rot:" << gameInfo.rotationForce << "." << endl;

                if(gameInfo.velocity.y >= gameRules.maxLandingYel) //If it's not smaller, it's not faster
                {
                  GLfloat temp = gameInfo.velocity.x;
                  if(temp < 0)
                    temp *= -1.0;

                  if(temp <= gameRules.maxLandingXvel) //it have to be slower
                  {

                    temp = gameInfo.rotationForce;
                    if(temp < 0)
                      temp *= -1.0;
                    if(temp <= gameRules.maxLandingRotForce)
                    {
                      return(1);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    prevColType = baseIt->collision;
    if(prevColType==2)
      baseA = baseIt->p;
  }
  return(0);
}

void renderRadar(vector<entity> ents, vector<vector <vert> > Poly)
{
  //The radar and everything should be the same size regardless of zoom
  //assume
  char txt[32];

  GLfloat scale = dispInfo.glZoom/100.0;
  GLfloat mapScale = scale / 10.0;

  //translate into place.
  glTranslatef(90.0*scale,-80.0*scale,0.0);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 1); // Always Passes, 1 Bit Plane, 1 As Mask
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // We Set The Stencil Buffer To 1 Where We Draw Any Polygon

  glColor4f(0,0,0, 0.8);
  glBegin( GL_QUADS );
    glVertex2f(15.0*scale, 15.0*scale );
    glVertex2f(-15.0*scale, 15.0*scale );
    glVertex2f(-15.0*scale, -15.0*scale );
    glVertex2f(15.0*scale, -15.0*scale );
  glEnd( );


  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


  glPushMatrix(); //Push matrix onto the stack
  //Draw the map, scale it down.
  for(vector< vector<vert> >::iterator PolyIt = Poly.begin(); PolyIt != Poly.end(); ++PolyIt)
  {
    glBegin( GL_LINE_STRIP );
      for(vector<vert>::iterator vertIt = PolyIt->begin(); vertIt != PolyIt->end(); ++vertIt)
      {
        if(vertIt->color[0] == -1.0)
        {
          glColor4f(0,0,0,0);
        } else {
          glColor4f(1,1,1,1);
        }
        glVertex2f( (vertIt->p.x-dispInfo.camPos.x)*mapScale,  (vertIt->p.y-dispInfo.camPos.y)*mapScale);
      }
    glEnd( );
  }

  //Draw destination (if any)
  if(gameInfo.destBase)
  {
    glColor3f(1,1,0);
     glEnable (GL_LINE_STIPPLE);
    glBegin( GL_LINES );
      glVertex2f(0,0);
      glVertex2f( (gameInfo.radarDest.x-dispInfo.camPos.x)*mapScale, (gameInfo.radarDest.y-dispInfo.camPos.y)*mapScale );
    glEnd( );
     glDisable (GL_LINE_STIPPLE);
  }

  glPointSize( 3.0 );
  for(vector<entity>::iterator entIt = ents.begin(); entIt != ents.end(); ++entIt)
  {
    glBegin(GL_POINTS);
    if(entIt->type==entShip)
    {
      glColor3f(0,0,1);
    } else if(entIt->type==entBase)
    {
      glColor3f(0,1,0);
    } else if(entIt->type==entEnemy) {
      glColor3f(1,0,0);
    } else {
      glColor3f(1,0,1);
    }
    glVertex2f((entIt->p.x-dispInfo.camPos.x)*mapScale,(entIt->p.y-dispInfo.camPos.y)*mapScale);
    glEnd( );
  }

  glEnd( );

  glPopMatrix();
  glDisable(GL_STENCIL_TEST);

  glColor3f( 0,1,0 );
  //Draw box around
  glBegin( GL_LINE_STRIP );
    glVertex2f(15*scale, 15*scale );
    glVertex2f(-15*scale, 15*scale );
    glVertex2f(-15*scale, -15*scale );
    glVertex2f(15*scale, -15*scale );
    glVertex2f(15*scale, 15*scale );
  glEnd( );

  //draw destination distance
  sprintf(txt, "> %i u", (int)(sqrt( pow( dispInfo.camPos.x - gameInfo.radarDest.x, 2) + pow( dispInfo.camPos.y - gameInfo.radarDest.y, 2 ) )*10.0) );
  glColor4f(0,1,0,1);
  glText->write(txt, FONT_DEFAULT,0, 40.0*scale, -15*scale, 15*scale+(glText->getHeight(FONT_DEFAULT)*20.0*scale));

  glTranslatef(-35*scale, 0, 0);

  GLfloat S = 15.0*scale;
  glColor4f(0,0,0, 0.8);
  glBegin( GL_QUADS );
    glVertex2f(S, S );
    glVertex2f(-S, S );
    glVertex2f(-S, -S );
    glVertex2f(S, -S );
  glEnd( );

  //Draw velocities
  gPs vel, maxL;
  vel.x = (S/gameRules.maxLandingXvel) * (gameInfo.velocity.x/2.0);
  vel.y = (S/gameRules.maxLandingYel) * (gameInfo.velocity.y/2.0);
  vel.y *= -1;

  if(vel.x > S)
  {
    vel.x = S;
  } else if(vel.x < -S)
  {
    vel.x = -S;
  }

  if(vel.y > S)
  {
    vel.y = S;
  } else if(vel.y < -S)
  {
    vel.y = -S;
  }

  glColor4f( 1,1,1,1 );
  glBegin( GL_LINES );
    glVertex2f( 0,0 );
    glVertex2f( vel.x, 0 );
    glVertex2f( 0,0 );
    glVertex2f( 0, vel.y );

    glVertex2f( 0,0 );
    glVertex2f( vel.x,vel.y );

    if(gameInfo.velocity.y > 0)
    {
      vel.y = (gameRules.maxLandingYel-gameInfo.velocity.y);
    } else {
      vel.y = (gameRules.maxLandingYel+gameInfo.velocity.y);
    }

    if(gameInfo.velocity.x > 0)
    {
      vel.x = (gameRules.maxLandingXvel-gameInfo.velocity.x);
    } else {
      vel.x = (gameRules.maxLandingXvel+gameInfo.velocity.x);
    }

    glColor3f(1,0,0);
    glVertex2f( S/2.0 +(vel.x*scale), -S);
    glVertex2f( S/2.0 +(vel.x*scale),  S);
    glVertex2f( -S/2.0 -(vel.x*scale), -S);
    glVertex2f( -S/2.0 -(vel.x*scale),  S);
    glVertex2f(-S,  S/2.0 + (vel.y*scale) );
    glVertex2f( S, S/2.0 + (vel.y*scale) );
    glVertex2f(-S, -S/2.0- (vel.y*scale)  );
    glVertex2f( S, -S/2.0- (vel.y*scale)  );

    //reuse for drawing fuel
    vel.y = (double(S)/double(gameRules.fuelMaxFuel) * double(gameInfo.fuel))*2.0;
    glColor3f( 1,1,0 );
    glVertex2f(-14*scale, -S );
    glVertex2f(-14*scale, -S+vel.y);

    //and for drawing ammo
    vel.y = (double(S)/double(gameRules.ammoMaxAmmo) * double(gameInfo.ammo))*2.0;
    glColor3f( 1,0,1 );
    glVertex2f(-13*scale, -S );
    glVertex2f(-13*scale, -S+vel.y);
  glEnd( );

  glColor3f( 0, 1, 0);
  glBegin( GL_LINE_STRIP );
    glVertex2f(15*scale, 15*scale );
    glVertex2f(-15*scale, 15*scale );
    glVertex2f(-15*scale, -15*scale );
    glVertex2f(15*scale, -15*scale );
    glVertex2f(15*scale, 15*scale );
  glEnd( );

  //Draw speed
  sprintf(txt, "%i u/T", abs((int)gameInfo.velocity.x*100)+abs((int)gameInfo.velocity.y*100));
  glText->write(txt, FONT_DEFAULT,0, 40.0*scale, -15*scale, 15*scale+(glText->getHeight(FONT_DEFAULT)*20.0*scale));

  glLoadIdentity();

}

void setMission(int missionId, vector<entity> ents)
{
  gameInfo.destBase = gameInfo.nextObjective[missionId];

  //Set destination on radar.
  for(vector<entity>::iterator searchIt = ents.begin(); searchIt != ents.end(); ++searchIt)
  {
    if(searchIt->id == gameInfo.destBase)
    {
      gameInfo.radarDest = searchIt->p;
    }
  }
}

void initGame(vector< vector<vert> >& polys, vector<entity>& ents)
{
  loadMap(polys, ents);


 gameInfo.currentObjective=0;
gameInfo.score=0;
gameInfo.destBase=0;
gameInfo.fuel = gameRules.fuelMaxFuel;
gameInfo.ammo = gameRules.ammoMaxAmmo;

  if(gameInfo.nextObjective.size() > 1)
    setMission(0,ents);



}

void advanceLevel(vector< vector<vert> >& polys, vector<entity>& ents)
{
  ostringstream t;
  t.clear();
  cout << " --- Completed Level " << gameInfo.level << " --- " << endl;
  cout << "Level: " << gameInfo.level << endl << "Score:" << gameInfo.score << endl;
  gameInfo.level++;
  t << DATADIR"levels/" << gameInfo.level << ".level";
  levelFile = t.str();
  initGame(polys,ents);
  gameState = GameStatePlaying;
}

void initNewGame(vector< vector<vert> >& polys, vector<entity>& ents)
{
  gameRules.maxLandingRotForce = 1.0; //degrees, both ways
  gameRules.maxLandingYel = -0.20; //downward
  gameRules.maxLandingXvel= 0.15; //both sides
  gameRules.fuelConsumptionThrust = 15;
  gameRules.fuelConsumptionTurn = 4;
  gameRules.fuelMaxFuel = 5500;
  gameRules.ammoMaxAmmo = 1000;

  dispInfo.glZoom = 28;
  setSeneSize();
  gameState = GameStatePlaying;

  gameInfo.numMissions=0; //Amount of missions taken
  gameInfo.level=gameRules.startLevel;
  initGame(polys, ents);
}

void shipCrash(entity ship)
{
  sparkler.spawn(ship.p, 0, 360, 1, 100);
  gameInfo.velocity.x=0;
  gameInfo.velocity.y=0;
  gameInfo.thrust=0;
  gameInfo.rotationForce=0;
  gameState = GameStateGameOver;
}

struct gameInfoStruct snap;
gPs snapPos;
entity snapShip;
void loadState(vector<entity>& entVect)
{
  gameInfo = snap;
  for(vector<entity>::iterator it=entVect.begin(); it != entVect.end(); ++it)
  {
    if(it->type==entShip)
    {
      *it = snapShip;
    }
  }

}

void saveState(vector<entity> entVect)
{
  snap = gameInfo;
  for(vector<entity>::iterator it=entVect.begin(); it != entVect.end(); ++it)
  {
    if(it->type==entShip)
    {
      snapPos = it->p;
      snapShip = *it;
    }
  }
}

bool screenShot()
{
  FILE *fscreen;

  char cName[256];
  int i = 0;
  bool found=0;
  while(!found)
  {
    sprintf(cName, "screenshot_%i.tga",i);
    fscreen = fopen(cName,"rb");
    if(fscreen==NULL)
      found=1;
    else
      fclose(fscreen);
      i++;
  }
  int nS = dispInfo.dispSize.x * dispInfo.dispSize.y * 3;
  GLubyte *px = new GLubyte[nS];
  if(px == NULL)
  {
    cout << "Alloc err, screenshot failed." <<endl;
    return 0;
  }
  fscreen = fopen(cName,"wb");

  glPixelStorei(GL_PACK_ALIGNMENT,1);

  glReadPixels(0, 0, dispInfo.dispSize.x, dispInfo.dispSize.y, GL_BGR, GL_UNSIGNED_BYTE, px);


  unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
  unsigned char header[6] = {dispInfo.dispSize.x%256,dispInfo.dispSize.x/256,dispInfo.dispSize.y%256,dispInfo.dispSize.y/256,24,0};
  fwrite(TGAheader, sizeof(unsigned char), 12, fscreen);
  fwrite(header, sizeof(unsigned char), 6, fscreen);

  fwrite(px, sizeof(GLubyte), nS, fscreen);
  fclose(fscreen);
  delete [] px;
  cout << "Wrote screenshot to '" << cName << "'" <<endl;
  return 1;
}


void demoRec(Uint8* ks)
{
  demoFrame tf;
  tf.up = ks[SDLK_UP];
  tf.left = ks[SDLK_LEFT];
  tf.right = ks[SDLK_RIGHT];
  tf.shoot = ks[SDLK_SPACE];

  demoFrames.push_back(tf);
}

void saveDemo()
{
  if(gameInfo.recording != 1)
    return;

  gameInfo.recording=0;

  fstream demo;
  demo.open( "demo.bin", ios::out | ios::trunc | ios::binary );

  if(!demo.is_open())
    return;

  demoFrame df;
  uint32_t dSize= (uint32_t)demoFrames.size();
#if __BYTE_ORDER == __BIG_ENDIAN
  dSize = swapbytes32(dSize);
#endif

  demo.write( (char *)(&dSize), sizeof(uint32_t) );
  cout << "Demo saved frames:" << demoFrames.size() << endl;

  for(int i = 0; i < demoFrames.size(); i++)
  {
    df = demoFrames[i];
    demo.write((char *)(&df), sizeof(demoFrame));
  }
}

void loadDemo(char *demoFile)
{
  demoFrames.clear();
  fstream demo;
  demo.open( demoFile, ios::in | ios:: binary );
  if(!demo.is_open())
  {
    cout << "Could not open" << demoFile <<endl;
    return;
  }
  gameInfo.recording=0;

  uint32_t dSize=-1;
  demo.read( (char *)(&dSize), sizeof(uint32_t) );
#if __BYTE_ORDER == __BIG_ENDIAN
  dSize = swapbytes32(dSize);
#endif
  cout  << "Demo Load frames:" << dSize << endl;

  demoFrame df;
  while(demoFrames.size() != dSize)
  {
    demo.read( (char *)(&df), sizeof(demoFrame));
    demoFrames.push_back(df);
  }
  cout << "Loaded frames:" << demoFrames.size() << endl;
  gameInfo.playing=1;
  gameInfo.demoFrame=0;

}

void demoPlay(Uint8* ks)
{
  //advance one frame
  if(gameInfo.demoFrame != demoFrames.size() )
  {
    ks[SDLK_UP]= demoFrames[gameInfo.demoFrame].up;
    ks[SDLK_LEFT]= demoFrames[gameInfo.demoFrame].left;
    ks[SDLK_RIGHT]= demoFrames[gameInfo.demoFrame].right;
    ks[SDLK_SPACE]= demoFrames[gameInfo.demoFrame].shoot;
    gameInfo.demoFrame++;
  } else {
    gameInfo.demoFrame=0;
    gameInfo.playing=0;
  }
}

int main(int argc, char **argv)
{

  srand ( time(NULL) );

  soundOn=1;
  if(argc > 1)
  {
    gameRules.startLevel = atoi(argv[1]);
    levelFile = DATADIR"levels/";
    levelFile.append(argv[1]);
    levelFile.append(".level");

    if(argc == 3)
    {
      loadDemo(argv[2]);
    }
  } else {
    levelFile = DATADIR"levels/0.level";
  }
  gameState = GameStateNewGame;


  #ifdef SHOWRENDER
  struct timeval timeStart,timeStop;
  int renderTime;
  #endif

  SDL_Event event;
  //Init sdl and screen
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) <0 )
  {
    cout << SDL_GetError() << endl;
  }
  SDL_WM_SetCaption("OldSkoolGravityGame "VERSION, "OSGG");
  SDL_ShowCursor(SDL_DISABLE);
  SDL_WM_SetIcon( IMG_Load( DATADIR"icon.png" ), 0 );

  cout << "Old Skool Gravity Game "VERSION". GPL v3"<<endl;
  cout << " ./osgg [NUM [d]]" << endl;
  cout << " NUM - optional, starting level " << endl;
  cout << " d - optional (only if NUM is supplied) name of demo file to play" << endl;
  cout << "Editor controls:\n esc = cancel poly creation, or exit program. \n del = remove.\n 1 = collision on, 2 = collision off, 3 Platform\n";
  cout << " 4 = white, 5 = grey, 6=Invisible, 7 = red, 8 = green, 9 = blue\n";
  cout << " 0 = line. q = ship, w = base, e = enemy\n d = write first poly to verts.txt\n";
  cout << " m = define mission waypoints, n=use waypoints\n";
  cout << " s = toggle editor and game\n g = save map to '"<<levelFile<<"', l = load map\n";
  cout << " t = Start new game, F11 fullscreen" << endl <<"F12 = screenshot" << endl;
  cout << " F1 = start demo recording, F2 = stop demo recording and save to 'demo.bin'" << endl;
  cout << "F10 toggle sound" << endl;
  cout << "Using file '" << levelFile << "'" << endl;

  //Set initial resolution
  dispInfo.glZoom=100.0;

  dispInfo.screenRes.x = SDL_GetVideoInfo()->current_w;
  dispInfo.screenRes.y = SDL_GetVideoInfo()->current_h;


  setRes(800,600);

  soundMan = new soundClass;
  soundMan->init();

/** Editor vars **/
  vector<gPs> editorWayPoints;
  vector<int> editorIntWayPoints;

  vector<entity> ents; //entities
  vector<vert> activeVerts;
  vector< vector<vert> > polys;
  int editMode=0; // 0 = Start Poly 1 = Insert Verts, 2 = Cancel Poly, 3 = SelectPoly, 4 = move poly
  gPs* dragVert; //Pointer to the verticies position
  GLfloat crossColor[3];
  GLfloat lineColor[3] = { 1, 1, 1 };
  gameInfo.numBases = 0;

  int newType=entLine;
  int newCol=1; //collision on new lines.
  int editorDeletePoly=0; //0= none, 1= remove key pressed, 2=remove it;

/** GameVars **/
vector<vert> testVerts;
vert tempVert;
gPs collisionPoint;
bool crashed=0;
char score[256];


GLfloat scale;

readEnt("ship.txt", gameInfo.shipStaticVerts);
readEnt("base.txt", gameInfo.baseStaticVerts);
readEnt("enemy.txt", gameInfo.enemyStaticVerts);

  //Enter Main loop
  while(gameState != GameStateQuit)
  {
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          gameState = GameStateQuit;
        break;
        case SDL_KEYDOWN:

          if(gameState==GameStateGameOver)
          {
            initGame(polys, ents);
            gameState=GameStatePlaying;
          }

          switch(event.key.keysym.sym)
          {
            case SDLK_ESCAPE:

              if(editMode==1)
              {
                editMode=2;
              } else {
                gameState = GameStateQuit;
              }
            break;
            case SDLK_DELETE:
              editorDeletePoly=1;
            break;
            case SDLK_1:
              newCol=1;
               cout << "Collision on the next lines." << endl;
             break;
            case SDLK_2:
              newCol=0;
              cout << "Collision off the next lines." << endl;
              break;
            case SDLK_3:
              newCol=2;
              cout << "Next Lines are platforms." << endl;
              break;
            case SDLK_4:
              lineColor[0] = 1.0;
              lineColor[1] = 1.0;
              lineColor[2] = 1.0;
              break;
            case SDLK_5:
              lineColor[0] = 0.4;
              lineColor[1] = 0.4;
              lineColor[2] = 0.4;
              break;
            case SDLK_6:
              lineColor[0] = -1.0;
              lineColor[1] =  1.0;
              lineColor[2] =  1.0;
              break;
            case SDLK_7:
              lineColor[0] = 1.0;
              lineColor[1] = 0.0;
              lineColor[2] = 0.0;
              break;
            case SDLK_8:
              lineColor[0] = 0.0;
              lineColor[1] = 1.0;
              lineColor[2] = 0.0;
              break;
            case SDLK_9:
              lineColor[0] = 0.0;
              lineColor[1] = 0.0;
              lineColor[2] = 1.0;
              break;
            case SDLK_0:
              newType=entLine;
              break;
            case SDLK_q:
              newType=entShip;
              break;
            case SDLK_w:
              newType=entBase;
              break;
            case SDLK_m:
              newType=entWp;
              break;
            case SDLK_n:
              gameInfo.nextObjective = editorIntWayPoints;
              break;
            case SDLK_e:
              newType=entEnemy;
              break;
            case SDLK_o:
              loadState(ents);
              break;
            case SDLK_p:
              saveState(ents);
              break;

            case SDLK_d:
              saveMap(polys, ents, "verts.txt");
            break;

            case SDLK_s:
              if(gameState == GameStateEditor)
                gameState = GameStatePlaying;
                else
                gameState = GameStateEditor;
            break;
            case SDLK_g:
              saveMap(polys, ents, levelFile.data());
              break;
            case SDLK_l:
              loadMap(polys, ents);
              break;
            case SDLK_t:
              gameState=GameStateNewGame;
              break;
            case SDLK_F10:
              soundOn ? soundOn=0:soundOn=1;
              break;
            case SDLK_F11:
              dispInfo.fullScreen ? dispInfo.fullScreen=0 : dispInfo.fullScreen=1;
              if(dispInfo.fullScreen)
              {
                setRes(dispInfo.screenRes.x,dispInfo.screenRes.y);
              } else {
                setRes(800,600);
              }
              break;
            case SDLK_F1:
              gameInfo.recording=1;
              demoFrames.clear();
              break;
            case SDLK_F2:
              saveDemo();
              break;
            case SDLK_F12:
              screenShot();
              break;
          }
        break;//case sdlkeydown
        case SDL_VIDEORESIZE:
          setRes(event.resize.w, event.resize.h);
        break;
        case SDL_MOUSEMOTION:
          dispInfo.mousePos.x = (event.motion.x - dispInfo.dispHalfSize.x) * dispInfo.glUnitsPrPixel;
          dispInfo.mousePos.y = (event.motion.y - dispInfo.dispHalfSize.y) * dispInfo.glUnitsPrPixel * -1;
          dispInfo.mousePos.x += dispInfo.camPos.x;
          dispInfo.mousePos.y += dispInfo.camPos.y;
        break;
        case SDL_MOUSEBUTTONDOWN:
          if(event.button.button == SDL_BUTTON_LEFT)
          {
            dispInfo.mouseDrag=1;
          }
          else if(event.button.button == SDL_BUTTON_RIGHT)
          {
          dispInfo.mouseRightDrag=1;
          }
          else if(event.button.button == SDL_BUTTON_WHEELDOWN)
          {
          dispInfo.glZoom += 2;
          setSeneSize();
          }
          else if(event.button.button == SDL_BUTTON_WHEELUP)
          {
          dispInfo.glZoom -= 2;
          setSeneSize();
          }

        break;
        case SDL_MOUSEBUTTONUP:
          if(event.button.button == SDL_BUTTON_LEFT)
          dispInfo.mouseDrag=0;
          else if(event.button.button == SDL_BUTTON_RIGHT)
          dispInfo.mouseRightDrag=0;
        break;
      }
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glLoadIdentity();

    soundMan->play();

    #ifdef WIN32
    Sleep( 16 );
    #else
    usleep(16666); // around 60 fps
    #endif

    #ifdef SHOWRENDER
    gettimeofday(&timeStart, NULL);
    #endif

    switch(gameState)
    {
      case GameStateNewGame:
        initNewGame(polys,ents);
        gameState=GameStatePlaying;
      break;

      case GameStateNextLevel:
        advanceLevel(polys, ents);
      break;

      case GameStatePause:
      case GameStateGameOver: //This is the same as pause
        renderPolys(polys);
        renderEntities(ents);
        renderRadar(ents,polys);
        sparkler.render();
      break;
      case GameStatePlaying:

        renderPolys(polys);
        renderEntities(ents);
        renderRadar(ents,polys);
        sparkler.render();
        bullets.render();

        //Render HUD.
        gameInfo.score += 1;
        glColor4f( 0,1,0,1 );
        scale = dispInfo.glZoom/100.0;

        sprintf(score, "%i", gameInfo.score);
        glText->write(score, FONT_DEFAULT,1, 50.0*scale, 0.0, dispInfo.glSceneSize.y- (glText->getHeight(FONT_DEFAULT)*30.0)*scale );

        //Ents:
        for(vector<entity>::iterator it = ents.begin(); it != ents.end(); ++it)
        {
          if(it->type == entShip)
          {
            Uint8* keyStates = SDL_GetKeyState( NULL );

            //Record?
            if(gameInfo.recording)
            {
              demoRec(keyStates);

            //Override input
            }
            else if(gameInfo.playing)
            {
              demoPlay(keyStates);
            }

            if(keyStates[SDLK_LEFT])
            {
              if(gameInfo.fuel > 0)
              {
                gameInfo.rotationForce += TURNINCRATE;
                gameInfo.fuel -= gameRules.fuelConsumptionTurn;
              }
            } else if(keyStates[SDLK_RIGHT])
            {
              if(gameInfo.fuel > 0)
              {
                gameInfo.rotationForce -= TURNINCRATE;
                gameInfo.fuel -= gameRules.fuelConsumptionTurn;
              }
            }

            if(keyStates[SDLK_UP])
            {
              if(gameInfo.fuel > 0)
              {
                gameInfo.thrust = THRUSTINCRATE;
                gameInfo.fuel -= gameRules.fuelConsumptionThrust;
              } else {
                gameInfo.thrust = 0;
              }
            } else
            {
              gameInfo.thrust = 0;
            }

            if(gameInfo.reloadTime != 0)
            {
              gameInfo.reloadTime--;
            }
            if(keyStates[SDLK_SPACE])
            {
              if(gameInfo.reloadTime == 0 && gameInfo.ammo >= 100)
              {
                gameInfo.ammo -= 100;
                gameInfo.reloadTime = 25;
                bullets.shoot(*it, gameInfo.velocity);
              }
            }

            gameInfo.velocity.x += gameInfo.thrust * cos( (it->rotation*0.0174532925) );
            gameInfo.velocity.y += gameInfo.thrust * sin( (it->rotation*0.0174532925) );

            gameInfo.velocity.y -= GRAVITY;

            if(gameInfo.rotationForce > 0)
            {
              gameInfo.rotationForce /= 1.01;
              if(gameInfo.rotationForce < 0)
              {
                gameInfo.rotationForce = 0;
              }
            } else if(gameInfo.rotationForce < 0)
            {
              gameInfo.rotationForce /= 1.01;
              if(gameInfo.rotationForce > 0)
              {
                gameInfo.rotationForce = 0;
              }
            }

            //sound
            if(gameInfo.thrust != 0)
            {
              soundMan->add(sfxNozzle);
            }

            //Collision with terrain
            for(vector< vector<vert> >::iterator PolyIt = polys.begin(); PolyIt != polys.end(); ++PolyIt)
            {
              if(PolyCol(*PolyIt, gameInfo.shipVerts))
              {
                shipCrash(*it);
              }
            }

            crashed=0;
           //Collision with ents
            for(vector<entity>::iterator entIt = ents.begin(); entIt != ents.end(); ++entIt)
            {
              //Collition with entities
              if(entIt->type==entBase)
              {
                updateEntVerts(*entIt, gameInfo.baseVerts, gameInfo.baseStaticVerts);
                crashed=PolyCol(gameInfo.baseVerts, gameInfo.shipVerts);
              } else if(entIt->type==entEnemy)
              {
                updateEntVerts(*entIt, gameInfo.enemyVerts, gameInfo.enemyStaticVerts);
                crashed=PolyCol(gameInfo.enemyVerts, gameInfo.shipVerts);

                //check if this enemy is being hit
                if(bullets.col(gameInfo.enemyVerts, 0))
                {
                  sparkler.spawn(entIt->p, 0, 360, 1, 100);
                  entIt = ents.erase(entIt);
                  gameInfo.score -= 3000;
                  break;
                }
              }

              if(crashed)
              {
                shipCrash(*it);
              } else {
                //Is he landing on a base?
                if(entIt->type==entBase)
                {
                  if(landCol(gameInfo.baseVerts, gameInfo.shipVerts))
                  {
                    //Save state
//                     saveState(ents);

                    //Subtract 1 from score again
                    gameInfo.score -= 1;

                    if(gameInfo.fuel < gameRules.fuelMaxFuel)
                    gameInfo.fuel += 15;
                    if(gameInfo.fuel > gameRules.fuelMaxFuel)
                    gameInfo.fuel = gameRules.fuelMaxFuel;

                    if(gameInfo.ammo < gameRules.ammoMaxAmmo)
                    gameInfo.ammo += 3;
                    if(gameInfo.ammo > gameRules.ammoMaxAmmo)
                    gameInfo.ammo = gameRules.ammoMaxAmmo;


                    it->rotation=90.0;

                    if(gameInfo.velocity.y < 0.0)
                      gameInfo.velocity.y=0;

                    gameInfo.velocity.x=0;
                    gameInfo.rotationForce=0;

                    if(gameInfo.numBases > 1)
                    {
                      //Is this the destination base?
                      if(gameInfo.destBase == entIt->id)
                      {
                        gameInfo.score -= 1000;
                        gameInfo.numMissions++;

                        //Do he have more missions left?
                        gameInfo.currentObjective++;
                        if(gameInfo.currentObjective < gameInfo.nextObjective.size())
                        {
                          //Give it to him
                          setMission(gameInfo.currentObjective, ents);
                        } else {
                          gameState = GameStateNextLevel;
                        }
                      }
                    }
                  }
                }
              } //not crashing
            }

            it->rotation += gameInfo.rotationForce;
            it->p.x += gameInfo.velocity.x;
            it->p.y += gameInfo.velocity.y;

            dispInfo.camPos = it->p;
          }
        }



      break;

      /** END OF GAME **/

      case GameStateEditor:


      crossColor[0] = 1.0;
      crossColor[1] = 1.0;
      crossColor[2] = 1.0;
      //Search all polygons for verticies to see if player wish to edit it
      if(editMode != 4)
      {
        if(editMode==3)
        editMode=0;
        for(vector <vector <vert> >::iterator it = polys.begin(); it != polys.end(); ++it)
        {
          for(vector <vert>::iterator itt = it->begin(); itt != it->end(); ++itt)
          {
            if(boxCol(dispInfo.mousePos, itt->p, (dispInfo.glZoom+1)/100))
            {
              crossColor[0] = 1.0;
              crossColor[1] = 0.0;
              crossColor[2] = 0.0;

              if(editorDeletePoly)
              {
                editorDeletePoly=2;

                break;
              }

              if(dispInfo.mouseDrag)
              {
                editMode=4;
                dragVert = &itt->p;
              }
            }
          }
          if(editorDeletePoly==2)
          {
            polys.erase(it);
            break;
          }
        }

        //Do the same for ents
        for(vector<entity>::iterator it = ents.begin(); it != ents.end(); ++it)
        {
          if(boxCol(dispInfo.mousePos, it->p, (dispInfo.glZoom+1)/100))
          {
            if(newType==entWp)
           {
              crossColor[0] = 0.0;
              crossColor[1] = 1.0;
              crossColor[2] = 0.0;
              if(dispInfo.mouseDrag)
              {
                editorWayPoints.push_back( it->p );
                editorIntWayPoints.push_back(it->id );
                dispInfo.mouseDrag = 0;
              }
           } else {
              crossColor[0] = 1.0;
              crossColor[1] = 0.0;
              crossColor[2] = 1.0;
              if(dispInfo.mouseDrag)
              {
                editMode=4;
                dragVert = &it->p;
              }

              if(editorDeletePoly)
              {
                editorDeletePoly=2;
              }
              if(editorDeletePoly==2)
              {
                ents.erase(it);
                break;
              }
            }
          }

        }

      }

      editorDeletePoly=0;

      if(editMode==4)
      {
        crossColor[0] = 1.0;
        crossColor[1] = 0.0;
        crossColor[2] = 0.0;

        if(!dispInfo.mouseDrag)
        {
          editMode=0;
        } else {
          *dragVert = dispInfo.mousePos;
        }
      }

      //Add stuff
      if(dispInfo.mouseDrag && (editMode == 0 || editMode ==1) )
      {
        editMode = 1;
        dispInfo.mouseDrag = 0;
        //Add line to poly
        if(newType==entLine)
        {
          vert tVert;
          tVert.color[0] = lineColor[0];
          tVert.color[1] = lineColor[1];
          tVert.color[2] = lineColor[2];
          tVert.collision = newCol;

          //Finished the poly
          if(activeVerts.size() > 1 && boxCol(dispInfo.mousePos, activeVerts[0].p, (dispInfo.glZoom+1)/100))
          {
            editMode = 0;
            tVert.p = activeVerts[0].p;
            activeVerts.push_back(tVert);

            polys.push_back(activeVerts);
            activeVerts.clear();
          } else {
            tVert.p = dispInfo.mousePos;
            activeVerts.push_back(tVert);
          }

        }

        //Add ship starting point
        if(newType==entShip)
        {

          //Search for an existing ship and remove that if found
          for(vector<entity>::iterator it = ents.begin(); it != ents.end(); ++it)
          {
            if(it->type == entShip)
            {
              ents.erase(it);
              break;
            }
          }
          entity tEnt;
          tEnt.p = dispInfo.mousePos;
          tEnt.type=newType;
          tEnt.rotation = 90.0;
          tEnt.id=-1; // ship don't have
          ents.push_back(tEnt);
          editMode = 0;
        }

        //Add base entity
        if(newType==entBase)
        {
          entity tEnt;
          tEnt.p = dispInfo.mousePos;
          tEnt.type=newType;
          tEnt.rotation = 90.0;
          gameInfo.numBases++;
          tEnt.id = gameInfo.numBases;
          ents.push_back(tEnt);
          editMode = 0;
        }

        if(newType==entEnemy)
        {
          entity tEnt;
          tEnt.p = dispInfo.mousePos;
          tEnt.type=newType;
          tEnt.rotation = 90.0;
          tEnt.id = -1;
          ents.push_back(tEnt);
          editMode = 0;
        }

      }

      if(editMode==1)
      {
        crossColor[0] = 0.0;
        crossColor[1] = 1.0;
        crossColor[2] = 0.0;
      }


      if( editMode == 2 )
      {
        activeVerts.clear();
        editMode = 0;
      }

      //Moving around
      if( dispInfo.mouseRightDrag )
      {
        dispInfo.mouseRightDrag=0;
        dispInfo.camPos.x += dispInfo.mousePos.x - dispInfo.camPos.x;
        dispInfo.camPos.y += dispInfo.mousePos.y - dispInfo.camPos.y;

        SDL_WarpMouse( dispInfo.dispHalfSize.x, dispInfo.dispHalfSize.y );
      }

      drawGrid();


      if(activeVerts.size() > 1)
      {
        if( boxCol(dispInfo.mousePos, activeVerts[0].p, (dispInfo.glZoom+1)/100) )
        {
          crossColor[0] = 0.0;
          crossColor[1] = 0.0;
          crossColor[2] = 1.0;
        }
      }

      if(crossColor[0] == 1 && crossColor[1] == 1 && crossColor[2] == 1)
      {
        crossColor[0] = lineColor[0];
        crossColor[1] = lineColor[1];
        crossColor[2] = lineColor[2];
      }

      glColor3f(crossColor[0], crossColor[1], crossColor[2]);
      //Draw the cross
      glBegin( GL_LINES );
        // -
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x - 5.0, dispInfo.mousePos.y-dispInfo.camPos.y );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x - 1.0, dispInfo.mousePos.y-dispInfo.camPos.y );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x + 5.0, dispInfo.mousePos.y-dispInfo.camPos.y );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x + 1.0, dispInfo.mousePos.y-dispInfo.camPos.y );
        // |
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x, dispInfo.mousePos.y-dispInfo.camPos.y + 5.0 );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x, dispInfo.mousePos.y-dispInfo.camPos.y + 1.0 );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x, dispInfo.mousePos.y-dispInfo.camPos.y - 5.0 );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x, dispInfo.mousePos.y-dispInfo.camPos.y - 1.0 );
      glEnd( );

      glColor3f( 1, 1,1);
      glBegin( GL_POINTS );
        glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x, dispInfo.mousePos.y-dispInfo.camPos.y );
      glEnd( );

      glBegin( GL_POINTS );
        glVertex2f( 0,0 );
        glVertex2f( -2.5,2.5 );
        glVertex2f(  2.5,2.5 );
        glVertex2f(  2.5,-2.5);
        glVertex2f( -2.5,-2.5);
      glEnd( );

      //Draw the line not placed yet
      if( editMode == 1)
      {
        if(activeVerts.size() > 0)
        {
          glBegin( GL_LINES );
          glColor3f(activeVerts.back().color[0], activeVerts.back().color[1], activeVerts.back().color[2]);
          glVertex2f( activeVerts.back().p.x-dispInfo.camPos.x, activeVerts.back().p.y-dispInfo.camPos.y );
          glColor3f( lineColor[0], lineColor[1], lineColor[2]);
          glVertex2f( dispInfo.mousePos.x-dispInfo.camPos.x, dispInfo.mousePos.y-dispInfo.camPos.y );
          glEnd() ;
        }
      }

      //Render active verticies
      glBegin( GL_LINE_STRIP | GL_POINTS );
      for(vector<vert>::iterator it = activeVerts.begin(); it != activeVerts.end(); ++it)
      {
        glColor3f(it->color[0], it->color[1], it->color[2]);
        glVertex2f(it->p.x-dispInfo.camPos.x, it->p.y-dispInfo.camPos.y);
      }
      glEnd( );

      //Render mission path
      glBegin( GL_LINE_STRIP );
      glColor4f( 1,1,0,1 );
      for(int i=0; i < editorWayPoints.size(); i++)
      {
       glVertex2f(editorWayPoints[i].x - dispInfo.camPos.x, editorWayPoints[i].y - dispInfo.camPos.y);
      }
      glEnd( );

      //Render gameworld
      renderPolys(polys);
      renderEntities(ents);

      break;
    }

    #ifdef SHOWRENDER
      gettimeofday(&timeStop, NULL);
      renderTime = timeStop.tv_usec - timeStart.tv_usec;
      cout << "GameLogic took:" << renderTime << endl;
    #endif

    //TODO: add timer to check if it blocks (we don't want to wait if it do, then vsync is on)
    SDL_GL_SwapBuffers( );

  }


  SDL_ShowCursor(SDL_ENABLE);

  return(0);
}
