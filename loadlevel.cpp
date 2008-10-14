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

class powerupLoaderClass {
  public:
  //most common
  string chances[4];
  int powerups;
  int powerupsGiven;
  string evilPowerups;

  powerupLoaderClass() {
    int chance=-1;
    powerups=0;
    powerupsGiven=0;
    string line;
    
    //Which powerups are evil?
    evilPowerups = "2HE37"; //P
    
    ifstream powerFile(DATADIR"/powerups.txt");
    if(!powerFile.is_open())
    {
      cout << " Could not open '"<<DATADIR<<"/powerups.txt'"<<endl;
      var.quit=1;
      return;
    }
      
    while(!powerFile.eof())
    {
      getline(powerFile, line);
      if(line.compare("Most:")==0)
      {
        chance=0;
      } else if(line.compare("More:")==0)
      {
        chance=1;
      } else if(line.compare("Less:")==0)
      {
        chance=2;
      } else if(line.compare("Least:")==0)
      {
        chance=3;
      } else {
        //No matches, either its a comment or a powerup.
        if(!line.compare(0,2,"//")==0 && line.compare("")!=0)
        {
          chances[chance] += line[0];
          powerups++;
        }
      }

    }
  }

  char selectRandomPowerup()
  {
    int i = rand()%100;
    powerupsGiven++;
    if(i < 1) //1% of powerups are from this class
    {
      i=rand()%chances[3].length();
      return(chances[3][i]);
    } else if(i < 11) //10% of powerups are from this class
    {
      i=rand()%chances[2].length();
      return(chances[2][i]);
    } else if(i < 41) //30% of powerups are from this class
    {
      i=rand()%chances[1].length();
      return(chances[1][i]);
    } else { //Rest of the powerups are from this class
      i=rand()%chances[0].length();
      return(chances[0][i]);
    }
  }

  char randomPowerup(char p)
  {
    int i;
    //First, decide if we are going to put a powerup in the brick. (based on p)
    i=rand()%100;
    if(p=='Q') //100% chance of evil
    {
      return(randomEvilPowerup());
    } else if(p=='K') //100% chance
    {
      return(selectRandomPowerup());
    } else if(p=='N' && i < 1) //1%
    {
     return(selectRandomPowerup());
    } else if(p=='M' && i < 2) //2%
    {
      return(selectRandomPowerup());
    } else if(p=='L' && i < 5) //5%
    {
      return(selectRandomPowerup());
    } else if(p=='J' && i < 10) //10%
    {
      return(selectRandomPowerup());
    }
   
    //Return no powerup or return the specified.
    if(p=='K' || p=='N' || p=='M' || p=='L' || p=='J')
    {
      return('0');
    } else {
      return(p);
    }

  }
  
  char randomEvilPowerup()
  {
    int i=rand()%evilPowerups.length();
    return(evilPowerups[i]);
  }

};

