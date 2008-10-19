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
  string name;
};

struct score *sortScores(int *rl)
{
  ifstream hsList;
  string line;
  int i=0,t;

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
        final[i].score = atoi(line.substr(0, line.find('|')).data());
        final[i].name = line.substr(line.find('|')+1);

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
GLuint texSaveGameSlot[6];
string saveGameName;
int saveGameSlot;
GLuint texHighScore[20];
textureClass tex[65];
GLuint dl;

bool themeChanged; //If the theme has changed change the banner.
int themesOffset; //If there is more than 5 themes, we need to scoll...
void generateThemeTextures(); //When this is called, the list of themes is read, and put into textures
vector<struct themeInfo> tI; //Vector of theme info
GLuint *themeTextureIndex; //Pointer to array of theme textures

public:
  GLuint titleHighscoreTex[20];
  int numHighScores;
  menuClass()
  {
    glGenTextures(20, &texHighScore[0]);
    glGenTextures(10, &titleHighscoreTex[0]);
    glGenTextures(6, &texSaveGameSlot[0]);
    genHsTex(); //load the highscore file (if exists) and print to texture
    textureManager texMgr;

    dl = glGenLists(5); //Generate displaylists


    SDL_Color txtColorRed = {255,0,0};
    SDL_Color txtColorGreen = {0,255,0};
    SDL_Color txtColorBlue = {0,0,255};
    SDL_Color txtColorWhite = {255,255,255};
    SDL_Color txtColorBlack = {0,0,0};

    texMgr.load(useTheme("/gfx/menu/menu0.png",setting.gfxTheme), tex[0]);
    texMgr.load(useTheme("/gfx/menu/but0.png",setting.gfxTheme), tex[1]);
    texMgr.load(useTheme("/gfx/menu/but1.png",setting.gfxTheme), tex[2]);
    texMgr.load(useTheme("/gfx/menu/but2.png",setting.gfxTheme), tex[3]);


    glGenTextures(1, &tex[4].prop.texture);
    glGenTextures(1, &tex[5].prop.texture);
    glGenTextures(1, &tex[6].prop.texture);
    glGenTextures(1, &tex[7].prop.texture);
    glGenTextures(1, &tex[8].prop.texture);
    glGenTextures(1, &tex[9].prop.texture);
    glGenTextures(1, &tex[10].prop.texture);
    glGenTextures(1, &tex[11].prop.texture);
    glGenTextures(1, &tex[12].prop.texture);
    glGenTextures(1, &tex[13].prop.texture);
    glGenTextures(1, &tex[14].prop.texture);
    glGenTextures(1, &tex[15].prop.texture);
    glGenTextures(1, &tex[16].prop.texture);
    glGenTextures(1, &tex[17].prop.texture);
    glGenTextures(1, &tex[18].prop.texture);
    glGenTextures(1, &tex[19].prop.texture);
    glGenTextures(1, &tex[20].prop.texture);
    glGenTextures(1, &tex[21].prop.texture);
    glGenTextures(1, &tex[22].prop.texture);
    glGenTextures(1, &tex[23].prop.texture);
    glGenTextures(1, &tex[24].prop.texture);
    glGenTextures(1, &tex[25].prop.texture);
    glGenTextures(1, &tex[26].prop.texture);
    glGenTextures(1, &tex[27].prop.texture);
    glGenTextures(1, &tex[28].prop.texture);
    glGenTextures(1, &tex[29].prop.texture);
    glGenTextures(1, &tex[30].prop.texture);
    glGenTextures(1, &tex[31].prop.texture);
    glGenTextures(1, &tex[32].prop.texture);
    texMgr.load(useTheme("/gfx/menu/highscorebg.png",setting.gfxTheme), tex[33]);
    glGenTextures(1, &tex[34].prop.texture);
    glGenTextures(1, &tex[35].prop.texture);

    //More
    glGenTextures(1, &tex[36].prop.texture);
    glGenTextures(1, &tex[37].prop.texture);
    glGenTextures(1, &tex[38].prop.texture);
    glGenTextures(1, &tex[39].prop.texture);
    glGenTextures(1, &tex[40].prop.texture);
    glGenTextures(1, &tex[41].prop.texture);
    glGenTextures(1, &tex[42].prop.texture);
    glGenTextures(1, &tex[43].prop.texture);
    glGenTextures(1, &tex[44].prop.texture);
    glGenTextures(1, &tex[45].prop.texture);
    glGenTextures(1, &tex[46].prop.texture);
    glGenTextures(1, &tex[47].prop.texture);
    glGenTextures(1, &tex[48].prop.texture);
    glGenTextures(1, &tex[49].prop.texture);
    glGenTextures(1, &tex[50].prop.texture);
    glGenTextures(1, &tex[51].prop.texture);
    glGenTextures(1, &tex[52].prop.texture);
    glGenTextures(1, &tex[53].prop.texture);
    glGenTextures(1, &tex[54].prop.texture);
    glGenTextures(1, &tex[55].prop.texture);
    
    glGenTextures(1, &tex[56].prop.texture); // "Themes"
    glGenTextures(1, &tex[57].prop.texture); 
    glGenTextures(1, &tex[58].prop.texture); 
    glGenTextures(1, &tex[59].prop.texture); 


    writeTxt(fonts[0],txtColorBlack,"Exit Game", tex[4].prop.texture,1); //0
    writeTxt(fonts[0],txtColorBlack,"Settings",  tex[5].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Highscores",tex[6].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Save Game", tex[7].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Load Game", tex[8].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Continue",  tex[9].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"New Game",  tex[10].prop.texture,1);

    writeTxt(fonts[0],txtColorRed,  "Exit Game?",tex[11].prop.texture,1); //7
    writeTxt(fonts[0],txtColorBlack,"Yes",    tex[12].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"No",     tex[13].prop.texture,1);

    writeTxt(fonts[0],txtColorWhite,"Settings", tex[14].prop.texture,1); //10
    writeTxt(fonts[0],txtColorBlack,"Video Settings",tex[15].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Sound Settings",tex[16].prop.texture,1);

    writeTxt(fonts[0],txtColorWhite,"Video Settings",tex[17].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Fullscreen: On",tex[18].prop.texture,1); //14
    writeTxt(fonts[0],txtColorBlack,"Fullscreen: Off",tex[40].prop.texture,1); //14

    writeTxt(fonts[0],txtColorBlack,"Eyecandy: On",tex[19].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Eyecandy: Off",tex[39].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"1600x1200", tex[20].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"1024x768", tex[21].prop.texture,1); //18
    writeTxt(fonts[0],txtColorBlack,"Background: On", tex[22].prop.texture,1); //18
    writeTxt(fonts[0],txtColorBlack,"Background: Off", tex[38].prop.texture,1); //18



    writeTxt(fonts[0],txtColorWhite,"Sound Settings", tex[23].prop.texture,1); //19
    writeTxt(fonts[0],txtColorBlack,"Sound: On", tex[24].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Sound: Off", tex[25].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Stereo: On", tex[36].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Stereo: Off", tex[37].prop.texture,1);

    writeTxt(fonts[0],txtColorWhite,"New Game", tex[28].prop.texture,1);
    writeTxt(fonts[0],txtColorGreen,"Easy", tex[29].prop.texture,1);
    writeTxt(fonts[0],txtColorBlue, "Normal", tex[30].prop.texture,1);

    writeTxt(fonts[0],txtColorBlack,"Back", tex[32].prop.texture,1); //28

    writeTxt(fonts[0],txtColorBlack,"Calibrate Joystick", tex[41].prop.texture,1);
    writeTxt(fonts[0],txtColorWhite,"Calibrate Joystick", tex[42].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Digital Joystick", tex[43].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Analog Joystick", tex[44].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Start Calibration", tex[45].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"CenterStick", tex[46].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Move Left", tex[47].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Move Right", tex[48].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Saving...", tex[49].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Finished.", tex[50].prop.texture,1);

    writeTxt(fonts[0],txtColorWhite,"Connect WiiMote", tex[51].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Connect WiiMote", tex[52].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"1+2 on WiiMote", tex[53].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Wiimote Connected!", tex[54].prop.texture,1);
    writeTxt(fonts[0],txtColorBlack,"Failed. Try again.", tex[55].prop.texture,1);

    writeTxt(fonts[0],txtColorBlack,"Themes", tex[56].prop.texture,1);
    writeTxt(fonts[0],txtColorWhite,"Restart to apply", tex[57].prop.texture,1);
    
    generateThemeTextures();
    
    refreshLoadList(-1);

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

  glNewList(dl+3, GL_COMPILE);
      glBegin( GL_QUADS );
        glTexCoord2f(0.0f,-0.02f); glVertex3f(  -0.5, 0.07, 0.0 );
        glTexCoord2f(1.0f,-0.02f); glVertex3f(   0.5, 0.07, 0.0 );
        glTexCoord2f(1.0f,0.13f); glVertex3f(   0.5,-0.07, 0.0 );
        glTexCoord2f(0.0f,0.13f); glVertex3f(  -0.5,-0.07, 0.0 );
      glEnd( );
    glEndList( );

    //Grøn
    glNewList(dl+4, GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, tex[3].prop.texture);
      glBegin( GL_QUADS );
        glTexCoord2f(0.132f,0.3f); glVertex3f(  -0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.3f); glVertex3f(   0.5, 0.07, 0.0 );
        glTexCoord2f(0.87f, 0.7f); glVertex3f(   0.5,-0.07, 0.0 );
        glTexCoord2f(0.132f,0.7f); glVertex3f(  -0.5,-0.07, 0.0 );
      glEnd( );
    glEndList( );
  }

  void genHsTex()
  {
    SDL_Color color = { 255,255,255 };
    char txt[128];

      struct score *final;
      int lines;

      final = sortScores(&lines);


      //Fill out remaining highscore slots (if any)
      for(int t=lines; t < 20; t++)
      {
          sprintf(txt, "SDL-Ball - %i", (t*-1)+20 );
          writeTxt(fonts[3], color, txt, texHighScore[t],0);
      }
      //Intro screen
      numHighScores=0;
      for(int t=lines; t < 7; t++)
      {
        numHighScores++;
        switch(t)
        {
          case 0:
          sprintf(txt, "SDL-BALL v "VERSION );
          break;
          case 1:
          sprintf(txt, "-----------------------");
          break;
          case 2:
          sprintf(txt, "Copyleft GPLv3 2008 Jimmy Christensen");
          break;
          case 3:
          sprintf(txt, "Based on Dx-Ball by Michael P. Welch");
          break;
          case 4:
          sprintf(txt, "based on Megaball by ED and AL MACKEY");
          break;
          case 5:
          sprintf(txt, "-----------------------");
          break;
          case 6:
          sprintf(txt, "Greetings to everyone!");
          break;
          
        }
          writeTxt(fonts[3], color, txt, titleHighscoreTex[t],1);
      }

     //Highscores list
     for(int t=0; t < lines; t++)
     {
        sprintf(txt, "%i - %s", final[t].score, final[t].name.data());
        writeTxt(fonts[3], color, txt, texHighScore[t],0);
     }
     
     //Intro screen
      for(int t=0; t < lines && t<10; t++)
      {
        numHighScores++;
        sprintf(txt, "%i. %s  %i", t+1,final[t].name.data(),final[t].score);
        writeTxt(fonts[3], color, txt, titleHighscoreTex[t],1);
      }

     if(lines>0)
      delete[] final;
  }

  void refreshLoadList(int slot)
  {
    SDL_Color txtColorBlack = {0,0,0};
    string slotName[6];
    listSaveGames(slotName);
    if(slot == -1)
    {
      for(int i=0; i < 6; i++)
      {
        writeTxt(fonts[0],txtColorBlack,slotName[i].data(), texSaveGameSlot[i],1);
      }
    } else {
        writeTxt(fonts[0],txtColorBlack,slotName[slot].data(), texSaveGameSlot[slot],1);
    }
  }
  void enterSaveGameName(SDL_Event e)
  {
    if(e.key.keysym.sym != SDLK_RETURN && e.key.keysym.sym != SDLK_ESCAPE)
    {
      if ( (e.key.keysym.unicode & 0xFF80) == 0 && e.key.keysym.unicode != 0) {
        if(e.key.keysym.sym == SDLK_BACKSPACE)
        {
          if(saveGameName.length() > 0)
            saveGameName.erase(saveGameName.length()-1);
        } else {
          saveGameName += e.key.keysym.unicode;
        }

        SDL_Color color = { 255,255,255 };
        if(saveGameName.length() < 1)
          writeTxt(fonts[0], color, " ", texSaveGameSlot[saveGameSlot],1);
        else
          writeTxt(fonts[0], color, saveGameName.data(), texSaveGameSlot[saveGameSlot],1);
      }
    } else {
      if(e.key.keysym.sym == SDLK_RETURN)
      {
        //player saved
        saveGame(saveGameSlot, saveGameName);
      }
      refreshLoadList(saveGameSlot);
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

      for(i=0; i < 6; i++)
      {
        glTranslatef(0.0,0.22,0.0f);

        if( (i == 3 && player.level > 0 && !var.startedPlaying) || i!=3)
        {
          if(var.menuItem-1==i)
          {
            glCallList(dl+2);
          }
          else
          {
            glCallList(dl+1);
          }
          glBindTexture(GL_TEXTURE_2D, tex[i+4].prop.texture);
          glCallList(dl+3);

        }

      }

      //new game for sigselv
      glTranslatef(0.0,0.22,0.0f);
      if(var.menuItem==7)
        glCallList(dl+2);
      else
        glCallList(dl+4);

      glBindTexture(GL_TEXTURE_2D, tex[10].prop.texture);
      glCallList(dl+3);


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
            if(player.level > 0 && !var.startedPlaying)
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

      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);

      //settings
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);

      glCallList(dl+4);

      glBindTexture(GL_TEXTURE_2D, tex[14].prop.texture);
      glCallList(dl+3);

      //Video
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[15].prop.texture);
      glCallList(dl+3);

      //sound
      glTranslatef(0.0,-0.22,0.0f);
      #ifdef WITH_SOUND
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[16].prop.texture);
      glCallList(dl+3);
      #endif
      
      //Calibrate
      glTranslatef(0.0,-0.22,0.0f);
      if(SDL_JoystickOpened(0))
      {
        if(var.menuItem==4)
          glCallList(dl+2);
        else
          glCallList(dl+1);
        glBindTexture(GL_TEXTURE_2D, tex[41].prop.texture);
        glCallList(dl+3);
      }

      glTranslatef(0.0,-0.22,0.0f);
      #ifdef WITH_WIIUSE
      if(!var.wiiConnect)
      {
        if(var.menuItem==3)
          glCallList(dl+2);
        else
          glCallList(dl+1);
        glBindTexture(GL_TEXTURE_2D, tex[52].prop.texture);
        glCallList(dl+3);
      }
      #endif
      
        glTranslatef(0.0,-0.22,0.0f);
        if(var.menuItem==2)
          glCallList(dl+2);
        else
          glCallList(dl+1);
        glBindTexture(GL_TEXTURE_2D, tex[56].prop.texture);
        glCallList(dl+3);

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
      glCallList(dl+4);

      glBindTexture(GL_TEXTURE_2D, tex[17].prop.texture);
      glCallList(dl+3);

      //Toggle full
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(setting.fullscreen)
        glBindTexture(GL_TEXTURE_2D, tex[18].prop.texture);
      else
        glBindTexture(GL_TEXTURE_2D, tex[40].prop.texture);
      glCallList(dl+3);

      //Eyecandy
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(setting.eyeCandy)
        glBindTexture(GL_TEXTURE_2D, tex[19].prop.texture);
      else
        glBindTexture(GL_TEXTURE_2D, tex[39].prop.texture);
      glCallList(dl+3);

      //1600
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==4)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[20].prop.texture);
      glCallList(dl+3);

      //1024
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==3)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[21].prop.texture);
      glCallList(dl+3);

      //background
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==2)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(setting.showBg)
        glBindTexture(GL_TEXTURE_2D, tex[22].prop.texture);
      else
        glBindTexture(GL_TEXTURE_2D, tex[38].prop.texture);
      glCallList(dl+3);

      //Back
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);

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
            initScreen();
