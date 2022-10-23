#include <eeBudServerManager.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BH1750.h>
#include <Adafruit_NeoPixel.h>

#define DS18B20_PIN 33
#define RELAIS 32
#define PIR 25
#define LEDS 26

//strip
#define NB_PIXELS 9
Adafruit_NeoPixel strip(NB_PIXELS, LEDS, NEO_GBR + NEO_KHZ800);

//Luminosite
BH1750 luxMetre;
float Memoire_Luminosite = 9999;
float Luminosite_Actuelle;

//Température
OneWire oneWire(DS18B20_PIN);
DallasTemperature DS18B20(&oneWire);

float Memoire_Temperature = 9999;
unsigned long Moment_Present = 0;
unsigned long Moment_Memorise = 0;
float Temperature_Actuelle;

DeviceAddress tempDeviceAddress;
int resolution = 12;
unsigned long lastTempRequest = 0;
int delayInMillis = 0;
float temperature = 0.0;
int idle = 0;

//Bouton
bool Etat_Relais;

//PIR
bool Etat_Mouvement;
int pirVal;

//Serveur
eeBudServerManager Serveur("Cadre domotique");

int Info_Temperature;
int Info_Luminosite;
int Info_Mouvement;

int Bouton_Rechargement_Off;
int Bouton_Rechargement_On;
int Bouton_Lumiere_BC;
int Bouton_Lumiere_BF;
int Bouton_Lumiere_Alarme;
int Bouton_Lumiere_Rainbow;
int Bouton_Lumiere_Balayage;
int Bouton_Lumiere_Theatre;
int Bouton_Lumiere_TRainbow;
int Bouton_Lumiere_Off;

bool bool_Bouton_Lumiere_Alarme = false;
bool bool_Bouton_Lumiere_Rainbow = false;
bool bool_Bouton_Lumiere_Balayage = false;
bool bool_Bouton_Lumiere_Theatre = false;
bool bool_Bouton_Lumiere_TRainbow = false;

int Couleur_Lumiere_BC;
int Couleur_Lumiere_BF;
int Couleur_Alarme;
int Couleur_Alarme_1;
int Couleur_Balayage;
int Couleur_Balayage_1;
int Couleur_Theatre;
int Couleur_Theatre_1;

int Range_Luminosite;

int Etat_Lumiere;

int Event;

float Difference_Temperature_Parametree;
float Difference_Luminosite_Parametree;
int Sensibilite_Tactile;
String URL_Mouvement;
String URL_Temperature;
String URL_Rechargement;
String URL_Luminosite;
String URL_Lumiere_Etat;
String URL_Lumiere_Luminosite;


