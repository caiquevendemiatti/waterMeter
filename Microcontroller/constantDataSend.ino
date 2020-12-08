#include <ESP8266WiFi.h>
//Dados da rede Wi-Fi
const char* ssid     = "TP-LINK_B989D8";
const char* password = "null";
//IP servidor Windows const
byte server1[] = {192, 168, 0, 102};


//IDs dos equipamentos simulados
int id = 1;
//Variaveis para simulação de leitura
int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Conecta a rede Wi-Fi selecionada
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    //aguarda conexão
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(3000);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8095;
    if (!client.connect(server1, 8095)) {
      //tenta conexão com o servidor passando IP e Porta
      Serial.println("connection failed");
      return;
    }  
 
    // Cria a URL para a requisição ao Windows
    String url = "/API/index.php?action=write&nseC=";
    url += id;
    url += "&sensor1=";
    url += sensor1;
    url += "&sensor2=";
    url += sensor2;
    url += "&sensor3=";
    url += sensor3;

    client.print(String("GET ") + url + "\r\n");
    unsigned long timeout = millis();  
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    Serial.println("closing connection");

 delay(3000);
}