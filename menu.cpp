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

struct score {
  int score;
  string level;
  string name;
};

struct score *sortScores(int *rl)
{
  ifstream hsList;
  string line;
  int i=0,t;
  int delim[2];

  struct score *final=NULL, *temp=NULL;
  final = new struct score[1];

  hsList.open(privFile.highScoreFile.data());

  if(hsList.is_open())
  {
    while(!hsList.eof())
    {
      getline(hsList,line);

      if(line != "")
      {
        if(line[0]=='[')
        {
          delim[0] = line.find(']');
          delim[1] = line.find('|', delim[0]+1);
          
          final[i].level=line.substr(0, delim[0]+1);
          final[i].level+="  ";
          final[i].score = atoi(line.substr(delim[0]+1, delim[1]).data());
          final[i].name = line.substr(delim[1]+1);
        } else {
          final[i].level.clear();
          final[i].score = atoi(line.substr(0, line.find('|')).data());
          final[i].name = line.substr(line.find('|')+1);
        }

        temp = new struct score[i+1];

        for(t = 0; t < i+1; t++)
        {
          temp[t] = final[t];
        }

        delete[] final;
        final = new struct score[i+2];

        for(t = 0; t < i+1; t++)
        {
          final[t] = temp[t];
        }
        delete[] temp;

        i++;
      }
    }
    hsList.close();
  }

  if(i > 20)
    *rl=20;
  else
    *rl=i;

  int k;
  bool done=0;
  bool swaps=0;

  temp = new struct score[1];
  while(!done)
  {
    for(k=0; k < i; k++)
    {
      for(t=0; t < i; t++)
      {
        if(final[k].score > final[t].score)
        {
          swaps=1;
          temp[0] = final[t];
          final[t] = final[k];
          final[k] = temp[0];
        } else {
          swaps=0;
        }
      }
    }
    if(swaps == 0)
    {
      done=1;
    }
  }
  delete[] temp;
  return(final);
}

class menuClass {
private:
string saveGameName[6]; //The name of saveGames
int saveGameSlot; //Where player choose to save/load to/from
textureClass tex[5];
GLuint dl;

bool themeChanged; //If the theme has changed change the banner.
vector<struct themeInfo> tI; //Vector of theme info

public:
  char highScores[20][255];
  
