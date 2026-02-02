#include "SateliteUdpVideoStreamClient2.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"


#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/transportlayer/common/L4PortTag_m.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/applications/udpapp/UdpVideoStreamServer.h"


#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/Packet.h"
//#include "inet/common/signals/INetSignals.h"      // Para sinais gerais do INET
#include "inet/mobility/contract/IMobility.h"


#include "./packet-mariese-cenario/SateliteMessage_m.h"
#include "./packet-mariese-cenario/ServerConnMessage_m.h"

#include "./video_model/video_class.cc"
#include "./data/data_struct.cc"
#include "./objeto_model/objeto_class.cc"



#include "veins_inet/VeinsInetMobility.h"


#include <map>

#include "/home/anegramariese/src/Simu5G-1.1.0/src/common/LteControlInfo_m.h" // Possivelmente para sinais de controle LTE/NR
#include "/home/anegramariese/src/Simu5G-1.1.0/src/common/LteCommon.h" // Possivelmente para sinais de controle LTE/NR
#include "/home/anegramariese/src/Simu5G-1.1.0/src/common/binder/Binder.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/LtePhyUe.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/LtePhyUeD2D.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/NRPhyUe.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/NRSchedulerGnbUl.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/LteSchedulerEnbUl.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/LteSchedulerEnb.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/LteScheduler.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduling_modules/LtePf.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/LtePhyEnb.h"        // Se estiver usando LTE e precisar de funcoes especificas
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/NRPhyUe.h"         // Se estiver usando 5G NR
#include "/home/anegramariese/src/Simu5G-1.1.0/src/common/LteControlInfo_m.h" // Pode ser util para sinais de controle LTE/NR



#include <iostream>
#include <random>
#include <cmath>
#include <list>
#include <xgboost/c_api.h> // API C++ do XGBoost
#include <unordered_set>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#define SERVER1 "server1"
#define SERVER2 "server2"
#define GNB "gNB"
#define ENB "eNB"
#define DISTANCIA_GNB 800
#define DISTANCIA_ENB 1000

//#define  MODEL_FILE "/home/anegramariese/src/Simu5G-1.1.0/simulations/NR/cars/modelo_final_regressao.json"
//#define  MODEL_FILE "/home/anegramariese/src/Simu5G-1.1.0/simulations/NR/cars/modelo_final_regressaoL.json"
//#define  MODEL_FILE "/home/anegramariese/src/Simu5G-1.1.0/simulations/NR/cars/modelo_final_regressaolog.json"
#define  MODEL_FILE "/home/anegramariese/src/Simu5G-1.1.0/simulations/NR/cars/modelo_final_classificacao.json"
//#define  MODEL_FILE "/home/anegramariese/src/Simu5G-1.1.0/simulations/NR/cars/modelo_final1_regressao.json"
//#define  MODEL_FILE "/home/anegramariese/src/Simu5G-1.1.0/simulations/NR/cars/modelo_final2_regressao.json"


#define epsilonScore 0.000001

#define escalonamento 2 //0=VORTEX; 1-EDF; 2-EDF2;  3-SDF; 4-SDF2


using namespace std;
using namespace inet;

Define_Module(SateliteUdpVideoStreamClient2);

simsignal_t SateliteUdpVideoStreamClient2::reqStreamBytesSignal = registerSignal("reqStreamBytes");

SateliteUdpVideoStreamClient2::~SateliteUdpVideoStreamClient2() {

    for (auto &elem : streams)
        cancelAndDelete(elem.second.timer);

//    if(outputFile.is_open())
//        outputFile.close();
}

void SateliteUdpVideoStreamClient2::escreveLogFinal(dataStruct *ds) {
    cout << "atualizando log final..." << endl;

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));
        inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

        veins::VeinsInetMobility *mobilityModule = check_and_cast<
                veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

        ds->carId = getParentModule()->getIndex();
        ds->time = simTime().dbl();
        ds->armazenamentoUtilizado = armazenamentoUtilizado;
        ds->armazenamentoMaximo = maxArmazenamento;
        ds->totalVideosGerados = totalVideosGerados;
        ds->totalVideosEnviados = totalVideosEnviados;
        ds->tempoDeConectividade = timeInicioEnvio - tempoInicial;
        if (!log_final.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_final.open(file_final.str());
        }

    //carId;taxaDeTransmissao;time;cordX;cordY;vel;distGNB;distENB;videoId;
    //tamanhoVideo;prioridadeVideo;timeConexao;serverConectado;distAntena;maxTransmissaoDeDados;tempoDeConectividade;timeFinal
        if (log_final.is_open()) {
            log_final << ""
                    << ds->carId << ";"
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->idObj << ";"
                    << ds->tC << ";"
                    << ds->tP<< ";"
                    << ds->ProptC<< ";"
                    << ds->serverConectado << ";"
                    << ds->timeFinal << ";\n";

    //        log_final.close();

    //    outputFile.close();

    //    cout << "" << ds->tamanhoVideo << ";" << ds->prioridadeVideo << ";" << ds->deadlineVideo << endl;
    //        outputFile << "" << ds->tamanhoVideo << ";" << ds->prioridadeVideo << ";" << ds->deadlineVideo << endl;
    //        outputFile.close();
        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamClient2::escreveLogSelecao(dataStruct *ds) {
    cout << "atualizando log Seleção..." << endl;

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));
        inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

        veins::VeinsInetMobility *mobilityModule = check_and_cast<
                veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

        ds->carId = getParentModule()->getIndex();
        ds->time = simTime().dbl();
        ds->armazenamentoUtilizado = armazenamentoUtilizado;
        ds->armazenamentoMaximo = maxArmazenamento;
        ds->totalVideosGerados = totalVideosGerados;
        ds->totalVideosEnviados = totalVideosEnviados;
        ds->tempoDeConectividade = timeInicioEnvio - tempoInicial;


        if (!log_selecao.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_selecao.open(file_selecao.str());
        }

    //carId;taxaDeTransmissao;time;cordX;cordY;vel;distGNB;distENB;videoId;
    //tamanhoVideo;prioridadeVideo;timeConexao;serverConectado;distAntena;maxTransmissaoDeDados;tempoDeConectividade;timeFinal
        if (log_selecao.is_open()) {
            log_selecao << ""
                    << ds->carId << ";"
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->idObj << ";"
                    << ds->tC << ";"
                    << ds->tP<< ";"
                    << ds->ProptC<< ";"
                    << ds->serverConectado << ";"
                    << ds->timeFinal << ";\n";

    //        log_final.close();

    //    outputFile.close();

    //    cout << "" << ds->tamanhoVideo << ";" << ds->prioridadeVideo << ";" << ds->deadlineVideo << endl;
    //        outputFile << "" << ds->tamanhoVideo << ";" << ds->prioridadeVideo << ";" << ds->deadlineVideo << endl;
    //        outputFile.close();
        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamClient2::escreveLogConexao(dataStruct *ds) {
    //cout << "atualizando log conexão..." << endl;

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));
        inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

        veins::VeinsInetMobility *mobilityModule = check_and_cast<
                veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

        ds->carId = getParentModule()->getIndex();
        ds->time = simTime().dbl();
        ds->armazenamentoUtilizado = armazenamentoUtilizado;
        ds->armazenamentoMaximo = maxArmazenamento;
        ds->totalVideosGerados = totalVideosGerados;
        ds->totalVideosEnviados = totalVideosEnviados;
        ds->cordX = positionCarro.getX();
        ds->cordY = positionCarro.getY();
        ds->vel = hypot(mobilityModule->getCurrentVelocity().getX(),
                mobilityModule->getCurrentVelocity().getY());
        ds->ri = ri;
    //    ds->tempoDeConectividade = simTime().dbl() - tempoInicial;

        if (!log_conexao.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_conexao.open(file_conexao.str());
        }

        if (log_conexao.is_open()) {
    //carId;time;videoId;serverConectado;totalDeDados;distAntena;maxTransmissaoDados;tempoDeConectividade;\n;

            log_conexao << ""
                    << ds->carId << ";"
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->videoId << ";"
                    << ds->idObj << ";"
                    << ds->serverConectado << ";"
                    << ds->totalDeDados / 1024 / 1024 << ";"
                    << ds->distAntena << ";"
                    << double(ds->maxTransmissaoDados / 1024 / 1024) << ";"
                    << ds->tempoDeConectividade << ";\n";

    //        log_conexao.close();
        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamClient2::escreveLogCriacao(dataStruct *ds) {
    //cout << "atualizando log criação..." << endl;

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));
        inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

        veins::VeinsInetMobility *mobilityModule = check_and_cast<
                veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

        ds->carId = getParentModule()->getIndex();
        ds->time = simTime().dbl();
        ds->armazenamentoUtilizado = armazenamentoUtilizado;
        ds->armazenamentoMaximo = maxArmazenamento;
        ds->totalVideosGerados = totalVideosGerados;
        ds->totalVideosEnviados = totalVideosEnviados;
        ds->cordX = positionCarro.getX();
        ds->cordY = positionCarro.getY();
        ds->vel = hypot(mobilityModule->getCurrentVelocity().getX(),
                mobilityModule->getCurrentVelocity().getY());
        ds->ri = ri;
    //    ds->tempoDeConectividade = simTime().dbl() - tempoInicial;

    //    dist_server1
    //    distanciaAntena(GNB, SERVER1, tos, distanciaMaxima, &dist_server1, vel)


        if (!log_criacao.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_criacao.open(file_criacao.str());
        }

        if (log_criacao.is_open()) {

    //carId;
    //taxaDeTransmissao;time;cordX;cordY;vel;distGNB;distENB;videoId;tamanhoVideo;prioridadeVideo;deadlineVideo;\n";


            log_criacao << ""
                    << ds->carId << ";"
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->ri << ";"
                    << ds->cordX << ";"
                    << ds->cordY << ";"
                    << ds->vel << ";"
                    << distGNB << ";"
                    << distENB << ";"
                    << ds->videoId << ";"
                    << ds->tempDetect << ";"
                    << ds->tamanhoVideo << ";"
                    << ds->prioridadeVideo << ";"
                    << ds->deadlineVideo<< ";"
                    << ds->numObj << ";"
                    << ds->idObj << ";"
                    << ds->ObDetect << ";"
                    << ds->obSize<< ";"
                    << ds->coordObX << ";"
                    << ds->coordObY << ";"
    //                << ds->distAntena << ";" << ds->serverConectado
                    <<";\n";

    //        log_criacao.close();

        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamClient2::escreveLogObjEnviados(dataStruct *ds) {
    //cout << "atualizando log criação..." << endl;

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));
        inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

        veins::VeinsInetMobility *mobilityModule = check_and_cast<
                veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

        ds->carId = getParentModule()->getIndex();
        ds->time = simTime().dbl();



        if (!log_ObjEnviados.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_ObjEnviados.open(file_ObjEnviados.str());
        }

        if (log_ObjEnviados.is_open()) {

    //carId;
    //taxaDeTransmissao;time;cordX;cordY;vel;distGNB;distENB;videoId;tamanhoVideo;prioridadeVideo;deadlineVideo;\n";


            log_ObjEnviados << ""
                    << ds->carId << ";"
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->serverConectado << ";"
                    << ds->idObj << ";"
                    << ds->tempDetect << ";"
                    << ds->obSize<< ";"
                    << ds->prioridadeVideo << ";"
                    << ds->deadlineVideo<< ";"
                    << ds->ObDetect << ";"
                    << ds->coordObX << ";"
                    << ds->coordObY << ";\n";

    //        log_criacao.close();

        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamClient2::escreveLogCloud(dataStruct *ds) {
    //cout << "atualizando log criação..." << endl;

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));
        inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

        veins::VeinsInetMobility *mobilityModule = check_and_cast<
                veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

        ds->carId = getParentModule()->getIndex();
        ds->time = simTime().dbl();


        if (!log_cloud.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_cloud.open(file_cloud.str());
        }

        if (log_cloud.is_open()) {

    //carId;
    //taxaDeTransmissao;time;cordX;cordY;vel;distGNB;distENB;videoId;tamanhoVideo;prioridadeVideo;deadlineVideo;\n";


            log_cloud << ""
                    << ds->carId << ";"
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->idObj << ";"
                    << ds->tC << ";"
                    << ds->tP<< ";"
                    << ds->ProptC<< ";"
                    << ds->serverConectado << ";"
                    << ds->timeFinal << ";\n";




    //        log_criacao.close();

        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}



