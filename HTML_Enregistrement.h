const char Page_Enregistrement[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <title>eeBud</title>
    <style>
		@font-face {
			font-family:'Roboto';
			src: url(‘/fonts/Roboto-Regular.woff2’) format(‘woff2’),
			url(‘/fonts/Robot-Regular.ttf’) format(‘truetype’)
		}
      
		.FondCouleur {
			position:relative;
			font-family: 'Roboto', sans-serif;
			background-color: rgba(30,80,100,1);		
			text-align: center;
			color: white;
			margin-left:-10px;
      margin-right:-10px;
      margin-top:-30px;
		}  
  
		.Titre {
			font-family: 'Roboto', sans-serif;			 
			text-align: center;
			color: white;
			font-weight:bold;
      word-wrap:break-word;
      font-size: 80px;      
		} 
      
    .SousTitre {
			font-family: 'Roboto', sans-serif;			 
			text-align: center;
			color: white;
			font-weight:bold;
      word-wrap:break-word;
      font-size: 40px;      
		} 
  
		
      
    .FondBlanc {
			position:relative;
			font-family: 'Roboto', sans-serif;
			background-color: rgba(255,255,255,1);			 
			text-align: center;
			color: rgba(30,80,100,1);		
			font-size: 50px;
      padding-left: 15px;
      padding-right: 15px;
		}
      
    .FondBlancTitre {
			position:relative;
			font-family: 'Roboto', sans-serif;
			background-color: rgba(255,255,255,1);			 
			text-align: center;
			color: rgba(30,80,100,1);		
			font-size: 70px;
      padding-left: 15px;
      padding-right: 15px;
		} 
      
		
    
      
	</style>
  </head>
  <body>
    <div class="FondCouleur"> <br>
      <h1 class="Titre">%APP%</h1>
      <h2 class="SousTitre">Connexion au WiFi</h2>
      <br>
    </div>
    <div class="FondBlancTitre"> <br>
      <b><label>Paramètres enregistrés!<br>
        </label></b> </div>
    <div class="FondBlanc"> <br>
      <label>Tentative de redemarrage avec l'adresse IP:</label> <br>
      <label id="ip" name="ip">%IP%</label><br>
      <br>
      <label>Vous pourrez donc vous connecter à l'appareil depuis votre réseau
        local à cette adresse.</label><br>
    </div>
  </body>
</html>



)rawliteral";