  menuClass()
  {

    refreshHighScoreList(); //load the highscore file (if exists)
    textureManager texMgr;

    dl = glGenLists(4); //Generate displaylists (+0 = background, +1,2 = but

    texMgr.load(useTheme("/gfx/menu/menu0.png",setting.gfxTheme), tex[0]);
    texMgr.load(useTheme("/gfx/menu/but0.png",setting.gfxTheme), tex[1]);
    texMgr.load(useTheme("/gfx/menu/but1.png",setting.gfxTheme), tex[2]);
    texMgr.load(useTheme("/gfx/menu/but2.png",setting.gfxTheme), tex[3]);
    texMgr.load(useTheme("/gfx/menu/highscorebg.png",setting.gfxTheme), tex[4]);
    
    tI = getThemes(); //Read themes and put them in the vector tI
    listSaveGames(saveGameName);
    
    //Baggrund
    glNewList(dl, GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, tex[0].prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(0.0f,0.0f);glVertex3f( -1.0, 1.0, 0.0 );
        glTexCoord2f(1.0f,0.0f);glVertex3f(  1.0, 1.0, 0.0 );
        glTexCoord2f(1.0f,1.0f);glVertex3f(  1.0,-1.0, 0.0 );
        glTexCoord2f(0.0f,1.0f);glVertex3f( -1.0,-1.0, 0.0 );
      glEnd( );
    glEndList( );


    //Hvide
    glNewList(dl+1, GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, tex[1].prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(0.132f,0.3f); glVertex3f(  -0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.3f); glVertex3f(   0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.7f); glVertex3f(   0.5,-0.07, 0.0 );
        glTexCoord2f(0.132f,0.7f); glVertex3f(  -0.5,-0.07, 0.0 );
      glEnd( );
    glEndList( );

      //Røde
    glNewList(dl+2, GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, tex[2].prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(0.132f,0.3f); glVertex3f(  -0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.3f); glVertex3f(   0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.7f); glVertex3f(   0.5,-0.07, 0.0 );
        glTexCoord2f(0.132f,0.7f); glVertex3f(  -0.5,-0.07, 0.0 );
      glEnd( );
    glEndList( );

    //Grøn
    glNewList(dl+3, GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, tex[3].prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(0.132f,0.3f); glVertex3f(  -0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.3f); glVertex3f(   0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.7f); glVertex3f(   0.5,-0.07, 0.0 );
        glTexCoord2f(0.132f,0.7f); glVertex3f(  -0.5,-0.07, 0.0 );
      glEnd( );
    glEndList( );
  }

  void refreshHighScoreList()
  {
      struct score *final;
      int lines;

      final = sortScores(&lines);

      //Fill out remaining highscore slots (if any)
      for(int t=lines; t < 20; t++)
      {
        switch(t)
        {
          case 0:
          sprintf(highScores[t], "SDL-BALL v " VERSION );
          break;
          case 1:
          sprintf(highScores[t], "-----------------------");
          break;
          case 2:
          sprintf(highScores[t], "Copyleft GPLv3 2008-2014 Jimmy Christensen");
          break;
          case 3:
          sprintf(highScores[t], "Based on Dx-Ball by Michael P. Welch");
          break;
          case 4:
          sprintf(highScores[t], "Based on Megaball by Ed and Al Mackey");
          break;
          case 5:
          sprintf(highScores[t], "-----------------------");
          break;
          case 6:
          sprintf(highScores[t], "Thanks to players, packagers and patchers!");
          break;
	  case 7:
          sprintf(highScores[t], "Greetings to everyone!");
	  break;
          default:
          sprintf(highScores[t], "SDL-Ball - %i", (t*-1)+20 );
          break;
          
        }
      }

     //Highscores list
     for(int t=0; t < lines; t++)
     {
        sprintf(highScores[t], "%s%i - %s", final[t].level.data(), final[t].score, final[t].name.data());
     }
 
     if(lines>0)
      delete[] final;
  }

  void enterSaveGameName(SDL_Event e)
  {
    if(e.key.keysym.sym != SDLK_RETURN && e.key.keysym.sym != SDLK_ESCAPE)
    {
      if ( (e.key.keysym.unicode & 0xFF80) == 0 && e.key.keysym.unicode != 0) {
        if(e.key.keysym.sym == SDLK_BACKSPACE)
        {
          if(saveGameName[saveGameSlot].length() > 0)
            saveGameName[saveGameSlot].erase(saveGameName[saveGameSlot].length()-1);
        } else {
          saveGameName[saveGameSlot] += e.key.keysym.unicode;
        }
      }
    } else {
      if(e.key.keysym.sym == SDLK_RETURN)
      {
        //player saved
        saveGame(saveGameSlot, saveGameName[saveGameSlot]);
      }
      var.enterSaveGameName=0;
    }
  }


  void doMenu()
  {
    int i=0;
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-3.0f);
    glCallList(dl); //baggrunden

    glTranslatef(0.0,-1.0,0.0f);
    
    if(var.menu==1)
    {

      //Exit
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(0,0,0,1);
      glText->write("Quit SDL-Ball", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //Settings
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==2)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(0,0,0,1);
      glText->write("Settings", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
      
      //highscores
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==3)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(0,0,0,1);
      glText->write("Highscores", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
      
      //Save
      glTranslatef(0.0,0.22,0.0f);
      if(player.level > 0)// && !var.startedPlaying)
      {
        if(var.menuItem==4)
          glCallList(dl+2);
        else
          glCallList(dl+1);
  
        glColor4f(0,0,0,1);
        glText->write("Save Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
        glColor4f(1,1,1,1);
      } else {
        glColor4f(1,1,1,1);
        if(var.menuItem==4)
        {
          glCallList(dl+2);
          glColor4f(0.5,0.5,0.5,1);
          glText->write("Not in Level 1", FONT_MENU, 1, 1.0, 0.0, -0.005);
        } else {
          glCallList(dl+1);
          glColor4f(0.5,0.5,0.5,1);
          glText->write("Save Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
        }
        glColor4f(1,1,1,1);
  
      }
      
      
      //Load
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(0,0,0,1);
      glText->write("Load Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //Continue
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(0,0,0,1);
      glText->write("Continue", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //new game
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==7)
        glCallList(dl+2);
      else
        glCallList(dl+3);

      glColor4f(0,0,0,1);
      glText->write("New Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);


      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1: //quit
            var.menu=6;
            break;
          case 2: //settings
            var.menu=2;
            break;
          case 3: //highscores
            var.menu=7;
            break;
          case 4: //Save Game
            if(player.level > 0)// && !var.startedPlaying)
              var.menu=9;
            break;
          case 5: //Load game
            var.menu=8;
            break;
          case 6: //resume
            resumeGame();
            break;
          case 7: //new game
            var.menu=5;
        }
        var.menuPressed=0;
      }
    var.menuNumItems=7;
    } else if(var.menu==2)
    {
      //Settings

      //Back
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //settings
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);

      glCallList(dl+3);

      glColor4f(1,1,1,1);
      glText->write("Settings", FONT_MENU, 1, 1.0, 0.0, -0.005);


      //Video
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("Video", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);


      //sound
      glTranslatef(0.0,-0.22,0.0f);
      #ifndef NOSOUND
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("Sound", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
      #endif
      
      //Calibrate
      glTranslatef(0.0,-0.22,0.0f);
      if(SDL_JoystickOpened(0))
      {
        if(var.menuItem==4)
          glCallList(dl+2);
        else
          glCallList(dl+1);
        glColor4f(0,0,0,1);
        glText->write("Calibrate Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
        glColor4f(1,1,1,1);
      }

      glTranslatef(0.0,-0.22,0.0f);
      #ifdef WITH_WIIUSE
      if(!var.wiiConnect)
      {
        if(var.menuItem==3)
          glCallList(dl+2);
        else
          glCallList(dl+1);
        glColor4f(0,0,0,1);
        glText->write("Connect WiiMote", FONT_MENU, 1, 1.0, 0.0, -0.005);
        glColor4f(1,1,1,1);
      }
      #endif
      
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==2)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("Themes", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);


      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=1;
            break;
          case 6:
            var.menu=3;
            break;
          case 5:
            var.menu=4;
            break;
          case 4:
            if(SDL_JoystickOpened(0))
            {
              var.menuJoyCalStage=0;
              var.menu=10;
            }
            break;

          #ifdef WITH_WIIUSE
          case 3:
          if(!var.wiiConnect)
          {
            var.menuJoyCalStage=-1;
            var.menu=11;
          }
          break;
          #endif
          case 2:
            var.menu=12;
          break;
        }
        var.menuPressed=0;
      }
      var.menuNumItems=6;
    } else if(var.menu==3)
    {
      //Video options
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+3);

      glColor4f(1,1,1,1);
      glText->write("Video Options", FONT_MENU, 1, 1.0, 0.0, -0.005);

      //Toggle full
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      if(setting.fullscreen)
        glText->write("Fullscreen:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else
        glText->write("Fullscreen:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //Eyecandy
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      if(setting.eyeCandy)
        glText->write("Eyecandy:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else
        glText->write("Eyecandy:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //1600
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==4)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("1600x1200", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //1024
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==3)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("1024x768", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
      
      //background
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==2)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      if(setting.showBg)
        glText->write("Background:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else
        glText->write("Background:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //Back
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=2;
            break;
          case 2:
            if(!setting.showBg)
              setting.showBg=1;
              else
              setting.showBg=0;
            writeSettings();
            break;
          case 3:
            setting.resx = 1024;
            setting.resy = 768;
#ifndef WIN32
            if(initScreen())
#endif
              writeSettings();
            break;
          case 4:
            setting.resx = 1600;
            setting.resy = 1200;
#ifndef WIN32
            if(initScreen())
#endif
              writeSettings();
            break;
          case 5:
            if(setting.eyeCandy)
              setting.eyeCandy=0;
              else
              setting.eyeCandy=1;
            writeSettings();
            break;
          case 6:
            if(setting.fullscreen)
              setting.fullscreen=0;
              else
              setting.fullscreen=1;
#ifndef WIN32
            if(initScreen())
#endif
              writeSettings();
            break;
        }
        var.menuPressed=0;
      }
      var.menuNumItems=6;
    } else if(var.menu==4)
    {
      //Audio
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+3);
      glColor4f(1,1,1,1);
      glText->write("Audio", FONT_MENU, 1, 1.0, 0.0, -0.005);


      //Sound on/off
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      if(setting.sound)
        glText->write("Sound:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else
        glText->write("Sound:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
      
      //Stereo
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==5) {
        glCallList(dl+2);
      } else {
        glCallList(dl+1);
        glColor4f(0,0,0,1);
      }
      if(setting.stereo) {
        glText->write("Stereo:On", FONT_MENU, 1, 1.0, 0.0, -0.005);
      } else {
        glText->write("Stereo:Off", FONT_MENU, 1, 1.0, 0.0, -0.005);
      }
      glColor4f(1,1,1,1);

      //Back
      glTranslatef(0.0,-0.88,0.0f);
      if(var.menuItem==1) {
        glCallList(dl+2);
      } else {
        glCallList(dl+1);
      }
      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
      
      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=2;
            break;
          case 5:
            if(setting.stereo)
              setting.stereo=0;
            else
              setting.stereo=1;
            writeSettings();
            break;
          case 6:
            if(setting.sound) {
              setting.sound=0;
            } else {
              setting.sound=1;
              soundMan.loadsounds();
            }
            writeSettings();

            break;

        }
        var.menuPressed=0;
      }

      var.menuNumItems=7;

    } else if(var.menu==5)
    {
      //New game?
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+3);
      
      glColor4f(0,0,0,1);
      glText->write("New Game", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
//       glBindTexture(GL_TEXTURE_2D, tex[28].prop.texture);
//       glCallList(dl+3);

      //Easy
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
        
      glColor4f(0,1,0,1);
      glText->write("Easy", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
//       glBindTexture(GL_TEXTURE_2D, tex[29].prop.texture);
//       glCallList(dl+3);

      //Normal
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
        
      glColor4f(0,0,1,1);
      glText->write("Normal", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

//       glBindTexture(GL_TEXTURE_2D, tex[30].prop.texture);
//       glCallList(dl+3);

      //Hard
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==4)
        glCallList(dl+2);
      else
        glCallList(dl+1);

      glColor4f(1,0,0,1);
      glText->write("Hard", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
//       glBindTexture(GL_TEXTURE_2D, tex[31].prop.texture);
//       glCallList(dl+3);

      //Back
      glTranslatef(0.0,-0.66,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
        
      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

//       glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
//       glCallList(dl+3);

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=1;
            break;
          case 4: //New game, hard
            player.difficulty=HARD;
            writeSettings();
            resumeGame();
            initNewGame();
            break;
          case 5: //New Game, Normal
            player.difficulty=NORMAL;
            writeSettings();
            resumeGame();
            initNewGame();
            break;
          case 6: //New game, easy
            player.difficulty=EASY;
            writeSettings();
            resumeGame();
            initNewGame();
            break;

        }
        var.menuPressed=0;
      }

      var.menuNumItems=7;
    } else if(var.menu==6)
    {
      //Exit
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+2);
      glColor4f(0,0,0,1);
      glText->write("Exit Game?", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //Yes
      glTranslatef(0.0,-0.44,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(1,0,0,1);
      glText->write("Yes.", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      //Noes!
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==4)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,1,0,1);
      glText->write("No way!", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 4: //no
            var.menu=1;
            break;
          case 5: //Yes
            var.quit=1;
            break;

        }
        var.menuPressed=0;
      }
      var.menuNumItems=5;

    } else if(var.menu==7) // highscores
    {
      //Highscores
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      if(var.menuItem==7)
        glCallList(dl+2);
        else
        glCallList(dl+3);

      glColor4f(1,1,1,1);
      glText->write("Highscores", FONT_MENU, 1, 1.0, 0.0, -0.005);

      glTranslatef(0.0, -0.75, 0.0f);
      glBindTexture(GL_TEXTURE_2D, tex[4].prop.texture);
      glEnable(GL_TEXTURE_2D);
      glColor4f(1.0,1.0,1.0,1.0);
      glBegin( GL_QUADS );
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(  -0.8, 0.63, 0.0 );
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(   0.8, 0.63, 0.0 );
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(   0.8,-0.63, 0.0 );
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(  -0.8,-0.63, 0.0 );
      glEnd( );

      glLoadIdentity();
      glTranslatef(0.0, 0.435,-3.0f);

      //Find out how many lines we have room for in the box.
      int nl= (1.26/glText->getHeight(FONT_MENUHIGHSCORE));
      //If theres room for more than 20, only show 20..
      if (nl > 20) {
        nl = 20;
      }

      for(int i=0; i < nl; i++)
      {
        glTranslatef(0.0, -glText->getHeight(FONT_MENUHIGHSCORE),0.0f);
        glText->write(highScores[i], FONT_MENUHIGHSCORE, 0, 1.0, -0.75, 0.0);
      }


      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 7: 
            var.menu=1;
            break;
        }
        var.menuPressed=0;
      }
    } else if(var.menu==8)
    {
      //Load game
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      if(var.menuItem==8)
        glCallList(dl+2);
      else
        glCallList(dl+3);
      glColor4f(1,1,1,1);
      glText->write("Load Game", FONT_MENU, 1, 1.0, 0.0, -0.005);

      for(int i=0; i < 6; i++)
      {
          glTranslatef(0.0,-0.22,0.0f);
          if(var.menuItem==6-i)
            glCallList(dl+2);
          else
            glCallList(dl+1);
          glColor4f(0,0,0,1);
          glText->write(saveGameName[i], FONT_MENU, 1, 1.0, 0.0, -0.005);
          glColor4f(1,1,1,1);
          
      }

      if(var.menuPressed)
      {
        if(var.menuItem==7)
        {
            var.menu=1;
        } else if(var.menuItem !=0)
        {
          loadGame(var.menuItem*-1 +6);
          var.menu=0;
          resumeGame();

        }
        var.menuPressed=0;
      }

    } else if(var.menu == 9)
    {
      //Save game
      //Back
      
      if(var.enterSaveGameName)
      {
        glLoadIdentity();
        glTranslatef(0.0, 0.54,-3.0f);
        glTranslatef(0.0, 0.12,0.0f);

        glColor4f(0,0,0,1);
        glText->write("Enter name and press Enter to save. ESC to cancel", FONT_MENU, 1, 1.0, 0.0, -0.005);
        glColor4f(1,0,0,1);
      }
      
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      
      if(var.menuItem==8)
        glCallList(dl+2);
      else
        glCallList(dl+3);
      glColor4f(1,1,1,1);
      glText->write("Save Game", FONT_MENU, 1, 1.0, 0.0, -0.005);


      for(int i=0; i < 6; i++)
      {
          glTranslatef(0.0,-0.22,0.0f);
          if(var.menuItem==6-i)
            glCallList(dl+2);
          else
            glCallList(dl+1);
          glColor4f(0,0,0,1);
          glText->write(saveGameName[i], FONT_MENU, 1, 1.0, 0.0, -0.005);
          glColor4f(1,1,1,1);

      }

      if(var.menuPressed)
      {
        if(var.menuItem==7)
        {
            var.menu=1;
        } else if(var.menuItem !=0)
        {
          var.enterSaveGameName = 1;
          saveGameSlot = (var.menuItem*-1 +6);
          saveGameName[saveGameSlot]="";
        }
          var.menuPressed=0;
      }

    } else if(var.menu == 10)
    {
      //Joystick options
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+3);
      glColor4f(1,1,1,1);
      glText->write("Calibrate Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);

      
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      if(setting.joyIsDigital)
        glText->write("Digital Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else
        glText->write("Analog Joystick", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);


      glTranslatef(0.0,-0.22,0.0f);
      if(!setting.joyIsDigital)
      {
        if(var.menuItem==5)
          glCallList(dl+2);
        else
          glCallList(dl+1);
        
        glColor4f(0,0,0,1);
        if(var.menuJoyCalStage==0)
          glText->write("Start Calibration", FONT_MENU, 1, 1.0, 0.0, -0.005);
        else if(var.menuJoyCalStage==1)
          glText->write("Center, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
        else if(var.menuJoyCalStage==2)
          glText->write("Move Left, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
        else if(var.menuJoyCalStage==3)
          glText->write("Move Right, Push", FONT_MENU, 1, 1.0, 0.0, -0.005);
        else if(var.menuJoyCalStage==4)
          glText->write("Saving...", FONT_MENU, 1, 1.0, 0.0, -0.005);
        else if(var.menuJoyCalStage==5)
          glText->write("Finished.", FONT_MENU, 1, 1.0, 0.0, -0.005);
        glColor4f(1,1,1,1);
      }

      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.22,0.0f);

      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=2;
            break;
          case 2:
            break;
          case 3:
            break;
          case 4:
            break;
          case 5:
            if(!var.menuJoyCalStage)
              var.menuJoyCalStage=1;
            break;
          case 6:
            if(setting.joyIsDigital)
              setting.joyIsDigital=0;
            else
              setting.joyIsDigital=1;

            writeSettings();
            break;
        }
        var.menuPressed=0;
      }
      var.menuNumItems=6;
    }
    #ifdef WITH_WIIUSE
    else if(var.menu == 11) //Wiimote
    {

      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+3);
      glColor4f(1,1,1,1);
      glText->write("Connect WiiMote", FONT_MENU, 1, 1.0, 0.0, -0.005);

      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6 && !var.wiiConnect)
        glCallList(dl+2);
      else
        glCallList(dl+1);
        
      glColor4f(0,0,0,1);
      if(var.menuJoyCalStage==-2)
        glText->write("1+2 on WiiMote", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else if(var.menuJoyCalStage==-3)
        glText->write("WiiMote Connected!", FONT_MENU, 1, 1.0, 0.0, -0.005);
      else if(var.menuJoyCalStage==-4)
        glText->write("Failed! Try again.", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);


      glCallList(dl+3);

      
      glTranslatef(0.0,-1.0,0.0);

      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
        
      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {

          case 6:
          if(!var.wiiConnect)
          {
            var.menu=11;
            var.menuJoyCalStage=-1;
          }
          break;

          case 1:
            var.menu=2;
            break;
        }
        var.menuPressed=0;
      }
      var.menuNumItems=6;
    }
    #endif
    else if(var.menu == 12) //Theme selector (Main screen)
    {
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+3);

      if(!themeChanged)
      {
        glColor4f(0,0,0,1);
        glText->write("Themes", FONT_MENU, 1, 1.0, 0.0, -0.005);
      } else {
        glColor4f(1,0,0,1);
        glText->write("Restart to apply", FONT_MENU, 1, 1.0, 0.0, -0.005);
      }
      glColor4f(1,1,1,1);
 
      int i=0;
      for(vector<struct themeInfo>::iterator it=tI.begin(); it < tI.end(); ++it)
      {
          if(i ==5)
            break;
          glTranslatef(0.0,-0.22,0.0f);
          if(var.menuItem==6-i)
            glCallList(dl+2);
          else if(it->name.compare(setting.gfxTheme)==0)
            glCallList(dl+3);
          else
            glCallList(dl+1);
          glColor4f(0,0,0,1);
          glText->write(it->name, FONT_MENU, 1, 1.0, 0.0, -0.005);
          glColor4f(1,1,1,1);
          i++;
      }
      
      for(int a=i; a < 6; a++)
      {
          glTranslatef(0.0,-0.22,0.0f);
      }

      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glColor4f(0,0,0,1);
      glText->write("Back", FONT_MENU, 1, 1.0, 0.0, -0.005);
      glColor4f(1,1,1,1);
 
    }

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=2;
            break;
        }
        
        for(i=0; i < (int)tI.size() && i<5; i++)
        {
          if(6-i == var.menuItem)
          {
            setting.gfxTheme = tI.at(i).name;
            setting.sndTheme = tI.at(i).name;
            setting.lvlTheme = tI.at(i).name;
            themeChanged=1;
            writeSettings();
          }
        }
        var.menuPressed=0;
      }
    }
};

