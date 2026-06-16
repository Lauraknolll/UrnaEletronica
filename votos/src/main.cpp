#include <Arduino.h>
#include <SD.h>
#include "candidato.h"
#include "gerencia_cartao.h"
#include "gerencia_display.h"
#include "teclado.h"
#include "votacao.h"
#include "Preferences.h"

#define MAX_CANDIDATOS 20
#define BUZZERPIN 15
#define ESP_RASP_TX 17 // pino para o envio de dado para rasp
#define ESP_RASP_RX 16 // pino para o recebimento de dado da rasp

// put function declarations here:
Candidato candidatos[MAX_CANDIDATOS];
int quantidade_candidatos = 0;
Votacao *votacao;
Preferences nvs;
int totalVotos;
int NovaEleicao;
HardwareSerial SerialESP(2);
const int PINO_ADMIN = 13;
const String SENHA_ADMIN = "9*52*0";

// funções
void faz_barulho();
void mandar_sinal_pra_rasp(String msg);
void gravar_resultados_NVS();
void grava_resultado_final();
void recupera_dados_NVS();
void zerar_eleicao();
void modoAcesso();
void modoAdministrador();

void setup()
{
  // put your setup code here, to run once:
  pinMode(ESP_RASP_RX, INPUT);
  pinMode(ESP_RASP_TX, OUTPUT);

  digitalWrite(ESP_RASP_TX, LOW);
  //========================================
  // define a velocidade de comunicação
  Serial.begin(115200);

  // define comunicação serial com a RASP
  // SerialESP.begin(115200, SERIAL_8N1,
  //   16,   // RX
  //   17    // TX
  // );
  // SerialESP.setTimeout(100000); // 100 segundos
  //==========================================
  Serial.println("ESP32 pronto");

  // inicializa buzzer
  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN, LOW);

  // inicializa botão de admin
  pinMode(PINO_ADMIN, INPUT_PULLUP);

  inicializar_cartao();
  ler_arquivo(SD, "/candidatos.txt", &quantidade_candidatos, candidatos);

  votacao = new Votacao(candidatos, quantidade_candidatos);

  // printo os candidatos cadastrados no terminal só pros testes
  for (int i = 0; i < quantidade_candidatos; i++)
  {
    Serial.printf("%s : %d\n", candidatos[i].nome, candidatos[i].numero);
  }

  Serial.printf("Iniciando votação! \n");

  inicializa_display();

  nvs.begin("urna", false); // namespace urna

  // sempre recupera a anterior
  NovaEleicao = 0;

  if (NovaEleicao)
  {
    int totalVotos = 0;
    zerar_eleicao();
  }
  else
  {
    recupera_dados_NVS();
    // recupero quantas pessoas já votaram
    for (int i = 0; i < quantidade_candidatos; i++)
    {
      totalVotos += votacao->getVotos(&candidatos[i]);
    }

    totalVotos += votacao->getNulos();
  }

  pinMode(BUZZERPIN, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:

  // se está acionando o botão para entrar no modo admin
  if (digitalRead(PINO_ADMIN) == LOW)
  {
    unsigned long inicio = millis();

    while (digitalRead(PINO_ADMIN) == LOW)
    {
      if (millis() - inicio > 5000) // conta se segurou por 5 segundos
      {
        modoAcesso();
        break;
      }
    }
  }

  String msg = "";
  Serial.print(Serial.available());
  // if(SerialESP.available()) // se a comunicação tá estabelecida com a rasp
  // {
  //   msg = SerialESP.readStringUntil('\n');
  //   Serial.println(msg);
  // if(msg == "Pode votar")

  // while (1){
  if ((digitalRead(ESP_RASP_RX) == HIGH))
  {
    escreve_info("Digite seu voto");
    while (true)
    {
      int tecla = ler_teclas();
      int voto = -1;

      // apertou tecla válida
      if (tecla != 'F')
      {
        voto = votacao->identifica_voto(tecla);

        // confirmou o voto
        if (voto != -1)
        {
          votacao->computa_votos(voto);
          gravar_resultados_NVS();
          totalVotos++;
          faz_barulho();
          // manda sinal que confirmou o voto
          digitalWrite(ESP_RASP_TX, HIGH);
          delay(50);
          digitalWrite(ESP_RASP_TX, LOW);
          // mandar_sinal_pra_rasp("Votação realizada");
          break;
        }
      }
    }
  }
}
//}
//}

