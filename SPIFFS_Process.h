#define JSON_CONFIG_Reseau "/reseau.json"
#define JSON_CONFIG_Params "/params.json"

DynamicJsonDocument SPIFFS_Config_Reseau(1024);
DynamicJsonDocument SPIFFS_Config_Params(4096);

String SPIFFS_ID = "null";
String SPIFFS_SSID = "null";
String SPIFFS_Pass = "null";
String SPIFFS_IP = "null";

String SPIFFS_Pass_AP = "null";
String SPIFFS_IP_AP = "null";
String SPIFFS_Actif_AP = "null";

String SPIFFS_ADD_Param = "";
String SPIFFS_ADD_Type = "";
String SPIFFS_ADD_Value = "";

bool _Debug = false;
bool _Scan = true;



// Initialisation SPIFFS
void SPIFFS_Initialisation() {
  if (!SPIFFS.begin(true)) {
    if (_Debug==true) Serial.println("eeBudServerManager:  SPIFFS - Erreur");
  }
  if (_Debug==true) Serial.println("eeBudServerManager:  SPIFFS - OK");
}
void SPIFFS_Format() {
  SPIFFS.format();
}

void SPIFFS_ResetParams() {

  SPIFFS.remove(JSON_CONFIG_Params);
  if (_Debug==true) Serial.println("eeBudServerManager:  Réinitialisation de la configuration terminée!");
}

void SPIFFS_ResetReseau() {
  SPIFFS.remove(JSON_CONFIG_Reseau);
  if (_Debug==true) Serial.println("eeBudServerManager:  Réinitialisation du réseau terminée!");
}

void SPIFFS_ResetAll() {
  SPIFFS.remove(JSON_CONFIG_Params);
  SPIFFS.remove(JSON_CONFIG_Reseau);
  if (_Debug==true) Serial.println("eeBudServerManager:  Réinitialisation terminée!");
}

void SPIFFS_AddParam() {
  JsonArray Params = SPIFFS_Config_Params["params"];
  if (Params.size() == 0) {
    JsonObject Param = Params.createNestedObject();
    Param["param"] = SPIFFS_ADD_Param;
    Param["value"] = SPIFFS_ADD_Value;
    Param["type"] = SPIFFS_ADD_Type;
    File configFile = SPIFFS.open(JSON_CONFIG_Params, "w");

    if (!configFile) {
      if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
    }

    if (serializeJson(SPIFFS_Config_Params, configFile) == 0) {
      if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'écrire le fichier de configuration...");
    }
    configFile.close();
    if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Params, Serial);
    if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
    delay(200);
  } else {
    for (int i = 0; i < Params.size(); i++) {

      if (SPIFFS_Config_Params["params"][i]["param"].as<String>() == SPIFFS_ADD_Param) {
        break;
      }
      if (i == Params.size() - 1) {
        JsonObject Param = Params.createNestedObject();
        Param["param"] = SPIFFS_ADD_Param;
        Param["value"] = SPIFFS_ADD_Value;
        Param["type"] = SPIFFS_ADD_Type;
        File configFile = SPIFFS.open(JSON_CONFIG_Params, "w");

        if (!configFile) {
          if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
        }

        if (serializeJson(SPIFFS_Config_Params, configFile) == 0) {
          if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'écrire le fichier de configuration...");
        }
        configFile.close();
       if (_Debug==true) serializeJsonPretty(SPIFFS_Config_Params, Serial);
        if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
        delay(200);
        break;
      }
      delay(10);
    }
  }
}

void SPIFFS_RemoveParam() {
  JsonArray Params = SPIFFS_Config_Params["params"];
  for (int i = 0; i < Params.size(); i++) {

    if (SPIFFS_Config_Params["params"][i]["param"].as<String>() == SPIFFS_ADD_Param) {
      Params.remove(i);
      File configFile = SPIFFS.open(JSON_CONFIG_Params, "w");

      if (!configFile) {
        if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
      }

      if (serializeJson(SPIFFS_Config_Params, configFile) == 0) {
        if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'écrire le fichier de configuration...");
      }
      configFile.close();
      if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Params, Serial);
      if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
      delay(200);
      break;
    }
    if (i == Params.size() - 1) {
      if (_Debug==true) Serial.println("eeBudServerManager:  Aucun paramètre supprimé.");
      break;
    }
    delay(10);
  }
}

