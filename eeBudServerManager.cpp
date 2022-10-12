#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <DNSServer.h>

#include "eeBudServerManager.h"
#include "SPIFFS_Process.h"
#include "HTML_Config.h"
#include "HTML_ManageurWiFi.h"
#include "HTML_ManageurWiFiSansR.h"
#include "HTML_Parametres.h"
#include "HTML_Index.h"
#include "HTML_Sys.h"
#include "HTML_ReseauxEnreg.h"
#include "HTML_Message.h"
#include "HTML_ConfigAP.h"
#include "HTML_Donnees.h"
#include "HTML_ModificationReseau.h"
#include "HTML_Key.h"
#include "HTML_Enregistrement.h"

char* NomProg;

String StringAdresseIP;
int TentativesReco = 0;
bool ConnexionPerdue = false;

//Async
AsyncWebSocketClient* globalClient = NULL;
IPAddress localIP;
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
AsyncWebServer server(80);
AsyncEventSource events("/events");
String ScanReseaux[10];
long MomentActuel_Surv = 0;
long MomentPrecedent_Surv = 0;
String TITRE = "";
String MESSAGE = "";
String IDNetwork = "";
int IDAction = 0;
bool DNS = false;
DNSServer dnsServer;

//Json Action
DynamicJsonDocument SPIFFS_Config_Actions(1024);
int Action = -1;

String ScriptConstruct = "";
bool ModeAP = false;
bool Scan = false;
unsigned long previousMillis = 0;
const long timeout = 10000;

class GestionnairePortailCaptif : public AsyncWebHandler {
public:
  GestionnairePortailCaptif() {}
  virtual ~GestionnairePortailCaptif() {}
  bool canHandle(AsyncWebServerRequest* request) {
    request->addInterestingHeader("ANY");
    return true;
  }
  void handleRequest(AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", Page_Key, RetourVariable);
  }
  static String RetourVariable(String var) {
    String RETOUR = "Null";
    if (var == "SCANRESEAUX") {
      RETOUR = "";
      for (int j = 0; j < 10; j++) {
        if (ScanReseaux[j] != "") {
          RETOUR += "<button onclick=\"Click('" + ScanReseaux[j] + "')\"class=\"reseau\" >" + ScanReseaux[j] + "</button>";
        }
        delay(10);
      }
    }

    if (var == "APP") RETOUR = NomProg;
    if (var == "KEY") RETOUR = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();
    if (var == "SSID") {
      if (SPIFFS_SSID == "null") {
        RETOUR = "";
      } else {
        RETOUR = SPIFFS_SSID;
      }
    }

    if (var == "IP") {
      if (SPIFFS_IP == "null") {
        RETOUR = "";
      } else {
        RETOUR = SPIFFS_IP;
      }
    }

    return RETOUR;
  }
};
void eeBudServerManager::Debug(bool Value) {
  _Debug = Value;
}
void eeBudServerManager::ServeurDNS() {

  if (_Debug == true) Serial.println("eeBudServerManager:  Démarrage du serveur DNS");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(NomProg);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", Page_Key, RetourVariable);
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* p = request->getParam(0);
    if (String(p->value().c_str()) == "wifi") {
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          if (p->name() == "ssid") SPIFFS_SSID = p->value().c_str();
          if (p->name() == "pass") SPIFFS_Pass = p->value().c_str();
          if (p->name() == "ip") SPIFFS_IP = p->value().c_str();
        }
        delay(10);
      }
      if (SPIFFS_EnregistrementReseau()) {
        if (ModeAP == false) {
          request->send_P(200, "text/html", Page_Config, RetourVariable);

        } else {
          request->send_P(200, "text/html", Page_Enregistrement, RetourVariable);
          delay(1000);
          ESP.restart();
        }

      } else {
        request->send(200, "text/plain", "Erreur d'écriture de la configuration...");
      }
    } else if (String(p->value().c_str()) == "key") {
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          if (p->name() == "key") SPIFFS_Pass_AP = p->value().c_str();
        }
        delay(10);
      }
      if (SPIFFS_EnregistrementAP()) {
        request->send_P(200, "text/html", Page_ManageurWiFiSansR, RetourVariable);
      } else {
        request->send(200, "text/plain", "Erreur d'écriture de la configuration...");
      }
    }
  });

  server.on("/load", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* p = request->getParam(0);

    if (String(p->name().c_str()) == "nav") {
      if (String(p->value().c_str()) == "wifisr") {
        request->send_P(200, "text/html", Page_ManageurWiFiSansR, RetourVariable);
      } else if (String(p->value().c_str()) == "key") {
        request->send_P(200, "text/html", Page_Key, RetourVariable);
      } else if (String(p->value().c_str()) == "reboot") {
        SPIFFS_Actif_AP = "1";
        if (SPIFFS_EnregistrementAP()) {
          TITRE = "Enregistrement terminé";
          MESSAGE = "Redémarrage de l'appareil en point d'accès...";
          request->send_P(200, "text/html", Page_Message, RetourVariable);
          delay(1000);
          ESP.restart();
        }
      }
    }
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new GestionnairePortailCaptif()).setFilter(ON_AP_FILTER);
  DNS = true;
  server.begin();
}


