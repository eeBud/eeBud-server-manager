#include "eeBudServerManager.h"

#include <Adafruit_NeoPixel.h>

#define LEDS_BAS 27
#define LEDS_DROITE 26
#define LEDS_GAUCHE 25

//strip
#define NB_PIXELS_BAS 61
#define NB_PIXELS_DROITE 30
#define NB_PIXELS_GAUCHE 30
#define Bouton_Gauche 12
#define Bouton_Droite 14
Adafruit_NeoPixel strip(NB_PIXELS_BAS, LEDS_BAS, NEO_GBR + NEO_KHZ800);
Adafruit_NeoPixel strip_D(NB_PIXELS_DROITE, LEDS_DROITE, NEO_GBR + NEO_KHZ800);
Adafruit_NeoPixel strip_G(NB_PIXELS_GAUCHE, LEDS_GAUCHE, NEO_GBR + NEO_KHZ800);

eeBudServerManager Serveur("eeBud - Rubans Etabli");

int Bouton_Lumiere_Off_All;

int Bouton_Lumiere_BC;
int Bouton_Lumiere_BF;
int Bouton_Lumiere_Off;

int Bouton_Lumiere_BC_D;
int Bouton_Lumiere_BF_D;
int Bouton_Lumiere_Off_D;

int Bouton_Lumiere_BC_G;
int Bouton_Lumiere_BF_G;
int Bouton_Lumiere_Off_G;

int Bouton_Lumiere_Anim;
bool bool_Bouton_Lumiere_Anim = false;

int Bouton_Lumiere_Anim1;
bool bool_Bouton_Lumiere_Anim1 = false;

int Couleur_Lumiere_BC;
int Couleur_Lumiere_BF;
int Couleur_Anim;
int Couleur_Anim_1;
int Couleur_Anim_2;
int Couleur_Anim_3;

int Couleur_Lumiere_BC_D;
int Couleur_Lumiere_BF_D;

int Couleur_Lumiere_BC_G;
int Couleur_Lumiere_BF_G;

int Range_Luminosite;
int Range_Luminosite_D;
int Range_Luminosite_G;

int Range_Coe;
int Range_Coe1;
int Etat_Lumiere;

int Etat_Lumiere_D;
int Etat_Lumiere_G;

int Info_Gestuel;
String URL_Gestuel;
int Event;


DFRobot_PAJ7620U2 paj;

/////////////////////////
int i = 0;
int j = 0;
int k = 0;

int id = 0;
int jd = 0;
int kd = 0;

int ig = 0;
int jg = 0;
int kg = 0;

bool EnCours = false;
bool Reverse = false;
bool realise = false;

int CouleurB = 0;
int CouleurG = 0;
int CouleurR = 0;
int BB = 0;
int GG = 0;
int RR = 0;

int CouleurB1 = 0;
int CouleurG1 = 0;
int CouleurR1 = 0;

int CouleurB2 = 0;
int CouleurG2 = 0;
int CouleurR2 = 0;

int Coe = 30;
int Coe1 = 10;