void SPIFFS_UpdateParam() {
  JsonArray Params = SPIFFS_Config_Params["params"];
  for (int i = 0; i < Params.size(); i++) {
    if (SPIFFS_Config_Params["params"][i]["param"].as<String>() == SPIFFS_ADD_Param) {
      if (SPIFFS_Config_Params["params"][i]["value"].as<String>() != SPIFFS_ADD_Value) {
        SPIFFS_Config_Params["params"][i]["value"] = SPIFFS_ADD_Value;

        File configFile = SPIFFS.open(JSON_CONFIG_Params, "w");
        if (!configFile) {
          if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
        }

        if (serializeJson(SPIFFS_Config_Params, configFile) == 0) {
          if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'écrire le fichier de configuration...");
        }
        configFile.close();
        if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Params, Serial);
        if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
        delay(200);
        break;
      }
    }
    delay(10);
  }
}

void SPIFFS_ChargementConfigurationReseau() {
  //Lecture du fichier Reseau
  if (SPIFFS.exists(JSON_CONFIG_Reseau)) {
    if (_Debug==true) Serial.println("eeBudServerManager:  Lecture du fichier de configuration réseau");
    File configFile = SPIFFS.open(JSON_CONFIG_Reseau, "r");
    if (configFile) {
      DeserializationError error = deserializeJson(SPIFFS_Config_Reseau, configFile);
      if (error) if (_Debug==true) Serial.println(F("eeBudServerManager:  Lecture impossible"));
      configFile.close();
      JsonArray reseaux = SPIFFS_Config_Reseau["reseau"];
      int DernierReseau = reseaux.size() - 1;
      SPIFFS_SSID = SPIFFS_Config_Reseau["reseau"][DernierReseau]["ssid"].as<String>();
      SPIFFS_Pass = SPIFFS_Config_Reseau["reseau"][DernierReseau]["pass"].as<String>();
      SPIFFS_IP = SPIFFS_Config_Reseau["reseau"][DernierReseau]["ip"].as<String>();

      SPIFFS_Pass_AP = SPIFFS_Config_Reseau["ap"][0]["pass"].as<String>();
      SPIFFS_IP_AP = SPIFFS_Config_Reseau["ap"][0]["ip"].as<String>();
      SPIFFS_Actif_AP = SPIFFS_Config_Reseau["ap"][0]["actif"].as<String>();
    }
  }
}

void SPIFFS_ChargementConfigurationParams() {
  //Lecture du fichier Params
  if (SPIFFS.exists(JSON_CONFIG_Params)) {
    if (_Debug==true) Serial.println("eeBudServerManager:  Lecture des fichiers de configuration des paramètres");
    File configFile = SPIFFS.open(JSON_CONFIG_Params, "r");
    if (configFile) {
      DeserializationError error = deserializeJson(SPIFFS_Config_Params, configFile);
      if (error) if (_Debug==true) Serial.println(F("eeBudServerManager:  Lecture impossible"));
      configFile.close();
    }
  }
}

bool SPIFFS_EnregistrementReseau() {
  if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement de la configuration réseau...");

  JsonArray reseaux = SPIFFS_Config_Reseau["reseau"];
  JsonObject info = reseaux.createNestedObject();
  info["id"] = reseaux.size() - 1;
  info["ssid"] = SPIFFS_SSID;
  info["pass"] = SPIFFS_Pass;
  info["ip"] = SPIFFS_IP;


  File configFile = SPIFFS.open(JSON_CONFIG_Reseau, "w");

  if (!configFile) {
    if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
    return false;
  }

  if (serializeJson(SPIFFS_Config_Reseau, configFile) == 0) {
    if (_Debug==true) Serial.println(F("eeBudServerManager:  Impossible d'écrire le fichier de configuration..."));
    return false;
  }
  configFile.close();
  if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Reseau, Serial);
  if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
  return true;
}

bool SPIFFS_RemoveReseau() {
  JsonArray Reseau = SPIFFS_Config_Reseau["reseau"];
  for (int i = 0; i < Reseau.size(); i++) {

    if (SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() == SPIFFS_ID) {
      Reseau.remove(i);
      File configFile = SPIFFS.open(JSON_CONFIG_Reseau, "w");

      if (!configFile) {
        if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration réseau pour écriture...");
        return false;
      }

      if (serializeJson(SPIFFS_Config_Reseau, configFile) == 0) {
        if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'écrire le fichier de configuration...");
        return false;
      }
      configFile.close();
      if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Reseau, Serial);
      if (_Debug==true) Serial.println("eeBudServerManager:  Suppression terminée!");
      delay(200);
      return true;
    }
    delay(10);
  }
}