void eeBudServerManager::ServeurStation() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String SPass = SPIFFS_Pass_AP;
    char CPass[50];
    SPass.toCharArray(CPass, 50);
    if (!request->authenticate("admin", CPass))
      return request->requestAuthentication();
    request->send_P(200, "text/html", Page_Index, RetourVariable);
  });
  server.serveStatic("/", SPIFFS, "/");

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* p = request->getParam(0);
    if (String(p->name().c_str()) == "send") {
      Action = String(p->value().c_str()).toInt();
    }
  });


  server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* p = request->getParam(0);
    if (String(p->name().c_str()) == "key" && String(p->value().c_str()) == SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>()) {

      AsyncWebParameter* p = request->getParam(1);

      if (String(p->name().c_str()) == "button") {
        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        for (int i = 0; i < Actions.size(); i++) {
          if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "bouton") {
            if (String(p->value().c_str()) == SPIFFS_Config_Actions["actions"][i]["id"].as<String>()) {
              Action = String(p->value().c_str()).toInt();
              request->send(200, "text/plain", String(Action));
              break;
            }
          }
        }
      } else if (String(p->name().c_str()) == "toogle") {
        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        for (int i = 0; i < Actions.size(); i++) {
          if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "toogle") {
            //p = request->getParam(2);
            if (String(p->value().c_str()) == SPIFFS_Config_Actions["actions"][i]["id0"].as<String>() || String(p->value().c_str()) == SPIFFS_Config_Actions["actions"][i]["id1"].as<String>()) {
              Action = String(p->value().c_str()).toInt();
              request->send(200, "text/plain", String(Action));
              break;
            }
          }
        }
      } else if (String(p->name().c_str()) == "info") {
        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        for (int i = 0; i < Actions.size(); i++) {

          if (SPIFFS_Config_Actions["actions"][i]["id"].as<String>() == String(p->value().c_str())) {
            String SNom = String(p->value().c_str());
            char CNom[50];
            SNom.toCharArray(CNom, 50);
            p = request->getParam(2);
            SPIFFS_Config_Actions["actions"][i]["value"] = String(p->value().c_str());
            String SValue = String(p->value().c_str());
            char CValue[50];
            SValue.toCharArray(CValue, 50);
            events.send(CValue, CNom, millis());
            request->send(200, "text/plain", "OK");
            break;
          }
        }
      }
    }
  });


  server.on("/save", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* p = request->getParam(0);
    if (String(p->name().c_str()) == "config") {

      if (String(p->value().c_str()) == "params") {
        int params = request->params();
        for (int i = 0; i < params; i++) {
          AsyncWebParameter* p = request->getParam(i);
          if (p->isPost()) {
            SPIFFS_ADD_Param = p->name().c_str();
            SPIFFS_ADD_Value = p->value().c_str();
            SPIFFS_UpdateParam();
          }
          delay(10);
        }
        request->send_P(200, "text/html", Page_Config, RetourVariable);
      }


      if (String(p->value().c_str()) == "AP") {
        int params = request->params();
        for (int i = 0; i < params; i++) {
          AsyncWebParameter* p = request->getParam(i);
          if (p->isPost()) {

            if (p->name() == "pass") SPIFFS_Pass_AP = p->value().c_str();
            if (p->name() == "ip") SPIFFS_IP_AP = p->value().c_str();
            if (p->name() == "actif") SPIFFS_Actif_AP = p->value().c_str();
          }
          delay(10);
        }
        if (SPIFFS_EnregistrementAP()) {
          TITRE = "Redémarrage de l'apparail.";
          MESSAGE = "";
          request->send_P(200, "text/html", Page_Message, RetourVariable);
          delay(1000);
          ESP.restart();
        } else {
          request->send(200, "text/plain", "Erreur d'écriture de la configuration...");
        }
      }


      if (String(p->value().c_str()) == "wifi") {
        int params = request->params();
        for (int i = 0; i < params; i++) {
          AsyncWebParameter* p = request->getParam(i);
          if (p->isPost()) {
            if (p->name() == "ssid") SPIFFS_SSID = p->value().c_str();
            if (p->name() == "pass") SPIFFS_Pass = p->value().c_str();
            if (p->name() == "ip") SPIFFS_IP = p->value().c_str();
          }
          delay(10);
        }
        if (SPIFFS_EnregistrementReseau()) {
          if (ModeAP == false) {
            request->send_P(200, "text/html", Page_Config, RetourVariable);

          } else {
            request->send_P(200, "text/html", Page_Enregistrement, RetourVariable);
            delay(1000);
            ESP.restart();
          }

        } else {
          request->send(200, "text/plain", "Erreur d'écriture de la configuration...");
        }
      }
    } else if (String(p->name().c_str()) == "update") {

      if (String(p->value().c_str()) == "wifi") {
        int params = request->params();
        for (int i = 0; i < params; i++) {
          AsyncWebParameter* p = request->getParam(i);
          if (p->isPost()) {
            SPIFFS_ID = IDNetwork;

            if (p->name() == "ssid") SPIFFS_SSID = p->value().c_str();
            if (p->name() == "pass") SPIFFS_Pass = p->value().c_str();
            if (p->name() == "ip") SPIFFS_IP = p->value().c_str();
          }
          delay(10);
        }
        //SPIFFS_Config = Config;
        if (SPIFFS_UpdateReseau()) {
          request->send_P(200, "text/html", Page_ReseauxEnreg, RetourVariable);
        } else {
          TITRE = "Erreur!";
          MESSAGE = "Erreur d'écriture de la configuration...";
          request->send_P(200, "text/html", Page_Message, RetourVariable);
        }
      }
    }
  });



  server.on("/load", HTTP_GET, [](AsyncWebServerRequest* request) {
    String SPass = SPIFFS_Pass_AP;
    char CPass[50];
    SPass.toCharArray(CPass, 50);
    if (!request->authenticate("admin", CPass))
      return request->requestAuthentication();
    AsyncWebParameter* p = request->getParam(0);
    if (String(p->name().c_str()) == "nav") {

      if (String(p->value().c_str()) == "config")
        request->send_P(200, "text/html", Page_Config, RetourVariable);

      if (String(p->value().c_str()) == "wifi") {
        request->send_P(200, "text/html", Page_ManageurWiFi, RetourVariable);
      }

      if (String(p->value().c_str()) == "wifisr") {
        request->send_P(200, "text/html", Page_ManageurWiFiSansR, RetourVariable);
      }

      if (String(p->value().c_str()) == "params")
        request->send_P(200, "text/html", Page_Parametres, RetourVariable);

      if (String(p->value().c_str()) == "system")
        request->send_P(200, "text/html", Page_Sys, RetourVariable);

      if (String(p->value().c_str()) == "configAP")
        request->send_P(200, "text/html", Page_ConfigAP, RetourVariable);

      if (String(p->value().c_str()) == "rst")
        request->send_P(200, "text/html", Page_Donnees, RetourVariable);

      if (String(p->value().c_str()) == "listnetwork")
        request->send_P(200, "text/html", Page_ReseauxEnreg, RetourVariable);

      if (String(p->value().c_str()) == "update") {
        p = request->getParam(1);
        if (String(p->name().c_str()) == "network") {
          IDNetwork = String(p->value().c_str());
          request->send_P(200, "text/html", Page_ModificationReseau, RetourVariable);
        }
      }

      if (String(p->value().c_str()) == "id") {

        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        String Retour = "";

        for (int i = 0; i < Actions.size(); i++) {

          if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "bouton") {
            String Nom = SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
            String ID = SPIFFS_Config_Actions["actions"][i]["id"].as<String>();
            String Pass = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();

            Retour += "<div class=\"FondCadre\">";
            Retour += "Nom: <b>" + Nom + "</b>";
            Retour += "<br>";
            Retour += "Type: <b>Button</b>";
            Retour += "<br>";
            Retour += "ID: <b>" + ID;
            Retour += "</b><br>";
            Retour += "<br>";
            Retour += "<a href=\"http://" + StringAdresseIP + "/update?key=" + Pass + "&button=" + ID + "\">http://" + StringAdresseIP + "/update?key=" + Pass + "&button=" + ID + "</a>";
            Retour += "<br>";
            Retour += "</div>";
            Retour += "<br>";


          } else if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "toogle") {
            String Nom = SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
            String Value0 = SPIFFS_Config_Actions["actions"][i]["value0"].as<String>();
            String Value1 = SPIFFS_Config_Actions["actions"][i]["value1"].as<String>();
            String ID0 = SPIFFS_Config_Actions["actions"][i]["id0"].as<String>();
            String ID1 = SPIFFS_Config_Actions["actions"][i]["id1"].as<String>();
            String Pass = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();

            Retour += "<div class=\"FondCadre\">";
            Retour += "Nom: <b>" + Nom + "</b>";
            Retour += "<br>";
            Retour += "Type: <b>Toogle</b>";
            Retour += "<br>";
            Retour += "ID - " + Value0 + ": <b>" + ID0;
            Retour += "</b><br>";
            Retour += "ID - " + Value1 + ": <b>" + ID1;
            Retour += "</b><br>";
            Retour += "<br>POST ";
            Retour += Value0 + ": <a href=\"http://" + StringAdresseIP + "/update?key=" + Pass + "&toogle=" + ID0 + "\">http://" + StringAdresseIP + "/update?key=" + Pass + "&toogle=" + ID0 + "</a>";
            Retour += "<br>POST ";
            Retour += Value1 + ": <a href=\"http://" + StringAdresseIP + "/update?key=" + Pass + "&toogle=" + ID1 + "\">http://" + StringAdresseIP + "/update?key=" + Pass + "&toogle=" + ID1 + "</a>";
            Retour += "<br>";
            Retour += "GET: <a href=\"http://" + StringAdresseIP + "/get?key=" + Pass + "&toogle=" + ID0 + "\">http://" + StringAdresseIP + "/get?key=" + Pass + "&toogle=" + ID0 + "</a>";
            Retour += "<br>";
            Retour += "</div>";
            Retour += "<br>";

          } else if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "info") {
            String Nom = SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
            String ID = SPIFFS_Config_Actions["actions"][i]["id"].as<String>();
            String Pass = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();

            Retour += "<div class=\"FondCadre\">";
            Retour += "Nom: <b>" + Nom + "</b>";
            Retour += "<br>";
            Retour += "Type: <b>Info</b>";
            Retour += "<br>";
            Retour += "ID: <b>" + ID;
            Retour += "</b><br>";
            Retour += "<br>POST: ";
            Retour += "<a href=\"http://" + StringAdresseIP + "/update?key=" + Pass + "&info=" + ID + "&value=Info Test\">http://" + StringAdresseIP + "/update?key=" + Pass + "&info=" + ID + "&value=Info Test</a>";
            Retour += "<br>";
            Retour += "GET: <a href=\"http://" + StringAdresseIP + "/get?key=" + Pass + "&info=" + ID + "\">http://" + StringAdresseIP + "/get?key=" + Pass + "&info=" + ID + "</a>";
            Retour += "<br>";
            Retour += "</div>";
            Retour += "<br>";
          }

          delay(10);
        }
        TITRE = "Identités des commandes";
        MESSAGE = Retour;
        request->send_P(200, "text/html", Page_Message, RetourVariable);
      }

      if (String(p->value().c_str()) == "removenetwork") {


        SPIFFS_ID = IDNetwork;
        if (SPIFFS_RemoveReseau()) {
          request->send_P(200, "text/html", Page_ReseauxEnreg, RetourVariable);
        } else {
          TITRE = "Erreur!";
          MESSAGE = "Impossible de supprimer ce réseau...";
          request->send_P(200, "text/html", Page_Message, RetourVariable);
        }
      }



      if (String(p->value().c_str()) == "reboot") {
        TITRE = "Redémarrage de l'apparail.";
        MESSAGE = "";
        request->send_P(200, "text/html", Page_Message, RetourVariable);
        delay(1000);
        ESP.restart();
      }

      if (String(p->value().c_str()) == "rstnetwork") {
        JsonArray Reseau = SPIFFS_Config_Reseau["reseau"];
        if (Reseau.size() == 0) {
          if (_Debug == true) Serial.println("eeBudServerManager:  Configuration vide! Suppression inutile!");
          request->send_P(200, "text/html", Page_Sys, RetourVariable);
        } else {
          SPIFFS_ResetReseau();
          SPIFFS_CreationConfigReseau();
          TITRE = "Paramètres réseaux supprimés!";
          MESSAGE = "";
          request->send_P(200, "text/html", Page_Message, RetourVariable);
        }
      }

      if (String(p->value().c_str()) == "rstparams") {
        JsonArray Params = SPIFFS_Config_Params["params"];
        if (Params.size() == 0) {
          if (_Debug == true) Serial.println("eeBudServerManager:  Configuration vide! Suppression inutile!");
          request->send_P(200, "text/html", Page_Sys, RetourVariable);
        } else {
          SPIFFS_ResetParams();
          SPIFFS_CreationConfigParams();
          TITRE = "Paramètres de configuration supprimés!";
          MESSAGE = "";
          request->send_P(200, "text/html", Page_Message, RetourVariable);
        }
      }

      if (String(p->value().c_str()) == "rstall") {
        delay(100);
        SPIFFS_Format();
        delay(100);

        TITRE = "Tous les paramètres sont supprimés!";
        MESSAGE = "";
        request->send_P(200, "text/html", Page_Message, RetourVariable);

        delay(100);
        ESP.restart();
      }
    }
  });


  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* p = request->getParam(0);
    if (String(p->name().c_str()) == "key" && String(p->value().c_str()) == SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>()) {
      AsyncWebParameter* p = request->getParam(1);

      if (String(p->name().c_str()) == "toogle") {

        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        for (int i = 0; i < Actions.size(); i++) {
          if (SPIFFS_Config_Actions["actions"][i]["id0"].as<String>() == String(p->value().c_str()) || SPIFFS_Config_Actions["actions"][i]["id1"].as<String>() == String(p->value().c_str())) {

            String Nom = SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
            String State = SPIFFS_Config_Actions["actions"][i]["state"].as<String>();
            State.replace(Nom, "");
            request->send(200, "text/plain", State);

            break;
          }
          if (i == Actions.size() - 1) {
            request->send(200, "text/plain", "Null");
            break;
          }
          delay(10);
        }
      } else if (String(p->name().c_str()) == "info") {
        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        for (int i = 0; i < Actions.size(); i++) {
          if (SPIFFS_Config_Actions["actions"][i]["id"].as<String>() == String(p->value().c_str())) {

            request->send(200, "text/plain", SPIFFS_Config_Actions["actions"][i]["value"].as<String>());

            break;
          }
          if (i == Actions.size() - 1) {
            request->send(200, "text/plain", "Null");
            break;
          }
          delay(10);
        }

      } else if (String(p->name().c_str()) == "id") {

        JsonArray Actions = SPIFFS_Config_Actions["actions"];
        String Retour = String(NomProg) + ": Identite des commandes.";
        Retour += "\n";
        Retour += "\n";
        for (int i = 0; i < Actions.size(); i++) {

          if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "bouton" || SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "info") {
            Retour += "Nom: " + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
            Retour += "\n";
            Retour += "ID: " + SPIFFS_Config_Actions["actions"][i]["id"].as<String>();
            Retour += "\n";
            Retour += "\n";
          } else if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "toogle") {
            Retour += "Nom: " + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
            Retour += "\n";
            Retour += "ID " + SPIFFS_Config_Actions["actions"][i]["value0"].as<String>() + ": " + SPIFFS_Config_Actions["actions"][i]["id0"].as<String>();
            Retour += "\n";
            Retour += "ID " + SPIFFS_Config_Actions["actions"][i]["value1"].as<String>() + ": " + SPIFFS_Config_Actions["actions"][i]["id1"].as<String>();
            Retour += "\n";
            Retour += "\n";
          }

          delay(10);
        }
        request->send(200, "text/plain", Retour);
      }
    }
  });


  events.onConnect([](AsyncEventSourceClient* client) {
    if (_Debug == true) Serial.println("eeBudServerManager:  Client Connecté.");
    if (client->lastId()) {
      if (_Debug == true) Serial.printf("eeBudServerManager:  Client reconnecté. Dernier ID: %u\n", client->lastId());
    }
    client->send("eeBudServerManager:  Un salut de eeBud!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.onNotFound([](AsyncWebServerRequest* request) {
    TITRE = "Erreur: 404";
    MESSAGE = "Cette page n'existe pas...";
    request->send_P(200, "text/html", Page_Message, RetourVariable);
  });

  server.begin();
}

