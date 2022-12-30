





#include <SPI.h>
#include <TFT_eSPI.h>
#include <eeBudServerManager.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>



float PrixM3_eau = 1.97;
float factorK = 10.2;
int ServoAjustDeb = 2;
int ServoAjustTemp = 2;
float ToleranceTemp = 0.5;
float ToleranceDebit = 0.3;
int Debit_MesureInterval = 1000;
int DS_MesureInterval = 30000;
int Ajust_MesureInterval = 3000;
int ServoMin = 0;
int ServoMax = 90;
int ServoMinM = 4;
int ServoMaxM = 86;

float flow_Lmin = 0;


int MemAjust_MesureInterval = 3000;


const int Nb_Profils = 3;                                //Nombre de profils
char* Profil_Noms[] = { "Bain", "eeBud", "Mme eeBud" };  //Nom de profil
int Profil_Temp[] = { 38, 39, 37 };                      //Température des profils
int Profil_Flow[] = { 5, 3, 3 };                         //Débit des profils (L/min)
char* Profil_Mode[] = { "Bain", "Douche", "Douche" };    //Mode des profils

//Position approximative avant ajustement 30°C 31 32 ...
int ServoTemp[] = { 51, 52, 53, 54, 55, 56, 57, 58, 60, 61, 62 };

//Déclaration des pins

//DS18B20
const int Pin_Onewire = 27;

//TFT
const int AlimTFT = 22;

//Boutons
const int Bouton_Gauche = 33;
const int Bouton_Droite = 25;
const int Bouton_Select = 26;
const int Bouton_Back = 32;

//DebitMetre
const int sensorPin = 2;

//Servomoteurs
const int PIN_ServoDouche = 13;
const int PIN_ServoBain = 12;
const int PIN_ServoMitigeur = 16;

//Serveur
eeBudServerManager Serveur("eeBud - Mitigeur");
int Event;
int Boutons[Nb_Profils];
int BoutonStop;
int IndexFlow;
int IndexVolume;
int IndexCout;
int IndexTemp;
String APITemp;
String APIVol;
String APICout;
int IndexDebug;

//Déclaration des variables
//TFT
TFT_eSPI tft = TFT_eSPI();
unsigned long TFT_Moment_Present = 0;
unsigned long TFT_Moment_Memorise = 0;
int TFT_Interval = 10000;
int y;
int TFT_DebitValue;
int TFT_VolumeValue;
int TFT_PrixValue;
int TFT_Temperature;
int TFT_Demarrage;

//DebitMetre
volatile int pulseConter;
unsigned long Debit_Moment_Present = 0;
unsigned long Debit_Moment_Memorise = 0;
bool Debit_Mesure = false;
bool Affichage_Consommation = false;
float volume = 0;
long t0 = 0;

//Servos
Servo ServoDouche;
Servo ServoBain;
Servo ServoMitigeur;

//DS18B20
OneWire oneWire(Pin_Onewire);
DallasTemperature ds(&oneWire);
unsigned long DS_Moment_Present = 0;
unsigned long DS_Moment_Memorise = 0;
bool Affichage_Temperature = false;
bool DS_Mesure = false;
float TempMesuree = 0.0;

//AUTRE
String Mode_Debit = "Off";
String Menu = "Profil";
int Consigne_Temperature;
int Consigne_Flow;
String Consigne_Mode;

bool Ajust = false;
bool EC_Ajust = false;
unsigned long Ajust_Moment_Present = 0;
unsigned long Ajust_Moment_Memorise = 0;
int Profil_Position = 0;
bool VeilleTFT = false;
bool AutorVeilleTFT = true;
//int MemServoBain = 60;
//int MemServoDouche = 60;
int MemServo = 60;

int MemServoMitigeur = 45;
bool Etall = false;
int NCNb_Profils = 3;
float Prix;
bool PostResult = false;


int po = 0;

