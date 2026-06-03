#include "votacao.h"
#include "gerencia_display.h"

Votacao::Votacao(Candidato vetor_candidatos[], int quantidade_candidatos) {
    candidatos = vetor_candidatos;
    quantidade = quantidade_candidatos;
    nulos = 0;
}

void Votacao::computa_votos(int voto)
{
    //pessoa votou nulo
    if (voto == 00)
    {
        (nulos)++;
        return;
    }

    for (int i = 0; i < quantidade; i++)
    {
        //pessoa votou no candidato i
        if(voto == candidatos[i].numero)
        {
            candidatos[i].quantidade_votos++;
        }
    }
    //se votou em um número que não é de nenhum candidato só não acontece nada
    return;
}

int Votacao::identifica_voto(char tecla_pressionada)
{
    static String numero = "";

    switch (tecla_pressionada)
    {
        //quer corrigir o voto
        case '*':
        {
            //se já apertou algum número
            if(numero.length() > 0)
            {
                //apago tudo que tinha escrito
                numero = "";
                escreve_info("       ");
            }
            break;
        }
        //confirmou o voto
        case '#':
        {
            int voto;
            voto = numero.toInt();
            numero = "";
            escreve_info("Voto Confirmado!");
            return voto;
        }
        //votando o primeiro ou segundo número
        default:
        {
            if (tecla_pressionada >= '0' && tecla_pressionada <= '9')
            {
                int voto;
                numero += tecla_pressionada;
                //mostro o que digitou independente do que é 
                escreve_digitado(numero);
                voto = numero.toInt();
                Candidato* cand;
                //procuro se o número digitado é um dos candidatos
                cand = procura_candidato(voto);

                //é um candidato válido
                if(cand != nullptr)
                {   
                    //mostro as informações do candidato na tela (número, nome e foto)
                    String foto = "/" + String(getNumero(cand)) + ".jpg";
                    escreve_foto(foto);
                    escreve_candidato(getNumero(cand), getNome(cand));
                    Serial.printf("Candidato : %s\n", getNome(cand));
                }
            break;
            }
         }
    }
    return -1;
}

Candidato* Votacao::procura_candidato(int num)
{
    for (int i = 0; i < quantidade; i++)
    {
        //numero corresponde a algum candidato?
        if(num == candidatos[i].numero)
        {
            return &candidatos[i];
        }
    }
    return nullptr;
}

void Votacao::setVotos(Candidato* candidato, int votos)
{
    candidato->quantidade_votos = votos;
}

void Votacao::setNulos(int Nulos)
{
    nulos = Nulos;
}

String Votacao::getNome(Candidato* candidato)
{
    return candidato->nome;
}

int Votacao::getNumero(Candidato* candidato)
{
    return candidato->numero;
}

int Votacao::getVotos(Candidato* candidato)
{
    return candidato->quantidade_votos;
}

int Votacao::getNulos()
{
    return nulos;
}