eeBudServerManager::eeBudServerManager(char* NomProgramme) {
  NomProg = NomProgramme;
}

bool eeBudServerManager::ResetParams() {
  JsonArray Params = SPIFFS_Config_Params["params"];
  if (Params.size() == 0) {
    if (_Debug == true) Serial.println("eeBudServerManager:  Configuration vide! Suppression inutile!");
    return false;
  } else {
    SPIFFS_ResetParams();
    SPIFFS_CreationConfigParams();
    return true;
  }
}

bool eeBudServerManager::ResetNetwork() {
  JsonArray Reseau = SPIFFS_Config_Reseau["reseau"];
  if (Reseau.size() == 0) {
    if (_Debug == true) Serial.println("eeBudServerManager:  Configuration vide! Suppression inutile!");
    return false;
  } else {
    SPIFFS_ResetReseau();
    SPIFFS_CreationConfigReseau();
    return true;
  }
}
void eeBudServerManager::ResetAll() {
  JsonArray Params = SPIFFS_Config_Params["params"];
  if (Params.size() == 0) {
    if (_Debug == true) Serial.println("eeBudServerManager:  Configuration vide! Suppression inutile!");
  } else {
    SPIFFS_ResetParams();
    SPIFFS_CreationConfigParams();
  }
  JsonArray Reseau = SPIFFS_Config_Reseau["reseau"];
  if (Reseau.size() == 0) {
    if (_Debug == true) Serial.println("eeBudServerManager:  Configuration vide! Suppression inutile!");

  } else {
    SPIFFS_ResetReseau();
    SPIFFS_CreationConfigReseau();
  }
}