int SateliteUdpVideoStreamClient2::quantidadeTempoDeVida(int prioridadeArquivo) {
    int ttl = -1;

    if (prioridadeArquivo == 0) {
        // alta prioridade
        ttl = uniform(0, 5);
    } else if (prioridadeArquivo == 1) {
        // média prioridade
        ttl = uniform(6, 35);
    } else {
        // baixa prioridade
        ttl = uniform(36, 90);
    }
    return ttl;
}


//////////////Modificações Professor António///////////
list<Objeto> SateliteUdpVideoStreamClient2::identfyObjects(int numObj, double videoSize) {

    list<Objeto> objetos;

    IMobility *mobilityCarro = check_and_cast<IMobility*>(
            getModuleByPath("^.mobility"));
    inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

    veins::VeinsInetMobility *mobilityModule = check_and_cast<
            veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));
    cEnvir &envir = *omnetpp::getEnvir();
    int seedSetNumber = envir.getConfigEx()->getActiveRunNumber();

    double somaSizeob=0;
    double obSize=uniform(minSize, videoSize, seedSetNumber);
    int j=0;
    for (int n = 0; n < numObj; n++) {
        int idObj= n;
        int obDetect = binomial(2, 0.95);
        double coordObjX = uniform(positionCarro.getX()-300, positionCarro.getX()+300);
        double coordObjY = uniform(positionCarro.getY()-300, positionCarro.getY()+300);

                // cria o tamanho de cada objeto detectado, sendo que a soma deles tem que ser igual
                // ou menor que videoSize

        if (n == 0) {
            obSize = obSize;
            double somaSizeob=somaSizeob+obSize;
        } else{
            obSize = uniform(minSize, videoSize-somaSizeob, seedSetNumber);
        }

        Objeto ob = Objeto(idObj, obDetect, coordObjX, coordObjY, obSize);
        //Objeto ob = Objeto(idObj, ObDetect, obSize);
        objetos.push_back(ob);
    }

    //cout<<"Número de Objetos identificados "<<numObj<<endl;

    //for(Objeto ob : objetos) {
       // cout<<"Objeto Detectado "<<ob.idObj<<endl;
    //}

    return objetos;
}

void SateliteUdpVideoStreamClient2::gerarVideo() {
    cout << "\ncar[" << getParentModule()->getIndex() << "] gerarVideo" << endl;

    cout << "armazenamento utilizado: " << armazenamentoUtilizado << endl;
    cout << "armazenamento máximo: " << maxArmazenamento << endl;

    IMobility *mobilityCarro = check_and_cast<IMobility*>(
            getModuleByPath("^.mobility"));
    inet::Coord positionCarro = mobilityCarro->getCurrentPosition();

    veins::VeinsInetMobility *mobilityModule = check_and_cast<
            veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));

    double coordX = positionCarro.getX();
    double coordY = positionCarro.getY();

    double minSize = 0.4 * 1024 * 1024; // 0.4mB
    double maxSize = 3 * 1024 * 1024; // 3mB

    random_device rd;
    mt19937 gen(rd());
    binomial_distribution<int> binomial(2, 0.75);

    cEnvir &envir = *omnetpp::getEnvir();
    int seedSetNumber = envir.getConfigEx()->getActiveRunNumber();

    // cria novos valores para videoSize, prioridade e tempoDeVida
    double videoSize = uniform(minSize, maxSize, seedSetNumber);
