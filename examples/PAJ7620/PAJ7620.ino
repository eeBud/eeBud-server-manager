#include <eeBudServerManager.h>
#include <DFRobot_PAJ7620U2.h>

eeBudServerManager Serveur("eeBud - Contrôleur gestuel");

int Bouton_Veille;
int Info_Gestuel;
String URL_Gestuel;

DFRobot_PAJ7620U2 paj;

void setup() {

  Serveur.Init(); 
  Serveur.AddParamText("URL Gestuel", "");
  Info_Gestuel = Serveur.IndexAddInfo("Geste", "", "50");  
  URL_Gestuel = Serveur.GetParamText("URL Gestuel");

  while(paj.begin() != 0){    
    delay(500);
  }
  paj.setGestureHighRate(true);
 
}


void loop() {
  Serveur.IndexGetEvent();

  DFRobot_PAJ7620U2::eGesture_t Geste = paj.getGesture();
  if (Geste != paj.eGestureNone) {

    String S_Geste = paj.gestureDescription(Geste);
    
    if (URL_Gestuel != "") {
      String Temp = URL_Gestuel;
      Temp.replace("#VALUE#", String(S_Geste));
      Serveur.HTTPPost(Temp);
      delay(500);
    }

    Serveur.IndexSetInfo(Info_Gestuel, S_Geste);    
    
  }
  
}