void setup() {
  Serial.begin(115200);
  Serial.println("------Cadre domotique------");
  Serveur.Debug(true);
  Serveur.Init();

  //Paramètres
  Serveur.AddParamText("Différence de température pour envoi", "0.5");
  Serveur.AddParamText("Différence de luminosité pour envoi", "10");
  Serveur.AddParamText("Sensibilité du tactile", "20");
  Serveur.AddParamText("URL Mouvement", "");
  Serveur.AddParamText("URL Température", "");
  Serveur.AddParamText("URL Luminosité", "");
  Serveur.AddParamText("URL Rechargement", "");
  Serveur.AddParamText("URL Lumière Etat", "");
  Serveur.AddParamText("URL Lumière Luminosité", "");
  
  Difference_Temperature_Parametree = Serveur.GetParamText("Différence de température pour envoi").toFloat();
  Difference_Luminosite_Parametree = Serveur.GetParamText("Différence de luminosité pour envoi").toFloat();
  Sensibilite_Tactile = (Serveur.GetParamText("Sensibilité du tactile")).toInt();
  URL_Mouvement = Serveur.GetParamText("URL Mouvement");
  URL_Temperature = Serveur.GetParamText("URL Température");
  URL_Luminosite = Serveur.GetParamText("URL Luminosité");
  URL_Rechargement = Serveur.GetParamText("URL Rechargement");
  URL_Lumiere_Etat = Serveur.GetParamText("URL Lumière Etat");
  URL_Lumiere_Luminosite = Serveur.GetParamText("URL Lumière Luminosité");

  ///////////////Mise en page de l'index///////////////
  //Infos
  Serveur.IndexBackgroundStart();
  Serveur.IndexAddSegWhite();
  Serveur.IndexAddTitle("Infos", "100");
  Serveur.IndexAddSegWhite();
  Info_Mouvement = Serveur.IndexAddInfo("Mouvement", "", "80");
  Info_Temperature = Serveur.IndexAddInfo("Température", "°C", "50");
  Info_Luminosite = Serveur.IndexAddInfo("Luminosité", "lux", "50");
  Etat_Lumiere = Serveur.IndexAddInfo("Leds", "", "50");
  int *Bouton_Rechargement = Serveur.IndexAddToogle("Rechargement", "Off", "On", "Off");
  Bouton_Rechargement_Off = Bouton_Rechargement[0];
  Bouton_Rechargement_On = Bouton_Rechargement[1];
  Serveur.IndexBackgroundFinish();

  //LEDS
  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Leds", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Off = Serveur.IndexAddButton("Off");
  Bouton_Lumiere_BC = Serveur.IndexAddButton("Blanc chaud");
  Bouton_Lumiere_BF = Serveur.IndexAddButton("Blanc froid");
  Range_Luminosite = Serveur.IndexAddRange("Luminosité", 0, 255, 200);

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Animations", "100");
  Serveur.IndexAddSeg();
  Bouton_Lumiere_Alarme = Serveur.IndexAddButton("Alarme");
  Bouton_Lumiere_Balayage = Serveur.IndexAddButton("Balayage");
  Bouton_Lumiere_Theatre = Serveur.IndexAddButton("Théatre");
  Bouton_Lumiere_Rainbow = Serveur.IndexAddButton("Arc-en-ciel");
  Bouton_Lumiere_TRainbow = Serveur.IndexAddButton("Arc-en-ciel 2");

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Couleurs", "100");
  Serveur.IndexAddSeg();
  Couleur_Lumiere_BC = Serveur.IndexAddColor("Blanc chaud", 255, 95, 10);
  Couleur_Lumiere_BF = Serveur.IndexAddColor("Blanc froid", 255, 255, 255);
  Couleur_Alarme = Serveur.IndexAddColor("Couleur alarme 1", 255, 0, 0);
  Couleur_Alarme_1 = Serveur.IndexAddColor("Couleur alarme 2", 50, 0, 0);
  Couleur_Balayage = Serveur.IndexAddColor("Couleur balayage 1", 60, 110, 200);
  Couleur_Balayage_1 = Serveur.IndexAddColor("Couleur balayage 2", 250, 120, 0);
  Couleur_Theatre = Serveur.IndexAddColor("Couleur théatre 1", 60, 110, 200);
  Couleur_Theatre_1 = Serveur.IndexAddColor("Couleur théatre 2", 250, 120, 0);

  Serveur.IndexAddSeg();

  ///////////////Fin de mise en page de l'index///////////////


  //RELAIS
  pinMode(RELAIS, OUTPUT);
  Relais_Off();

  //PIR
  pinMode(PIR, INPUT);

  //Sonde Dallas
  DS18B20.begin();

  //Luminosité
  Wire.begin();
  luxMetre.begin();

  //Strip
  strip.begin();
  strip.setBrightness(Serveur.IndexGetRange(Range_Luminosite));
  strip_Off();
  Serveur.IndexSetInfo(Etat_Lumiere, "Off");
  Switch_Animation("Off");  
  Post_Etat_Lumiere("Off");  
}


