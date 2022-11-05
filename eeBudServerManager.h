#ifndef eeBudServerManager_h
#define eeBudServerManager_h
#include "Arduino.h"

class eeBudServerManager {

public:
  eeBudServerManager(char* NomProgramme);
  //Setup
  void Init();

  void AddParamText(String Name, String Default);
  void AddParamBool(String Name, bool Default);

  int IndexAddButton(String Name);
  void IndexAddTitle(String Name, String Size);
  int IndexAddInfo(String Name, String Unit, String Size);
  int* IndexAddToogle(String Name, String Value_0, String Value_1, String Default);
  int IndexAddColor(String Name, int Default_R, int Default_G, int Default_B);
  int IndexAddRange(String Name, int Min, int Max, int Default);
  void IndexAddHTML(String Value);
  void IndexBackgroundStart();
  void IndexBackgroundFinish();

  //Loop
  int IndexGetEvent();

  //Autre
  String GetParamText(String Name);
  bool GetParamBool(String Name);
  void RemoveParam(String Name);
  void UpdateParamText(String Name, String Value);
  void UpdateParamBool(String Name, bool Value);

  bool ResetParams();
  bool ResetNetwork();
  void ResetAll();

  int IndexGetRange(int ID);
  int IndexGetColor_R(int ID);
  int IndexGetColor_G(int ID);
  int IndexGetColor_B(int ID);
  void IndexAddSeg();
  void IndexAddSegWhite();

  void IndexSetInfo(int ID, String Value);
  void IndexSetToogle(int ID);

  void IndexResetEvent();

  String HTTPGet(String URL);
  void HTTPPost(String URL);

  int GetWiFiStatus();

  void Debug(bool Value);
  void Scan_Network(bool Value);

private:
  bool initPA();
  bool initWiFi();
  bool WiFiConnect();

  void ServeurDNS();
  void ServeurStation();

  static String RetourVariable(String var);
  void ScanWiFi();
  String ScanWiFiReco();
};




#endif