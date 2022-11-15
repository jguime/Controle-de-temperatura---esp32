/*=====CONTROLADOR DE TEMPERATURA COM MULTSPEED=========*/ 
/* =====================================================*/
/*  Desenvolvido em prol do GRAND PRIX - OBOTICARIO     */
/*  Por: JOAO GUILHERME. - ESTUDANTE DE AUTOMAÇÃO/ADS   */ 
/*------------------------------------------------------*/ 


// Importe das bibliotecas
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Informações do Wifi e login do site
const char* ssid = "iPhone De João";
const char* password = "12345678g";

const char* http_username = "admin";
const char* http_password = "admin";

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";


//Define do sensor de temperatura
#define sensor_temp A5

// Define das saídas para cada velocidade 
#define v1 11
#define v2 12
#define v3 2

bool automatico = false;
bool manual = false;
int velocidade = 0;
int indice = 0 ;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  //float lersensor = analogRead(sensor_temp)*0.097751710;
  // adicionar filtro digital para entrada analógica.
  float lersensor = random(0,100);  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  Serial.println(lersensor);
 
 
  if(lersensor>0)
  {
    if (lersensor > 25){ 
      //velocidade = 3; // TEMPERATURA ALTA
    } 
    if(lersensor > 23  && lersensor <= 25){
     // velocidade = 2; // TEMPERATURA NORMAL
    }
    if(lersensor > 0 && lersensor <= 23){
     // velocidade = 1;// TEMPERATURA BAIXA
    }
    // inicia função para setar temperatura  
   // set_velo(velocidade);
    
  }else {
    Serial.println("\nErro de leitura!!");
   //set_velo(0);
    }
    return String(lersensor);
  }

void set_velo(int vel_value)
{  
  switch(vel_value){
    case(3):
      Serial.println("\nTEMPERATURA ALTA");
    	Serial.println(" ");
  	  	digitalWrite(v1, LOW);
        digitalWrite(v2, LOW);
        digitalWrite(v3, HIGH);
        break;  
    case 2:
        Serial.println("\nTEMPERATURA NORMAL");
    	  Serial.println(" ");
        digitalWrite(v1, LOW);
        digitalWrite(v2, HIGH);
        digitalWrite(v3, LOW);
        break;
    case 1 :
        Serial.println("\nTEMPERATURA BAIXA");
    	  Serial.println(" ");
        digitalWrite(v1, HIGH);
        digitalWrite(v2, LOW);
        digitalWrite(v3, LOW);
       break;
      case 100:
        Serial.println("\nMODO AUTO LIGADO");
        Serial.println(" ");
  	  	automatico = true; 
        break;      
      default:
        Serial.println("ERRO NA LEITURA DA TEMPERATURA");
        digitalWrite(v1, LOW);
        digitalWrite(v2, LOW);
        digitalWrite(v3, LOW);
        break;  
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
        *{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            text-align: center;
            height: auto;

        }
        h3{
            color: white;
            margin-top: 25px;
            position:relative;

        }
        h2{
            position: relative;
            display: inline-block;
            margin: 0px;
            padding-top: 10px;
            color: white;
            
        }
        .auto{
            margin-top: 30px;
            float: left;
            position: absolute;
            text-align: center;

        }
        .bt{
            float: left;
            margin-left: 15px ;
            margin-top: 25px;

        }
        .menu{
            vertical-align: middle;
            text-align: center;
            display: inline-block;
            width: 100vw;
            height: 70px;
            background-color: rgb(0, 0, 155);
        }   

        h4{
            padding-top: 5px;
        }

        .tag{
            width: 30%;
            float: left;
            margin: 8px;
            text-align: center;
            

            
        }

        .block{
            width: 100%;
            height: 100%;
            display: inline-block;
            text-align: center;
        }
        .Container_info{
            width: 100%;
            border-radius: 15px;
        }


        .container_L, .Container_R{
            width: 300px;
            border-radius: 15px;
            border: 2px solid rgb(2, 2, 2);
            
            display: inline-block;
            margin-top: 20px;
            
        }
        .container_C{
            margin-top: 30px;
        }
        .table-responsive{
            width: 100% !important;
        }
        
        .table {
            border: 1px solid rgb(2, 2, 2);
            width: 50% !important; /*Importante manter o !important rs */
            margin: auto;
            text-align: left;
        } 
        
        .table-status {
            margin-top: 20px;
            margin-bottom: 20px !important;
        }

            
        .switch {
            position: relative; 
            display: inline-block; 
            width: 90px; 
            height: 54px;
        } 

        .switch input {
            display: none
        }

        .slider {
            position: absolute; 
            top: 0; 
            left: 0; 
            right: 0; 
            bottom: 0; 
            background-color: #ccc; 
            border-radius: 30px
        }

        .slider:before {
            position: absolute; 
            content: ""; 
            height: 39px; 
            width: 39px; 
            left: 8px; 
            bottom: 8px; 
            background-color: #fff; 
            -webkit-transition: .4s; 
            transition: .4s; 
            border-radius: 20px
        }

        input:checked+.slider {
            background-color: rgb(0, 144, 53)
        }

        input:checked+.slider:before {
            -webkit-transform: translateX(34px); 
            -ms-transform: translateX(34px); 
            transform: translateX(34px)
        }
  </style>
</head>
<body>
  <main>
        <div class="menu">
            <button class="bt" onclick="logoutButton()">Logout</button>
             <div><h3>CONTROLE DE TEMPERATURA</h3></div>
            <div class="auto">
                <p>AUTO</p>                    
                  <input type="checkbox" onchange="toggleCheckbox(this)" id="100" + outputState(1)>          
            </div>
            
        </div>
        <div class="block">
            <div class="container_L">
                <h><div class="tag">Velocidade 1</div>
                    <label class="switch">
                        <input type="checkbox" onchange="toggleCheckbox(this)" id="1" + outputState(1)>
                    <span class="slider round"></span>
                </label>
                </h4>
            </div>
            <div class="Container_R">
                <h4><div class="tag">Velocidade 2</div>
                    <label class="switch">
                        <input type="checkbox" onchange="toggleCheckbox(this)" id="2" + outputState(2)>                   
                    <span class="slider round"></span>
                </label>
                </h4>                  
            </div>
            <div class="Container_R">
                <h4><div class="tag">Velocidade 3</div>
                    <label class="switch">
                        <input type="checkbox" onchange="toggleCheckbox(this)" id="3" + outputState(3)>                    
                    <span class="slider round"></span>
                </label>
                </h4>                  
            </div>
            <div class="container_C">
              <i class="fas fa-thermometer-half">
              <p>
                
                <span class="dht-labels">Temperature</span>
                <span id="temperature">%TEMPERATURE%</span>
                <sup class="units">°C</sup>
              </p>
                
              </div>
        </div>
    </div>
    </main>
  <script>
  function toggleCheckbox(element) {
      var xhr = new XMLHttpRequest();
      
        if(element.checked){ 
          xhr.open("GET", "/update?output="+element.id+"&state=1", true);
          
        }else { 
          xhr.open("GET", "/update?output="+element.id+"&state=0", true);}
        xhr.send();
  }
  function logoutButton() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/logout", true);
    xhr.send();
    setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
  }
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {document.getElementById("temperature").innerHTML = this.responseText;}
    };
    xhttp.open("GET", "/temperature", true);
    xhttp.send();
  }, 5000 ) ;
  </script>
