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

#include <unistd.h>

class controllerClass {
  private:
  paddle_class *paddle;
  bulletsClass *bullet;
  ballManager *bMan;
  #define ITEMSELECTTIME 300 /* Milliseconds before changing item */
  int shotTime, itemSelectTime;
  float accel;
  SDL_Joystick *joystick;
  Sint16 joystickx;
  Sint16 joysticky;
  Uint8 joystickbtnA, joystickbtnB;
  bool joyBtnALock; //TODO: implement this in the rest of the joystick functions (current only using it in cal)
  GLfloat joystickLeftX; //There are two because the calibrated values differ
  GLfloat joystickRightX;
  int calMin, calMax, calLowJitter, calHighJitter;

  #ifdef WITH_WIIUSE
  wiimote** wiimotes;
  float motePitch;
  GLfloat moteAccel;
  #endif

  public:
  controllerClass(paddle_class *pc, bulletsClass *bu, ballManager *bm);
  ~controllerClass();
  void movePaddle(GLfloat px);
  void btnPress();
  bool get();
  void calibrate();
  #ifdef WITH_WIIUSE
  bool connectMote();
  #endif
};

controllerClass::controllerClass(paddle_class *pc, bulletsClass *bu, ballManager *bm)
{
  paddle = pc;
  bullet = bu;
  bMan = bm;
  shotTime=200;
  itemSelectTime=0;

  //Try to open a joystick.
  if(setting.joyEnabled && SDL_NumJoysticks() > 0)
  {
    joystickLeftX = setting.controlMaxSpeed / setting.JoyCalMin;
    joystickRightX = setting.controlMaxSpeed / setting.JoyCalMax;
    joystick = NULL;
    joystick = SDL_JoystickOpen(0);
    if(SDL_JoystickOpened(0))
    {
      cout << "Using joystick: '"<<SDL_JoystickName(0)<<"' as "<<(setting.joyIsDigital ? "digital":"analog")<<"."<<endl;
      SDL_JoystickEventState( SDL_ENABLE );
    } else {
      cout << "Failed to open joystick: '"<<SDL_JoystickName(0)<<"'"<<endl;
    }
  }

  #ifdef WITH_WIIUSE
  var.wiiConnect=0;
  moteAccel = setting.controlMaxSpeed / 90.0;
  #endif

}

controllerClass::~controllerClass()
{
  SDL_JoystickClose(0);

  #ifdef WITH_WIIUSE
  if(var.wiiConnect)
  {
    wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
  }
  #endif
}

void controllerClass::movePaddle(GLfloat px)
{
  if(!var.paused)
  {
    paddle->posx = px;
    if(paddle->posx > 1.66-paddle->width-0.06)
    {
      paddle->posx=1.66-paddle->width-0.06;
    }
    else if(paddle->posx < -1.66+paddle->width+0.06)
    {
      paddle->posx=-1.66+paddle->width+0.06;
    }
  }
}

void controllerClass::btnPress()
{
  if(var.titleScreenShow)
  {
    var.titleScreenShow=0;
    SDL_WarpMouse(var.halfresx,0);
    return;
  }
  
  struct pos p;
  if(shotTime > 150)
  {
    shotTime=0;
    if(!var.menu && !var.paused)
    {
      bMan->unglue();
      var.startedPlaying=1;
      if(player.powerup[PO_GUN])
      {
        p.x = paddle->posx-paddle->width/1.5;
        p.y = paddle->posy;
        bullet->shoot(p);
        p.x = paddle->posx+paddle->width/1.5;
        bullet->shoot(p);
      }
    }
  }
}

