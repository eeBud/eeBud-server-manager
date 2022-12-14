const char Page_ConfigAP[] PROGMEM = R"rawliteral(
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
.label1 {
          
            display: inline-flex;
            align-items: center;
            cursor: pointer;
        }

        .toggle {
            z-index: 12;
            isolation: isolate;
            position: relative;
            height: 90px;
            width: 800px;
            border-radius: 45px;
            background: #d6d6d6;
            overflow: hidden;
        }

        .toggle-inner {
          	z-index: 14;
            position: absolute;
            top: 4px;
            left: 4px;
            height: 82px;
            width: 792px;
            border-radius: 45px;
            overflow: hidden;
          
        }

        .active-bg {
            z-index: 13;
           position: absolute;
            top: 0;
            left: 0;
            height: 90px;
            width: 800px;
            background: rgba(30,80,100,1);
            transform: translate3d(-800px, 0, 0);
            transition: transform .2s linear 0.17s;
        }

        .toggle-state {
           
            display: none;
        }

        .indicator {
          	z-index: 15;
            position: absolute;
            height: 82px;
            width: 1600px;
            background: rgba(30,80,100,1);
            border-radius: 41px;
            transform: translate3d(-1200px, 0, 0);
            transition: transform .5s cubic-bezier(0.85, 0.05, 0.18, 1.35);
          
        }

        .textd { 
          cursor: pointer;
          position: relative;
          z-index: 16;
          height: 80px;
  			  width: 400px;
          top: 4px;
  			  left: 0px;
          background: transparent;
    		  border: none;
          text-align: center;
          line-height: 80px;
          color: white;
          font-family: 'Roboto', sans-serif;
          font-size: 40px;  
          transform: translate3d(0px, 0, 0);
  			  transition: transform .7s cubic-bezier(0.85, 0.05, 0.18, 1.35);
       
        }
      
      	

              
      .toggle-state:checked ~ .textd {           
            transform: translate3d(400px, 0, 0); 
        		color:rgba(30,80,100,1);
        }

        .toggle-state:checked ~ .active-bg {
            position: absolute;
            transform: translate3d(0px, 0, 0);
          
        }

        .toggle-state:checked ~ .toggle-inner .indicator {
           position: absolute;
            transform: translate3d(400px, 0, 0);
          background: white;
          
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
    </style>
  </head>
  <body>
    <div class="FondCouleur"> <br>
      <br>
      <button class="BoutonBlanc" onclick="window.location.href = '/load?nav=config';" style="text-decoration:none">
        Retour </button>
      <h1 class="Titre">%APP%</h1>
      <h2 class="SousTitre">Configuration du point d'accès</h2>
      <br>
    </div>

    <form action="/save?config=AP" method="POST">
      <div class="FondBlanc"> <br>
        <p> <b><label>Clé:</label></b> <br>
          <input class="box" id="pass" name="pass" style="width: 800px; text-align: center;" value="%MDPAP%" type="text"><br><br>
          
          <b><label>Adresse IP:</label></b> <br>
          <input class="box" id="ip" name="ip" style="width: 800px; text-align: center;"value="%IPAP%" type="text"> <br><br>
          
          
          
          
          
          %ACTIFAP%
          <br><br>
        </p>
      </div>
      <br>
      <br>
      <div class="FondCouleur"> <br>
        <div class="FondBlanc"> <br>
         <br>
          <div class="TotalBouton"> <span name="fond" id="fond" class="fond_bouton">
            </span> <input class="Bouton" value="Valider" style="text-align: center;" type="submit">
               </div>
          <br>
        </div>
        <br>
      </div>
    </form>
    <script> 
      function ClickToogle(toogle) {        
        if (toogle.checked) {
          document.getElementById("actif").value = "1";
          document.getElementById("texte").innerHTML = "Actif";          
        }else{          
          document.getElementById("actif").value = "0";
          document.getElementById("texte").innerHTML = "Inactif";          
        }
      }
    </script>
  </body>
</html>





)rawliteral";