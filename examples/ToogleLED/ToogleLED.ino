// On inclut les bibliothèque
#include <eeBudServerManager.h>
//Définition des pins de la LED
#define LED 32

//Création du serveur qui sera nommé "LED connectée".
eeBudServerManager Serveur("LED connectée");

//ID pour la création de l'index eeBud Server Manager et la récupération des paramètres.
int *Bouton;

//Variable de l'écouteur d'événements
int Event;

void setup() {  
  //Initialisation du serveur.
  Serveur.Init();

  //Mise en page de l'index avec un seul bouton basculant
  Bouton = Serveur.IndexAddToogle("Etat de la LED", "Off", "On", "Off");
//Notre bouton a donc récupéré 2 ID qui sont Bouton[0] qui veut dire "Off" et Bouton[1] qui veut dire "On"

  //Réglage du port numérique associé à la LED.
  pinMode(LED, OUTPUT);
}

void loop() {
//On écoute les événements envoyés par le serveur (dans ce cas: le bouton basculant).
  Event = Serveur.IndexGetEvent();

//On crée la condition pour activer la LED depuis l'écoute de l'événement.
//Si l'événement envoyé par le serveur = ID Off (Bouton[0]) du bouton créé.
if (Event == Bouton[0]) {

    //On passe la led sur Off
    digitalWrite(LED, LOW);
    //On retourne l'état au serveur à l'aide de l'ID
    Serveur.IndexSetToogle(Bouton[0]);
    //On réinitialise l'événement
    Serveur.IndexResetEvent();

  } else if (Event == Bouton[1]) {

    //On passe la led sur On
    digitalWrite(LED, HIGH);
    //On retourne l'état de la LED au serveur
    Serveur.IndexSetToogle(Bouton[1]);
    //On réinitialise l'événement
    Serveur.IndexResetEvent();

  }
}