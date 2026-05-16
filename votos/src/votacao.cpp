#include "votacao.h"

void computa_votos(Candidato candidatos[], int *quantidade, int voto, int *nulos)
{
    //pessoa votou nulo
    if (voto == 00)
    {
        (*nulos)++;
        return;
    }

    for (int i = 0; i < *quantidade; i++)
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