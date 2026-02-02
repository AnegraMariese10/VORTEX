#ifndef __DATA_STRUCT
#define __DATA_STRUCT

#include <iostream>
#include <string>
using namespace std;

struct dataStruct
{
	// dados básicos
	int carId = -1;
	double time = -1;
	int escalonamento=-1;
	unsigned long long armazenamentoUtilizado = -1;
	unsigned long long armazenamentoMaximo = -1;
	int totalVideosGerados = -1;
	int totalVideosEnviados = -1;
	
	// dados carro
	double cordX = -1;
	double cordY = -1;
	double vel = -1;
	double ri = -1;
	
	// dados geração de video
	double tamanhoVideo = -1;
	int prioridadeVideo = -1;
	double deadlineVideo = -1;
	double tC = -1;
	double tP = -1;
	double ProptC = -1;
	double tFila = -1;
	// dados dos objetos detectados
	int numObj=-1;
	double obSize = -1;
    double SizeObj = -1;
    int idObj = -1;
    double tempDetect=-1;
    int ObDetect=-1;
    double coordObX=-1;
    double coordObY=-1;

	// dados conexão servidor
	string serverConectado = " ";
	string cloud;
	double totalDeDados = -1;
	double distAntena = -1;
	
	// dados resposta servidor
	int resposta = -1;
	double maxTransmissaoDados = -1;
	double tempoDeConectividade = -1;
	
	// dados envio pacote
	int videoId = -1;
	int pacotesRestantes = -1;
	double tamanhoPacote = -1;
	string destino = " ";
	double timeConexao = -1;
	double timeFinal = -1;
};

#endif