void setup() {

  Serial.begin(115200);
  Serial.println("------Mitigeur------");


  //TFT
  //Rétro éclairage TFT
  pinMode(AlimTFT, OUTPUT);
  tft.init();
  TFTOn();
  tft.setRotation(0);
  tft.fillScreen(TFT_WHITE);
  TftAddSeg(60, false);
  TftAdd("true", "Demarrage", 30, 2, "Bold");
  TftAdd("true", "du mitigeur", 30, 2, "Bold");
  TFT_Demarrage = TftAdd("false", "Initialisation", 30, 1, "Normal");
  TftAddSeg(90, false);
  TftAdd("false", "        By eeBud", 30, 1, "Oblic");

  Serveur.Debug(true);
  Serveur.Init();

  //SPIFFS Serveur
  Serveur.AddParamText("Prix m3 d'eau", "1.97");
  Serveur.AddParamText("Facteur de débit", "10.2");
  Serveur.AddParamText("Facteur ajustement débit", "2");
  Serveur.AddParamText("Facteur ajustement température", "2");
  Serveur.AddParamText("Tolérence de température", "0.5");
  Serveur.AddParamText("Tolérence de débit", "0.5");
  Serveur.AddParamText("Position min servo D", "2");
  Serveur.AddParamText("Position max servo D", "88");
  Serveur.AddParamText("Position min servo M", "4");
  Serveur.AddParamText("Position max servo M", "86");
  Serveur.AddParamText("Intervalle d'ajustement", "3000");
  Serveur.AddParamText("API Température", "");
  Serveur.AddParamText("API Volume", "");
  Serveur.AddParamText("API Coût", "");


  PrixM3_eau = Serveur.GetParamText("Prix m3 d'eau").toFloat();
  factorK = Serveur.GetParamText("Facteur de débit").toFloat();
  ServoAjustDeb = (Serveur.GetParamText("Facteur ajustement débit")).toInt();
  ServoAjustTemp = (Serveur.GetParamText("Facteur ajustement température")).toInt();
  ToleranceTemp = Serveur.GetParamText("Tolérence de température").toFloat();
  ToleranceDebit = Serveur.GetParamText("Tolérence de débit").toFloat();
  ServoMin = (Serveur.GetParamText("Position min servo D")).toInt();
  ServoMax = (Serveur.GetParamText("Position max servo D")).toInt();
  ServoMinM = (Serveur.GetParamText("Position min servo M")).toInt();
  ServoMaxM = (Serveur.GetParamText("Position max servo M")).toInt();
  Ajust_MesureInterval = (Serveur.GetParamText("Intervalle d'ajustement")).toInt();
  APITemp = Serveur.GetParamText("API Température");
  APIVol = Serveur.GetParamText("API Volume");
  APICout = Serveur.GetParamText("API Coût");

  MemServo = (ServoMax - ServoMin) / 2;
  MemServoMitigeur = ServoMin;

  //MEP Index
  Serveur.IndexBackgroundStart();
  Serveur.IndexAddSegWhite();
  Serveur.IndexAddTitle("Infos", "100");
  Serveur.IndexAddSegWhite();
  IndexTemp = Serveur.IndexAddInfo("Température", "°C", "50");
  IndexFlow = Serveur.IndexAddInfo("Débit", "L/min", "50");
  IndexVolume = Serveur.IndexAddInfo("Volume", "L", "50");
  IndexCout = Serveur.IndexAddInfo("Coût", "€", "80");
  IndexDebug = Serveur.IndexAddInfo("Debug", "", "10");
  Serveur.IndexBackgroundFinish();

  Serveur.IndexSetInfo(IndexFlow, "0");
  Serveur.IndexSetInfo(IndexVolume, "0");
  Serveur.IndexSetInfo(IndexCout, "0");
  Serveur.IndexSetInfo(IndexTemp, "N/A");


  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Profils", "100");
  Serveur.IndexAddSeg();

  BoutonStop = Serveur.IndexAddButton("Stop");
  for (byte i = 0; i < Nb_Profils; i++) {
    Boutons[i] = Serveur.IndexAddButton(Profil_Noms[i]);
  }



  //Servos
  ServoDouche.attach(PIN_ServoDouche);
  ServoBain.attach(PIN_ServoBain);
  ServoMitigeur.attach(PIN_ServoMitigeur);
  ServoDouche.write(ServoMax);
  ServoBain.write(ServoMax);
  ServoMitigeur.write(ServoMin);


  //Débit Mètre
  attachInterrupt(digitalPinToInterrupt(sensorPin), ISRCountPulse, RISING);
  t0 = millis();



  //TFT
  TftUpdate(TFT_Demarrage, false, "Configuration des pins", 25, 1, "Normal");
  //delay(1000);

  //Boutons
  pinMode(Bouton_Gauche, INPUT_PULLUP);
  pinMode(Bouton_Droite, INPUT_PULLUP);
  pinMode(Bouton_Select, INPUT_PULLUP);
  pinMode(Bouton_Back, INPUT_PULLUP);



  //DS18B20
  ds.begin();

  NCNb_Profils = Nb_Profils - 1;
  //TFT
  TftUpdate(TFT_Demarrage, false, "Bienvenue!", 25, 1, "Normal");
  delay(2000);
  tft.fillScreen(TFT_WHITE);
  MenuProfil(0);
  TftMenu(true, "", "OK", true);
  TFT_Moment_Memorise = millis();
}