int eeBudServerManager::IndexGetEvent() {
  if (DNS == true) {
    dnsServer.processNextRequest();
  } else {

    MomentActuel_Surv = millis();
    if ((MomentActuel_Surv - MomentPrecedent_Surv) > 10000) {
      MomentPrecedent_Surv = MomentActuel_Surv;
      if (ModeAP == false && Scan == false && TentativesReco == 0) {

        if (WiFi.status() != WL_CONNECTED) {
          if (_Debug == true) Serial.println("eeBudServerManager:  Perte de connexion WiFi...");
          ScanWiFi();

          if (_Debug == true) Serial.println("eeBudServerManager:  Tentative de reconnexion...");
          if (initWiFi()) {
            TentativesReco = 0;
            ModeAP = false;
            ConnexionPerdue = false;
            if (_Debug == true) Serial.println("eeBudServerManager:  Connexion retrouvée!");
          } else {
            if (initPA()) {

              if (_Debug == true) Serial.println("eeBudServerManager:  Impossible de se reconnecter! Activation du point d'accès...");
              ModeAP = true;
              ConnexionPerdue = true;
            }
          }
        }
      } else if (ConnexionPerdue == true && Scan == false) {
        TentativesReco++;
        if (_Debug == true) Serial.println("eeBudServerManager:  " + String(TentativesReco));
        if (TentativesReco == 60) {
          if (_Debug == true) Serial.println("eeBudServerManager:  Lancement de la tentative de reconnexion...");
          TentativesReco = 0;
          ModeAP = false;
        }
      }
    }

    return Action;
  }
}