void loadlevel(string file, brick bricks[] ,int level)
{
  ifstream levelfile(file.data());
  if(!levelfile.is_open())
  {
    cout << " Could not open " << file << endl;
    var.quit=1;
    return;
  }
  string line;
  int levelread=0,brick=0,ch=0;
  bool useCustomDifficulty=0; //If this is still 0 when level is loaded, set difficulty = static_difficulty
  var.numlevels=0;
  var.scrollInfo.direction[0] = 0;
  var.scrollInfo.direction[1] = 0;
  var.scrollInfo.direction[2] = 0;
  var.scrollInfo.direction[3] = 0;
  
  while(!levelfile.eof())
  {
    getline(levelfile,line);
    //Har vi fundet start?
    if(levelread == 0)
    {
      //Nej, er det nu?
      if(line.substr(0,11) == "** Start **")
      {
        levelread++;
      }
    } else if(levelread == 1)
    {
      //Do the level stop now?
      if(line.substr(0,10) == "** Stop **")
      {
        levelread=0;
        var.numlevels++;
        brick=0;
      } else { //Reading data from level
        
        if(var.numlevels == level)
        {
          if(line[0] == '>')
          {
            if(line.substr(0,7) == "> right")
            {
              var.scrollInfo.speed[0] = atol( line.substr(8,line.length()).data() );
              var.scrollInfo.direction[0] = 1;
            } else if(line.substr(0,6) == "> left")
            {
              var.scrollInfo.speed[1] = atol( line.substr(7,line.length()).data() );
              var.scrollInfo.direction[1] = 1;
            } else if(line.substr(0,4) == "> up")
            {
              var.scrollInfo.speed[2] = atol( line.substr(5,line.length()).data() );
              var.scrollInfo.direction[2] = 1;
            } else if(line.substr(0,6) == "> down")
            {
              var.scrollInfo.speed[3] = atol( line.substr(7,line.length()).data() );
              var.scrollInfo.direction[3] = 1;
            } else if(line.substr(0,5) =="> inc")
            {
              useCustomDifficulty=1;
              difficulty.hitbrickinc[NORMAL] = atof( line.substr(6, line.length()).data() );
            }
          } else {
            while(line[ch] != 0)
            {
              bricks[brick].powerup = line[ch];
              bricks[brick].type = line[ch+1];
              
              if(bricks[brick].type == 'D')
              {
                char rgb[3][5];
                
                sprintf(rgb[0], "0x%c%c", line[ch+2], line[ch+3]);
                sprintf(rgb[1], "0x%c%c", line[ch+4], line[ch+5]);
                sprintf(rgb[2], "0x%c%c", line[ch+6], line[ch+7]);
                
                bricks[brick].color[0] = 0.003921569*strtol(rgb[0], NULL, 16);
                bricks[brick].color[1] = 0.003921569*strtol(rgb[1], NULL, 16);
                bricks[brick].color[2] = 0.003921569*strtol(rgb[2], NULL, 16);
  
                ch +=6;
              }
              //cout << "Level: " << levelnum  << " brick: " << brick << " Powerup: " << line[ch] << " Type: " << line[ch+1]<<"\n";
  
              brick++;
              ch +=2;
            }
            ch=0;
          } //Not a command
        } //denne level settes op
      }
    }
  }
   cout << "Read " << var.numlevels << " levels from '"<< file <<"'"  << endl;
  levelfile.close();
  
  if(!useCustomDifficulty)
    difficulty = static_difficulty;
}