//    long videoSize = rand() % (maxSize - minSize + 1) + minSize;
    int prioridade = binomial(gen);
    //double tempoDeVida = quantidadeTempoDeVida(prioridade);
    //double tempoDeVida=

    cout << "novo vídeo gerado " << endl;


    int numObj = uniform(1,3);
    list<Objeto> objetos = identfyObjects(numObj,videoSize);

    int detec=0;
    if (lista_deteccao.size() > 0){
        armazenamentoUtilizado=0;
        for (Video v : lista_deteccao){
            armazenamentoUtilizado=armazenamentoUtilizado + v.getobSize();
        }
    }else{
        armazenamentoUtilizado=0;
    }

    if (lista_InDead.size() > 0){
        armazenamentoUtilizado=0;
        for (Video v : lista_InDead){
            armazenamentoUtilizado=armazenamentoUtilizado + v.getobSize();
        }
    }else{
        armazenamentoUtilizado=0;
    }


    for (Objeto ob : objetos) {
        if ((armazenamentoUtilizado + ob.getobSize()) < maxArmazenamento) {
            // adiciona novo vídeo a lista de vídeos
            double distCar=sqrt(pow((ob.coordObjX-coordX),2) + pow((ob.coordObjY-coordY),2));
            double tempoDeVida=(distCar*5)/300;
            Video nv = Video(totalVideosGerados, simTime().dbl(), prioridade, videoSize, tempoDeVida, coordX, coordY, totalVideosGerados, ob.ObDetect, ob.obSize, ob.coordObjX, ob.coordObjY);

            
            double tP= (double(nv.getobSize() / 1024 / 1024)*8)/30;
            double tC=nv.getDeadline()-(simTime().dbl()+tP);


            addToListInOrder(&lista_deteccao, nv);
            cout << "Objeto na lista de detecçção" << endl;

            dataStruct ds;
            ds.tamanhoVideo = double(nv.tamanho / 1024 / 1024);
            ds.prioridadeVideo = nv.prioridade;
            ds.deadlineVideo = nv.deadline;
            ds.videoId = nv.id;
            ds.numObj = numObj;
            ds.idObj = nv.idObj;
            ds.tempDetect=nv.timeCriacao;
            ds.ObDetect=nv.obDetect;
            ds.obSize = double(nv.obSize / 1024 / 1024);
            ds.coordObX=nv.coordObjX;
            ds.coordObY=nv.coordObjY;

            escreveLogCriacao(&ds);
            totalVideosGerados++;

            armazenamentoUtilizado += nv.getobSize();




            cout << "tamanho (mB): " << double(videoSize / 1024 / 1024) << endl;
            //cout << "tamanho (B): " << videoSize << endl;
            //cout << "prioridade: " << prioridade << endl;
            //cout << "ttl: " << tempoDeVida << endl;
            cout<<"Número de Objetos Detectados: "<<numObj<<endl;
            cout<<"Objeto Detectado: "<<ob.idObj<<endl;
            cout<<"Tamanho do Objeto Detectado (mB): "<< double(ob.obSize / 1024 / 1024)<<endl;
            //cout << "tamanho do Objeto Detectado (B): " << ob.obSize << endl;

            //cout << "sendingState: " << sendingState << endl;

            if (sendingState == 0){
                sendingState = -1;
                scheduleAt(simTime().dbl() + uniform(0.0, 0.2), selfMsg);
            }
        } else {
            cout
            << "limite de armazenamento insuficiente para nova detecção"
            << endl;
            cout<<"Número de Objetos na Lista : "<< lista_deteccao.size() <<endl;
            cout<<"Armazenamento Utilizado : "<< armazenamentoUtilizado <<endl;
            //conexaoServer();
        }
    }

    //atualizaListaMaiorPrioridade();
    // 1. Verificar e cancelar o evento anterior, se estiver agendado
    //if ((this->gerarVideoMsg != nullptr) && (this->gerarVideoMsg->isScheduled())) {
    //  cancelEvent(this->gerarVideoMsg);
    //}
        // **********************************

    cout << "próximo vídeo será gerado em: " << simTime().dbl() + 1.0 << endl;
    scheduleAt(simTime().dbl() + 1.0, gerarVideoMsg);
}