void setup() {
  Serial.begin(115200);
  Serial.println("------Rubans------");
  Serveur.Debug(true);
  Serveur.Init();

  Serveur.AddParamText("URL Gestuel", "");
  URL_Gestuel = Serveur.GetParamText("URL Gestuel");

  Serveur.IndexAddTitle("Geste", "100");
  Info_Gestuel = Serveur.IndexAddInfo("Geste", "", "50");

  //LEDS
  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Tout", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Off_All = Serveur.IndexAddButton("Off");
  Couleur_Anim = Serveur.IndexAddColor("Couleur anim 1", 255, 95, 10);
  Couleur_Anim_1 = Serveur.IndexAddColor("Couleur anim 2", 0, 125, 220);
  Couleur_Anim_2 = Serveur.IndexAddColor("Couleur anim 3", 180, 0, 220);
  Couleur_Anim_3 = Serveur.IndexAddColor("Couleur anim 4", 0, 0, 255);
  Bouton_Lumiere_Anim1 = Serveur.IndexAddButton("Animation 1");
  Range_Coe1 = Serveur.IndexAddRange("Coef", 1, 23, 10);
  //Bouton_Lumiere_BC = Serveur.IndexAddButton("Blanc chaud");
  //Bouton_Lumiere_BF = Serveur.IndexAddButton("Blanc froid");
  //Range_Luminosite = Serveur.IndexAddRange("Luminosité", 0, 255, 200);

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Bas", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Off = Serveur.IndexAddButton("Off");
  Bouton_Lumiere_BC = Serveur.IndexAddButton("Blanc chaud");
  Bouton_Lumiere_BF = Serveur.IndexAddButton("Blanc froid");
  Range_Luminosite = Serveur.IndexAddRange("Luminosité", 0, 255, 200);

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Droite", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Off_D = Serveur.IndexAddButton("Off");
  Bouton_Lumiere_BC_D = Serveur.IndexAddButton("Blanc chaud");
  Bouton_Lumiere_BF_D = Serveur.IndexAddButton("Blanc froid");
  Range_Luminosite_D = Serveur.IndexAddRange("Luminosité", 0, 255, 200);

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Gauche", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Off_G = Serveur.IndexAddButton("Off");
  Bouton_Lumiere_BC_G = Serveur.IndexAddButton("Blanc chaud");
  Bouton_Lumiere_BF_G = Serveur.IndexAddButton("Blanc froid");
  Range_Luminosite_G = Serveur.IndexAddRange("Luminosité", 0, 255, 200);

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Animations", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Anim = Serveur.IndexAddButton("Animation");

  Range_Coe = Serveur.IndexAddRange("Coef", 0, 255, 30);

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Couleurs", "100");
  Serveur.IndexAddSeg();
  Couleur_Lumiere_BC = Serveur.IndexAddColor("Blanc chaud bas", 255, 95, 10);
  Couleur_Lumiere_BF = Serveur.IndexAddColor("Blanc froid bas", 0, 0, 255);
  Couleur_Lumiere_BC_D = Serveur.IndexAddColor("Blanc chaud droite", 195, 110, 10);
  Couleur_Lumiere_BF_D = Serveur.IndexAddColor("Blanc froid droite", 255, 255, 255);
  Couleur_Lumiere_BC_G = Serveur.IndexAddColor("Blanc chaud gauche", 195, 110, 10);
  Couleur_Lumiere_BF_G = Serveur.IndexAddColor("Blanc froid gauche", 255, 255, 255);



  
  //Strip
  strip.begin();
  strip.setBrightness(Serveur.IndexGetRange(Range_Luminosite));
  strip_D.begin();
  strip_D.setBrightness(Serveur.IndexGetRange(Range_Luminosite_D));
  strip_G.begin();
  strip_G.setBrightness(Serveur.IndexGetRange(Range_Luminosite_G));


  strip_Off();
  strip_Off_D();
  strip_Off_G();
  Serveur.IndexSetInfo(Etat_Lumiere, "Off");
  Serveur.IndexSetInfo(Etat_Lumiere_D, "Off");
  Serveur.IndexSetInfo(Etat_Lumiere_G, "Off");

  //Boutons
  pinMode(Bouton_Gauche, INPUT_PULLUP);
  pinMode(Bouton_Droite, INPUT_PULLUP);
}


