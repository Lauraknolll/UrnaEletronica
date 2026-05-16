#include <Arduino.h>
#include <SD.h>
#include "candidato.h"
#include "gerencia_cartao.h"
#include "teclado.h"
#include "votacao.h"

#define MAX_CANDIDATOS 20
#define BUZZERPIN 33 //pra depois que mudar de teclado

// put function declarations here:
//máximo de 20 candidatos
Candidato candidatos[MAX_CANDIDATOS];
int quantidade_candidatos = 0;
int nulos = 0;
void faz_barulho();

void setup() {
  // put your setup code here, to run once:

  //define a velocidade de comunicação 
  Serial.begin(115200);

  inicializar_cartao();
  //leio os candidatos no arquivo /candidatos.txt do cartão SD e guardo no vetor dos candidatos
  //ler_arquivo(SD, "/candidatos.txt", &quantidade_candidatos, candidatos);
  //Serial.printf("Leitura concluída com sucesso. \n");
  //agora candidatos já contém todos os candidatos

  //candidatos pro teste do teclado
  candidatos[0] = {10,"fulaninha", 20};
  candidatos[1] = {20, "ciclaninho",5};
  nulos = 15;
  quantidade_candidatos = 2;

  SD.remove("/resultados.txt");

  Serial.printf("Resultados: \n");
  //chama função
    for(int i = 0; i < quantidade_candidatos; i++)
    {
      Serial.printf("%s : %d\n",candidatos[i].nome, candidatos[i].quantidade_votos);

      String linha = String(candidatos[i].nome) + " : " + String(candidatos[i].quantidade_votos);
      escrever_arquivo(SD, "/resultados.txt", linha.c_str());
    }

    Serial.printf("Nulos : %d\n", nulos);
    String linha_nulos = "Nulos : " + String(nulos);
    escrever_arquivo(SD, "/resultados.txt", linha_nulos.c_str());

  //Serial.printf("Iniciando votação! \n");

  //pinMode(BUZZERPIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*int voto = ler_teclas();

  //se um voto foi confimado
  if(voto != -1)
  {
    computa_votos(candidatos, &quantidade_candidatos, voto, &nulos);
    faz_barulho();
    //manda sinal de confirmação pra rasp
    //usando o 99 pra gravar os resultados
    //remove o que tinha antes pra sobrescrever, será que fica certo? 
    //SD.remove("/resultados.txt");
    if (voto == 99) 
    {
      Serial.printf("Resultados: \n");
      //chama função
      for(int i = 0; i < quantidade_candidatos; i++)
      {
        Serial.printf("%s : %d\n",candidatos[i].nome, candidatos[i].quantidade_votos);

        String linha = String(candidatos[i].nome) + " : " + String(candidatos[i].quantidade_votos);
        escrever_arquivo(SD, "/resultados.txt", linha.c_str());
      }

      Serial.printf("Nulos : %d\n", nulos);
      String linha_nulos = "Nulos : " + String(nulos);
      escrever_arquivo(SD, "/resultados.txt", linha_nulos.c_str());
    }
  }*/
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