void loop(void) {
  Event = Serveur.IndexGetEvent();

  for (byte i = 0; i < Nb_Profils; i++) {
    if (Event == Boutons[i]) {
      TFTOn();
      Profil_Position = i;
      TFT_Interval = 60000;
      AutorVeilleTFT = false;
      Menu = "Mode";
      Consigne_Temperature = Profil_Temp[Profil_Position];
      Consigne_Flow = Profil_Flow[Profil_Position];
      Consigne_Mode = Profil_Mode[Profil_Position];
      MenuProfilSelect(Profil_Position);
      Serveur.IndexResetEvent();
    }
  }
  if (Event == BoutonStop) {
    Menu = "Profil";
    Profil_Position = 0;
    Gestion_Debit_Eau(0);
    MenuProfil(0);
    TftMenu(true, "", "OK", true);
    Serveur.IndexResetEvent();
  }

  TFT_Moment_Present = millis();
  if (TFT_Moment_Present - TFT_Moment_Memorise >= TFT_Interval && AutorVeilleTFT == true) {
    TFT_Moment_Memorise = millis();
    TFTOff();
    TFT_Interval = 10000;
    if (PostResult == false) {
      PostResult = true;
      Post(APIVol, String(volume));
      Post(APICout, String(Prix));
      Serveur.IndexSetInfo(IndexFlow, "0");
      //Serveur.IndexSetInfo(IndexVolume, "0");
      //Serveur.IndexSetInfo(IndexCout, "0");
      Serveur.IndexSetInfo(IndexTemp, "N/A");
      volume = 0;
      Prix = 0;
    }
  }
  if (!AutorVeilleTFT) TFT_Moment_Memorise = millis();

  if (Affichage_Consommation) GetDebit();
  if (Affichage_Temperature) GetTemp();
  if (Ajust) Adjust();

  if (VeilleTFT) {
    if (digitalRead(Bouton_Droite) == 0 || digitalRead(Bouton_Gauche) == 0 || digitalRead(Bouton_Select) == 0 || digitalRead(Bouton_Back) == 0) {
      TFTOn();
      TFT_Moment_Memorise = millis();
      delay(500);
    }
  } else {
    if (digitalRead(Bouton_Droite) == 0 || digitalRead(Bouton_Gauche) == 0 || digitalRead(Bouton_Select) == 0 || digitalRead(Bouton_Back) == 0) {
      TFT_Moment_Memorise = millis();
    }
    if (Menu == "Profil") {

      if (digitalRead(Bouton_Droite) == 0) {
        if (Profil_Position < NCNb_Profils) {
          Profil_Position++;
          MenuProfil(Profil_Position);

          if (Profil_Position == NCNb_Profils) {
            TftMenu(true, "", "OK", false);
          } else {
            TftMenu(true, "", "OK", true);
          }
          Consigne_Temperature = Profil_Temp[Profil_Position];
          Consigne_Flow = Profil_Flow[Profil_Position];
        }
        delay(500);
      } else if (digitalRead(Bouton_Gauche) == 0) {
        if (Profil_Position > 0) {
          Profil_Position--;
          MenuProfil(Profil_Position);
          TftMenu(true, "", "OK", true);
          Consigne_Temperature = Profil_Temp[Profil_Position];
          Consigne_Flow = Profil_Flow[Profil_Position];
        } else if (Profil_Position == 0) {
          Profil_Position--;
          MenuConsommation();
          TftMenu(false, "", "OK", true);
        } else if (Profil_Position == -1) {
          Profil_Position = 0;
          MenuProfil(Profil_Position);
          TftMenu(true, "", "OK", true);
          Consigne_Temperature = Profil_Temp[Profil_Position];
          Consigne_Flow = Profil_Flow[Profil_Position];
        }
        delay(500);
      } else if (digitalRead(Bouton_Select) == 0) {  // SELECTION DU PROFIL ET OUVERTURE DE L'EAU
        if (Profil_Position >= 0) {
          TFT_Interval = 60000;
          AutorVeilleTFT = false;
          Menu = "Mode";
          Consigne_Temperature = Profil_Temp[Profil_Position];
          Consigne_Flow = Profil_Flow[Profil_Position];
          Consigne_Mode = Profil_Mode[Profil_Position];
          MenuProfilSelect(Profil_Position);

          delay(500);
        } else {

          po = po + 2;
          EtalTemp();
          delay(500);
        }
      } else if (digitalRead(Bouton_Back) == 0) {
        if (Profil_Position == -1) {
          if (Etall) {
            Etall = false;
            Gestion_Debit_Eau(0);
          } else {
            Etall = true;
            Etal();
          }
          delay(500);
        }
      }
    } else if (Menu == "Debit") {
      if (digitalRead(Bouton_Droite) == 0) {
        Consigne_Flow++;

        MenuProfilSelect(Profil_Position);
        delay(500);
      } else if (digitalRead(Bouton_Gauche) == 0) {
        Consigne_Flow--;
        MenuProfilSelect(Profil_Position);
        delay(500);
      } else if (digitalRead(Bouton_Select) == 0) {
        Menu = "Temp";
        MenuProfilSelect(Profil_Position);

        delay(500);
      } else if (digitalRead(Bouton_Back) == 0) {
        Menu = "Profil";
        Profil_Position = 0;
        Gestion_Debit_Eau(0);
        MenuProfil(0);
        TftMenu(true, "", "OK", true);
        delay(500);
      }
    } else if (Menu == "Temp") {
      if (digitalRead(Bouton_Droite) == 0) {
        Consigne_Temperature++;

        MenuProfilSelect(Profil_Position);
        delay(500);
      } else if (digitalRead(Bouton_Gauche) == 0) {
        Consigne_Temperature--;
        MenuProfilSelect(Profil_Position);
        delay(500);
      } else if (digitalRead(Bouton_Select) == 0) {
        Menu = "Debit";
        MenuProfilSelect(Profil_Position);

        delay(500);
      } else if (digitalRead(Bouton_Back) == 0) {
        Menu = "Profil";
        Profil_Position = 0;
        Gestion_Debit_Eau(0);
        MenuProfil(0);
        TftMenu(true, "", "OK", true);
        delay(500);
      }
    } else if (Menu == "Mode") {

      if (digitalRead(Bouton_Droite) == 0) {
        if (Consigne_Mode == "Douche") {
          Consigne_Mode = "Bain";
        } else {
          Consigne_Mode = "Douche";
        }

        MenuProfilSelect(Profil_Position);
        delay(500);
      } else if (digitalRead(Bouton_Gauche) == 0) {
        if (Consigne_Mode == "Douche") {
          Consigne_Mode = "Bain";
        } else {
          Consigne_Mode = "Douche";
        }

        MenuProfilSelect(Profil_Position);
        delay(500);
      } else if (digitalRead(Bouton_Select) == 0) {
        Menu = "Temp";
        MenuProfilSelect(Profil_Position);

        delay(500);
      } else if (digitalRead(Bouton_Back) == 0) {
        Menu = "Profil";
        Profil_Position = 0;
        Gestion_Debit_Eau(0);
        MenuProfil(0);
        TftMenu(true, "", "OK", true);
        delay(500);
      }
    }
  }
}

