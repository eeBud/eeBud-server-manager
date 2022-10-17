const char Page_Message[] PROGMEM = R"rawliteral(
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

    .FondCadre {
			position:relative;
			font-family: 'Roboto', sans-serif;
			border: solid rgba(30,80,100,1);
			text-align: center;
			color: rgba(30,80,100,1);	
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
      
		.BoutonBlanc {
            padding: 5px;
            background: white;
            border-radius: 40px;
            height: 90px;
            font-size: 40px;
            border: 3px solid white;
            transition: transform .4s cubic-bezier(0.65, 0.05, 0.18, 1.6);
            color: rgba(30,80,100,1);
            width: 300px;
        }


            .BoutonBlanc:active {
                transform: scale(1.1);
                box-shadow: 0px 0px 5px 5px rgba(255,255,255,0.5);
            }
    
      
	</style>
  </head>
  <body>
    <div class="FondCouleur"> <br>
    <br>
      <br>
      <button class="BoutonBlanc" onclick="window.location.href = '/load?nav=config';" style="text-decoration:none">
        Retour </button>
      <h1 class="Titre">%APP%</h1>
      <h2 class="SousTitre">Système</h2>
      <br>
    </div>
    <div class="FondBlancTitre"> <br>
      <b><label>%TITRE%<br>
        </label></b> </div>
    <div class="FondBlanc"> <br>
      <label>%MESSAGE%</label> <br>
      <br><br>
      <b><label style="font-size: 150px;" id="tim" name="tim"></label></b><br>
    </div>
    
  </body>
</html>




   )rawliteral";