void loop() {

  //Gestion de la température et luminosité
  Moment_Present = millis();
  if ((Moment_Present - Moment_Memorise) > 30000 || (Memoire_Luminosite - Luminosite_Actuelle) >= Difference_Luminosite_Parametree || (Luminosite_Actuelle - Memoire_Luminosite) >= Difference_Luminosite_Parametree || (Memoire_Temperature - Temperature_Actuelle) >= Difference_Temperature_Parametree || (Temperature_Actuelle - Memoire_Temperature) >= Difference_Temperature_Parametree) {
    Moment_Memorise = Moment_Present;

    //Température
    DS18B20.requestTemperatures();
    Temperature_Actuelle = DS18B20.getTempCByIndex(0);
    if ((Memoire_Temperature - Temperature_Actuelle) >= Difference_Temperature_Parametree || (Temperature_Actuelle - Memoire_Temperature) >= Difference_Temperature_Parametree) {
      Memoire_Temperature = Temperature_Actuelle;
      Serveur.IndexSetInfo(Info_Temperature, String(Temperature_Actuelle));
      if (URL_Temperature != "") {
        String Temporaire = URL_Temperature;
        Temporaire.replace("#VALUE#", String(Temperature_Actuelle));
        Serveur.HTTPPost(Temporaire);
        delay(500);
      }
    }

    //Luminosité
    Luminosite_Actuelle = luxMetre.readLightLevel();
    if ((Memoire_Luminosite - Luminosite_Actuelle) >= Difference_Luminosite_Parametree || (Luminosite_Actuelle - Memoire_Luminosite) >= Difference_Luminosite_Parametree) {
      Memoire_Luminosite = Luminosite_Actuelle;
      Serveur.IndexSetInfo(Info_Luminosite, String(Luminosite_Actuelle));
      if (URL_Luminosite != "") {
        String Temporaire = URL_Luminosite;
        Temporaire.replace("#VALUE#", String(Luminosite_Actuelle));
        Serveur.HTTPPost(Temporaire);
        delay(500);
      }
    }
  }

  Event = Serveur.IndexGetEvent();

  if (Event == Bouton_Lumiere_Off) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Off");
    Switch_Animation("Off");
    strip_Off();
    Post_Etat_Lumiere("Off");

  } else if (Event == Bouton_Lumiere_BC) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Blanc chaud"); 
    Switch_Animation("Off");   
    Anim_Lumiere(strip.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BC), Serveur.IndexGetColor_G(Couleur_Lumiere_BC), Serveur.IndexGetColor_R(Couleur_Lumiere_BC)));
    Post_Etat_Lumiere("Blanc%20chaud");

  } else if (Event == Bouton_Lumiere_BF) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Blanc froid");
    Switch_Animation("Off");    
    Anim_Lumiere(strip.Color(Serveur.IndexGetColor_B(Couleur_Lumiere_BF), Serveur.IndexGetColor_G(Couleur_Lumiere_BF), Serveur.IndexGetColor_R(Couleur_Lumiere_BF)));
    Post_Etat_Lumiere("Blanc%20froid");

  } else if (Event == Bouton_Lumiere_Alarme) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Alarme");
    Switch_Animation("bool_Bouton_Lumiere_Alarme");
    Post_Etat_Lumiere("Alarme");

  } else if (Event == Bouton_Lumiere_Balayage) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Balayage");
    Switch_Animation("bool_Bouton_Lumiere_Balayage");
    Post_Etat_Lumiere("Balayage");

  } else if (Event == Bouton_Lumiere_Theatre) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Théatre");
    Switch_Animation("bool_Bouton_Lumiere_Theatre");
    Post_Etat_Lumiere("Théatre");

  } else if (Event == Bouton_Lumiere_Rainbow) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Arc-en-ciel");
    Switch_Animation("bool_Bouton_Lumiere_Rainbow");
    Post_Etat_Lumiere("Arc-en-ciel");

  } else if (Event == Bouton_Lumiere_TRainbow) {
    Serveur.IndexResetEvent();
    Serveur.IndexSetInfo(Etat_Lumiere, "Arc-en-ciel 2");
    Switch_Animation("bool_Bouton_Lumiere_TRainbow");
    Post_Etat_Lumiere("Arc-en-ciel%202");

  } else if (Event == Range_Luminosite) {    
    Serveur.IndexResetEvent();
    strip.setBrightness(Serveur.IndexGetRange(Range_Luminosite));
    Post_Etat_Luminosite(String(Serveur.IndexGetRange(Range_Luminosite)));
    strip.show();    

  }else if (Event == Bouton_Rechargement_Off) {
    //Gestion du relais depuis l'index
    Relais_Off();
  } else if (Event == Bouton_Rechargement_On) {
    Relais_On();
  }

  if (bool_Bouton_Lumiere_Alarme == true) {    
    Anim_Alarm(strip.Color(Serveur.IndexGetColor_B(Couleur_Alarme), Serveur.IndexGetColor_G(Couleur_Alarme), Serveur.IndexGetColor_R(Couleur_Alarme)), 50);
    Anim_Alarm(strip.Color(0, 0, 0), 50);
    Anim_UnAlarm(strip.Color(Serveur.IndexGetColor_B(Couleur_Alarme_1), Serveur.IndexGetColor_G(Couleur_Alarme_1), Serveur.IndexGetColor_R(Couleur_Alarme_1)), 50);
    Anim_UnAlarm(strip.Color(0, 0, 0), 50);

  } else if (bool_Bouton_Lumiere_Balayage == true) {    
    colorWipe(strip.Color(Serveur.IndexGetColor_B(Couleur_Balayage), Serveur.IndexGetColor_G(Couleur_Balayage), Serveur.IndexGetColor_R(Couleur_Balayage)), 25);
    colorWipe(strip.Color(Serveur.IndexGetColor_B(Couleur_Balayage_1), Serveur.IndexGetColor_G(Couleur_Balayage_1), Serveur.IndexGetColor_R(Couleur_Balayage_1)), 25);
    colorUnWipe(strip.Color(Serveur.IndexGetColor_B(Couleur_Balayage), Serveur.IndexGetColor_G(Couleur_Balayage), Serveur.IndexGetColor_R(Couleur_Balayage)), 25);
    colorUnWipe(strip.Color(Serveur.IndexGetColor_B(Couleur_Balayage_1), Serveur.IndexGetColor_G(Couleur_Balayage_1), Serveur.IndexGetColor_R(Couleur_Balayage_1)), 25);

  } else if (bool_Bouton_Lumiere_Theatre == true) {    
    theaterChase(strip.Color(Serveur.IndexGetColor_B(Couleur_Theatre), Serveur.IndexGetColor_G(Couleur_Theatre), Serveur.IndexGetColor_R(Couleur_Theatre)), 50);
    theaterChase(strip.Color(Serveur.IndexGetColor_B(Couleur_Theatre_1), Serveur.IndexGetColor_G(Couleur_Theatre_1), Serveur.IndexGetColor_R(Couleur_Theatre_1)), 50);

  } else if (bool_Bouton_Lumiere_Rainbow == true) {    
    rainbow(5);

  } else if (bool_Bouton_Lumiere_TRainbow == true) {    
    theaterChaseRainbow(50);

  }


  //PIR
  pirVal = digitalRead(PIR);
  if (pirVal == HIGH) {
    if (Etat_Mouvement == false) {
      Etat_Mouvement = true;
      Serveur.IndexSetInfo(Info_Mouvement, "0");
      if (URL_Mouvement != "") {
        String Temporaire = URL_Mouvement;
        Temporaire.replace("#VALUE#", "0");
        Serveur.HTTPPost(Temporaire);
        delay(500);
      }
    }
  } else {
    if (Etat_Mouvement == true) {
      Etat_Mouvement = false;
      Serveur.IndexSetInfo(Info_Mouvement, "1");
      if (URL_Mouvement != "") {
        String Temporaire = URL_Mouvement;
        Temporaire.replace("#VALUE#", "1");
        Serveur.HTTPPost(Temporaire);
        delay(500);
      }
    }
  }
}