void eeBudServerManager::IndexResetEvent() {
  Action = -1;
}

String eeBudServerManager::HTTPGet(String URL) {
  if (URL != "") {
    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "text/plain");
    int httpResponseCode = http.POST("HTTP POST eeBud Server Manager");
    if (httpResponseCode > 0) {
      String response = http.getString();
      return response;
    } else {
      if (_Debug == true) Serial.println("eeBudServerManager:  Erreur POST HTTP: ");
      if (_Debug == true) Serial.println(httpResponseCode);
      return "Error";
    }
  }
}

void eeBudServerManager::HTTPPost(String URL) {
  if (URL != "") {
    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "text/plain");
    int httpResponseCode = http.POST("HTTP POST eeBud Server Manager");
  }
}

void eeBudServerManager::IndexAddSeg() {
  JsonArray Actions = SPIFFS_Config_Actions["actions"];
  JsonObject Action = Actions.createNestedObject();
  Action["nom"] = "eeSeg";
  Action["type"] = "eeSeg";
}

int eeBudServerManager::IndexAddButton(String Name) {
  JsonArray Actions = SPIFFS_Config_Actions["actions"];
  JsonObject Action = Actions.createNestedObject();
  Action["id"] = IDAction;
  IDAction++;
  Action["nom"] = Name;
  Action["type"] = "bouton";
  return IDAction - 1;
}

int eeBudServerManager::IndexAddInfo(String Name, String Unit, String Size) {
  JsonArray Actions = SPIFFS_Config_Actions["actions"];
  JsonObject Action = Actions.createNestedObject();
  Action["id"] = IDAction;
  IDAction++;
  Action["nom"] = Name;
  Action["type"] = "info";
  Action["value"] = "";
  Action["size"] = Size;
  Action["unit"] = Unit;
  return IDAction - 1;
}

void eeBudServerManager::IndexSetInfo(int ID, String Value) {
  JsonArray Actions = SPIFFS_Config_Actions["actions"];
  for (int i = 0; i < Actions.size(); i++) {
    if (SPIFFS_Config_Actions["actions"][i]["id"].as<String>() == String(ID)) {
      SPIFFS_Config_Actions["actions"][i]["value"] = Value;
      char cID[5] = "";
      sprintf(cID, "%d", ID);

      char CValue[50];
      Value.toCharArray(CValue, 50);
      events.send(CValue, cID, millis());
      break;
    }
    delay(10);
  }
}

int* eeBudServerManager::IndexAddToogle(String Name, String Value_0, String Value_1, String Default) {
  JsonArray Actions = SPIFFS_Config_Actions["actions"];

  static int ID[2];
  JsonObject Action = Actions.createNestedObject();
  Action["id0"] = IDAction;
  ID[0] = IDAction;
  IDAction++;
  Action["id1"] = IDAction;
  ID[1] = IDAction;
  IDAction++;
  Action["nom"] = Name;
  Action["type"] = "toogle";
  Action["value0"] = Value_0;
  Action["value1"] = Value_1;
  Action["state"] = Default;

  return ID;
}

void eeBudServerManager::IndexSetToogle(int ID) {
  JsonArray Actions = SPIFFS_Config_Actions["actions"];
  for (int i = 0; i < Actions.size(); i++) {
    if (SPIFFS_Config_Actions["actions"][i]["id0"].as<String>() == String(ID) || SPIFFS_Config_Actions["actions"][i]["id1"].as<String>() == String(ID)) {
      String Value;
      if (SPIFFS_Config_Actions["actions"][i]["id0"].as<String>() == String(ID)) {
        Value = SPIFFS_Config_Actions["actions"][i]["value0"].as<String>();
        SPIFFS_Config_Actions["actions"][i]["state"] = Value;
      } else if (SPIFFS_Config_Actions["actions"][i]["id1"].as<String>() == String(ID)) {
        Value = SPIFFS_Config_Actions["actions"][i]["value1"].as<String>();
        SPIFFS_Config_Actions["actions"][i]["state"] = Value;
      }

      char cID[5] = "";
      sprintf(cID, "%d", ID);

      char CValue[50];
      Value.toCharArray(CValue, 50);
      events.send(CValue, cID, millis());

      break;
    }
    delay(10);
  }
}

void eeBudServerManager::AddParamText(String Name, String Default) {
  SPIFFS_ADD_Param = Name;
  SPIFFS_ADD_Value = Default;
  SPIFFS_ADD_Type = "text";
  SPIFFS_AddParam();
}

void eeBudServerManager::AddParamBool(String Name, bool Default) {
  SPIFFS_ADD_Param = Name;
  SPIFFS_ADD_Value = String(Default);
  SPIFFS_ADD_Type = "toogle";
  SPIFFS_AddParam();
}

String eeBudServerManager::GetParamText(String Name) {
  JsonArray Params = SPIFFS_Config_Params["params"];
  for (int i = 0; i < Params.size(); i++) {
    if (SPIFFS_Config_Params["params"][i]["param"].as<String>() == Name) {
      return SPIFFS_Config_Params["params"][i]["value"].as<String>();
      break;
    }
    if (i == Params.size() - 1) {
      return "null";
    }
    delay(10);
  }
}

bool eeBudServerManager::GetParamBool(String Name) {
  JsonArray Params = SPIFFS_Config_Params["params"];
  for (int i = 0; i < Params.size(); i++) {
    if (SPIFFS_Config_Params["params"][i]["param"].as<String>() == Name) {
      if (SPIFFS_Config_Params["params"][i]["value"].as<String>() == "0") {
        return false;
      } else if (SPIFFS_Config_Params["params"][i]["value"].as<String>() == "1") {
        return true;
      } else {
        return false;
      }
      break;
    }
    if (i == Params.size() - 1) {
      return false;
    }
    delay(10);
  }
}