void SateliteUdpVideoStreamClient2::atualizaListaMaiorPrioridade() {
    //    move vídeos de lista_media e lista_baixa para lista_alta caso estejam a 1s
    //    de acabar o deadline sem mudar a prioridade do vídeo
        cout << "ATUALIZANDO LISTA DE MAIOR PRIORIDADE" << endl;
        int idRemover;

        list<int> id_media_remover;
        list<int> id_baixa_remover;

    //    cout << "lista_media: " << endl;
        for (Video v : lista_media){
            if (v.getDeadline() - simTime().dbl() <= 2) {
    //            cout << v.getId() << endl;
                //lista_alta.push_front(v);
                addToListInOrder(&lista_alta, v);

                idRemover = v.getId();
                id_media_remover.push_front(idRemover);
                //cout << idRemover << endl;
                lista_videos.remove_if([idRemover](Video &v) {
                    return v.getId() == idRemover;
                });
            }
        }
    //    cout << "lista_baixa: " << endl;
        for (Video v : lista_baixa){
            //cout << v.getId() << endl;
            if (v.getDeadline() - simTime().dbl() <= 2) {
    //            lista_alta.push_front(v);
                addToListInOrder(&lista_alta, v);

                idRemover = v.getId();
                id_baixa_remover.push_front(idRemover);
                //cout << idRemover << endl;
                lista_videos.remove_if([idRemover](Video &v){
                    return v.getId() == idRemover;
                });
            }
        }
    //


        // TODO fazendo remoção de int no lugar de Video
        for (auto it = lista_media.begin(); it != lista_media.end();) {
            if (find(id_media_remover.begin(), id_media_remover.end(), it->getId()) != id_media_remover.end()) {
                it = lista_media.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = lista_baixa.begin(); it != lista_baixa.end();) {
            if (find(id_baixa_remover.begin(), id_baixa_remover.end(), it->getId()) != id_baixa_remover.end()) {
                it = lista_baixa.erase(it);
            } else {
                ++it;
            }
        }
}

void SateliteUdpVideoStreamClient2::addToListInOrder(list<Video> *list,Video nv) {
    //Observação: Ordenar os dados por tamanho, independente do modelo,
    // descarrega mais dados
    if (list->size() != 0){
        auto it = list->begin();
        while ((it != list->end()) && (nv.getobSize() > it->obSize)){
            ++it;
        }
    //        cout << it->getId() << endl;
        list->insert(it, nv);
    } else {
        list->push_back(nv);
    }

}

double SateliteUdpVideoStreamClient2::percentile(std::vector<double>& v, double p)
{
    if (v.empty()) return 0.0;

    std::vector<double> tmp = v;
    std::sort(tmp.begin(), tmp.end());

    double idx = (p / 100.0) * (tmp.size() - 1);
    int lo = floor(idx);
    int hi = ceil(idx);

    if (lo == hi) return tmp[lo];
    return tmp[lo] * (hi - idx) + tmp[hi] * (idx - lo);
}

void SateliteUdpVideoStreamClient2::updateTAging()
{
    if ((int)waitSamples.size() < minSamples)
        return;

    double P90 = percentile(waitSamples, 90.0);
    T_aging = alphaAging * P90;
}

void SateliteUdpVideoStreamClient2::updateTmaxTx()
{
    if ((int)txSamples.size() < minSamplesTx)
        return;

    double P90 = percentile(txSamples, 90.0);
    T_max = alphaTx * P90;

    // opcional: EV pra debug
    EV << "[TMAX] samples=" << txSamples.size()
       << " P90=" << P90
       << " T_max=" << T_max << " s" << endl;
}

Video SateliteUdpVideoStreamClient2::getVideoMaiorPrioridade(list<Video> lista) {


    Video videoMenorDeadline = Video();
    Video candidatoCritico;
    Video candidatoLatencia;
    Video videoEscolhido = Video();
    Video DeadlinePerdido;


    double menorDeadline = 1000000; // 91 pq o maior deadline possível são 90s
    double menorTD = 10000000000;
    int perdeuDeadline=0;

    double risco;
    double dist;

    double maiorScore = 0; //

    double menorScore = 1000000; //
    double menorTP = 1000000; //
    int txTrans=30;
    double tPEscolhido;

    double z;
    double scoreS;




    //VORTEX Hibrido
        std::vector<double> tCvals;
        struct FeasibleCand {
            double tC;
            double tt;
            Video  v;
        };
        std::vector<FeasibleCand> feasibles;
        feasibles.reserve(lista.size());



        bool criticoEncontrado = false;
        bool NcriticoEncontrado = false;

        bool feasivelEncontrado1 = false;
        bool existeValido = false;
        bool encontrouSDF = false;
        bool encontrouAging = false;
        bool encontrouPrincipal = false;
        bool encontrouFallback = false;
        bool encontrouElite = false;


        Video candidatoSDF;
        Video candidatoAging;
        Video candidatoPrincipal;
        Video candidatoFallback;
        Video candidatoElite;
        Video candidatoCritico;
        Video candidatoCritico1;



        double menorSize = 1e18;
        double maiorEspera = -1.0;

        double melhorTC = 1e18;
        double melhorTT_elite = 1e18;
        double melhorTC_fallback = 1e18;
        double melhorTT_fallback = 1e18;
        double best_tt_critico = 1e18;
        double bestTtCrit    = 1e18;



        // VORTEX sem RN
        for (Video v : lista){
            double tt = (double(v.getobSize() / 1024 / 1024)*8)/30;
            double tC=v.getDeadline()-simTime().dbl()-tt;
            double proptC=tC/tt;


            if (tC <= 0){//((escalonamento == 8) && (tC <= 0)){
                    // inviável para edge antes do deadline
                int id = v.getIdObj();
                if (vencidosSet.insert(id).second) {
                    vencidosIds.push_back(v);
                }
                continue; // não entra na disputa do LLF
            }

            if(proptC <= 1){

               //Utilizando EDF
               if((v.getDeadline() < menorDeadline) || (fabs(v.getDeadline() - menorDeadline) < 1e-9 && (tt < bestTtCrit))){
                   menorDeadline = v.getDeadline();
                   //menorDeadline = tC;
                   bestTtCrit = tt;
                   candidatoCritico = v;
                   criticoEncontrado = true;
               }

            }else{
                  feasivelEncontrado1 = true;
                  feasibles.push_back({tC, tt, v});


                  tCvals.push_back(tC);

                  // fallback SEM filtro (menor tt entre todos os folgados)
                  if (tt < melhorTT_fallback) {
                      melhorTT_fallback = tt;
                      candidatoFallback = v;
                      encontrouFallback = true;
                  }
            }
        }
        if(criticoEncontrado){
            return candidatoCritico;
        }else if(feasivelEncontrado1){

            /*int N = (int)feasibles.size();
            int K = std::max(1, (int)std::ceil(0.10 * N));  // Top 10%

            // particiona para colocar os K maiores tC no começo
            std::nth_element(
                feasibles.begin(), feasibles.begin() + (K - 1), feasibles.end(),
                [](const FeasibleCand& a, const FeasibleCand& b) {
                    return a.tC > b.tC;  // maior tC primeiro
                }
            );

            // agora escolhe menor tt dentro do Top K
            double melhorTT_elite = 1e18;
            bool encontrouElite = false;
            Video candidatoElite;

            for (int i = 0; i < K; i++) {
                if (feasibles[i].tt < melhorTT_elite) {
                    melhorTT_elite = feasibles[i].tt;
                    candidatoElite = feasibles[i].v;
                    encontrouElite = true;
                }
            }
            // decisão final
            if (encontrouElite){
                return candidatoElite;

            }else */if (encontrouFallback){
                double tP=(double(candidatoFallback.getobSize() / 1024 / 1024)*8)/30;
                double tC=candidatoFallback.getDeadline()-(simTime().dbl()+tP);
                double proptC=tC/tP;

                dataStruct ds;
                ds.idObj = candidatoFallback.getIdObj();
                ds.tC =tC;
                ds.tP =tP;
                ds.ProptC=proptC;
                ds.serverConectado = "edge";
                ds.timeFinal = simTime().dbl();

                escreveLogSelecao(&ds);
                return candidatoFallback;

            }else{
                return DeadlinePerdido;
            }

        }else{
            if (!vencidosIds.empty()){
                Video videoOff = vencidosIds.front();

                double tP=(double(videoOff.getobSize() / 1024 / 1024)*8)/30;
                double tC=videoOff.getDeadline()-(simTime().dbl()+tP);
                double proptC=tC/tP;

                dataStruct ds;
                ds.idObj = videoOff.getIdObj();
                ds.tC =tC;
                ds.tP =tP;
                ds.ProptC=proptC;
                ds.serverConectado = "cloud";
                ds.timeFinal = simTime().dbl();

                escreveLogFinal(&ds);
                //escreveLogCloud(&ds);

                int idRemover = videoOff.getIdObj();

                //cout << "Enviando objeto para Cloud " << videoOff.getIdObj() << endl;
                lista.remove_if([idRemover](Video &x){
                    return x.getIdObj() == idRemover;
                });
                vencidosIds.remove_if([idRemover](Video &x){
                    return x.getIdObj() == idRemover;
                });

            }
            return DeadlinePerdido;
        }
}

void SateliteUdpVideoStreamClient2::defineProximoVideoDead(){
    proximoVideo = getVideoMaiorPrioridade(lista_InDead);
}

void SateliteUdpVideoStreamClient2::defineProximoVideo(double capacidadeMax) {

    Video ObjetoEscolhido;


    if(lista_InDead.size() > 0){//VORTEX, EDF-ASD, SDF-ASD
        cout << "Número de Objetos na Lista In " << lista_InDead.size() << endl;

        ObjetoEscolhido = getVideoMaiorPrioridade(lista_InDead);
        //cout << "Id do objeto " << TesteProximoVideo.getIdObj() << endl;

        if(ObjetoEscolhido.getIdObj() >= 0){
            cout << "Um objeto selecionado! Objeto " << ObjetoEscolhido.getIdObj() << endl;
            proximoVideo = ObjetoEscolhido;
        }else{
            sendingState = 0;
            cout << "Não tem Objetos Válidos para Descarrear!" << endl;
            proximoVideo = ObjetoEscolhido;
        }
    }

    if(lista_deteccao.size() > 0){
        cout << "Número de Objetos na Lista de Detecção " << lista_deteccao.size() << endl;

        ObjetoEscolhido = getVideoMaiorPrioridade(lista_deteccao);
        //cout << "Id do objeto " << ObjetoEscolhido.getIdObj() << endl;

        if(ObjetoEscolhido.getIdObj() >= 0){
            cout << "Um objeto selecionado! Objeto " << ObjetoEscolhido.getIdObj() << endl;
            proximoVideo = ObjetoEscolhido;
        }else{
            sendingState = 0;
            cout << "Não tem Objetos Válidos para Descarrear!" << endl;
            proximoVideo = ObjetoEscolhido;
        }

    }
}

void SateliteUdpVideoStreamClient2::initialize(int stage) {
    ApplicationBase::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
            //cout << "\ncar[" << getParentModule()->getIndex()
            //        << "] initialize INITSTAGE_LOCAL" << endl;
            file_criacao << "./data_sim2/N30/CAV/criacao/" << getParentModule()->getIndex()
                    << ".txt";
            file_conexao << "./data_sim2/N30/CAV/conexao/" << getParentModule()->getIndex()
                    << ".txt";
            file_final << "./data_sim2/N30/CAV/final/" << getParentModule()->getIndex()
                    << ".txt";


            file_selecao << "./data_sim2/N30/CAV/selecao/" << getParentModule()->getIndex()
                                << ".txt";


            file_ObjEnviados << "./data_sim2/N30/CAV/ObEnviadosEdge/" << getParentModule()->getIndex()
                    << ".txt";
            file_cloud << "./data_sim2/N30/CAV/ObEnviadosCloud/" << getParentModule()->getIndex()
                    << ".txt";

            localPort = par("localPort");
            selfMsg = new cMessage("UDPVideoStreamStart");
            gerarVideoMsg = new cMessage("gerarNovoVideo");
            enviarProximoVideoMsg = new cMessage("enviarProximoVideo");
            sendInterval = &par("sendInterval");
            destinationPort = par("serverPort");
            naArea = false;

            distENB = 0;
            distGNB = 0;
            carVel = 0;

            T_aging = 1.0;
            alphaAging = 1.5;
            minSamples = 30;
            waitSamples.clear();

            duracaoJanela = 0.1;   // 100 ms
            inicioJanela = simTime();

            startTime = simTime().dbl();
    //        packetLen = &par("packetLen");
            numStreams = 0;
            numPkSent = 0;
            WATCH_MAP(streams);

            maxArmazenamento = 10LL * 1024 * 1024 * 1024;

            cout << "car pktLen: " << pktLen << endl;


            // 1. Defina o caminho para o arquivo do modelo
            // O arquivo deve estar na pasta de execução do OMNeT++
            model_path = MODEL_FILE;

            if (XGBoosterCreate(nullptr, 0, &booster) != 0) {
                throw cRuntimeError("Falha Crítica: Nao foi possivel criar o Booster: %s", XGBGetLastError());
            }

            int success = XGBoosterLoadModel(booster, model_path.c_str());

            if (success != 0) {
                // Limpeza antes de lançar o erro
                XGBoosterFree(booster);
                xgbBooster = nullptr;

                // Este é o throw que você estava vendo: ele avisa que a simulação não pode começar.
                throw cRuntimeError(
                    "Falha Crítica: Nao foi possivel carregar o modelo XGBoost de %s. Verifique o caminho e a integridade do arquivo. Erro: %s",
                    model_path.c_str(),
                    XGBGetLastError()
                );
            }

            EV << "Modelo XGBoost carregado com sucesso de: " << model_path << endl;


            log_criacao.open(file_criacao.str());
            log_conexao.open(file_conexao.str());
            log_final.open(file_final.str());
            log_selecao.open(file_selecao.str());
            log_ObjEnviados.open(file_ObjEnviados.str());
            log_cloud.open(file_cloud.str());

    //        outputFile << "elemento;time;armazenamentoUtilizado;armazenamentoMaximo;totalVideosGerados;totalVideosEnviados;" <<
    //                      "cordX;cordY;vel;taxaTransmissao" <<
    //                      "tamanhoVideo;prioridadeVideo;deadlineVideo;"<<
    //                      "serverConectado;totalDeDados;distAntena;" <<
    //                      "resposta;tempoDeConectividade;maxTransmissaoDados;" <<
    //                      "videoId;pacotesRestantes;tamanhoPacote;destino;" << "\n";
    //                      "videoId;tamanhoPacote;destino" << "\n";

            // idpistaobdetect;
            log_criacao
                  << "carId;time;escalonamento;taxaDeTransmissao;cordX;cordY;vel;distGNB;distENB;videoId;tempDetect;tamanhoVideo;prioridadeVideo;deadlineVideo;numObj;idObj; obDetect; obSize;coordObjX;coordObjY;\n";

            log_conexao
                    << "carId;time;escalonamento;videoId;idObj;serverConectado;totalDeDados;distAntena;maxTransmissaoDados;tempoDeConectividade;\n";

            log_final
                << "carId;time;escalonamento;idObj;tC;tP;ProptC;serverConectado;timeFinal;\n";

            log_selecao
               << "carId;time;escalonamento;idObj;tC;tP;ProptC;serverConectado;timeFinal;\n";



            log_ObjEnviados
                << "carId;time;escalonamento;server;idObj;tempDetect;ObSize;prioridade;"
                   "Obdeadline;ObDetect;coordobjeX;coordobjeY;\n";

            log_cloud
            << "carId;time;escalonamento;idObj;tC;tP;ProptC;serverConectado;timeFinal;\n";


       }
}

