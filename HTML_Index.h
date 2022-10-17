const char Page_Index[] PROGMEM = R"rawliteral(
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
            transition: transform .3s cubic-bezier(0.85, 0.05, 0.18, 1.35);
          
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
  			  transition: transform .5s cubic-bezier(0.85, 0.05, 0.18, 1.35);
       
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
            padding-bottom:30px;
        }

        .Seg {
            position: relative;
            background-color: rgba(30,80,100,1);
            border-radius: 5px;          
            margin-left: 30px;
            margin-right: 30px;
            height: 10px;            
        }

        .Segw {
            position: relative;
            background-color: white;
            border-radius: 5px;          
            margin-left: 30px;
            margin-right: 30px;
            height: 10px;            
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
            font-size: 50px;
            
            
        }       

        

        .BoutonBlanc {
            padding: 5px;
            background: white;
            border-radius: 45px;
            height: 90px;
            font-size: 40px;
            border: 3px solid white;
            transition-duration: 200ms;
            transition-timing-function: linear;
            color: rgba(30,80,100,1);
            width: 300px;
        }


            .BoutonBlanc:active {
                transform: scale(1.1);
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

        .fond_couleur {
          	margin-left: auto;
    				margin-right: auto;
            width: 800px;
            height: 90px;
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

        input[type=range] {
          margin-top:50px;  
  				-webkit-appearance: none;  
  				width: 850px;
				}
      
				input[type=range]:focus {
  				outline: none;
				}
				input[type=range]::-webkit-slider-runnable-track {
  				width: 850px;
  				height: 8px;
  				cursor: pointer;
  				animate: 0.2s;
  				background: #d6d6d6;
  				border-radius: 8px;
  				border: 0px solid #000000;
				}
      
				input[type=range]::-webkit-slider-thumb {
 					box-shadow: 0px 0px 0px #000000;
  				border: 0px solid #000000;
  				height: 50px;
  				width: 80px;
  				border-radius: 25px;
  				background: rgba(30,80,100,1);
  				cursor: pointer;
  				-webkit-appearance: none;
  				margin-top: -21px;
				}

        .input_color {
  				appearance:none;
  				width: 140px;
  				height: 140px;
  				background-color: transparent;
          border: none;
          position: absolute;
          right:-10px;
          top:-30px;
                  
				}
      	
      	.input_color::-webkit-color-swatch {
        	border-radius: 20px;
        	
      	}
      
      	.FondBlanc_Color {
            position: relative;
            font-family: 'Roboto', sans-serif;
            background-color: rgba(255,255,255,1);
            text-align: left;
            color: rgba(30,80,100,1);
            font-size: 50px;            
        		height: 140px;
        		width: 650px;
        		line-height: 140px;       				
        
        		margin-left: auto;
    				margin-right: auto;
            
        	}

    </style>
  </head>
  <body>
    <div class="FondCouleur">
      <br>
      <br>
      <br>
      <button class="BoutonBlanc" onclick="window.location.href = '/load?nav=config';" style="text-decoration:none">            
        Configuration           
      </button>
      <h1 class="Titre">%APP%</h1>  
    </div>
    <div style="margin-top: 30px;" class="FondBlanc">
        
      %ACTIONS%      
      
      <br>       
        
    </div>
    
    <script>
      function Click(element) {        
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/update?send="+element.id+"", true);
        xhr.send();
      }

      function ClickToogle(element, value0, value1, id0, id1) {
        var xhr = new XMLHttpRequest();
        if (element.checked) {
          xhr.open("POST", "/update?send="+id1+"", true);
          document.getElementById("Texte" + element.id + "").innerHTML = "" + value1 + "";          
          
        }else{          
          xhr.open("POST", "/update?send="+id0+"", true);
          document.getElementById("Texte" + element.id + "").innerHTML = "" + value0 + ""; 
          
        }    
        xhr.send();
      }

      function printColor(element, ev) {
          const color = ev.target.value;
          const r = parseInt(color.substr(1, 2), 16);
          const g = parseInt(color.substr(3, 2), 16);
          const b = parseInt(color.substr(5, 2), 16);
          var xhr = new XMLHttpRequest();
          xhr.open("POST", "/update?send_color="+element.id+"&r="+r+"&g="+g+"&b="+b+"", true);
          xhr.send();
          
      }

      
      function ValidRange(element) {        
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/update?send_range="+element.id+"&value="+element.value+"", true);
        xhr.send();
      }

      
      
    </script>
    
  </body>
</html>

)rawliteral";