void eeBudServerManager::RemoveParam(String Name) {
  SPIFFS_ADD_Param = Name;
  SPIFFS_RemoveParam();
}

void eeBudServerManager::UpdateParamText(String Name, String Value) {
  SPIFFS_ADD_Param = Name;
  SPIFFS_ADD_Param = Value;
  SPIFFS_UpdateParam();
}

void eeBudServerManager::UpdateParamBool(String Name, bool Value) {
  SPIFFS_ADD_Param = Name;
  if (Value == true) {
    SPIFFS_ADD_Param = "true";
  } else {
    SPIFFS_ADD_Param = "false";
  }

  SPIFFS_UpdateParam();
}


int eeBudServerManager::GetWiFiStatus() {
  if (ModeAP == true && ConnexionPerdue == false) {
    return 2;
  }
  if (ModeAP == false && ConnexionPerdue == false) {
    return 1;
  }
  if (ConnexionPerdue == true) {
    return 3;
  }
}

void eeBudServerManager::ScanWiFi() {
  //Scan des réseaux wifi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int n = WiFi.scanNetworks();
  if (n != 0) {
    for (int i = 0; i < n; i++) {
      if (i < 9) {
        ScanReseaux[i] = WiFi.SSID(i);
      }
      delay(10);
    }
  }
}



bool eeBudServerManager::initPA() {
  if (_Debug == true) Serial.println("eeBudServerManager:  Configuration du Point d'acces: ");

  IPAddress AP_local_ip;
  AP_local_ip.fromString(SPIFFS_Config_Reseau["ap"][0]["ip"].as<String>());
  delay(200);
  IPAddress AP_gateway(192, 168, 1, 1);
  IPAddress AP_subnet(255, 255, 255, 0);
  WiFi.mode(WIFI_AP);
  delay(200);
  WiFi.softAPConfig(AP_local_ip, AP_gateway, AP_subnet);


  String SPass = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();
  char CPass[100];
  SPass.toCharArray(CPass, 100);

  WiFi.softAP(NomProg, CPass);
  IPAddress IP = WiFi.softAPIP();
  if (_Debug == true) Serial.println("eeBudServerManager:  Adresse IP Point d'acces: ");
  if (_Debug == true) Serial.println(IP);
  ModeAP = true;
  return true;
}

bool eeBudServerManager::initWiFi() {
  if (_Debug == true) Serial.println("eeBudServerManager:  Initialisation du WiFi");

  String Conf = SPIFFS_Config_Reseau["reseau"][0]["ssid"];
  if (Conf == "null") {
    return false;
  }

  if (SPIFFS_IP != "" && SPIFFS_IP != "null") {
    String StringIP = SPIFFS_IP;
    IPAddress localIP;
    WiFi.mode(WIFI_STA);
    localIP.fromString(StringIP.c_str());
    if (!WiFi.config(localIP, gateway, subnet)) {
      if (_Debug == true) Serial.println("eeBudServerManager:  Erreur de configuration WiFi");
      return false;
    }
  }

  JsonArray reseaux = SPIFFS_Config_Reseau["reseau"];

  for (int scan = 0; scan < 10; scan++) {
    if (ScanReseaux[scan] != "") {
      for (int par = reseaux.size() - 1; par >= 0; par--) {
        String SSIDpar = SPIFFS_Config_Reseau["reseau"][par]["ssid"];
        if (SSIDpar == ScanReseaux[scan]) {
          String PASSpar = SPIFFS_Config_Reseau["reseau"][par]["pass"];

          WiFi.begin(SSIDpar.c_str(), PASSpar.c_str());
          if (_Debug == true) Serial.println("eeBudServerManager:  M2: Tentative de connexion à: " + SSIDpar);

          if (WiFiConnect()) {
            if (_Debug == true) Serial.println("eeBudServerManager:  M2: Connexion reussie à: " + SSIDpar);
            if (_Debug == true) Serial.println("eeBudServerManager:  M2: Adresse IP: ");
            if (_Debug == true) Serial.println(WiFi.localIP());
            StringAdresseIP = WiFi.localIP().toString();
            ModeAP = false;
            return true;
          } else {
            if (_Debug == true) Serial.println("eeBudServerManager:  M2: Connexion impossible à: " + SSIDpar);
          }
        }
      }
      delay(10);
    }
    delay(10);
  }

  for (int i = reseaux.size() - 1; i >= 0; i--) {

    String SSID = SPIFFS_Config_Reseau["reseau"][i]["ssid"];
    String PASS = SPIFFS_Config_Reseau["reseau"][i]["pass"];

    WiFi.begin(SSID.c_str(), PASS.c_str());
    if (_Debug == true) Serial.println("eeBudServerManager:  Tentative de connexion à: " + SSID);

    if (WiFiConnect()) {
      if (_Debug == true) Serial.println("eeBudServerManager:  Connexion reussie à: " + SSID);
      if (_Debug == true) Serial.println("eeBudServerManager:  Adresse IP: ");
      if (_Debug == true) Serial.println(WiFi.localIP());
      StringAdresseIP = WiFi.localIP().toString();
      ModeAP = false;
      return true;
    } else {
      if (_Debug == true) Serial.println("eeBudServerManager:  Connexion impossible à: " + SSID);
      if (i == 0) {
        return false;
      }
    }
    delay(10);
  }
}

bool eeBudServerManager::WiFiConnect() {
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while (WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= timeout) {
      return false;
    }
    delay(100);
  }
  ModeAP = false;
  return true;
}