void SateliteUdpVideoStreamClient2::finish(){
//    ApplicationBase::finish();
//    cout << "\ncar[" << getParentModule()->getIndex() <<  "] finish" << endl;

    //LteScheduler::finish();

    // Verifica se o handle do modelo foi alocado com sucesso antes de tentar liberá-lo
    if (booster != nullptr) {
        EV << "Liberando o modelo XGBoost da memória." << endl;
        XGBoosterFree(booster);
        booster = nullptr;
    }

    if (log_criacao.is_open())
        log_criacao.close();

    if (log_conexao.is_open())
        log_conexao.close();

    if (log_final.is_open())
        log_final.close();

    if (log_ObjEnviados.is_open())
        log_ObjEnviados.close();

    if (log_cloud.is_open())
           log_cloud.close();
}

void SateliteUdpVideoStreamClient2::socketDataArrived(UdpSocket *socket, Packet *packet){
    //cout << "\ncar[" << getParentModule()->getIndex() << "] socketDataArrived"<< endl;
    if (packet != nullptr)
        processStream(packet);
}

void SateliteUdpVideoStreamClient2::handleMessageWhenUp(cMessage *msg){
    cout << "\ncar[" << getParentModule()->getIndex() << "] handleMessageWhenUp"
            << endl;
    cout << "msgName: " << msg->getName() << endl;

    if (msg != nullptr) {
        cout << "msgName: " << msg->getName() << endl;
        //cout << "Self Message: " << msg->isSelfMessage() << endl;

        if(msg->isSelfMessage()){
            cout << "self message" << endl;


            if(strcmp(msg->getName(), "UDPVideoStreamStart") == 0){
                cout << "msgName: " << msg->getName() << endl;
                cout << "Self Message: " << msg->isSelfMessage() << endl;
                cout << "iniciando conexão com servidor.'.." << endl;
                conexaoServer();
            }else if(strcmp(msg->getName(), "gerarNovoVideo") == 0){
                cout << "msgName: " << msg->getName() << endl;
                cout << "Self Message: " << msg->isSelfMessage() << endl;
                cout << "gerando novo vídeo..." << endl;
                rotinaDistanciaAntena();
                conexaoServer();
                gerarVideo();
                //conexaoServer();
            }else{
                //cout << "msgName: " << msg->getName() << endl;
                //conexaoServer();
                if (strcmp(msg->getName(), "VideoStrmUpReqNegativo") == 0) {
                    cout << "msgName: " << msg->getName() << endl;
                    cout << "Self Message: " << msg->isSelfMessage() << endl;
                    scheduleAt(simTime().dbl()+ uniform(0.0, 0.001), selfMsg);
                    //conexaoServer();
                    //cout << "VideoStrmUpReqNegativo..." << endl;
                }else {
                    cout << "msgName: " << msg->getName() << endl;
                    cout << "Enviando objeto detectado no vídeo..." << endl;
                    cout << "Self Message: " << msg->isSelfMessage() << endl;
                    timeInicioEnvio = simTime().dbl();
                    sendVideoPacket(msg);
                    cout << "Enviou objeto!" << endl;
                }
            }
        }else{
            cout << "msgName: " << msg->getName() << endl;
            cout << "Não Entrou no self message" << endl;
            socket.processMessage(msg);
        }
    }
}

void SateliteUdpVideoStreamClient2::rotinaDistanciaAntena(){
    //cout << "\ncar[" << getParentModule()->getIndex() << "] rotinaDistanciaAntena" << endl;

    //verificar calor de tos
    distanciaAntena(GNB, SERVER1, 10, DISTANCIA_GNB, &distGNB, &carVel);
    distanciaAntena(ENB, SERVER2, -1, DISTANCIA_ENB, &distENB, &carVel);

    ///Odificação Gemini

    // A. PRÉ-REQUISITOS (Verifique se GNB e ENB são membros ponteiros válidos)
       // if (!this->eNB || !this->gNB) return;

}