#endif
            writeSettings();
            break;
          case 4:
            setting.resx = 1600;
            setting.resy = 1200;
#ifndef WIN32
            initScreen();
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
            initScreen();
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
      glCallList(dl+4);
      glBindTexture(GL_TEXTURE_2D, tex[23].prop.texture);
      glCallList(dl+3);

      //Sound on/off
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(setting.sound)
        glBindTexture(GL_TEXTURE_2D, tex[24].prop.texture);
      else
        glBindTexture(GL_TEXTURE_2D, tex[25].prop.texture);
      glCallList(dl+3);

      //Stereo
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(setting.stereo)
        glBindTexture(GL_TEXTURE_2D, tex[36].prop.texture);
      else
        glBindTexture(GL_TEXTURE_2D, tex[37].prop.texture);
      glCallList(dl+3);


      //Back
      glTranslatef(0.0,-0.88,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);

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
            if(setting.sound)
              setting.sound=0;
            else
              setting.sound=1;
              soundMan.loadsounds();
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
      glCallList(dl+4);
      glBindTexture(GL_TEXTURE_2D, tex[28].prop.texture);
      glCallList(dl+3);

      //Easy
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[29].prop.texture);
      glCallList(dl+3);

      //Normal
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[30].prop.texture);
      glCallList(dl+3);

      //Back
      glTranslatef(0.0,-0.88,0.0f);
      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 1:
            var.menu=1;
            break;
          case 5: //New Game, Normal
            player.difficulty=1;
            resumeGame();
            initNewGame();
            break;
          case 6: //New game, easy
            player.difficulty=0;
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
      glBindTexture(GL_TEXTURE_2D, tex[11].prop.texture);
      glCallList(dl+3);

      //Yes
      glTranslatef(0.0,-0.44,0.0f);
      if(var.menuItem==5)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[12].prop.texture);
      glCallList(dl+3);

      //Noes!
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==4)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[13].prop.texture);
      glCallList(dl+3);

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
        glCallList(dl+4);

      glBindTexture(GL_TEXTURE_2D, tex[6].prop.texture);
      glCallList(dl+3);
      int i;

      glTranslatef(0.0, -0.75, 0.0f);
      glBindTexture(GL_TEXTURE_2D, tex[33].prop.texture);
      glEnable(GL_TEXTURE_2D);
      glColor4f(1.0,1.0,1.0,1.0);
      glBegin( GL_QUADS );
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(  -0.8, 0.63, 0.0 );
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(   0.8, 0.63, 0.0 );
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(   0.8,-0.63, 0.0 );
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(  -0.8,-0.63, 0.0 );
      glEnd( );

      glLoadIdentity();
      glTranslatef(0.0, 0.43,-3.0f);

      for(i=0; i < 20; i++)
      {
        glTranslatef(0.0, -0.06, 0.0);
        glEnable(GL_TEXTURE_2D);
        glColor4f(1.0,1.0,1.0,1.0);
        glBindTexture(GL_TEXTURE_2D, texHighScore[i]);
        glBegin( GL_QUADS );
          glTexCoord2f(-0.01f, 0.0f); glVertex3f(  -0.78, 0.03, 0.0 );
          glTexCoord2f(0.99f,0.0f); glVertex3f(   0.78, 0.03, 0.0 );
          glTexCoord2f(0.99f,0.042); glVertex3f(   0.78,-0.03, 0.0 );
          glTexCoord2f(-0.01f,0.042); glVertex3f(  -0.78,-0.03, 0.0 );
        glEnd( );

       }

      if(var.menuPressed)
      {
        switch(var.menuItem)
        {
          case 7: //Yes
            var.menu=1;
            break;
        }
        var.menuPressed=0;
      }
    } else if(var.menu==8)
    {
      //Load game

      //Back
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      if(var.menuItem==8)
        glCallList(dl+2);
      else
        glCallList(dl+4);
      glBindTexture(GL_TEXTURE_2D, tex[8].prop.texture);
      glCallList(dl+3);
      for(int i=0; i < 6; i++)
      {
          glTranslatef(0.0,-0.22,0.0f);
          if(var.menuItem==6-i)
            glCallList(dl+2);
          else
            glCallList(dl+1);
          glBindTexture(GL_TEXTURE_2D, texSaveGameSlot[i]);
          glCallList(dl+3);
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
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      if(var.menuItem==8)
        glCallList(dl+2);
      else
        glCallList(dl+4);
      glBindTexture(GL_TEXTURE_2D, tex[7].prop.texture);
      glCallList(dl+3);

      for(int i=0; i < 6; i++)
      {
          glTranslatef(0.0,-0.22,0.0f);
          if(var.menuItem==6-i)
            glCallList(dl+2);
          else
            glCallList(dl+1);
          glBindTexture(GL_TEXTURE_2D, texSaveGameSlot[i]);
          glCallList(dl+3);
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
          SDL_Color color = { 255,255,255 };
          writeTxt(fonts[0], color, " ", texSaveGameSlot[saveGameSlot],1);
        }
        var.menuPressed=0;
      }

    } else if(var.menu == 10)
    {
      //Joystick options
      glLoadIdentity();
      glTranslatef(0.0, 0.54,-3.0f);
      glCallList(dl+4);

      glBindTexture(GL_TEXTURE_2D, tex[41].prop.texture);
      glCallList(dl+3);

      //Toggle full
      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(setting.joyIsDigital)
        glBindTexture(GL_TEXTURE_2D, tex[43].prop.texture);
      else
        glBindTexture(GL_TEXTURE_2D, tex[44].prop.texture);
      glCallList(dl+3);

      //Eyecandy
      glTranslatef(0.0,-0.22,0.0f);
      if(!setting.joyIsDigital)
      {
        if(var.menuItem==5)
          glCallList(dl+2);
        else
          glCallList(dl+1);

        if(var.menuJoyCalStage==0)
          glBindTexture(GL_TEXTURE_2D, tex[45].prop.texture);
          else if(var.menuJoyCalStage==1)
          glBindTexture(GL_TEXTURE_2D, tex[46].prop.texture);
          else if(var.menuJoyCalStage==2)
          glBindTexture(GL_TEXTURE_2D, tex[47].prop.texture);
          else if(var.menuJoyCalStage==3)
          glBindTexture(GL_TEXTURE_2D, tex[48].prop.texture);
          else if(var.menuJoyCalStage==4)
          glBindTexture(GL_TEXTURE_2D, tex[49].prop.texture);
          else if(var.menuJoyCalStage==5)
          glBindTexture(GL_TEXTURE_2D, tex[50].prop.texture);

        glCallList(dl+3);
      }

      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.22,0.0f);

      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);

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
      glCallList(dl+4);

      glBindTexture(GL_TEXTURE_2D, tex[51].prop.texture);
      glCallList(dl+3);

      glTranslatef(0.0,-0.22,0.0f);
      if(var.menuItem==6 && !var.wiiConnect)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      if(var.menuJoyCalStage==-2)
        glBindTexture(GL_TEXTURE_2D, tex[53].prop.texture);
      else if(var.menuJoyCalStage==-3)
        glBindTexture(GL_TEXTURE_2D, tex[54].prop.texture);
      else if(var.menuJoyCalStage==-4)
        glBindTexture(GL_TEXTURE_2D, tex[55].prop.texture);
      glCallList(dl+3);


        glCallList(dl+3);

      glTranslatef(0.0,-0.22,0.0f);
      glTranslatef(0.0,-0.88,0.0f);

      if(var.menuItem==1)
        glCallList(dl+2);
      else
        glCallList(dl+1);
      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);

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
      glCallList(dl+4);

      if(!themeChanged)
      glBindTexture(GL_TEXTURE_2D, tex[56].prop.texture);
      else
      glBindTexture(GL_TEXTURE_2D, tex[57].prop.texture);
      glCallList(dl+3);
      
      int i=0;
      for(vector<struct themeInfo>::iterator it=tI.begin(); it < tI.end(); ++it)
      {
          if(i ==5)
            break;
          glTranslatef(0.0,-0.22,0.0f);
          if(var.menuItem==6-i)
            glCallList(dl+2);
          else if(it->name.compare(setting.gfxTheme)==0)
            glCallList(dl+4);
          else
            glCallList(dl+1);
          glBindTexture(GL_TEXTURE_2D, themeTextureIndex[i]);
          glCallList(dl+3);
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
      glBindTexture(GL_TEXTURE_2D, tex[32].prop.texture);
      glCallList(dl+3);
 
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

void menuClass::generateThemeTextures()
{
  tI = getThemes();
  const int numThemes = tI.size();
  SDL_Color txtColorRed = {255,0,0};
  SDL_Color txtColorBlack = {0,0,0};
  char txt[64];
  int i=0;
  themeTextureIndex = new GLuint[numThemes];
  glGenTextures(numThemes, themeTextureIndex);

  for(vector<struct themeInfo>::iterator it = tI.begin(); it < tI.end(); ++it)
  {
    
    sprintf(txt, "%s", it->name.data());
    if(it->valid)
    {
      writeTxt(fonts[0],txtColorBlack,txt, themeTextureIndex[i],1);
    } else {
      writeTxt(fonts[0],txtColorRed,txt, themeTextureIndex[i],1);
    }
    i++;
  }
}