void TftMenu(bool Left, String Back, String Select, bool Right) {
  tft.fillRect(0, 300, 240, 40, TFT_WHITE);
  if (Left == true) tft.fillRect(1, 300, 48, 20, TFT_CYAN);
  if (Back != "") tft.fillRect(51, 300, 68, 20, TFT_CYAN);
  if (Select != "") tft.fillRect(121, 300, 68, 20, TFT_CYAN);
  if (Right == true) tft.fillRect(191, 300, 48, 20, TFT_CYAN);
  tft.setTextSize(1);
  tft.setFreeFont(&FreeMonoOblique9pt7b);
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  if (Left == true) tft.drawString("<-", 25, 310);
  if (Back != "") tft.drawString(Back, 85, 310);
  if (Select != "") tft.drawString(Select, 155, 310);
  if (Right == true) tft.drawString("->", 215, 310);
}

int TftAdd(String Background, String Text, int height, int Siz, String FontStyle) {
  if (Background == "true") {
    tft.fillRect(0, y, 240, height, TFT_CYAN);
  } else if (Background == "false") {
    tft.fillRect(0, y, 240, height, TFT_WHITE);
  } else if (Background == "select") {
    tft.fillRect(0, y, 240, height, TFT_GOLD);
  }
  tft.setTextSize(1);
  if (FontStyle == "Normal") {
    if (Siz == 1) {
      tft.setFreeFont(&FreeMono9pt7b);
    } else if (Siz == 2) {
      tft.setFreeFont(&FreeMono12pt7b);
    } else if (Siz == 3) {
      tft.setFreeFont(&FreeMono18pt7b);
    }
  } else if (FontStyle == "Bold") {
    if (Siz == 1) {
      tft.setFreeFont(&FreeMonoBold9pt7b);
    } else if (Siz == 2) {
      tft.setFreeFont(&FreeMonoBold12pt7b);
    } else if (Siz == 3) {
      tft.setFreeFont(&FreeMonoBold18pt7b);
    }
  } else if (FontStyle == "Italic") {
    if (Siz == 1) {
      tft.setFreeFont(&FreeMonoOblique9pt7b);
    } else if (Siz == 2) {
      tft.setFreeFont(&FreeMonoOblique12pt7b);
    } else if (Siz == 3) {
      tft.setFreeFont(&FreeMonoOblique18pt7b);
    }
  }
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(Text, 120, y + (height / 2));
  y = y + height;
  return y - height;
}