const char* SateliteUdpVideoStreamClient2::distanciaAntena(const char *tipoAntena, const char *nomeServidor,
        int tos, int distanciaMaxima, double *dist, double *vel) {


    IMobility *mobilityCarro = check_and_cast<IMobility*>(
    getModuleByPath("^.mobility"));

    const char *address = "";

    // TODO recalcular caso 5g fora do alcance

    inet::Coord positionCarro = mobilityCarro->getCurrentPosition();
    cModule *parentModule = getParentModule()->getParentModule();
    for (cModule::SubmoduleIterator iter(parentModule); !iter.end(); iter++) {
        cModule *submodule = *iter;
        // Identificar se o submódulo é uma antena com base em algum critério
        // Por exemplo, verificar se o nome do módulo contém a palavra "antena"
        if (strstr(submodule->getName(), tipoAntena) != nullptr) {
            // Encontrou uma antena, agora obter seu módulo de mobilidade
            IMobility *mobility = check_and_cast<IMobility*>(
                    submodule->getSubmodule("mobility"));
            veins::VeinsInetMobility *mobilityModule = check_and_cast<
                    veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));
            if (mobilityModule != nullptr) {
                // Acessar a posição da antena
                inet::Coord position = mobility->getCurrentPosition();
                *dist = position.distance(positionCarro);
                *vel = hypot(mobilityModule->getCurrentVelocity().getX(),
                        mobilityModule->getCurrentVelocity().getY());
//                cout << "Distancia: " << position.distance(positionCarro) << endl;
//                cout << "velocidade veinsinetmobility:  << endl;

                if (position.distance(positionCarro) <= distanciaMaxima) {
                    socket.setTos(tos);
                    address = nomeServidor;

                    distAntenaCon = position.distance(positionCarro);
                    antenaEnvio = address;

                    if (naArea == false) {
                        if (tempoInicial == -1) {
                            tempoInicial = simTime().dbl();
                            //cout<< "momennto em que entrou no alcance da antena: "<< tempoInicial << endl;
                        }
                    }

                    naArea = true;
                } else {
                    naArea = false;
                }
            }
        }
    }
    //cout << "naArea: " << naArea << endl;
    //cout << "server: " << address << endl;
    return address;
}

inet::L3Address SateliteUdpVideoStreamClient2::resolveServerAddress(const char *serverName)
{
    omnetpp::cModule *networkModule = getParentModule()->getParentModule();

    if (!networkModule) {
        EV_ERROR << "ERRO FATAL: Nao foi possivel encontrar o modulo de rede principal." << endl;
        return inet::L3Address();
    }

    // A busca será feita DENTRO do módulo de rede, usando getSubmodule
    // 1. Tenta obter o MÓDULO (cModule) do servidor diretamente do módulo de rede.
    omnetpp::cModule *serverModule = networkModule->getSubmodule(serverName);

    if (!serverModule) {
        EV_WARN << "Módulo '" << serverName << "' nao encontrado. Nao eh fatal, apenas fora da rede." << endl;
        return inet::L3Address();
    }

    // 2. Resolve o endereço IP a partir do NOME COMPLETO do módulo encontrado.
    // L3AddressResolver é mais robusto quando fornecemos o nome completo do caminho do módulo.
    inet::L3Address address;

    // Checagem de segurança
    if (!inet::L3AddressResolver().tryResolve(serverModule->getFullPath().c_str(), address)) {

            // Se falhar (módulo existe mas não tem IP, ou o nome não resolve):
        EV_ERROR << "ERRO: O modulo foi encontrado (" << serverModule->getFullPath() << ") mas nao possui endereco L3 (IP) valido." << endl;
        return inet::L3Address();
    }

    // Se chegou aqui, o endereço foi resolvido com sucesso
    EV_INFO << "Endereco para " << serverName << " resolvido com sucesso: " << address << endl;
    return address;
}

void SateliteUdpVideoStreamClient2::conexaoServer(short tosEscolhido) {

    cout << "\ncar[" << getParentModule()->getIndex() << "] conexaoServer"<< endl;

        int serverPort = par("serverPort");

        random_device rd;
        mt19937 gen(rd());
        binomial_distribution<unsigned int> binomial(1, 0.5);

        IMobility *mobilityCarro = check_and_cast<IMobility*>(
                getModuleByPath("^.mobility"));

        Quaternion velocidadadeAng = mobilityCarro->getCurrentAngularVelocity();

        double dist;
        double vel;
        const char *address;



        if (tosEscolhido == -2){
            // seleciona antena de acordo com a prioridade do dado
               // if ((prioridadeEnviar <= 1)){ ((lista_alta.size() != 0) || (lista_media.size() != 0)){
               if(distENB <= distGNB){
                    cout << "tentando conexão com server2..." << endl;
                    tosEscolhido = -1;
                    address = distanciaAntena(ENB, SERVER2, tosEscolhido, DISTANCIA_ENB, &dist, &vel);
                    cout << "Adrsess server 2..." << address << endl;
               }else{
                   cout << "tentando conexão com server1..." << endl;
                   tosEscolhido = 10;
                   address = distanciaAntena(GNB, SERVER1, tosEscolhido, DISTANCIA_GNB, &dist, &vel);
                   cout << "Adrsess server 1..." << address << endl;
               }

        }else if (tosEscolhido == 10) {
            cout << "tentando conexão com server1..." << endl;
            address = distanciaAntena(GNB, SERVER1, tosEscolhido, DISTANCIA_GNB, &dist, &vel);
            cout << "Adrsess server 1..." << address << endl;
        }else if (tosEscolhido == -1) {
            cout << "tentando conexão com server2..." << endl;
            address = distanciaAntena(ENB, SERVER2, tosEscolhido, DISTANCIA_ENB, &dist, &vel);
            cout << "Address server 2..." << address << endl;
        }


        //cout << "Adrsess " << address << endl;
         /// Tentando conexão com o servidor
        L3Address svrAddr = L3AddressResolver().resolve(address);
        cout << "Servidor " << svrAddr << endl;

        if (svrAddr.isUnspecified() | (strcmp(address, "") == 0)){

             cout << "Antena fora de área, transmissão suspensa" << endl;
             //scheduleAt(simTime().dbl() + uniform(0.0, 0.2), selfMsg);
             return;
        }

        EV_INFO << "Envia primeira conexão com:  " << svrAddr << ":" << serverPort << "\n";

        cout << "DIST: " << dist << endl;
        cout << "VEL: " << vel << endl;
        cout << "tempoInicial: " << tempoInicial << endl;
        cout << "Servidor: " << svrAddr << endl;
        cout << "Porta: " << serverPort << endl;

        if (naArea) {
           Packet *pk = createVideoStreamReqPacket(tosEscolhido, dist, vel, tempoInicial);
           cout << "enviando: " << pk->getFullName() << endl;
           socket.sendTo(pk, svrAddr, serverPort);
           cout << "Pedido de Offloading Enviado" << endl;
        }else {
            sendingState = 0;
            cout << "Antena fora de área, transmissão suspensa" << endl;
        }

}

long SateliteUdpVideoStreamClient2::calculaTotalDadosSize(){
    // define qual é a prioridade da vez (alta, média ou baixa) a depender do tamanho das listas.
    // retorna a somatória de dados presentes nessa lista


    double totalSize = 0;

    if (lista_InDead.size() > 0){

            double totalSize = 0;
            cout << "Somando o tamanho dos objetos..." << endl;

            for (Video v: lista_InDead)
                totalSize += v.getobSize();
            cout << "Soma do total de objetos " << totalSize << endl;


    }

    if (lista_deteccao.size() > 0){
            double totalSize = 0;
            cout << "Somando o tamanho dos objetos..." << endl;
            for (Video v: lista_deteccao)
                totalSize += v.getobSize();
            cout << "Soma do total de objetos " << totalSize << endl;


    }
    return totalSize;

}

long SateliteUdpVideoStreamClient2::calculaTotalDadosSize(int prioridade) {
    // define qual é a prioridade da vez (alta, média ou baixa) a depender do tamanho das listas.
    // retorna a somatória de dados presentes nessa lista


    double totalSize = 0;

    if (lista_InDead.size() > 0){
        double totalSize = 0;
        cout << "Somando o tamanho dos objetos..." << endl;

        for (Video v : lista_InDead)
            totalSize += v.getobSize();
        cout << "Soma do total de objeto " << totalSize << endl;

    }
    if (lista_deteccao.size() > 0){
            double totalSize = 0;
            cout << "Somando o tamanho dos objetos..." << endl;

            for (Video v : lista_deteccao)
                totalSize += v.getobSize();
            cout << "Soma do total de objeto " << totalSize << endl;
    }
    return totalSize;
}