bool SPIFFS_UpdateReseau() {
  if (_Debug==true) Serial.println("eeBudServerManager:  Modification de la configuration réseau...");
  JsonArray reseaux = SPIFFS_Config_Reseau["reseau"];
  for (int i = 0; i < reseaux.size(); i++) {
    if (SPIFFS_Config_Reseau["reseau"][i]["id"].as<String>() == SPIFFS_ID) {


      SPIFFS_Config_Reseau["reseau"][i]["ssid"] = SPIFFS_SSID;
      SPIFFS_Config_Reseau["reseau"][i]["pass"] = SPIFFS_Pass;
      SPIFFS_Config_Reseau["reseau"][i]["ip"] = SPIFFS_IP;

      File configFile = SPIFFS.open(JSON_CONFIG_Reseau, "w");
      if (!configFile) {
        if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration réseau pour écriture...");
        return false;
      }

      if (serializeJson(SPIFFS_Config_Reseau, configFile) == 0) {
        if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'écrire le fichier de configuration réseau...");
        return false;
      }
      configFile.close();
      if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Reseau, Serial);
      if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
      delay(200);
      return true;
    }
    delay(10);
  }
}

bool SPIFFS_EnregistrementAP() {
  if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement de la configuration réseau...");

  SPIFFS_Config_Reseau["ap"][0]["pass"] = SPIFFS_Pass_AP;
  SPIFFS_Config_Reseau["ap"][0]["ip"] = SPIFFS_IP_AP;
  SPIFFS_Config_Reseau["ap"][0]["actif"] = SPIFFS_Actif_AP;



  File configFile = SPIFFS.open(JSON_CONFIG_Reseau, "w");

  if (!configFile) {
    if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
    return false;
  }

  if (serializeJson(SPIFFS_Config_Reseau, configFile) == 0) {
    if (_Debug==true) Serial.println(F("eeBudServerManager:  Impossible d'écrire le fichier de configuration..."));
    return false;
  }
  configFile.close();
  if (_Debug==true) serializeJsonPretty(SPIFFS_Config_Reseau, Serial);
  if (_Debug==true) Serial.println("eeBudServerManager:  Enregistrement terminé!");
  return true;
}

void SPIFFS_CreationConfigReseau() {
  if (_Debug==true) Serial.println("eeBudServerManager:  Création du fichier de configuration Réseau...");
  JsonArray reseau = SPIFFS_Config_Reseau.createNestedArray("reseau");
  JsonArray ap = SPIFFS_Config_Reseau.createNestedArray("ap");
  JsonObject Param = ap.createNestedObject();
  Param["pass"] = "";
  Param["ip"] = "192.168.1.1";
  Param["actif"] = "0";

  File configFile = SPIFFS.open(JSON_CONFIG_Reseau, "w");

  if (!configFile) {
    if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
  }

  if (serializeJson(SPIFFS_Config_Reseau, configFile) == 0) {
    if (_Debug==true) Serial.println(F("eeBudServerManager:  Impossible d'écrire le fichier de configuration..."));
  }
  configFile.close();
  if (_Debug==true)serializeJsonPretty(SPIFFS_Config_Reseau, Serial);
  if (_Debug==true) Serial.println("eeBudServerManager:  Création du fichier Réseau terminée!");
}

void SPIFFS_CreationConfigParams() {
  if (_Debug==true) Serial.println("eeBudServerManager:  Création du fichier de configuration des paramètres...");
  JsonArray Params = SPIFFS_Config_Params.createNestedArray("params");



  File configFile = SPIFFS.open(JSON_CONFIG_Params, "w");

  if (!configFile) {
    if (_Debug==true) Serial.println("eeBudServerManager:  Impossible d'ouvrir le fichier de configuration pour écriture...");
  }

  if (serializeJson(SPIFFS_Config_Params, configFile) == 0) {
    if (_Debug==true) Serial.println(F("eeBudServerManager:  Impossible d'écrire le fichier de configuration..."));
  }
  configFile.close();
  serializeJsonPretty(SPIFFS_Config_Params, Serial);
  if (_Debug==true) Serial.println("eeBudServerManager:  Création du fichier terminée!");
}