void TftUpdate(int Top, bool Background, String Text, int height, int Siz, String FontStyle) {
  if (Background == true) {
    tft.fillRect(0, Top, 240, height, TFT_CYAN);
  } else {
    tft.fillRect(0, Top, 240, height, TFT_WHITE);
  }
  tft.setTextSize(1);
  if (FontStyle == "Normal") {
    if (Siz == 1) {
      tft.setFreeFont(&FreeMono9pt7b);
    } else if (Siz == 2) {
      tft.setFreeFont(&FreeMono12pt7b);
    } else if (Siz == 3) {
      tft.setFreeFont(&FreeMono18pt7b);
    }
  } else if (FontStyle == "Bold") {
    if (Siz == 1) {
      tft.setFreeFont(&FreeMonoBold9pt7b);
    } else if (Siz == 2) {
      tft.setFreeFont(&FreeMonoBold12pt7b);
    } else if (Siz == 3) {
      tft.setFreeFont(&FreeMonoBold18pt7b);
    }
  } else if (FontStyle == "Italic") {
    if (Siz == 1) {
      tft.setFreeFont(&FreeMonoOblique9pt7b);
    } else if (Siz == 2) {
      tft.setFreeFont(&FreeMonoOblique12pt7b);
    } else if (Siz == 3) {
      tft.setFreeFont(&FreeMonoOblique18pt7b);
    }
  }
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(Text, 120, Top + (height / 2));
}