void Post_Etat_Lumiere(String Etat) {
  if (URL_Lumiere_Etat != "") {
    String Temporaire = URL_Lumiere_Etat;
    Temporaire.replace("#VALUE#", Etat);
    Serveur.HTTPPost(Temporaire);
    delay(500);
  }
}

void Post_Etat_Luminosite(String Etat) {
if (URL_Lumiere_Luminosite != "") {
    String Temporaire = URL_Lumiere_Luminosite;
    Temporaire.replace("#VALUE#", Etat);
    Serveur.HTTPPost(Temporaire);
    delay(500);
  }
}
void Switch_Animation(String Bool_Bouton) {
  int var = -1;

  if (Bool_Bouton == "bool_Bouton_Lumiere_Alarme") var = 0;
  if (Bool_Bouton == "bool_Bouton_Lumiere_Rainbow") var = 1;
  if (Bool_Bouton == "bool_Bouton_Lumiere_Balayage") var = 2;
  if (Bool_Bouton == "bool_Bouton_Lumiere_Theatre") var = 3;
  if (Bool_Bouton == "bool_Bouton_Lumiere_TRainbow") var = 4;

  bool_Bouton_Lumiere_Alarme = false;
  bool_Bouton_Lumiere_Rainbow = false;
  bool_Bouton_Lumiere_Balayage = false;
  bool_Bouton_Lumiere_Theatre = false;
  bool_Bouton_Lumiere_TRainbow = false;

  switch (var) {
    case 0:
      bool_Bouton_Lumiere_Alarme = true;
      break;
    case 1:
      bool_Bouton_Lumiere_Rainbow = true;
      break;
    case 2:
      bool_Bouton_Lumiere_Balayage = true;
      break;
    case 3:
      bool_Bouton_Lumiere_Theatre = true;
      break;
    case 4:
      bool_Bouton_Lumiere_TRainbow = true;
      break;
  }
}