void initlevels(brick bricks[], textureClass texLvl[])
{
  int brick;
  powerupLoaderClass powerupLoader;
  
  //Formelen for at regne offset ud er: xoffset = framewidth / picturewidth
                                      //yoffset = frameheight / pictureheight
  
  //Explosive
  texLvl[0].prop.ticks = 100;
  texLvl[0].prop.cols = 1;
  texLvl[0].prop.rows = 8;
  texLvl[0].prop.xoffset = 1.0;
  texLvl[0].prop.yoffset = 0.125;
  texLvl[0].prop.frames = 8;
  texLvl[0].prop.bidir = 1;
  texLvl[0].prop.playing = 1;
  texLvl[0].prop.padding=1;
  
  //normal brik
  texLvl[1].prop.ticks = 1000;
  texLvl[1].prop.cols = 1;
  texLvl[1].prop.rows = 1;
  texLvl[1].prop.xoffset = 1.0;
  texLvl[1].prop.yoffset = 1.0;
  texLvl[1].prop.frames = 0;
  texLvl[1].prop.playing = 0;
  texLvl[1].prop.padding=1;

  //cement
  texLvl[2].prop.ticks = 1000;
  texLvl[2].prop.cols = 1;
  texLvl[2].prop.rows = 2;
  texLvl[2].prop.xoffset = 1.0;
  texLvl[2].prop.yoffset = 0.5;
  texLvl[2].prop.frames = 2;
  texLvl[2].prop.playing = 0;

  //DOOM
  texLvl[3].prop.ticks = 250;
  texLvl[3].prop.cols = 8;
  texLvl[3].prop.rows = 16;
  texLvl[3].prop.xoffset = 0.125;
  texLvl[3].prop.yoffset = 0.062;
  texLvl[3].prop.frames = 8;
  texLvl[3].prop.bidir=1;
  texLvl[3].prop.playing = 1;
  
  //Glass
  texLvl[4].prop.ticks = 250;
  texLvl[4].prop.cols = 1;
  texLvl[4].prop.rows = 2;
  texLvl[4].prop.xoffset = 1.0;
  texLvl[4].prop.yoffset = 0.5;
  texLvl[4].prop.frames = 2;
  texLvl[4].prop.bidir=0;
  texLvl[4].prop.playing = 0;
  
  //Invisible
  texLvl[5].prop.ticks = 250;
  texLvl[5].prop.cols = 1;
  texLvl[5].prop.rows = 3;
  texLvl[5].prop.xoffset = 1.0;
  texLvl[5].prop.yoffset = 0.25;
  texLvl[5].prop.frames = 3;
  texLvl[5].prop.bidir=0;
  texLvl[5].prop.playing = 0;
  texLvl[5].prop.padding=1;

  //Set dem op
  int row,i;
  i=0;

  for(row=0;row<23;row++)
  {

    for(brick=0;brick < 26; brick++)
    {
    
      bricks[i].posx =  -1.54 + (GLfloat)brick*0.1232;
      bricks[i].posy =  0.95+ ((GLfloat)row*0.07)*-1;
      
      if(bricks[i].type != '0')
      {
        if(bricks[i].type != 'D') //type D får farverne fra loadlevel
        {
          bricks[i].color[0]=1.0f;
          bricks[i].color[1]=1.0f;
          bricks[i].color[2]=1.0f;
        }
        
        bricks[i].tex=texLvl[1];
        bricks[i].active=1;
        bricks[i].collide=1;
        bricks[i].isdyingnormally=0;
        bricks[i].isexploding=0;
        bricks[i].reflect=1;
        bricks[i].opacity=1.0;
        bricks[i].fade=1.0;
        bricks[i].fadespeed=2.0;
        bricks[i].zoom=1.0;
        bricks[i].zoomspeed=4.0;
        bricks[i].width=0.0616;
        bricks[i].height=0.035;
        
        bricks[i].score=11;
        bricks[i].destroytowin=1;
        bricks[i].hitsLeft=1;
             
        bricks[i].powerup = powerupLoader.randomPowerup(bricks[i].powerup);

        bricks[i].bricknum=brick;

        bricks[i].row=row;
        updated_nbrick[row][brick] = i; // This brick is active
        //cout<<"Brick:"<< nbrick[row][brick]<<endl;
      }  else {
        bricks[i].score=0;
        bricks[i].destroytowin=0;
        bricks[i].active=0;
        updated_nbrick[row][brick]=-1;
       // cout<<"Brick:"<< nbrick[row][brick]<<endl;
      }
        

      
      
      if(bricks[i].type == '1')
      {
        bricks[i].color[0]=0.0f;
        bricks[i].color[1]=0.0f;
        bricks[i].color[2]=1.0f;
      
      } else if(bricks[i].type == '2')
      {
        bricks[i].color[0]=1.0f;
        bricks[i].color[1]=1.0f;
        bricks[i].color[2]=0.0f;
      
      }else if(bricks[i].type == '3')
      {
        bricks[i].powerup = '0';
        bricks[i].tex= texLvl[2];
        bricks[i].score=30;
        bricks[i].destroytowin=0;
        
      } else if(bricks[i].type == '4')
      {
        bricks[i].tex=texLvl[4]; //glass texture
        bricks[i].hitsLeft = 2; //takes two hits to kill
        bricks[i].opacity=0.5f;
      
      } else if(bricks[i].type == '5')
      {
        bricks[i].color[0]=0.0f;
        bricks[i].color[1]=1.0f;
        bricks[i].color[2]=0.0f;
      
      } else if(bricks[i].type == '6')
      {
        bricks[i].color[0]=0.5f;
        bricks[i].color[1]=0.5f;
        bricks[i].color[2]=0.5f;
      
      } else if(bricks[i].type == '7')
      {
        bricks[i].color[0]=1.0f;
        bricks[i].color[1]=0.0f;
        bricks[i].color[2]=1.0f;
      
      } else if(bricks[i].type == '8')
      {
      
      } else if(bricks[i].type == '9')
      {
        bricks[i].tex=texLvl[5]; //invisible texture
        bricks[i].tex.frame=1;
        bricks[i].hitsLeft = 3; //takes 3 to kill
        bricks[i].color[0]=1.0f;
        bricks[i].color[1]=0.0f;
        bricks[i].color[2]=1.0f;
      
      } else if(bricks[i].type == 'A')
      {
        bricks[i].color[0]=1.0f;
        bricks[i].color[1]=0.0f;
        bricks[i].color[2]=0.0f;
      
      } else if(bricks[i].type == 'B')
      {
        bricks[i].tex=texLvl[0];
      } else if(bricks[i].type == 'C')
      {
        bricks[i].tex=texLvl[3];
        bricks[i].powerup = powerupLoader.randomEvilPowerup();
      }

      i++;
    }
  }
//    cout << "Powerups given to this level: " << powerupLoader.powerupsGiven << endl;
}