void MenuConsommation() {
  y = 0;
  volume = 0;
  Debit_Mesure = false;
  tft.fillScreen(TFT_WHITE);
  TftAdd("true", "Consommation", 30, 2, "Bold");
  TftAdd("true", "d'eau", 30, 2, "Bold");
  TFT_DebitValue = TftAdd("false", "0.00 L/min", 25, 2, "Normal");
  TFT_VolumeValue = TftAdd("false", "0.00 Litres", 25, 2, "Normal");
  TFT_PrixValue = TftAdd("false", "0.00 Euros", 25, 2, "Normal");
  TftAddSeg(2, true);
  TFT_Temperature = TftAdd("false", "0.00 Degres", 25, 2, "Normal");
  TftAddSeg(2, true);
  Affichage_Consommation = true;
  Affichage_Temperature = true;
}

void MenuProfil(int Profil) {
  y = 0;
  tft.fillScreen(TFT_WHITE);
  TftAdd("true", "Selection", 30, 2, "Bold");
  TftAdd("true", "du profil", 30, 2, "Bold");
  TftAddSeg(30, false);
  TftAdd("select", Profil_Noms[Profil], 30, 2, "Normal");
  TftAdd("select", String(Profil_Temp[Profil]) + " Degres", 30, 2, "Normal");
  TftAdd("select", "Debit: " + String(Profil_Flow[Profil]), 30, 2, "Normal");
  Affichage_Consommation = false;
  Affichage_Temperature = false;
  Ajust = false;
}

void MenuProfilSelect(int Profil) {
  y = 0;
  tft.fillScreen(TFT_WHITE);
  TftAdd("true", Profil_Noms[Profil], 30, 2, "Bold");
  TftAddSeg(5, false);
  if (Menu == "Debit") {
    TftAdd("select", "Debit: " + String(Consigne_Flow) + "L/min", 25, 2, "Normal");
  } else {
    TftAdd("true", "Debit: " + String(Consigne_Flow) + "L/min", 25, 2, "Normal");
  }
  if (Menu == "Temp") {
    TftAdd("select", "Consigne: " + String(Consigne_Temperature) + "oC", 25, 2, "Normal");
  } else {
    TftAdd("true", "Consigne: " + String(Consigne_Temperature) + "oC", 25, 2, "Normal");
  }
  if (Menu == "Mode") {
    TftAdd("select", "Mode: " + Consigne_Mode, 25, 2, "Normal");
  } else {
    TftAdd("true", "Mode: " + Consigne_Mode, 25, 2, "Normal");
  }
  TftAddSeg(10, false);
  TftAddSeg(2, true);
  TftAdd("false", "Mesures", 25, 2, "Normal");
  TftAddSeg(2, true);
  TFT_Temperature = TftAdd("false", "0.00 Degres", 25, 2, "Normal");


  TFT_DebitValue = TftAdd("false", "0.00 L/min", 25, 2, "Normal");
  TFT_VolumeValue = TftAdd("false", "0.00 Litres", 25, 2, "Normal");
  TFT_PrixValue = TftAdd("false", "0.00 Euros", 25, 2, "Normal");
  Affichage_Consommation = true;
  Affichage_Temperature = true;
  Ajust = true;
  if (Menu == "Debit") {
    TftMenu(true, "Off", "Mode", true);
  } else if (Menu == "Temp") {
    TftMenu(true, "Off", "Debit", true);
  } else if (Menu == "Mode") {
    TftMenu(true, "Off", "Temp", true);
  }
  PostResult = false;
}