// montar o pacote para enviar para o servidor contendo tamanho do pacote que deseja enviar posteriormente
// para verificar se o servidor aguenta receber o determinado pacote com o tamanho

Packet* SateliteUdpVideoStreamClient2::createVideoStreamReqPacket(short tosEscolhido, double dist, double vel, double tempoEntrada) {
    Packet *pk = new Packet("VideoStrmUpReq");
    //criando o pedido de descarregamento
//    proximoVideo = getVideoMaiorPrioridade();

    auto sateliteMessage = makeShared<SateliteMessage>();
    long totalSize = calculaTotalDadosSize();

//    sateliteMessage->setTamanhoPacote(20);  // configurar campos
    sateliteMessage->setTamanhoPacote(totalSize);  // configurar campos
    sateliteMessage->setClientAddr(getParentModule()->getFullName());
    sateliteMessage->setPortaCarro(localPort);
    sateliteMessage->setTosEscolhido(tosEscolhido);
    sateliteMessage->setChunkLength(B(32));
    sateliteMessage->setDistAntenna(dist);
    sateliteMessage->setVel(vel);
    sateliteMessage->setTempoEntrada(tempoEntrada);
    sateliteMessage->setRi(ri); // gerado aleatoriamente no .h (uniform(4,10))

    pk->insertAtBack(sateliteMessage);

    totalPacotes = ceil(totalSize / pktLen);


    const auto &payload = makeShared<ByteCountChunk>(B(1)); //FIXME set packet length

    pk->insertAtBack(payload);

    //long tamanhoPacote = pktLen;

    return pk;
}

// O veículo recebe a msg do servidor e se prepara pr enviar msg
void SateliteUdpVideoStreamClient2::processStream(Packet *msg) {
    // Registra o stream de Video
        cout << "\ncar[" << getParentModule()->getIndex() << "] processStream" << endl;


        //if (msg != nullptr) {



        if (strcmp(msg->getFullName(), "VideoStrmUpReqPositivo") == 0){

            cout << "tipo da mensagem: " << msg->getFullName() << endl;

            auto stltChunk = msg->peekAtFront<ServerConnMessage>();

            serverConectado = stltChunk->getSrcName();

            //capacidadeMaxEnvio=stltChunk->getCapacidadeMax();

            cout << "Servidor conectado: " << serverConectado << endl;



            if((lista_InDead.size() > 0) | (lista_deteccao.size() > 0)){

                cout << "Existem Objetos para Descarregar!" << endl;

                defineProximoVideo(capacidadeMaxEnvio);

                cout << "Objeto com Id "<< proximoVideo.getIdObj() << endl;

                totalDadosEnviar = calculaTotalDadosSize();


                //cout << "somatória do tamanho dos dados: " << double(totalDadosEnviar /1024 /1024) << endl;


                if (proximoVideo.getIdObj() >= 0){

                    //cout << "Objeto com Id "<< proximoVideo.getIdObj() << endl;

                    sendingState = 1;


                    //cout << "Id do Objeto "<< proximoVideo.getId() << endl;
                    cout << "Tamanho do Objeto "<< proximoVideo.getobSize() << endl;
                    cout << "deadline do Objeto "<< proximoVideo.getDeadline() << endl;


                    cout << "Objeto na fila de offloading" << endl;


                    dataStruct ds;
                    ds.videoId = proximoVideo.id;//proximoVideo.id;//
                    ds.idObj = proximoVideo.id;//proximoVideo.idObj;//
                    ds.serverConectado = serverConectado;
                    ds.totalDeDados = calculaTotalDadosSize();
                    ds.distAntena = distAntenaCon;
                    ds.maxTransmissaoDados = capacidadeMaxEnvio;
                    ds.tempoDeConectividade = simTime().dbl() - tempoInicial;

                    escreveLogConexao(&ds);



                    cMessage *timer = new cMessage("VideoStreamTmr");

                    VideoStreamData *d = &streams[timer->getId()];
                    d->videoId = proximoVideo.getId();
                    d->idObj = proximoVideo.getIdObj();
                    d->timer = timer;
                    d->clientAddr = msg->getTag<L3AddressInd>()->getSrcAddress();
                    d->clientPort = msg->getTag<L4PortInd>()->getSrcPort();
                    d->obSize = proximoVideo.getobSize();
                    d->bytesLeft = proximoVideo.getobSize();
                    d->numPkSent=0;
                    ASSERT(proximoVideo.getobSize() > 0);
                    //                delete msg;

                    numStreams++;
                    //emit(reqStreamBytesSignal, d);
                    emit(reqStreamBytesSignal, proximoVideo.getobSize());

                    // Transmite o primeiro pacote

                    sendVideoPacket(timer);
                    onObjectSent(proximoVideo);

                }else{
                    //cout << "Vídeo com Id "<< proximoVideo.getobSize() << endl;
                    sendingState = 0;
                }
            }



    ///////////Recebendo Objeto da Edge//////////////////////////



            if (stltChunk->getIdObj() > -1){

                if ((armazenamentoUtilizado + stltChunk->getObSize()) < maxArmazenamento) {
                    cout << "Verificando Objeto Enviado Pela Edge" << endl;

                    int idObjEnviado = stltChunk->getIdObj();
                    double TempDetectEnviado = stltChunk->getTempDetect();
                    int ObDetectEnviado = stltChunk->getObDetect();
                    double SizeObjEnviado = stltChunk->getObSize();
                    int prioridadeEnviado = stltChunk->getPrioridade();
                    double deadlineEnviado = stltChunk->getTempoDeVida();
                    double coordObXEnviado = stltChunk->getCoordObjX();
                    double coordObYEnviado = stltChunk->getCoordObjY();
                    double timeEnvObj = simTime().dbl();

                    //Criando o Objeto Enviado pela Edge
                    Video nv = Video(proximoVideo.getId(), timeEnvObj, prioridadeEnviado, SizeObjEnviado, deadlineEnviado, proximoVideo.coordX,  proximoVideo.coordY, idObjEnviado, ObDetectEnviado, SizeObjEnviado, coordObXEnviado, coordObYEnviado);
                    if(deadlineEnviado > simTime().dbl()){
                            cout << "Descarregando Objeto Enviado Pela Edge do " << serverConectado << endl;

                            // Add a lista de enviados
                            addToListInOrder(&lista_enviados, nv);

                            //Colocando no arquivo objetos enviados da edge
                            dataStruct ds;
                            ds.serverConectado = serverConectado;
                            ds.idObj = idObjEnviado;
                            ds.tempDetect=TempDetectEnviado;
                            ds.ObDetect = ObDetectEnviado;
                            ds.obSize = double(SizeObjEnviado / 1024 / 1024);
                            ds.prioridadeVideo = prioridadeEnviado;
                            ds.deadlineVideo = deadlineEnviado;
                            ds.coordObX = coordObXEnviado;
                            ds.coordObY = coordObYEnviado;

                            escreveLogObjEnviados(&ds);

                    }else{//Envia Objeto para Nuvem
                        addToListInOrder(&lista_OutEnviados, nv);
                    }
                }//Objeto Existente
            }//Existe Espaço para Armazenamento no Veículo



        }else if (strcmp(msg->getFullName(),  "VideoStrmUpReqNegativo") == 0) {
            sendingState = 0;
            cout << "tipo da mensagem: " << msg->getFullName() << endl;
            cout << "request negado pelo servidor" << endl;
            conexaoServer();
            //cout << "Capacidade de Armazenamento Disponível no Veículo " << maxArmazenamento-armazenamentoUtilizado << endl;
        }

}

