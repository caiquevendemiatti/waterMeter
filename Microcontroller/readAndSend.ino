#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include<EEPROM.h>
#include<FS.h>
#include <user_interface.h>;

os_timer_t tmr0; //Instantiate one timer

bool _timeout = false; //Variables for controlling timer
bool _resettime = false;

/*Inserção de variaveis
  correspondetes a leitura dos dados*/

/*Variables for reading data*/

const int pin = 4; //Flow sensor input pin (GPIO4)

unsigned int Pulso; //Flow sensor pulses counter


float vazaoagua; //Water flow
float vazaomedia; //Mean water flow
float soma = 0;  //Read consumption variables
float somalitros = 0; //Consumption sum
float somapressao = 0; //Pressure sum
float pressaomedia = 0; //Mean pressure

//Variables for converting electrical unities to pressure
double p_volts = 0; 
int p_analogica = 0;
double p_bar = 0;

//Control variables
int i = 0;
int j = 0; 
int falha = 0;
int conteeprom = 0;

//Pressure sensor parameters 

//Tensão mínima a ser lida pela placa
float V_min = 319; //Minimum input voltage

float V_max = 5000; //Maximum input voltage

float P_max = 120000; //Maximum sensor pressure

float P_min = 0; //Minimum sensor pressure

/*Data server parameters*/
byte server1[] = {192, 168, 0, 50};
const int httpPort1 = 80;

//Meter serial number
char nse[16] = "20180524130542O";


void setup() {
  pinMode(13, OUTPUT); //Define pin for generating microcontroller self reset
  digitalWrite(13, HIGH);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Bem vindo ao Y-Água");
  Serial.println("");

  //Configure timers
  os_timer_disarm(&tmr0);
  os_timer_setfn(&tmr0, flagtimer, NULL);

  //Configura pino de input de sinal de vazão
  pinMode(pin, INPUT);
  //define a porta em questão como de interrupção,
  //define função de desvio e configura interrupção
  //por borda de subida
  attachInterrupt(pin, toggle, RISING);
  //Gera delay para inicialização do
  //roteador em caso de queda de energia
  timer(2000);
  timer(3000);
  //chama função de conexão de WiFi
  conectaWiFi();
  //Define pino do Led de conexão WiFi
  pinMode(D8, OUTPUT);

  if (WiFi.status() != WL_CONNECTED) {
    //se não conseguiu conectar imprime status
    digitalWrite(D8, LOW);
  }
  else
  {
    digitalWrite(D8, HIGH);
  }

}

//Início programa de leitura e envio de dados//

void loop() {
  //Leitura dos sensores

  while (i < 60) {
    //realiza 60 mediçoes dos sensores
    //Serial.print(i);
    yield(); //Alimenta WatchDogs para evitar resets desenecessários
    ESP.wdtFeed();
    //Reinicia variavel de contagem de pulso
    Pulso = 0;
    //Chama função que realiza a contagem dos pulsos
    medida();
    //Calcula a vazão
    vazaoagua = Pulso / 6.6;

    if (vazaoagua != 0) {
      //calcula volume consumido
      somalitros = somalitros + (vazaoagua / 60);
      soma = soma + vazaoagua;
      j++;
    }

    //Lê o sinal de pressão
    //E realiza conversão
   //faz a leitura da pressão
    p_analogica = analogRead(A0); 
    //converte de valor digital para volts
    p_volts = map(p_analogica, 0, 1023, 0, 3300); 
    //Converte de volts para mBar
    p_bar = (((((p_volts - V_min) * (P_max - P_min)) / (V_max - V_min)) + P_min)); 
    p_bar = p_bar/1000;

    if (p_bar <= 0) {
      //caso seja lida uma pressão menor que zero
      //mantém o valor zerado
      p_bar = 0;
    }

    somapressao = somapressao + p_bar;
    i++;
    timer(10);
  }  //Fim da leitura

  if (j == 0) {
    //se não houve consumo zera a vazão média
    vazaomedia = 0;
  }

  else {
    //se houve consumo
    //calcula a média com relação aos pontos medidos
    vazaomedia = soma / j;
  }

  //calcula pressão média
  pressaomedia = somapressao / 300;

  //Chama função que envia os dados,
  //passando os valores a serem recebidos
  //e número de série do equipamento
  envURL(pressaomedia, vazaomedia, somalitros, nse);
  timer(10); //tempo para o microcontrolador

  i = 0; //reseta variaveis do calculo da média

  vazaomedia = 0;
  somapressao = 0;
  pressaomedia = 0;
  j = 0;
  somalitros = 0;
  soma = 0;
}

//FUNÇÃO DE MEDIÇAO DE VAZÃO
void medida() {
  //Realiza a contagem dos pulsos
  //define flag de estouro de timer como falsa
  _timeout = false;
  //habilita as interrupções por parte do sensor
  sei();
  //Define tempo para a execução da leitura da vazao
  timer(1000);
  //desabilita interrupções por parte do sensor
  cli();

}
void toggle() {
  //função que conta o numero de pulsos
  Pulso = Pulso + 1;
}
//FIM DA FUNÇÃO DE MEDIÇAO DE VAZÃO

//FUNÇÃO DE DELAY
void timer(int t_time) {
  //Inidica ao Timer seu Tempo em mS e se sera repetido (1)ou apenas uma vez (0)
  _timeout = false;
  os_timer_arm(&tmr0, t_time, 0);
  while (!_timeout) {
    //aguarda até estouro do timer
    yield();
  }
}

void flagtimer(void*z) {
  //interrupção de estouro de timer
  //seta flag de estouro de timer
  _timeout = true;
}
//----FIM FUNÇÃO DE DELAY


//------------REALIZA O ENVIO DE DADOS PARA O SERVIDOR

void envURL(float sens1, float sens2, float consumo, String nseC) {
  WiFiClient client;
  // Serial.println("ENVIA");
  if (WiFi.status() == WL_CONNECTED)  {
    //se não conseguiu conectar imprime status
    //altera estado do indicador de conexão
    digitalWrite(D8, LOW);

    if (client.connect(server1, httpPort1)) {
      //mantém lede de conexão aceso
      digitalWrite(D8, HIGH);
      //cria a URL para enviar ao servidor com ultimos dados lidos
      String url = "/php/sendinfo.php?nseC=";
      url += nseC;
      url += "&sensor1=";
      url += sens1;
      url += "&sensor2=";
      url += sens2;
      url += "&sensor3=";
      url += consumo;

      //conecta ao servidor
      client.connect(server1, httpPort1);
      //Realiza requisição ao servidor
      client.print(String("GET ") + url + "\r\n");
      client.stop();
    }
  }
  else {
    //caso não consiga conexão com o servidor
    //Apaga led indicador de conexão
    digitalWrite(D8, LOW);
    falha++; //incrementa contador de falhas
    if (falha == 30)    {
      //Caso realize 30 mediçoes
      //e não consiga conexão
      //reseta por Hardware o microcontrolador
      digitalWrite(13, LOW); //zera pino 13 para reset
    }
    yield();
    ESP.wdtFeed();
    return;
  }
}


void conectaWiFi()
{
  //Comandos para configuração de WiFi
  //Sem necessidade de inserir parâmetros
  //No código
  String ssid_eagua;
  WiFiManager wifiManager;

  //Configura IP fixo para o portal de reconexão
  wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 0, 1),
                                  IPAddress(192, 168, 0, 1),
                                  IPAddress(255, 255, 255, 0));
  wifiManager.setConfigPortalTimeout(180);
  //Define SSID para rede de reconfiguração
  wifiManager.autoConnect("Recupera Y-Agua");
}