void TFTOn() {
  digitalWrite(AlimTFT, HIGH);
  VeilleTFT = false;
}

void TFTOff() {
  digitalWrite(AlimTFT, LOW);
  VeilleTFT = true;
}

void ISRCountPulse() {
  pulseConter++;
}

void GetDebit() {
  if (Debit_Mesure == false) {
    pulseConter = 0;
    Debit_Mesure = true;
    Debit_Moment_Memorise = millis();
  }

  Debit_Moment_Present = millis();
  if (Debit_Moment_Present - Debit_Moment_Memorise >= Debit_MesureInterval) {
    Debit_Mesure = false;
    Debit_Moment_Present = millis();
    float Hz = (float)pulseConter * 1000 / (Debit_Moment_Present - Debit_Moment_Memorise);
    flow_Lmin = Hz / factorK;
    volume += flow_Lmin / 60 * (millis() - t0) / 1000.0;
    Prix = (PrixM3_eau / 1000) * volume;
    t0 = millis();
    TftUpdate(TFT_DebitValue, false, String(flow_Lmin) + " L/min", 25, 2, "Normal");
    TftUpdate(TFT_VolumeValue, false, String(volume) + " Litres", 25, 2, "Normal");
    TftUpdate(TFT_PrixValue, false, String(Prix) + " Euros", 25, 2, "Normal");

    Serveur.IndexSetInfo(IndexFlow, String(flow_Lmin));
    Serveur.IndexSetInfo(IndexVolume, String(volume));
    Serveur.IndexSetInfo(IndexCout, String(Prix));
  }
}

void SumVolume(float dV) {
  volume += dV / 60 * (millis() - t0) / 1000.0;
  t0 = millis();
}
void TftAddSeg(int height, bool Background) {
  if (Background == true) {
    tft.fillRect(0, y, 240, height, TFT_CYAN);
  } else {
    tft.fillRect(0, y, 240, height, TFT_WHITE);
  }
  y = y + height;
}



void GetTemp() {
  if (DS_Mesure == false) {

    DS_Mesure = true;
    DS_Moment_Memorise = millis();
  }

  DS_Moment_Present = millis();
  if (DS_Moment_Present - DS_Moment_Memorise >= DS_MesureInterval) {
    DS_Mesure = false;

    ds.requestTemperatures();
    TempMesuree = ds.getTempCByIndex(0);

    TftUpdate(TFT_Temperature, false, String(TempMesuree) + " Degres", 25, 2, "Normal");
    Serveur.IndexSetInfo(IndexTemp, String(TempMesuree));
    Post(APITemp, String(TempMesuree));
  }
}







void Gestion_Debit_Eau(int Debit) {
  MemServo = ServoDouche.read();
  if (ServoBain.read() < MemServo) MemServo = ServoBain.read();
  MemServoMitigeur = ServoMitigeur.read();
  if (Debit == 0) {
    ServoDouche.write(ServoMax);
    ServoBain.write(ServoMax);
    EC_Ajust = false;
    AutorVeilleTFT = true;
  }
}




float TempMem = 0;
float TempEC = 0;
String Evo = "null";