void loop() {
  Event = Serveur.IndexGetEvent();

  
  
  if (Event == Bouton_Lumiere_Off) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Off");
    Switch_Animation("Off");
    strip_Off();
    i = 0;
    j = 0;
    k = 0;

    EnCours = false;
    Reverse = false;
    realise = false;


  } else if (Event == Bouton_Lumiere_Off_D) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere_D, "Off");
    Switch_Animation("Off");
    strip_Off_D();


  } else if (Event == Bouton_Lumiere_Off_G) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere_G, "Off");
    Switch_Animation("Off");
    strip_Off_G();


  } else if (Event == Bouton_Lumiere_Off_All) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Off");
    Serveur.IndexSetInfo(Etat_Lumiere_D, "Off");
    Serveur.IndexSetInfo(Etat_Lumiere_G, "Off");
    Switch_Animation("Off");
    strip_Off_D();
    strip_Off_G();
    strip_Off();
    i = 0;
    j = 0;
    k = 0;

    EnCours = false;
    Reverse = false;
    realise = false;


  } else if (Event == Bouton_Lumiere_BC) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Blanc chaud");
    Switch_Animation("Off");
    Anim_Lumiere(strip.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BC), Serveur.IndexGetColor_G(Couleur_Lumiere_BC), Serveur.IndexGetColor_R(Couleur_Lumiere_BC)));

  } else if (Event == Bouton_Lumiere_BC_D) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere_D, "Blanc chaud");
    Switch_Animation("Off");
    Anim_Lumiere_D(strip_D.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BC_D), Serveur.IndexGetColor_G(Couleur_Lumiere_BC_D), Serveur.IndexGetColor_R(Couleur_Lumiere_BC_D)));

  } else if (Event == Bouton_Lumiere_BC_G) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere_G, "Blanc chaud");
    Switch_Animation("Off");
    Anim_Lumiere_G(strip_G.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BC_G), Serveur.IndexGetColor_G(Couleur_Lumiere_BC_G), Serveur.IndexGetColor_R(Couleur_Lumiere_BC_G)));

  } else if (Event == Bouton_Lumiere_BF) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Blanc froid");
    Switch_Animation("Off");
    Anim_Lumiere(strip.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BF), Serveur.IndexGetColor_G(Couleur_Lumiere_BF), Serveur.IndexGetColor_R(Couleur_Lumiere_BF)));

  } else if (Event == Bouton_Lumiere_BF_D) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere_D, "Blanc froid");
    Switch_Animation("Off");
    Anim_Lumiere_D(strip_D.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BF_D), Serveur.IndexGetColor_G(Couleur_Lumiere_BF_D), Serveur.IndexGetColor_R(Couleur_Lumiere_BF_D)));

  } else if (Event == Bouton_Lumiere_BF_G) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere_G, "Blanc froid");
    Switch_Animation("Off");
    Anim_Lumiere_G(strip_G.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BF_G), Serveur.IndexGetColor_G(Couleur_Lumiere_BF_G), Serveur.IndexGetColor_R(Couleur_Lumiere_BF_G)));

  } else if (Event == Bouton_Lumiere_Anim) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Animation");
    Switch_Animation("bool_Bouton_Lumiere_Anim");

  } else if (Event == Bouton_Lumiere_Anim1) {
    realise = false;
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Animation 1");
    Switch_Animation("bool_Bouton_Lumiere_Anim1");

  } else if (Event == Range_Luminosite) {
    Serveur.IndexResetEvent();
    strip.setBrightness(Serveur.IndexGetRange(Range_Luminosite));
    strip.show();

  } else if (Event == Range_Coe) {
    Serveur.IndexResetEvent();
    Coe = Serveur.IndexGetRange(Range_Coe);
  } else if (Event == Range_Coe1) {
    Serveur.IndexResetEvent();
    Coe1 = Serveur.IndexGetRange(Range_Coe1);
    realise = false;
  }


  if (bool_Bouton_Lumiere_Anim == true) {
    Anim();
  } else if (bool_Bouton_Lumiere_Anim1 == true) {
    Anim1();
  }
}

void Switch_Animation(String Bool_Bouton) {
  int var = -1;

  if (Bool_Bouton == "bool_Bouton_Lumiere_Anim") var = 0;
  if (Bool_Bouton == "bool_Bouton_Lumiere_Anim1") var = 1;
  /*if (Bool_Bouton == "bool_Bouton_Lumiere_Rainbow") var = 1;
  if (Bool_Bouton == "bool_Bouton_Lumiere_Balayage") var = 2;
  if (Bool_Bouton == "bool_Bouton_Lumiere_Theatre") var = 3;
  if (Bool_Bouton == "bool_Bouton_Lumiere_TRainbow") var = 4;*/

  bool_Bouton_Lumiere_Anim = false;
  bool_Bouton_Lumiere_Anim1 = false;
  /*bool_Bouton_Lumiere_Rainbow = false;
  bool_Bouton_Lumiere_Balayage = false;
  bool_Bouton_Lumiere_Theatre = false;
  bool_Bouton_Lumiere_TRainbow = false;*/

  switch (var) {
    case 0:
      bool_Bouton_Lumiere_Anim = true;
      break;
    case 1:
      bool_Bouton_Lumiere_Anim1 = true;
      break;
      /*case 2:
      bool_Bouton_Lumiere_Balayage = true;
      break;
    case 3:
      bool_Bouton_Lumiere_Theatre = true;
      break;
    case 4:
      bool_Bouton_Lumiere_TRainbow = true;
      break;*/
  }
}

//Animation des leds
void strip_Off() {
  strip.clear();
  strip.show();
}

void strip_Off_D() {
  strip_D.clear();
  strip_D.show();
}

void strip_Off_G() {
  strip_G.clear();
  strip_G.show();
}