// O Client nesse caso, tem que fazer o envio dos vídeos em pacotes para o servidor fazer o armazenamento
void SateliteUdpVideoStreamClient2::sendVideoPacket(cMessage *timer) {

    cout << "\ncar[" << getParentModule()->getIndex() << "] sendVideoPacket" << endl;

        // Aqui eu devo construir o Stream de video e dividí-lo em pacotes de forma a enviar
        // logo depois esses pacotes via socket
        //int streamID = timer->getId();
        //int streamID = timer->getKind();
        if (timer != nullptr){

            auto it = streams.find(timer->getId());

            if (it == streams.end())
               throw cRuntimeError("Model error: Stream not found for timer");

            VideoStreamData *d = &(it->second);

            //Enviando pedido de descarregamento
            IMobility *mobilityCarro = check_and_cast<IMobility*>(
                    getModuleByPath("^.mobility"));
            inet::Coord positionCarro = mobilityCarro->getCurrentPosition();
            veins::VeinsInetMobility *mobilityModule = check_and_cast<
                    veins::VeinsInetMobility*>(getModuleByPath("^.mobility"));



            if (proximoVideo.getIdObj() >= 0){
                cout << "Id do Objeto " << proximoVideo.getIdObj()  << endl;

                auto sateliteMessage = makeShared<SateliteMessage>();

                sateliteMessage->setTamanhoPacote(20);  // configurar campos
                sateliteMessage->setClientAddr(getParentModule()->getFullName());
                sateliteMessage->setPortaCarro(localPort);
                sateliteMessage->setTosEscolhido(-2);
                sateliteMessage->setChunkLength(B(32));
                sateliteMessage->setDistAntenna(-1);
                sateliteMessage->setVel(-1);
                sateliteMessage->setId(getParentModule()->getIndex());  // configurar campos
                sateliteMessage->setCoordX(positionCarro.getX());
                sateliteMessage->setCoordY(positionCarro.getY());
                sateliteMessage->setIdObj(proximoVideo.getIdObj());
                sateliteMessage->setTempDetect(proximoVideo.getTimeCriacao());
                sateliteMessage->setObDetect(proximoVideo.getobDetect());
                sateliteMessage->setPrioridade(proximoVideo.getPrioridade());
                sateliteMessage->setTempoDeVida(proximoVideo.getDeadline());
                sateliteMessage->setObSize(proximoVideo.getobSize());
                sateliteMessage->setCoordObjX(proximoVideo.getCoordObjX());
                sateliteMessage->setCoordObjY(proximoVideo.getCoordObjY());



                ///cout << "Objeto Size: " << proximoVideo.getobSize() << endl;
                // generate and send a packet
                Packet *pkt = new Packet("VideoStrmPk");

                long newPktLen = pktLen;

                //if (newPktLen > d->bytesLeft)
                newPktLen = d->bytesLeft;

                const auto &payload = makeShared<ByteCountChunk>(B(32));
                payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

                pkt->insertAtBack(sateliteMessage);
                pkt->insertAtBack(payload);

                L3Address svrAddr = d->clientAddr;
                int svrPort = d->clientPort;

                emit(packetSentSignal, pkt);
                socket.sendTo(pkt, svrAddr, svrPort);

                cout << "Tamanho pacote: " << newPktLen << endl;
                cout << "Objeto enviado: " << proximoVideo.getobDetect() << endl;

               ///////////////////////////////////////////////////////////////



        //        d->bytesLeft -= newPktLen;

                cout << "Bytes do objeto: " << d->bytesLeft << endl;

                double bytesLeft = d->bytesLeft - newPktLen;
                totalDadosEnviar -= newPktLen;

                cout << "Bytes faltantes: " << bytesLeft << endl;
                d->numPkSent++;
                numPkSent++;
                // reschedule timer if there's bytes left to send



                if (bytesLeft > 0) {
                    cout << "DESENCADEANDO ENVIO DO PRÓXIMO PACOTE" << endl;
                    cout << "Bytes faltantes do objeto: " << bytesLeft << endl;
                    cout << "Tamanho do objeto: " << proximoVideo.getobSize() << endl;
                    cout << "Servidor  " << svrAddr << endl;


                    simtime_t s_interval = (*sendInterval);

                    timer = new cMessage("VideoStreamTmr");
                    d = &streams[timer->getId()];

                    d->videoId = proximoVideo.getId();
                    d->idObj = proximoVideo.getIdObj();
                    d->timer = timer;
                    d->clientAddr = svrAddr;
                    d->clientPort = svrPort;
                    d->obSize = proximoVideo.getobSize();
                    d->bytesLeft = bytesLeft;
                    d->numPkSent = 0;
                    ASSERT(d->obSize > 0);
                    scheduleAt(simTime() + s_interval, timer);


                }else {

                    cout << "Objeto Enviado!!" << endl;

                    double tP=(double(proximoVideo.getobSize() / 1024 / 1024)*8)/30;
                    double tC=proximoVideo.getDeadline()-(simTime().dbl()+tP);
                    double ProptC=tC/tP;


                    dataStruct ds;
                    ds.idObj = proximoVideo.getIdObj();
                    ds.tC =tC;
                    ds.tP =tP;
                    ds.ProptC=ProptC;
                    ds.serverConectado = serverConectado;
                    ds.timeFinal = simTime().dbl();



                    escreveLogFinal(&ds);

                    totalVideosEnviados++;


                    armazenamentoUtilizado -= proximoVideo.getobSize();
                    capacidadeMaxEnvio -= proximoVideo.getobSize();

                    int idRemover = proximoVideo.getIdObj();

                    if (lista_InDead.size() > 0){
                        lista_InDead.remove_if([idRemover](Video &LD) {
                              return LD.getIdObj() == idRemover;});
                    }
                    if (lista_deteccao.size() > 0){
                        lista_deteccao.remove_if([idRemover](Video &LD){
                             return LD.getIdObj() == idRemover;});
                    }

                }
            }else{
                sendingState = 0;
            }

        }

}
void SateliteUdpVideoStreamClient2::onObjectSent(Video v)
{
    double waitingTime = simTime().dbl() - v.getTimeCriacao();
    waitSamples.push_back(waitingTime);
    updateTAging();
}

void SateliteUdpVideoStreamClient2::socketErrorArrived(UdpSocket *socket, Indication *indication) {
    cout << "\ncar[" << getParentModule()->getIndex() << "] socketErrorArrived"
            << endl;

    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void SateliteUdpVideoStreamClient2::socketClosed(UdpSocket *socket) {

    cout << "\ncar[" << getParentModule()->getIndex() << "] socketClosed"
            << endl;
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void SateliteUdpVideoStreamClient2::handleStartOperation(
     LifecycleOperation *operation) {
    cout << "\ncar[" << getParentModule()->getIndex()
           << "] handleStartOperation" << endl;

    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    socket.bind(localPort);

    //scheduleAt(simTime().dbl()+1, selfMsg);

    // TTL
    // Tempo de vida do pacote
    // verificar se posso usar para definir o tempo de vida do pacote
    // com uma distribuição binpmial 3, 0.75
    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
       socket.setTimeToLive(timeToLive);

    // DSCP
    // Server para diferenciar e classificar os pacotes de acordo com o tipo de serviço
    // verificar se posso usar para definir a prioridade do pacote
    // com uma distribuição binpmial 3, 0.75
    // Talvez não precise usar
    int dscp = par("dscp");
    if (dscp != -1)
       socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
       socket.setTos(-1);


    rotinaDistanciaAntena();
    conexaoServer();
    gerarVideo();

   }

void SateliteUdpVideoStreamClient2::clearStreams() {
    cout << "\ncar[" << getParentModule()->getIndex() << "] clearStreams"
            << endl;
    for (auto &elem : streams)
        cancelAndDelete(elem.second.timer);
    streams.clear();
}

void SateliteUdpVideoStreamClient2::handleStopOperation(
        LifecycleOperation *operation) {
    cout << "\ncar[" << getParentModule()->getIndex() << "] handleStopOperation"
            << endl;

    clearStreams();

//    lista_alta.clear();
//    lista_baixa.clear();
//    lista_media.clear();
//    lista_videos.clear();

    socket.setCallback(nullptr);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SateliteUdpVideoStreamClient2::handleCrashOperation(
        LifecycleOperation *operation) {

    cout << "\ncar[" << getParentModule()->getIndex()
            << "] handleCrashOperation" << endl;
    clearStreams();
    if (operation->getRootModule() != getContainingNode(this)){ // closes socket when the application crashed only
        socket.destroy(); //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
    socket.setCallback(nullptr);
    }
}