</body>
</html>
)rawliteral";

const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <p>Logged out or <a href="/">return to homepage</a>.</p>
  <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
</body>
</html>
)rawliteral";

//Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readTemperature();
  }
  return String();
}

String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  
  pinMode(v1, OUTPUT);
  pinMode(v2, OUTPUT);
  pinMode(v3, OUTPUT);

  digitalWrite(v1, LOW);
  digitalWrite(v2, LOW);
  digitalWrite(v3, LOW);  

  pinMode(sensor_temp, INPUT);
 // pinMode(output, OUTPUT);
 // digitalWrite(output, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemperature().c_str());
  }); 
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", logout_html);
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String inputMessage1;
    String inputMessage2;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_1;
      
      //digitalWrite(inputMessage1.toInt(),inputMessage2.toInt());
      
      if(inputMessage1.toInt() == 100){
        inputMessage2.toInt() == 1 ? automatico = true:automatico = false;
        inputMessage2.toInt() == 0 ? manual = true:manual = false;        
      
      }else if(manual){
          set_velo(inputMessage1.toInt());
          Serial.println("manual");
        }
      
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
      inputParam = "none";
    }
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Set to: ");
      Serial.println(inputMessage2);
      request->send(200, "text/plain", "OK");
  });
  
  // Start server
  server.begin();
}
  
void loop() {
  if(automatico){
     float lersensor = readTemperature().toFloat();  // Read temperature as Fahrenheit (isFahrenheit = true)
    //float t = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    Serial.print(lersensor);
  
 
    if(lersensor>0)
    {
      if (lersensor > 25){ 
        velocidade = 3; // TEMPERATURA ALTA
      } 
      if(lersensor > 23  && lersensor <= 25){
       velocidade = 2; // TEMPERATURA NORMAL
      }
      if(lersensor > 0 && lersensor <= 23){
       velocidade = 1;// TEMPERATURA BAIXA
      }
       //inicia função para setar temperatura  
       set_velo(velocidade);   
    }else {
      Serial.println("\nErro de leitura!!");
      set_velo(0);
      } 
     delay(5000); 
  }
  //else if(manual){}
  
}