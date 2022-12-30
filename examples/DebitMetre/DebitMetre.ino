
#include <eeBudServerManager.h>

float PrixM3_eau = 1.97;
float factorK = 10.2;
int Debit_MesureInterval = 2000;
float flow_Lmin = 0;

//Serveur
eeBudServerManager Serveur("eeBud - Débitmètre");
int Event;
String APIVol;
String APICout;
int IndexFlow;
int IndexVolume;
int IndexCout;
int Bouton_Off;
int Bouton_On;
int Bouton_Reset;

//DebitMetre
const int sensorPin = 2;
volatile int pulseConter;
unsigned long Debit_Moment_Present = 0;
unsigned long Debit_Moment_Memorise = 0;
float volume = 0;
float Prix;
long t0 = 0;
bool Debit_Mesure = false;
bool Actif = true;

void setup() {
  
  Serveur.Init();
  
  //SPIFFS Serveur - Sauvegarde des données
  Serveur.AddParamText("Prix m3 d'eau", "1.97");
  Serveur.AddParamText("Facteur", "10.2");
  Serveur.AddParamText("Intervalle de mesure", "2000");
  Serveur.AddParamText("API Volume", "");
  Serveur.AddParamText("API Coût", "");

  //Récupération des données dans des variables
  PrixM3_eau = Serveur.GetParamText("Prix m3 d'eau").toFloat();
  factorK = Serveur.GetParamText("Facteur").toFloat();
  Debit_MesureInterval = (Serveur.GetParamText("Intervalle de mesure")).toInt();
  APIVol = Serveur.GetParamText("API Volume");
  APICout = Serveur.GetParamText("API Coût");

  //Mise en page de l'index
  Serveur.IndexBackgroundStart();
  Serveur.IndexAddSegWhite();
  Serveur.IndexAddTitle("Infos", "100");
  Serveur.IndexAddSegWhite();
  IndexFlow = Serveur.IndexAddInfo("Débit", "L/min", "50");
  IndexVolume = Serveur.IndexAddInfo("Volume", "L", "50");
  IndexCout = Serveur.IndexAddInfo("Coût", "€", "80");
  Serveur.IndexBackgroundFinish();

  Serveur.IndexAddSeg();
  Serveur.IndexAddTitle("Commandes", "100");
  Serveur.IndexAddSeg();
  int *Bouton = Serveur.IndexAddToogle("Actif", "Off", "On", "On");
  Bouton_Off = Bouton[0];
  Bouton_On = Bouton[1];
  Bouton_Reset = Serveur.IndexAddButton("Reset");

  Serveur.IndexSetInfo(IndexFlow, "0");
  Serveur.IndexSetInfo(IndexVolume, "0");
  Serveur.IndexSetInfo(IndexCout, "0");

  //Débit Mètre
  attachInterrupt(digitalPinToInterrupt(sensorPin), ISRCountPulse, RISING);
  t0 = millis();
}

void loop() {
  Event = Serveur.IndexGetEvent();

  if (Actif) GetDebit();

  if (Event == Bouton_Off) {
    Actif = false;
    Serveur.IndexResetEvent();
    Serveur.IndexSetToogle(Bouton_Off);

  } else if (Event == Bouton_On) {
    Actif = true;
    Serveur.IndexResetEvent();
    Serveur.IndexSetToogle(Bouton_On);
  } else if (Event == Bouton_Reset) {
    Serveur.IndexResetEvent();
    Raz();
  }
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
    
    Serveur.IndexSetInfo(IndexFlow, String(flow_Lmin));
    Serveur.IndexSetInfo(IndexVolume, String(volume));
    Serveur.IndexSetInfo(IndexCout, String(Prix));


  }
}

void Raz() {
  Post(APIVol, String(volume));
  Post(APICout, String(Prix));
  volume = 0;
  Prix = 0;
  Serveur.IndexSetInfo(IndexFlow, "0.00");
  Serveur.IndexSetInfo(IndexVolume, "0.00");
  Serveur.IndexSetInfo(IndexCout, "0.00");
}

void Post(String URL, String Val) {
  if (URL != "") {
    String Temporaire = URL;
    Temporaire.replace("#VALUE#", Val);
    Serveur.HTTPPost(Temporaire);
    delay(500);
  }
}