void Anim_Lumiere(uint32_t color) {
  for (int i = 0; i <= NB_PIXELS_BAS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void Anim_Lumiere_D(uint32_t color) {
  for (int i = 0; i <= NB_PIXELS_DROITE; i++) {
    strip_D.setPixelColor(i, color);
  }
  strip_D.show();
}

void Anim_Lumiere_G(uint32_t color) {
  for (int i = 0; i <= NB_PIXELS_GAUCHE; i++) {
    strip_G.setPixelColor(i, color);
  }
  strip_G.show();
}

void Anim1() {
  if (realise == false) {
    realise = true;
    int cCouleurB = Serveur.IndexGetColor_B(Couleur_Anim);
    int cCouleurG = Serveur.IndexGetColor_G(Couleur_Anim);
    int cCouleurR = Serveur.IndexGetColor_R(Couleur_Anim);

    int cCouleurB1 = Serveur.IndexGetColor_B(Couleur_Anim_1);
    int cCouleurG1 = Serveur.IndexGetColor_G(Couleur_Anim_1);
    int cCouleurR1 = Serveur.IndexGetColor_R(Couleur_Anim_1);

    int cCouleurB2 = Serveur.IndexGetColor_B(Couleur_Anim_2);
    int cCouleurG2 = Serveur.IndexGetColor_G(Couleur_Anim_2);
    int cCouleurR2 = Serveur.IndexGetColor_R(Couleur_Anim_2);

    

    float CB = cCouleurB;
    float CG = cCouleurG;
    float CR = cCouleurR;

    CouleurB = cCouleurB;
    CouleurG = cCouleurG;
    CouleurR = cCouleurR;
    CouleurB1 = cCouleurB1;
    CouleurG1 = cCouleurG1;
    CouleurR1 = cCouleurR1;

    float DiffB = (CouleurB1 - CouleurB);
    float DiffG = (CouleurG1 - CouleurG);
    float DiffR = (CouleurR1 - CouleurR);

    float CoefB = 0;
    if (DiffB != 0) {
      CoefB = DiffB / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    float CoefG = 0;
    if (DiffG != 0) {
      CoefG = DiffG / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    float CoefR = 0;
    if (DiffR != 0) {
      CoefR = DiffR / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    for (int a = 0; a < Coe1; a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = Coe1; a < (NB_PIXELS_BAS / 4) - Coe1; a++) {
      CB = CB + (CoefB);
      CouleurB = CB;
      CG = CG + (CoefG);
      CouleurG = CG;
      CR = CR + (CoefR);
      CouleurR = CR;
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = (NB_PIXELS_BAS / 4) - Coe1; a < (NB_PIXELS_BAS / 4); a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }

    CB = cCouleurB1;
    CG = cCouleurG1;
    CR = cCouleurR1;

    CouleurB = cCouleurB1;
    CouleurG = cCouleurG1;
    CouleurR = cCouleurR1;
    CouleurB1 = cCouleurB2;
    CouleurG1 = cCouleurG2;
    CouleurR1 = cCouleurR2;

    DiffB = (CouleurB1 - CouleurB);
    DiffG = (CouleurG1 - CouleurG);
    DiffR = (CouleurR1 - CouleurR);

    CoefB = 0;
    if (DiffB != 0) {
      CoefB = DiffB / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    CoefG = 0;
    if (DiffG != 0) {
      CoefG = DiffG / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    CoefR = 0;
    if (DiffR != 0) {
      CoefR = DiffR / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }


    for (int a = (NB_PIXELS_BAS / 4); a < (NB_PIXELS_BAS / 4) + Coe1; a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = (NB_PIXELS_BAS / 4) + Coe1; a < (NB_PIXELS_BAS / 2) - Coe1; a++) {
      CB = CB + (CoefB);
      CouleurB = CB;
      CG = CG + (CoefG);
      CouleurG = CG;
      CR = CR + (CoefR);
      CouleurR = CR;

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = (NB_PIXELS_BAS / 2) - Coe1; a < (NB_PIXELS_BAS / 2); a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }

    CB = cCouleurB2;
    CG = cCouleurG2;
    CR = cCouleurR2;

    CouleurB = cCouleurB2;
    CouleurG = cCouleurG2;
    CouleurR = cCouleurR2;
    CouleurB1 = cCouleurB1;
    CouleurG1 = cCouleurG1;
    CouleurR1 = cCouleurR1;

    DiffB = (CouleurB1 - CouleurB);
    DiffG = (CouleurG1 - CouleurG);
    DiffR = (CouleurR1 - CouleurR);

    CoefB = 0;
    if (DiffB != 0) {
      CoefB = DiffB / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    CoefG = 0;
    if (DiffG != 0) {
      CoefG = DiffG / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    CoefR = 0;
    if (DiffR != 0) {
      CoefR = DiffR / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }


    for (int a = (NB_PIXELS_BAS / 2); a < (NB_PIXELS_BAS / 2) + Coe1; a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = (NB_PIXELS_BAS / 2) + Coe1; a < (NB_PIXELS_BAS - (NB_PIXELS_BAS / 4)) - Coe1; a++) {
      CB = CB + (CoefB);
      CouleurB = CB;
      CG = CG + (CoefG);
      CouleurG = CG;
      CR = CR + (CoefR);
      CouleurR = CR;

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = (NB_PIXELS_BAS - (NB_PIXELS_BAS / 4)) - Coe1; a < (NB_PIXELS_BAS - (NB_PIXELS_BAS / 4)); a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }

    CB = cCouleurB1;
    CG = cCouleurG1;
    CR = cCouleurR1;

    CouleurB = cCouleurB1;
    CouleurG = cCouleurG1;
    CouleurR = cCouleurR1;
    CouleurB1 = cCouleurB;
    CouleurG1 = cCouleurG;
    CouleurR1 = cCouleurR;

    DiffB = (CouleurB1 - CouleurB);
    DiffG = (CouleurG1 - CouleurG);
    DiffR = (CouleurR1 - CouleurR);

    CoefB = 0;
    if (DiffB != 0) {
      CoefB = DiffB / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    CoefG = 0;
    if (DiffG != 0) {
      CoefG = DiffG / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }

    CoefR = 0;
    if (DiffR != 0) {
      CoefR = DiffR / ((NB_PIXELS_BAS / 4) - (Coe1 * 2));
    }


    for (int a = (NB_PIXELS_BAS - (NB_PIXELS_BAS / 4)); a < (NB_PIXELS_BAS - (NB_PIXELS_BAS / 4)) + Coe1; a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = (NB_PIXELS_BAS - (NB_PIXELS_BAS / 4)) + Coe1; a < NB_PIXELS_BAS - Coe1; a++) {
      CB = CB + (CoefB);
      CouleurB = CB;
      CG = CG + (CoefG);
      CouleurG = CG;
      CR = CR + (CoefR);
      CouleurR = CR;

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
    for (int a = NB_PIXELS_BAS - Coe1; a < NB_PIXELS_BAS; a++) {
      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
    }
  }
    int cCouleurB3 = Serveur.IndexGetColor_B(Couleur_Anim_3);
    int cCouleurG3 = Serveur.IndexGetColor_G(Couleur_Anim_3);
    int cCouleurR3 = Serveur.IndexGetColor_R(Couleur_Anim_3);
  /*for (int a = 0; a < NB_PIXELS_BAS+5; a++) {
    int cB = (strip.getPixelColor(a) >> 16);
    int cG = (strip.getPixelColor(a) >> 8);
    int cR = (strip.getPixelColor(a));
    int cB1 = (strip.getPixelColor(a-1) >> 16);
    int cG1 = (strip.getPixelColor(a-1) >> 8);
    int cR1 = (strip.getPixelColor(a-1));
    int cB2 = (strip.getPixelColor(a-2) >> 16);
    int cG2 = (strip.getPixelColor(a-2) >> 8);
    int cR2 = (strip.getPixelColor(a-2));
    int cB3 = (strip.getPixelColor(a-3) >> 16);
    int cG3 = (strip.getPixelColor(a-3) >> 8);
    int cR3 = (strip.getPixelColor(a-3));
    int cB4 = (strip.getPixelColor(a-4) >> 16);
    int cG4 = (strip.getPixelColor(a-4) >> 8);
    int cR4 = (strip.getPixelColor(a-4));


    strip.setPixelColor(a, strip.Color(0, 0, 0));
    strip.setPixelColor(a-1, strip.Color(cCouleurB3, cCouleurG3, cCouleurR3));
    strip.setPixelColor(a-2, strip.Color(cCouleurB3, cCouleurG3, cCouleurR3));
    strip.setPixelColor(a-3, strip.Color(cCouleurB3, cCouleurG3, cCouleurR3));
    strip.setPixelColor(a-4, strip.Color(0, 0, 0));

    strip.show();
    delay(10);
    strip.setPixelColor(a, strip.Color(cB, cG, cR));
    strip.setPixelColor(a-1, strip.Color(cB1, cG1, cR1));
    strip.setPixelColor(a-2, strip.Color(cB2, cG2, cR2));
    strip.setPixelColor(a-3, strip.Color(cB3, cG3, cR3));
    strip.setPixelColor(a-4, strip.Color(cB4, cG4, cR4));
  }*/
}

/*void Anim1() {
  if (realise == false) {
    realise = true;
    int cCouleurB = Serveur.IndexGetColor_B(Couleur_Anim);
    int cCouleurG = Serveur.IndexGetColor_G(Couleur_Anim);
    int cCouleurR = Serveur.IndexGetColor_R(Couleur_Anim);

    int cCouleurB1 = Serveur.IndexGetColor_B(Couleur_Anim_1);
    int cCouleurG1 = Serveur.IndexGetColor_G(Couleur_Anim_1);
    int cCouleurR1 = Serveur.IndexGetColor_R(Couleur_Anim_1);

    int cCouleurB2 = Serveur.IndexGetColor_B(Couleur_Anim_2);
    int cCouleurG2 = Serveur.IndexGetColor_G(Couleur_Anim_2);
    int cCouleurR2 = Serveur.IndexGetColor_R(Couleur_Anim_2);

    float CB = cCouleurB;
    float CG = cCouleurG;
    float CR = cCouleurR;

    CouleurB = cCouleurB;
    CouleurG = cCouleurG;
    CouleurR = cCouleurR;
    CouleurB1 = cCouleurB1;
    CouleurG1 = cCouleurG1;
    CouleurR1 = cCouleurR1;

    double DiffB = (CouleurB1 - CouleurB);
    double DiffG = (CouleurG1 - CouleurG);
    double DiffR = (CouleurR1 - CouleurR);

    if (DiffB < 0) DiffB = DiffB * -1;
    if (DiffG < 0) DiffG = DiffG * -1;
    if (DiffR < 0) DiffR = DiffR * -1;


    double CoefB = 0;
    if (DiffB != 0) {
      CoefB = DiffB / (NB_PIXELS_BAS / 4);
    }

    double CoefG = 0;
    if (DiffG != 0) {
      CoefG = DiffG / (NB_PIXELS_BAS / 4);
    }

    double CoefR = 0;
    if (DiffR != 0) {
      CoefR = DiffR / (NB_PIXELS_BAS / 4);
    }


    for (int a = 0; a < NB_PIXELS_BAS / 4; a++) {
      if (CouleurB < CouleurB1) {
        CB = CB + CoefB;
        CouleurB = CB;
        if (CouleurB > CouleurB1) CouleurB = CouleurB1;
      } else {
        CB = CB - CoefB;
        CouleurB = CB;
        if (CouleurB < CouleurB1) CouleurB = CouleurB1;
      }
      if (CouleurG < CouleurG1) {
        CG = CG + CoefG;
        CouleurG = CG;
        if (CouleurG > CouleurG1) CouleurG = CouleurG1;
      } else {
        CG = CG - CoefG;
        CouleurG = CG;
        if (CouleurG < CouleurG1) CouleurG = CouleurG1;
      }
      if (CouleurR < CouleurR1) {
        CR = CR + CoefR;
        CouleurR = CR;
        if (CouleurR > CouleurR1) CouleurR = CouleurR1;
      } else {
        CR = CR - CoefR;
        CouleurR = CR;
        if (CouleurR < CouleurR1) CouleurR = CouleurR1;
      }

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
      //delay(10);
    }
    
    //2
    CB = cCouleurB1;
    CG = cCouleurG1;
    CR = cCouleurR1;
    CouleurB = cCouleurB1;
    CouleurG = cCouleurG1;
    CouleurR = cCouleurR1;
    CouleurB1 = cCouleurB2;
    CouleurG1 = cCouleurG2;
    CouleurR1 = cCouleurR2;


    for (int a = NB_PIXELS_BAS / 4; a < (NB_PIXELS_BAS / 4)*2; a++) {
      if (CouleurB < CouleurB1) {
        CB = CB + CoefB;
        CouleurB = CB;
        if (CouleurB > CouleurB1) CouleurB = CouleurB1;
      } else {
        CB = CB - CoefB;
        CouleurB = CB;
        if (CouleurB < CouleurB1) CouleurB = CouleurB1;
      }
      if (CouleurG < CouleurG1) {
        CG = CG + CoefG;
        CouleurG = CG;
        if (CouleurG > CouleurG1) CouleurG = CouleurG1;
      } else {
        CG = CG - CoefG;
        CouleurG = CG;
        if (CouleurG < CouleurG1) CouleurG = CouleurG1;
      }
      if (CouleurR < CouleurR1) {
        CR = CR + CoefR;
        CouleurR = CR;
        if (CouleurR > CouleurR1) CouleurR = CouleurR1;
      } else {
        CR = CR - CoefR;
        CouleurR = CR;
        if (CouleurR < CouleurR1) CouleurR = CouleurR1;
      }

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
      //delay(10);
    }

    //3
    CB = cCouleurB2;
    CG = cCouleurG2;
    CR = cCouleurR2;
    CouleurB = cCouleurB2;
    CouleurG = cCouleurG2;
    CouleurR = cCouleurR2;
    CouleurB1 = cCouleurB1;
    CouleurG1 = cCouleurG1;
    CouleurR1 = cCouleurR1;


    for (int a = (NB_PIXELS_BAS / 4)*2; a < (NB_PIXELS_BAS / 4)*3; a++) {
      if (CouleurB < CouleurB1) {
        CB = CB + CoefB;
        CouleurB = CB;
        if (CouleurB > CouleurB1) CouleurB = CouleurB1;
      } else {
        CB = CB - CoefB;
        CouleurB = CB;
        if (CouleurB < CouleurB1) CouleurB = CouleurB1;
      }
      if (CouleurG < CouleurG1) {
        CG = CG + CoefG;
        CouleurG = CG;
        if (CouleurG > CouleurG1) CouleurG = CouleurG1;
      } else {
        CG = CG - CoefG;
        CouleurG = CG;
        if (CouleurG < CouleurG1) CouleurG = CouleurG1;
      }
      if (CouleurR < CouleurR1) {
        CR = CR + CoefR;
        CouleurR = CR;
        if (CouleurR > CouleurR1) CouleurR = CouleurR1;
      } else {
        CR = CR - CoefR;
        CouleurR = CR;
        if (CouleurR < CouleurR1) CouleurR = CouleurR1;
      }

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
      //delay(10);
    }

    //4
    CB = cCouleurB1;
    CG = cCouleurG1;
    CR = cCouleurR1;
    CouleurB = cCouleurB1;
    CouleurG = cCouleurG1;
    CouleurR = cCouleurR1;
    CouleurB1 = cCouleurB;
    CouleurG1 = cCouleurG;
    CouleurR1 = cCouleurR;


    for (int a = (NB_PIXELS_BAS / 4)*3; a < NB_PIXELS_BAS; a++) {
      if (CouleurB < CouleurB1) {
        CB = CB + CoefB;
        CouleurB = CB;
        if (CouleurB > CouleurB1) CouleurB = CouleurB1;
      } else {
        CB = CB - CoefB;
        CouleurB = CB;
        if (CouleurB < CouleurB1) CouleurB = CouleurB1;
      }
      if (CouleurG < CouleurG1) {
        CG = CG + CoefG;
        CouleurG = CG;
        if (CouleurG > CouleurG1) CouleurG = CouleurG1;
      } else {
        CG = CG - CoefG;
        CouleurG = CG;
        if (CouleurG < CouleurG1) CouleurG = CouleurG1;
      }
      if (CouleurR < CouleurR1) {
        CR = CR + CoefR;
        CouleurR = CR;
        if (CouleurR > CouleurR1) CouleurR = CouleurR1;
      } else {
        CR = CR - CoefR;
        CouleurR = CR;
        if (CouleurR < CouleurR1) CouleurR = CouleurR1;
      }

      strip.setPixelColor(a, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
      //delay(10);
    }

    
    
  }
}*/

void Anim() {


  if (Reverse == false) {
    if (CouleurB == BB && CouleurG == GG && CouleurR == RR) {
      //delay(2000);
      /*for (int l = 0; l <= NB_PIXELS; l++) {
        strip.setPixelColor(i, strip.Color(BB, GG, CouleurR));
        strip.show();
        delay(1);
      }*/
      Reverse = true;
      EnCours = false;
    }
    if (EnCours == false) {
      EnCours = true;

      i = 0;
      j = 0;
      k = 0;
      id = 0;
      jd = 0;
      kd = 0;
      ig = 0;
      jg = 0;
      kg = 0;

      CouleurB = Serveur.IndexGetColor_B(Couleur_Anim);
      CouleurG = Serveur.IndexGetColor_G(Couleur_Anim);
      CouleurR = Serveur.IndexGetColor_R(Couleur_Anim);
      BB = Serveur.IndexGetColor_B(Couleur_Anim_1);
      GG = Serveur.IndexGetColor_G(Couleur_Anim_1);
      RR = Serveur.IndexGetColor_R(Couleur_Anim_1);
    }

  } else {
    if (CouleurB == BB && CouleurG == GG && CouleurR == RR) {
      //delay(2000);
      Reverse = false;
      EnCours = false;
    }
    if (EnCours == false) {
      EnCours = true;

      i = 0;
      j = 0;
      k = 0;
      id = 0;
      jd = 0;
      kd = 0;
      ig = 0;
      jg = 0;
      kg = 0;

      CouleurB = Serveur.IndexGetColor_B(Couleur_Anim_1);
      CouleurG = Serveur.IndexGetColor_G(Couleur_Anim_1);
      CouleurR = Serveur.IndexGetColor_R(Couleur_Anim_1);
      BB = Serveur.IndexGetColor_B(Couleur_Anim);
      GG = Serveur.IndexGetColor_G(Couleur_Anim);
      RR = Serveur.IndexGetColor_R(Couleur_Anim);
    }
  }

  if (i < NB_PIXELS_BAS) {
    strip.setPixelColor(i, strip.Color(CouleurB, CouleurG, CouleurR));
    strip.show();
    delay(10);
    i++;
  }






  if (i >= NB_PIXELS_BAS && EnCours == true) {
    if (j < NB_PIXELS_BAS) {



      if (j == 0) {
        if (CouleurB < BB) {
          CouleurB = CouleurB + Coe;
          if (CouleurB > BB) CouleurB = BB;
        } else {
          CouleurB = CouleurB - Coe;
          if (CouleurB < BB) CouleurB = BB;
        }
        if (CouleurG < GG) {
          CouleurG = CouleurG + Coe;
          if (CouleurG > GG) CouleurG = GG;
        } else {
          CouleurG = CouleurG - Coe;
          if (CouleurG < GG) CouleurG = GG;
        }
        if (CouleurR < RR) {
          CouleurR = CouleurR + Coe;
          if (CouleurR > RR) CouleurR = RR;
        } else {
          CouleurR = CouleurR - Coe;
          if (CouleurR < RR) CouleurR = RR;
        }
      }

      strip.setPixelColor(j, strip.Color(CouleurB, CouleurG, CouleurR));
      strip.show();
      delay(1);

      j++;
      if (j >= NB_PIXELS_BAS) j = 0;
    }
  }








  /*for (int j = 0; j <= NB_PIXELS; j++) {

    if (B <= BB) {
      Serial.println("B");
      for (int k = B; k <= BB; k = k + 5) {

        strip.setPixelColor(j, strip.Color(k, G, R));

        strip.show();
        delay(1);
      }
    } else {
      Serial.println("BB");
      for (int k = B; k >= BB; k = k - 5) {
        strip.setPixelColor(j, strip.Color(k, G, R));

        strip.show();
        delay(1);
      }
    }
    if (G <= GG) {
      Serial.println("G");
      for (int k = G; k <= GG; k = k + 5) {
        strip.setPixelColor(j, strip.Color(BB, k, R));
        strip.show();
        delay(1);
      }

    } else {
      Serial.println("GG");
      for (int k = G; k >= GG; k = k - 5) {
        strip.setPixelColor(j, strip.Color(BB, k, R));
        strip.show();
        delay(1);
      }
    }
    if (R <= RR) {
      Serial.println("R");
      for (int k = R; k <= RR; k = k + 5) {
        strip.setPixelColor(j, strip.Color(BB, GG, k));
        strip.show();
        delay(1);
      }

    } else {
      Serial.println("RR");
      for (int k = R; k >= RR; k = k - 5) {
        strip.setPixelColor(j, strip.Color(BB, GG, k));
        strip.show();
        delay(1);
      }
    }
  }*/
}