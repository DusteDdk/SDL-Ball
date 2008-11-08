/* ************************************************************************* * 
    SDL-Ball - DX-Ball/Breakout remake with openGL and SDL for Linux 
    Level editor plugin for the gimp
    Copyright (C) 2008 DusteD.dk

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

/*
  Based on tutorial by David Neary, the excellent tutorial can be found here: 
                      http://developer.gimp.org/writing-a-plug-in/1/index.html
                                                                                */
#include <stdio.h>
#include <libgimp/gimp.h>

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
    "plug-in-SDLB",
    "Output level to console",
    "Create SDL-Ball level from pixels and output to console",
    "Jimmy Christensen",
    "SDL-Ball",
    "2008",
    "_Make Level",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-SDLB",
                             "<Image>/sdlb");
  printf("SDL-Ball Gimp Leveleditor plugin Registered\n");
}

static int samergb(guchar a[], guchar b[])
{
  if(a[0] == b[0] && a[1] == b[1] && a[2] == b[2])
  {
    return(1);
  }
  return(0);
}


static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if 
   * we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32;

  /*if (run_mode != GIMP_RUN_NONINTERACTIVE)
    g_message("Hello, world!\n");*/
    
    GimpDrawable *drawable;
    
    
    drawable = gimp_drawable_get (param[2].data.d_drawable); //aktiv drawable
    
    
    GimpPixelRgn rgn;
    
    guchar  pix[4];
    
    gimp_pixel_rgn_init(&rgn, drawable, 0,0, 26,23,FALSE,FALSE);
    
    int row,bri;
    
    #define R 0
    #define G 1
    #define B 2
    #define A 3
    
    #define BLU 0
    #define YEL 1
    #define CEM 2
    #define GLA 3
    #define GRE 4
    #define STE 5
    #define PUR 6
    #define WHI 7
    #define INV 8
    #define RED 9
    #define EXP 10
    #define DOO 11
    #define NON 12
    
    guchar type[12][4];
    
    type[BLU][R] = 0;
    type[BLU][G] = 0;
    type[BLU][B] = 255;
    
    type[YEL][R] = 255;
    type[YEL][G] = 255 ;
    type[YEL][B] = 0;

    type[CEM][R] = 0xC9;
    type[CEM][G] = 0xC9;
    type[CEM][B] = 0xC9;

    type[CEM][R] = 0xC9;
    type[CEM][G] = 0xC9;
    type[CEM][B] = 0xC9;
    
    type[GLA][R] = 0xFF;
    type[GLA][G] = 0xC5;
    type[GLA][B] = 0xC5;

    type[GRE][R] = 0x00;
    type[GRE][G] = 0xFF;
    type[GRE][B] = 0x00;

    type[STE][R] = 0xDB;
    type[STE][G] = 0xCF; //Deprecated
    type[STE][B] = 0xFF;
    
    type[PUR][R] = 0xFF;
    type[PUR][G] = 0x00;
    type[PUR][B] = 0xFF;

    type[WHI][R] = 0xFF;
    type[WHI][G] = 0xFF;
    type[WHI][B] = 0xFF;

    type[INV][R] = 0x00;
    type[INV][G] = 0xff;
    type[INV][B] = 0xd8;

    type[RED][R] = 0xFF;
    type[RED][G] = 0x00;
    type[RED][B] = 0x00;

    type[EXP][R] = 0xFF;
    type[EXP][G] = 0xB9;
    type[EXP][B] = 0x00;

    type[DOO][R] = 0x72;
    type[DOO][G] = 0x00;
    type[DOO][B] = 0xFF;

    type[NON][R] = 0x00;
    type[NON][G] = 0x00;
    type[NON][B] = 0x00;

    char level[23][26];
    
    char brickColor[23][26][7];
    
    char powerups[23][26];

    for(row=0; row < 23; row++)
    {
      for(bri=0; bri < 26; bri++)
      {
       // printf("ROW:%i BRI:%i\n",row,bri);
         gimp_pixel_rgn_get_pixel(&rgn, pix,bri,row);
         

          if(samergb(pix, type[BLU]))
          {
            level[row][bri]='1';
          } else if(samergb(pix, type[YEL]))
          {
            level[row][bri]='2';
          } else if(samergb(pix,type[CEM]))
          {
            level[row][bri]='3';
          } else if(samergb(pix,type[GLA]))
          {
            level[row][bri]='4';
          } else if(samergb(pix,type[GRE]))
          {
            level[row][bri]='5';
          } else if(samergb(pix,type[STE]))
          {
            level[row][bri]='6';
          } else if(samergb(pix,type[PUR]))
          {
            level[row][bri]='7';
          } else if(samergb(pix,type[WHI]))
          {
            level[row][bri]='8';
          } else if(samergb(pix,type[INV]))
          {
            level[row][bri]='9';
          } else if(samergb(pix,type[RED]))
          {
            level[row][bri]='A';
          } else if(samergb(pix,type[EXP]))
          {
            level[row][bri]='B';
          } else if(samergb(pix,type[DOO]))
          {
            level[row][bri]='C';
          } else if(samergb(pix, type[NON]))
          {
            //None
            level[row][bri]='0';
          } else 
          {
//            printf("Brick with custom color: '%02X,%02X,%02X'\n", pix[0], pix[1], pix[2]);
            sprintf(brickColor[row][bri], "%02X%02X%02X",pix[0],pix[1],pix[2]);
            level[row][bri]='D';
          }
          
          if(level[row][bri] != '0' && level[row][bri] != '3')
          {
              //5% chance of getting powerup
              powerups[row][bri]='L';

          } else {
            powerups[row][bri]='0';
          }
      }
    }
    
    printf("\n\n** Start **");
    for(row=0; row < 23; row++)
    {
      printf("\n");
      for(bri=0; bri < 26; bri++)
      {
        printf("%c%c",powerups[row][bri],level[row][bri]);
        if(level[row][bri] == 'D')
        {
          printf("%s", brickColor[row][bri]);
        }
      }
    }
    printf("** Stop **");


    printf("\n");

     gimp_drawable_flush (drawable);
     
      gimp_displays_flush ();
      gimp_drawable_detach (drawable);
    
    
    
    
    
}