void Relais_Off() {
  digitalWrite(RELAIS, LOW);
  Serveur.IndexSetToogle(Bouton_Rechargement_Off);
  if (URL_Rechargement != "") {
    String Temporaire = URL_Rechargement;
    Temporaire.replace("#VALUE#", "0");
    Serveur.HTTPPost(Temporaire);
    delay(500);
  }
  Serveur.IndexResetEvent();
  Etat_Relais = false;
}

void Relais_On() {
  digitalWrite(RELAIS, HIGH);
  Serveur.IndexSetToogle(Bouton_Rechargement_On);
  if (URL_Rechargement != "") {
    String Temporaire = URL_Rechargement;
    Temporaire.replace("#VALUE#", "1");
    Serveur.HTTPPost(Temporaire);
    delay(500);
  }
  Serveur.IndexResetEvent();
  Etat_Relais = true;
}


//Animation des leds
void strip_Off() {
  strip.clear();
  strip.show();
}

void Anim_Lumiere(uint32_t color) {
  for (int i = 0; i <= 9; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void Anim_Alarm(uint32_t color, int wait) {
  for (int i = 0; i <= 4; i++) {
    strip.setPixelColor(4 + i, color);
    strip.setPixelColor(4 - i, color);
    strip.show();
    delay(wait);
  }
}

void Anim_UnAlarm(uint32_t color, int wait) {
  for (int i = 4; i >= 0; i--) {
    strip.setPixelColor(4 + i, color);
    strip.setPixelColor(4 - i, color);
    strip.show();
    delay(wait);
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}
void colorUnWipe(uint32_t color, int wait) {
  for (int i = strip.numPixels() - 1; i >= 0; i--) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void pulse(uint8_t wait) {
  for (int j = 0; j <= 255; j++) {
    strip.fill(strip.Color(strip.gamma8(j), strip.gamma8(j), strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for (int j = 255; j >= 0; j--) {
    strip.fill(strip.Color(strip.gamma8(j), strip.gamma8(j), strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel((i + j) % 255));
      }
      strip.show();
      delay(wait);
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}