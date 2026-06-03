#ifndef votacao_h
#define votacao_h

#include "candidato.h"
#define MAX_CANDIDATOS 20

class Votacao {
    private:
        Candidato* candidatos;
        int quantidade;
        int nulos;
    
    public:
    Votacao(Candidato vetor_candidatos[], int quantidade_candidatos);

    void computa_votos(int voto);
    int identifica_voto(char tecla_pressionada);
    Candidato* procura_candidato(int num);

    void setVotos(Candidato* candidato, int votos);
    void setNulos(int Nulos);

    int getNulos();
    String getNome(Candidato* candidato);
    int getNumero(Candidato* candidato);
    int getVotos(Candidato* candidato);

};

#endif 