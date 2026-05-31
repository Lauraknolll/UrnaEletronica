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
#define ESP_RASP_TX 17 //pino para o envio de dado para rasp
#define ESP_RASP_RX 16 //pino para o recebimento de dado da rasp

// put function declarations here:
Candidato candidatos[MAX_CANDIDATOS];
int quantidade_candidatos = 0;
Votacao* votacao;
Preferences nvs;
int totalVotos;
int NovaEleicao = 1;

//funções
void faz_barulho();
bool mandar_sinal_pra_rasp();
String receber_sinal_da_rasp();
void gravar_resultados_NVS();
void grava_resultado_final();
void recupera_dados_NVS();
void zerar_eleicao();

void setup() {
  // put your setup code here, to run once:

  //define a velocidade de comunicação 
  Serial.begin(115200);

  //inicializa buzzer
  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN, LOW);

  inicializar_cartao();
  ler_arquivo(SD, "/candidatos.txt", &quantidade_candidatos, candidatos);

  votacao = new Votacao(candidatos, quantidade_candidatos);

  //printo os candidatos cadastrados no terminal só pros testes
  for(int i = 0; i < quantidade_candidatos; i++)
  {
    Serial.printf("%s : %d\n",candidatos[i].nome, candidatos[i].numero);
  }

  Serial.printf("Iniciando votação! \n");

  inicializa_display();

  nvs.begin("urna", false); //namespace urna

  if(NovaEleicao)
  {
    int totalVotos = 0;
    zerar_eleicao();
  }
  else
  {
    recupera_dados_NVS();
    // recupero quantas pessoas já votaram
    for(int i = 0; i < quantidade_candidatos; i++)
    {
      totalVotos += votacao->getVotos(&candidatos[i]);
    }

    totalVotos += votacao->getNulos();
  }

  pinMode(BUZZERPIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //fica lendo o que está recebendo do teclado
  int tecla = ler_teclas();
  int voto = -1;

  String sinal;
  sinal = receber_sinal_da_rasp();
  //quando receber o sinal escreve Digite seu voto porque tá liberado o voto?

  //apertou tecla válida
  if (tecla != 'F')
  {
    voto = votacao->identifica_voto(tecla);
  
    //confirmou o voto
    if(voto != -1)
    {
      votacao->computa_votos(voto);
      gravar_resultados_NVS();
      totalVotos++;
      //fingindo que tinha que votar 10 pessoas só, mas tem que receber um sinal que acabou
      if(totalVotos == 10)
      {
        grava_resultado_final();
      }
      faz_barulho();
      //manda sinal que confirmou o voto
      mandar_sinal_pra_rasp();
    }
  }
}

// put function definitions here:
//função que aciona o buzzer pra dizer que confirmou o voto
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

//função que manda um sinal para rasp quando e retorna true se o sinal foi enviado
bool mandar_sinal_pra_rasp()
{
  Serial2.println("1"); //Manda um sinal confirmando o voto para a rasp
  return true;          //Retorna true para indicar que o sinal foi enviado.
}

//função que recebe um sinal da rasp e retorna o sinal recebido para liberar o voto, ou vazio caso não tenha recebido nada
String receber_sinal_da_rasp()
{
  if(Serial2.available())                         //verifica a disponibilidade do sinal
  {
    String sinal = Serial2.readStringUntil('\n'); //lê o sinal até a quebra de linha
    return sinal;                                 //retorna o sinal recebido(estamos tratando como string)
  }
  return "";                                      //retorna vazio caso não esteja recebendo nada.
}

//grava resultados na mem flash da esp
void gravar_resultados_NVS()
{
  for(int i = 0; i < quantidade_candidatos; i++)
  {
    //caixinha do candidato
    String key = "cand" + String(votacao->getNumero(&candidatos[i]));
    //grava o total de votos do candidato
    nvs.putInt(key.c_str(), votacao->getVotos(&candidatos[i]));

    //pra saber se está funcionando
    Serial.printf("%s = %d\n",key.c_str(), nvs.getInt(key.c_str(), 0));
  }
  //grava nulos no fim
  nvs.putInt("nulos", votacao->getNulos());
  //pra saber se está funcionando
  Serial.printf("nulos = %d\n", nvs.getInt("nulos", 0));
}

//grava resultados no SD
void grava_resultado_final()
{
  for(int i = 0; i < quantidade_candidatos; i++)
  {
    String linha = String(votacao->getNome(&candidatos[i])) + " : " + String(votacao->getVotos(&candidatos[i]));
    escrever_arquivo(SD, "/resultados.txt", linha.c_str());
  }

  String linha_nulos = "Nulos : " + String(votacao->getNulos());
  escrever_arquivo(SD, "/resultados.txt", linha_nulos.c_str());
}

//recupera dados da mem flash da esp
void recupera_dados_NVS()
{
  for(int i = 0; i < quantidade_candidatos; i++)
  {
    String key = "cand" + String(votacao->getNumero(&candidatos[i]));
    // procura se tem alguma coisa na caixinha
    int votos = nvs.getInt(key.c_str(), 0);
    
    votacao->setVotos(&candidatos[i], votos);
  }

  int nulos = nvs.getInt("nulos", 0);
  votacao->setNulos(nulos);
}

//apaga mem flash da esp
void zerar_eleicao()
{
    nvs.clear();
}