bool controllerClass::get()
{
  Uint8 *keyStates;
  Uint8 keyDown[3]; //Need this since its not a good idea to write to keyStates for some reason
  shotTime += globalTicks;
  SDL_PumpEvents();
  keyStates = SDL_GetKeyState( NULL );
  keyDown[0] = keyStates[setting.keyLeft];
  keyDown[1] = keyStates[setting.keyRight];
  keyDown[2] = keyStates[setting.keyShoot];
  
  itemSelectTime += globalTicks;
  //Read joystick here so we can override keypresses if the joystick is digital
  //We shouldn't need to check if the joystick is enabled, since it won't be opened if its not enabled anyway.
  if(setting.joyEnabled && SDL_JoystickOpened(0))
  {
    joystickx = SDL_JoystickGetAxis(joystick, 0);
    joysticky = SDL_JoystickGetAxis(joystick, 1);
    joystickbtnA = SDL_JoystickGetButton(joystick, 0);
    joystickbtnB = SDL_JoystickGetButton(joystick, 1);


    if(joystickbtnA)
    {
      keyDown[2] = 1;
    }
    if(joystickbtnB && itemSelectTime > ITEMSELECTTIME)
    {
      itemSelectTime=0;
      gVar.shopBuyItem=1;
    }

    if(setting.joyIsDigital)
    {
      if(joystickx < -200)
      {
        keyDown[0]=1;
      } else if(joystickx > 200)
      {
        keyDown[1]=1;
      }
      if(joysticky < -200 && itemSelectTime > ITEMSELECTTIME)
      {
        itemSelectTime=0;
        gVar.shopNextItem = 1;
      } else if(joysticky > 200 && itemSelectTime > ITEMSELECTTIME)
      {
        itemSelectTime=0;
        gVar.shopPrevItem = 1;
      }
      
    } else {
      GLfloat x = 0.0; //This is the actual traveling speed of the paddle
      if(joystickx > setting.JoyCalHighJitter)
      {
        x = joystickRightX * joystickx;
      } else if(joystickx < setting.JoyCalLowJitter)
      {
        x = -(joystickLeftX * joystickx);
      }
      
      if(joysticky < setting.JoyCalLowJitter && itemSelectTime > ITEMSELECTTIME)
      {
        itemSelectTime=0;
        gVar.shopNextItem = 1;
      } else if(joysticky > setting.JoyCalHighJitter && itemSelectTime > ITEMSELECTTIME)
      {
        itemSelectTime=0;
        gVar.shopPrevItem = 1;
      }
      //Move the paddle:
      movePaddle( paddle->posx += (x*globalMilliTicks) );
    }
  }

  #ifdef WITH_WIIUSE
  if(var.wiiConnect)
  {
    if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
      switch(wiimotes[0]->event)
      {
        case WIIUSE_EVENT:
          if(IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_TWO))
          {
            keyDown[2]=1;
          } else if(IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_ONE) && itemSelectTime > ITEMSELECTTIME)
          {
            gVar.shopBuyItem = 1;
            itemSelectTime=0;
          }else if(IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_UP) && itemSelectTime > ITEMSELECTTIME)
          {
            itemSelectTime=0;
            gVar.shopPrevItem = 1;
          }else if(IS_PRESSED(wiimotes[0], WIIMOTE_BUTTON_DOWN) && itemSelectTime > ITEMSELECTTIME)
          {
            itemSelectTime=0;
            gVar.shopNextItem = 1;
          } else if(WIIUSE_USING_ACC(wiimotes[0]))
          {
            motePitch = wiimotes[0]->orient.pitch;
            motePitch *=-1;
	  }

        break;
        case WIIUSE_DISCONNECT:
        case WIIUSE_UNEXPECTED_DISCONNECT:
          var.wiiConnect=0;
          cout << "WiiMote disconnected." << endl;
          wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
        break;
      }
    }
    if(motePitch < -0.2 || motePitch > 0.2)
    {
      movePaddle( paddle->posx += ( moteAccel*motePitch)*globalMilliTicks );
    }
  }
  #endif

  //React to keystates here, this way, if joyisdig it will press keys
  if(keyDown[0])
  {
    accel+=globalMilliTicks*setting.controlAccel;
    if(accel > setting.controlMaxSpeed)
      accel=setting.controlMaxSpeed;
    movePaddle( paddle->posx - ( accel*globalMilliTicks) );
  } else if(keyDown[1])
  {
    accel+=globalMilliTicks*setting.controlAccel;
    if(accel > setting.controlMaxSpeed)
      accel=setting.controlMaxSpeed;
    movePaddle( paddle->posx + ( accel*globalMilliTicks) );
  } else {
      accel = setting.controlStartSpeed;
  }

  if(keyDown[2])
  {
    btnPress();
    return(1);
  } else {
    return(0);
  }
}

void controllerClass::calibrate()
{

  Sint16 x=0;
  if(SDL_JoystickOpened(0))
  {

    x = SDL_JoystickGetAxis(joystick, 0);

    if(SDL_JoystickGetButton(joystick, 0))
    {
      if(!joyBtnALock && var.menuJoyCalStage != 5)
      {
        var.menuJoyCalStage++;
        joyBtnALock=1;
      }
    } else {
      joyBtnALock=0;
    }
  }

  switch(var.menuJoyCalStage)
  {
    case 0:
      calMin=0;
      calMax=0;
      calLowJitter=0;
      calHighJitter=0;
    break;
    case 1:
      if(x < calLowJitter)
        calLowJitter=x;
      else if(x > calHighJitter)
        calHighJitter=x;
    break;
    case 2:
      if(x < calMin)
        calMin=x;
    break;
    case 3:
      if(x > calMax)
        calMax=x;
    break;
    case 4:
      setting.JoyCalMin=calMin;
      setting.JoyCalMax=calMax;
      setting.JoyCalLowJitter=calLowJitter;
      setting.JoyCalHighJitter=calHighJitter;
      cout << "Joystick calibration report:" << endl;
      cout << "calMin: " << calMin << endl << "calMax: " << calMax << endl;
      cout << "lowJit: " << calLowJitter << endl << "higJit: " << calHighJitter << endl;
      var.menuJoyCalStage++;
      writeSettings();
    break;
    #ifdef WITH_WIIUSE
    case -1: //We do this to make it draw a frame before freezing (searching)
      var.menuJoyCalStage--;
    break;
    case -2:
      wiimotes =  wiiuse_init(MAX_WIIMOTES);
      if(wiiuse_find(wiimotes, MAX_WIIMOTES, 4))
      {
        var.wiiConnect=1;
      } else {
        var.wiiConnect=0;
      }
      if(var.wiiConnect)
      {
        var.wiiConnect=0;
        if(wiiuse_connect(wiimotes, MAX_WIIMOTES))
        {
          var.wiiConnect=1;
          wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
          wiiuse_rumble(wiimotes[0], 1);
          usleep(500000);
          wiiuse_rumble(wiimotes[0], 0);
          wiiuse_motion_sensing(wiimotes[0], 1);
        }
        var.menuJoyCalStage=-3;
      } else {
        var.menuJoyCalStage=-4;
      }
    break;
    #endif
  }
}

