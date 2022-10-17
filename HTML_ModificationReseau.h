const char Page_ModificationReseau[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <title>eeBud</title>
    <style>
        @font-face {
            font-family: 'Roboto';
            src: url(‘/fonts/Roboto-Regular.woff2’) format(‘woff2’), url(‘/fonts/Robot-Regular.ttf’) format(‘truetype’)
        }

        .FondCouleur {
            position: relative;
            font-family: 'Roboto', sans-serif;
            background-color: rgba(30,80,100,1);
            text-align: center;
            color: white;
            margin-left: -10px;
            margin-right: -10px;
            margin-top: -30px;
        }

        .Titre {
            font-family: 'Roboto', sans-serif;
            text-align: center;
            color: white;
            font-weight: bold;
            word-wrap: break-word;
            font-size: 80px;
        }

        .SousTitre {
            font-family: 'Roboto', sans-serif;
            text-align: center;
            color: white;
            font-weight: bold;
            word-wrap: break-word;
            font-size: 40px;
        }



        .FondBlanc {
            position: relative;
            font-family: 'Roboto', sans-serif;
            background-color: rgba(255,255,255,1);
            text-align: center;
            color: rgba(30,80,100,1);
            font-size: 40px;
            padding-left: 15px;
            padding-right: 15px;
        }

        .box {
            padding: 5px;
            background: #FFFFFF;
            -webkit-border-radius: 40px;
            border-radius: 40px;
            height: 90px;
            font-size: 40px;
            border: 3px solid rgba(30,80,100,1);
            transition: transform .4s cubic-bezier(0.65, 0.05, 0.18, 1.6);
            width: 95px;
        }

            

            .box:focus {
                transform: scale(1.2);
                box-shadow: 0px 0px 5px 5px rgba(30,80,100,0.5);
            }

                .BoutonBlanc {
            padding: 5px;
            background: white;
            border-radius: 45px;
            height: 90px;
            font-size: 40px;
            border: 3px solid white;
            transition: transform .4s cubic-bezier(0.65, 0.05, 0.18, 1.6);
            color: rgba(30,80,100,1);
            width: 300px;
        }


            .BoutonBlanc:active {
                transform: scale(1.2);
                box-shadow: 0px 0px 5px 5px rgba(255,255,255,0.5);
            }

        .hide{display: none;}
        
      	.TotalBouton {
        		height: 140px;
            width: 850px;
            margin-left: auto;
    				margin-right: auto;
      	}
        .Bouton {
            visibility: visible;
            position: relative;
            top: -90px;
            left: 0px;
            background: rgba(30,80,100,1);
            border-radius: 43px;
            height: 86px;
            width: 804px;
            font-size: 45px;
            border: 3px solid rgba(30,80,100,1);
            transition: transform .4s cubic-bezier(0.65, 0.05, 0.18, 1.6);
            color: white;
            text-shadow: #00B6FF 0px 0px 4px;
        }

            .Bouton:active {
                transform: scale(1.2);                
                font-size: 55px;
                text-shadow: yellow 0px 0px 4px;
            }



        .fond_bouton {
          	margin-left: auto;
    				margin-right: auto;
            width: 800px;
            height: 90px;
            border-radius: 45px;
            border: 2px solid #145F77;
            background-image: linear-gradient(90deg,rgba(30,80,100,1), #FFFFFF,rgba(30,80,100,1) );            
            animation: 1s linear infinite alternate slidebg;            
            display: flex;
        }

            

        @keyframes slidebg {
            from {
                background-position: -150px;
            }

            to {
                background-position: 150px;
            }
        }

        .reseau {
            visibility: visible;
            position: relative;
            top: 0px;
            left: 0px;
            background: rgba(30,80,100,1);
            border-radius: 0px;
            height: 86px;
            width: 804px;
            font-size: 45px;
            border: 3px solid rgba(30,80,100,1);
            transition: transform .4s cubic-bezier(0.65, 0.05, 0.18, 1.6);
            color: white;
            text-shadow: #00B6FF 0px 0px 4px;
        }

            .reseau:active {
                transform: scale(1.1);                
                font-size: 55px;
                text-shadow: yellow 0px 0px 4px;
            }
    </style>
  </head>
  <body>
    <div class="FondCouleur"> <br>
      <br>
      <button class="BoutonBlanc" onclick="window.location.href = '/load?nav=listnetwork';" style="text-decoration:none">
        Retour </button>
      <h1 class="Titre">%APP%</h1>
      <h2 class="SousTitre">Modification des paramètres de connexion</h2>
      <br>
    </div>
    
    <form action="/save?update=wifi" method="POST">
      <div class="FondBlanc">       
        <p> 
        
        <b><label>Nom du réseau:</label></b> <br>
          <input class="box" id="ssid" name="ssid" style="width: 800px; text-align: center;"

            value="%SSIDUPDATE%" type="text"> <br>
          <br>
          <b><label>Mot de passe:</label></b> <br>
          <input class="box" id="pass" name="pass" style="width: 800px; text-align: center;"

           value="%PASSUPDATE%" type="password"> <br>
          <br>
          <b><label>Adresse IP:</label></b> <br>
          <input class="box" id="ip" name="ip" style="width: 800px; text-align: center;"

            value="%IPUPDATE%" type="text"> <br>
          
          
        </p>
      </div>
      
      
        <div class="FondBlanc"> <br>
         <br>
          <div class="TotalBouton"> <span name="fond" id="fond" class="fond_bouton">
            </span> <input class="Bouton" value="Modifier" style="text-align: center;" type="submit">
               </div>
          
        </div>
        
      
    </form>
    <div class="FondBlanc">
    <div class="TotalBouton"> <span name="fond" id="fond" class="fond_bouton">
        </span> <button class="Bouton" onclick="window.location.href = '/load?nav=removenetwork';" style="text-decoration:none">
          Supprimer</button> </div>
      </div>

    
    
  </body>
</html>





)rawliteral";