// put function definitions here:
// função que aciona o buzzer pra dizer que confirmou o voto
void faz_barulho()
{
  digitalWrite(BUZZERPIN, HIGH);
  delay(90);
  digitalWrite(BUZZERPIN, LOW);
  delay(40);

  digitalWrite(BUZZERPIN, HIGH);
  delay(90);
  digitalWrite(BUZZERPIN, LOW);
  delay(40);

  digitalWrite(BUZZERPIN, HIGH);
  delay(180);
  digitalWrite(BUZZERPIN, LOW);
}

// função que manda um sinal para rasp quando e retorna true se o sinal foi enviado
void mandar_sinal_pra_rasp(String msg)
{
  SerialESP.println(msg); // Manda um sinal confirmando o voto para a rasp
}

// grava resultados na mem flash da esp
void gravar_resultados_NVS()
{
  for (int i = 0; i < quantidade_candidatos; i++)
  {
    // caixinha do candidato
    String key = "cand" + String(votacao->getNumero(&candidatos[i]));
    // grava o total de votos do candidato
    nvs.putInt(key.c_str(), votacao->getVotos(&candidatos[i]));

    // pra saber se está funcionando
    Serial.printf("%s = %d\n", key.c_str(), nvs.getInt(key.c_str(), 0));
  }
  // grava nulos no fim
  nvs.putInt("nulos", votacao->getNulos());
  // pra saber se está funcionando
  Serial.printf("nulos = %d\n", nvs.getInt("nulos", 0));
}

// grava resultados no SD
void grava_resultado_final()
{
  limpar_arquivo(SD, "/resultados.txt");

  for (int i = 0; i < quantidade_candidatos; i++)
  {
    String linha = String(votacao->getNome(&candidatos[i])) + " : " + String(votacao->getVotos(&candidatos[i]));
    escrever_arquivo(SD, "/resultados.txt", linha.c_str());
  }

  String linha_nulos = "Nulos : " + String(votacao->getNulos());
  escrever_arquivo(SD, "/resultados.txt", linha_nulos.c_str());

  String totalvotos = "Total de votos : " + String(totalVotos);
  escrever_arquivo(SD, "/resultados.txt", totalvotos.c_str());
}

// recupera dados da mem flash da esp
void recupera_dados_NVS()
{
  for (int i = 0; i < quantidade_candidatos; i++)
  {
    String key = "cand" + String(votacao->getNumero(&candidatos[i]));
    // procura se tem alguma coisa na caixinha
    int votos = nvs.getInt(key.c_str(), 0);

    votacao->setVotos(&candidatos[i], votos);
  }

  int nulos = nvs.getInt("nulos", 0);
  votacao->setNulos(nulos);
}

// apaga mem flash da esp
void zerar_eleicao()
{
  nvs.clear();
  totalVotos = 0;
}

void modoAcesso()
{
  escreve_info("Digite a senha : ");
  String senha = "";
  String mascara = "";
  int tentativas = 0;

  while (true)
  {
    char tecla = ler_teclas();
    if (tecla != 'F')
    {
      senha += tecla;
      mascara += '*';
      escreve_senha(mascara);
      if (tecla == '#')
      {
        senha.remove(senha.length() - 1);
        if (senha == SENHA_ADMIN)
        {
          modoAdministrador();
          break;
        }
        else
        {
          escreve_info("Senha incorreta!!");
          delay(1500);
          escreve_info("Digite a senha : ");
          tentativas++;
          senha = "";
          mascara = "";
        }
      }
    }

    if (tentativas >= 3)
    {
      escreve_info("ACESSO NEGADO!!");
      delay(1000);
      escreve_info("Digite seu voto.");
      break;
    }
  }
}

void modoAdministrador()
{
  escreve_modoAdmin();

  while (true)
  {
    int tecla = ler_teclas();

    switch (tecla)
    {
    case '1':
    {
      zerar_eleicao();
      recupera_dados_NVS();
      escreve_info("Iniciando nova \n      eleicao.");
      delay(2000);
      escreve_info("Digite seu voto");
      return;
    }
    case '2':
    {
      escreve_info("Digite seu voto");
      return;
    }
    case '3':
    {
      grava_resultado_final();
      mandar_sinal_pra_rasp("Votação encerrada");
      escreve_info("Votos gravados com \n      sucesso!");
      delay(1000);
      modoAdministrador();
    }
    case '4':
    {
      escreve_info("Digite seu voto");
      return;
    }
    }
  }
}