String eeBudServerManager::RetourVariable(String var) {
  String RETOUR = "Null";
  if (var == "KEY") RETOUR = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();

  if (var == "SSIDUPDATE") {
    JsonArray Reseaux = SPIFFS_Config_Reseau["reseau"];
    for (int i = 0; i < Reseaux.size(); i++) {
      if (SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() == IDNetwork) {
        RETOUR = SPIFFS_Config_Reseau["reseau"][i]["ssid"].as<String>();
        break;
      }
      delay(10);
    }
  }

  if (var == "PASSUPDATE") {
    JsonArray Reseaux = SPIFFS_Config_Reseau["reseau"];
    for (int i = 0; i < Reseaux.size(); i++) {
      if (SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() == IDNetwork) {
        RETOUR = SPIFFS_Config_Reseau["reseau"][i]["pass"].as<String>();
        break;
      }
      delay(10);
    }
  }

  if (var == "IPUPDATE") {
    JsonArray Reseaux = SPIFFS_Config_Reseau["reseau"];
    for (int i = 0; i < Reseaux.size(); i++) {
      if (SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() == IDNetwork) {
        RETOUR = SPIFFS_Config_Reseau["reseau"][i]["ip"].as<String>();
        break;
      }
      delay(10);
    }
  }

  if (var == "RESEAUXENREG") {
    RETOUR = "";
    JsonArray Reseaux = SPIFFS_Config_Reseau["reseau"];
    for (int i = 0; i < Reseaux.size(); i++) {
      RETOUR += "<span class=\"FondBlanc\">" + SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() + "</span><button onclick=\"window.location.href = '/load?nav=update&network=" + SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() + "';\" class=\"reseau\">" + SPIFFS_Config_Reseau["reseau"][i]["ssid"].as<String>() + "</button><br>";
      delay(10);
    }
  }

  if (var == "TITRE") {
    RETOUR = TITRE;
  }

  if (var == "MESSAGE") {
    RETOUR = MESSAGE;
  }


  if (var == "SCANRESEAUX") {
    RETOUR = "";
    for (int j = 0; j < 10; j++) {
      if (ScanReseaux[j] != "") {

        RETOUR += "<button onclick=\"Click('" + ScanReseaux[j] + "')\"class=\"reseau\">" + ScanReseaux[j] + "</button><br>";
      }
      delay(10);
    }
  }

  if (var == "APP") RETOUR = NomProg;

  if (var == "SSID") {
    if (SPIFFS_SSID == "null") {
      RETOUR = "";
    } else {
      RETOUR = SPIFFS_SSID;
    }
  }

  if (var == "IP") {
    if (SPIFFS_IP == "null") {
      RETOUR = "";
    } else {
      RETOUR = SPIFFS_IP;
    }
  }


  if (var == "PARAMS") {
    RETOUR = "";
    JsonArray Params = SPIFFS_Config_Params["params"];
    for (int i = 0; i < Params.size(); i++) {
      if (SPIFFS_Config_Params["params"][i]["type"].as<String>() == "text") {
        RETOUR += "<b><label>" + SPIFFS_Config_Params["params"][i]["param"].as<String>() + ":</label></b> <br><input class=\"box\" id=\"" + SPIFFS_Config_Params["params"][i]["param"].as<String>() + "\" name=\"" + SPIFFS_Config_Params["params"][i]["param"].as<String>() + "\" style=\"width: 900px; text-align: center;\" value=\"" + SPIFFS_Config_Params["params"][i]["value"].as<String>() + "\" type=\"text\"> <br><br>";
      } else if (SPIFFS_Config_Params["params"][i]["type"].as<String>() == "toogle") {

        RETOUR += "<b><div style=\"font-size: 40px;\">" + SPIFFS_Config_Params["params"][i]["param"].as<String>() + "</div></b>";

        String Nom = SPIFFS_Config_Params["params"][i]["param"].as<String>();
        String Value = SPIFFS_Config_Params["params"][i]["value"].as<String>();

        if (SPIFFS_Config_Params["params"][i]["value"].as<String>() == "0") {
          RETOUR += "<div><label class=\"label1\"><div class=\"toggle\"><input onclick=\"ClickToogle(this, '" + Nom + "')\" class=\"toggle-state\" type=\"checkbox\"><div class=\"toggle-inner\"><div class=\"indicator\"></div></div><div class=\"active-bg\"></div><div id=\"Value" + Nom + "\" class=\"textd\">Inactif</div></div></label></div><br>";
        } else {
          RETOUR += "<div><label class=\"label1\"><div class=\"toggle\"><input onclick=\"ClickToogle(this,'" + Nom + "')\" class=\"toggle-state\" type=\"checkbox\" checked><div class=\"toggle-inner\"><div class=\"indicator\"></div></div><div class=\"active-bg\"></div><div id=\"Value" + Nom + "\" class=\"textd\">Actif</div></div></label></div><br>";
        }
        RETOUR += "<input class=\"hide\" id=\"" + Nom + "\" name=\"" + Nom + "\" value=\"" + Value + "\" type=\"text\">";
      }
      delay(10);
    }
  }

  if (var == "ACTIFAP") {
    RETOUR = "";
    String Actif = SPIFFS_Config_Reseau["ap"][0]["actif"].as<String>();
    if (Actif == "0") {
      RETOUR += "<br><div><label class=\"label1\"><div class=\"toggle\"><input onclick=\"ClickToogle(this)\" class=\"toggle-state\" type=\"checkbox\"><div class=\"toggle-inner\"><div class=\"indicator\"></div></div><div class=\"active-bg\"></div><div id=\"texte\" class=\"textd\">Inactif</div></div></label></div><br>";
    } else {
      RETOUR += "<br><div><label class=\"label1\"><div class=\"toggle\"><input onclick=\"ClickToogle(this)\" class=\"toggle-state\" type=\"checkbox\" checked><div class=\"toggle-inner\"><div class=\"indicator\"></div></div><div class=\"active-bg\"></div><div id=\"texte\" class=\"textd\">Actif</div></div></label></div><br>";
    }
    RETOUR += "<input class=\"hide\" id=\"actif\" name=\"actif\" value=\"" + Actif + "\" type=\"text\">";
  }

  if (var == "IPAP") {
    if (SPIFFS_IP_AP == "null") {
      RETOUR = "";
    } else {
      RETOUR = SPIFFS_IP_AP;
    }
  }

  if (var == "MDPAP") {
    if (SPIFFS_Pass_AP == "null") {
      RETOUR = "";
    } else {
      RETOUR = SPIFFS_Pass_AP;
    }
  }


  if (var == "ACTIONS") {
    ScriptConstruct = "";
    ScriptConstruct += "<script>";
    ScriptConstruct += "if (!!window.EventSource) {";
    ScriptConstruct += "var source = new EventSource('/events');";
    ScriptConstruct += "source.addEventListener('open', function(e) {";
    ScriptConstruct += "console.log(\"Events Connected\");";
    ScriptConstruct += "}, false);";
    ScriptConstruct += "source.addEventListener('error', function(e) {";
    ScriptConstruct += "if (e.target.readyState != EventSource.OPEN) {";
    ScriptConstruct += "console.log(\"Events Disconnected\");";
    ScriptConstruct += "}";
    ScriptConstruct += "}, false);";
    ScriptConstruct += "source.addEventListener('message', function(e) {";
    ScriptConstruct += "console.log(\"message\", e.data);";
    ScriptConstruct += "}, false);";
    RETOUR = "";
    JsonArray Actions = SPIFFS_Config_Actions["actions"];
    for (int i = 0; i < Actions.size(); i++) {
      if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "bouton") {
        //RETOUR += "<button id=\"" + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>() + "\" class=\"Bouton\" onclick=\"Click(this)\">" + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>() + "</button><br><br>";
        RETOUR += "<div class=\"TotalBouton\"> <span name=\"fond\" id=\"fond\" class=\"fond_bouton\"></span> <button id=\"" + SPIFFS_Config_Actions["actions"][i]["id"].as<String>() + "\" class=\"Bouton\" onclick=\"Click(this)\" style=\"text-decoration:none\"> " + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>() + " </button> </div>";

      } else if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "info") {
        String Nom = SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
        String Size = SPIFFS_Config_Actions["actions"][i]["size"].as<String>();
        String Value = SPIFFS_Config_Actions["actions"][i]["value"].as<String>();
        String Unit = SPIFFS_Config_Actions["actions"][i]["unit"].as<String>();
        RETOUR += "<div style=\"font-size: " + Size + "px;\">" + Nom + ": <b><span id=\"" + Nom + "\">" + Value + "</span></b><span id=\"Unit" + Nom + "\"> " + Unit + "</span></div><br>";
        ScriptConstruct += "source.addEventListener('" + SPIFFS_Config_Actions["actions"][i]["id"].as<String>() + "', function(e) {document.getElementById(\"" + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>() + "\").innerHTML = e.data;}, false);";

      } else if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "eeSeg") {
        RETOUR += "<div class=\"Seg\"></div><br>";

      } else if (SPIFFS_Config_Actions["actions"][i]["type"].as<String>() == "toogle") {

        RETOUR += "<div style=\"font-size: 40px;\">" + SPIFFS_Config_Actions["actions"][i]["nom"].as<String>() + "</div><br>";

        String Nom = SPIFFS_Config_Actions["actions"][i]["nom"].as<String>();
        String Value0 = SPIFFS_Config_Actions["actions"][i]["value0"].as<String>();
        String Value1 = SPIFFS_Config_Actions["actions"][i]["value1"].as<String>();
        String ID0 = SPIFFS_Config_Actions["actions"][i]["id0"].as<String>();
        String ID1 = SPIFFS_Config_Actions["actions"][i]["id1"].as<String>();

        if (SPIFFS_Config_Actions["actions"][i]["state"].as<String>() == Value1) {
          RETOUR += "<br><div><label class=\"label1\"><div class=\"toggle\"><input class=\"toggle-state\" name=\"" + Nom + "\" id=\"" + Nom + "\" value=\"" + Value1 + "\" type=\"checkbox\" onclick=\"ClickToogle(this,'" + Value0 + "','" + Value1 + "','" + ID0 + "','" + ID1 + "')\" checked><div class=\"toggle-inner\"><div class=\"indicator\"></div></div><div class=\"active-bg\"></div><div id=\"Texte" + Nom + "\" class=\"textd\">" + Value1 + "</div></div></label></div><br>";

        } else {
          RETOUR += "<br><div><label class=\"label1\"><div class=\"toggle\"><input class=\"toggle-state\" name=\"" + Nom + "\" id=\"" + Nom + "\" value=\"" + Value1 + "\" type=\"checkbox\" onclick=\"ClickToogle(this,'" + Value0 + "','" + Value1 + "','" + ID0 + "','" + ID1 + "')\"><div class=\"toggle-inner\"><div class=\"indicator\"></div></div><div class=\"active-bg\"></div><div id=\"Texte" + Nom + "\" class=\"textd\">" + Value0 + "</div></div></label></div><br>";
        }

        ScriptConstruct += "source.addEventListener('" + ID0 + "', function(e) {document.getElementById(\"" + Nom + "\").checked = false; document.getElementById(\"Texte" + Nom + "\").innerHTML = '" + Value0 + "';  }, false);";
        ScriptConstruct += "source.addEventListener('" + ID1 + "', function(e) {document.getElementById(\"" + Nom + "\").checked = true; document.getElementById(\"Texte" + Nom + "\" ).innerHTML = '" + Value1 + "';  }, false);";
      }
      if (i == Actions.size() - 1) {
        ScriptConstruct += "}</script>";
        RETOUR += ScriptConstruct;
      }
      delay(10);
    }
  }

  return RETOUR;
}

void eeBudServerManager::Init() {
  if (_Debug == true) Serial.println("\n\neeBudServerManager:  !!!---------------------------------------------------------------------------------------------------------!!!  \n\n");
  delay(100);

  SPIFFS_Initialisation();
  if (!SPIFFS.exists("/reseau.json")) SPIFFS_CreationConfigReseau();
  if (!SPIFFS.exists("/params.json")) SPIFFS_CreationConfigParams();
  SPIFFS_ChargementConfigurationReseau();
  SPIFFS_ChargementConfigurationParams();

  JsonArray Params = SPIFFS_Config_Actions.createNestedArray("actions");

  ScanWiFi();

  if (SPIFFS_Config_Reseau["ap"][0]["actif"].as<String>() == "0") {

    if (SPIFFS_Pass_AP == "") {
      ModeAP = true;
      ServeurDNS();
    } else {
      if (initWiFi()) {
        ModeAP = false;
        ServeurStation();
      } else {
        if (initPA()) {
          ModeAP = true;
          ServeurStation();
        }
      }
    }

  } else {
    if (initPA()) {
      ModeAP = true;
      ServeurStation();
    }
  }
}