void Adjust() {

  if (EC_Ajust == false) {

    EC_Ajust = true;
    ServoMitigeur.write(ServoTemp[Consigne_Temperature - 30]);
    if (Consigne_Mode == "Douche") {
      ServoDouche.write(MemServo);

    } else {
      ServoBain.write(MemServo);
    }
    Ajust_Moment_Memorise = millis();

    ds.requestTemperatures();
    TempMesuree = ds.getTempCByIndex(0);
    TempMem = TempMesuree;
    TftUpdate(TFT_Temperature, false, String(TempMesuree) + " Degres", 25, 2, "Normal");
    Serveur.IndexSetInfo(IndexTemp, String(TempMesuree));

  } else {




    Ajust_Moment_Present = millis();
    if (Ajust_Moment_Present - Ajust_Moment_Memorise >= Ajust_MesureInterval) {
      Ajust_Moment_Memorise = millis();

      ds.requestTemperatures();
      TempMesuree = ds.getTempCByIndex(0);
      TftUpdate(TFT_Temperature, false, String(TempMesuree) + " Degres", 25, 2, "Normal");
      Serveur.IndexSetInfo(IndexTemp, String(TempMesuree));

      TempEC = TempMesuree - TempMem;
      TempMem = TempMesuree;


      if (flow_Lmin < (Consigne_Flow - ToleranceDebit)) {  //Pas assez de débit
        DebitPlus();

      } else if (flow_Lmin > Consigne_Flow + ToleranceDebit) {  //Trop de débit
        DebitMoins();
      }



      //if (abs(TempMesuree - Consigne_Temperature) < 2) {
      if (TempMesuree > Consigne_Temperature + ToleranceTemp) {  //Trop chaud
        if (TempEC > -0.3) 
        TempMoins();



      } else if (TempMesuree < Consigne_Temperature - ToleranceTemp) {  //Trop Froid
        if (TempEC < 0.3) 
        TempPlus();
      }
      //} else {
      //ServoMitigeur.write(ServoTemp[Consigne_Temperature - 30]);
      //}
    }
  }
}



void TempMoins() {
  int PositionDemandee = ServoMitigeur.read() - ServoAjustTemp;
  Serial.println(ServoMitigeur.read());
  if (ServoMinM <= PositionDemandee) {
    ServoMitigeur.write(PositionDemandee);
     Serveur.IndexSetInfo(IndexDebug, "plus+");
  }
}
void TempPlus() {
  int PositionDemandee = ServoMitigeur.read() + ServoAjustTemp;
  Serial.println(ServoMitigeur.read());
  if (ServoMaxM >= PositionDemandee) {
    ServoMitigeur.write(PositionDemandee);
    Serveur.IndexSetInfo(IndexDebug, "moins-");
  }
}


void DebitPlus() {
  if (Consigne_Mode == "Douche") {
    int PositionDemandee = ServoDouche.read() - ServoAjustDeb;
    if (ServoMin <= PositionDemandee) {
      ServoDouche.write(PositionDemandee);
      ServoBain.write(ServoMax);
      
    }
  } else {
    int PositionDemandee = ServoBain.read() - ServoAjustDeb;
    if (ServoMin <= PositionDemandee) {
      ServoBain.write(PositionDemandee);
      ServoDouche.write(ServoMax);
    }
  }
}

void DebitMoins() {
  if (Consigne_Mode == "Douche") {
    int PositionDemandee = ServoDouche.read() + ServoAjustDeb;
    if (ServoMax >= PositionDemandee) {
      ServoDouche.write(PositionDemandee);
      ServoBain.write(ServoMax);
    }
  } else {
    int PositionDemandee = ServoBain.read() + ServoAjustDeb;
    if (ServoMax >= PositionDemandee) {
      ServoBain.write(PositionDemandee);
      ServoDouche.write(ServoMax);
    }
  }
}

void Etal() {
  ServoDouche.write(ServoMin);
  ServoMitigeur.write((ServoMaxM - ServoMinM) / 2);
}


void EtalTemp() {

  ServoDouche.write(55);
  ServoMitigeur.write(po);

  Serial.println(String(po));
}

void Post(String URL, String Val) {
  if (URL != "") {
    String Temporaire = URL;
    Temporaire.replace("#VALUE#", Val);
    Serial.println("Post: " + Temporaire);
    //Serveur.IndexSetInfo(IndexDebug, "Post: " + Temporaire);
    Serveur.HTTPPost(Temporaire);
    delay(1000);
    //Serveur.IndexSetInfo(IndexDebug, "Effectué: " + Temporaire);
  }
}
