#ifndef eeBudServerManager_h
#define eeBudServerManager_h
#include "Arduino.h"

class eeBudServerManager {

public:
  eeBudServerManager(char* NomProgramme);
  void Init();

  void AddParamText(String Name, String Default);
  void AddParamBool(String Name, bool Default);
  String GetParamText(String Name);
  bool GetParamBool(String Name);
  void RemoveParam(String Name);
  void UpdateParamText(String Name, String Value);
  void UpdateParamBool(String Name, bool Value);

  bool ResetParams();
  bool ResetNetwork();
  void ResetAll();

  int IndexAddButton(String Name);
  int IndexAddInfo(String Name, String Unit, String Size);
  int* IndexAddToogle(String Name, String Value_0, String Value_1, String Default);
  void IndexAddSeg();

  void IndexSetInfo(int ID, String Value);
  void IndexSetToogle(int ID);
  int IndexGetEvent();
  void IndexResetEvent();

  String HTTPGet(String URL);
  void HTTPPost(String URL);

  int GetWiFiStatus();

  void Debug(bool Value);

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