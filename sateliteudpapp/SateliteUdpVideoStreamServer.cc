#include "SateliteUdpVideoStreamServer.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/ipv4/Ipv4RoutingTable.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"

#include "./packet-mariese-cenario/ServerConnMessage_m.h"
#include "./packet-mariese-cenario/SateliteMessage_m.h"
#include "./video_model/video_class.cc"
#include "./data/data_struct.cc"
#include "./objeto_model/objeto_class.cc"

#include <iostream>
#include <cmath>
#include <string>
#include <list>

using namespace std;
using namespace inet;


#define escalonamento 2  //2=VORTEX;


Define_Module(SateliteUdpVideoStreamServer);

simsignal_t SateliteUdpVideoStreamServer::reqStreamBytesSignal = registerSignal(
        "reqStreamBytes");

SateliteUdpVideoStreamServer::~SateliteUdpVideoStreamServer() {
}

void SateliteUdpVideoStreamServer::initialize(int stage) {
    ApplicationBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        file_objetoDescarregado << "./data_sim2/N30/Server/Descarregados/" << getParentModule()->getIndex()
                            << ".txt";
        file_objetoEnviado << "./data_sim2/N30/Server/Enviados/" << getParentModule()->getIndex()
                                    << ".txt";
        file_objetoCloud << "./data_sim2/N30/Server/Cloud/" << getParentModule()->getIndex()
                                            << ".txt";


        selfMsg = new cMessage("UDPVideoStreamStart");
        sendInterval = &par("sendInterval");
        obSize = &par("obSize");
        //ObDetect=&par("ObDetect");
        localPort = par("localPort");
        packetLen = &par("packetLen");
        numStreams = 0;
        numPkSent = 0;
        serverPort = par("serverPort");

        WATCH_MAP(streams);

        cModule *parentModule = getParentModule();
        cout << "Modulo pai: " << parentModule->getFullName() << endl;

        socket_car.setOutputGate(gate("socketOut"));
        socket_car.setCallback(this);


        // Capacidade da Edge para o 4G e 5G
        // 4G tem capacidade de 30 GB
        // 5G tem capacidade de 10 GB
        if (strcmp(parentModule->getFullName(), "server2") == 0) {
            capacidadeArmazenamento = 500LL * 1024 * 1024 * 1024;
            //armazenamentoUtilizado = 0LL;
//            capacidadeArmazenamento = 10LL * 1024 * 1024;
        } else if (strcmp(parentModule->getFullName(), "server1") == 0) {
            capacidadeArmazenamento = 1000LL * 1024 * 1024 * 1024;
            //armazenamentoUtilizado = 0LL;
//            capacidadeArmazenamento = 30LL * 1024 * 1024;
        } else {
            cout << "Erro. Não existe módulo." << endl;
        }

        limiteArmazenamentoDescarregamento = capacidadeArmazenamento * 0.75;
//        limiteArmazenamentoDescarregamento = 30 * 1024 * 1024 * 1024; // limite para testes

        cout << "Capacidade de armazenamento: " << capacidadeArmazenamento
                << endl;
        cout << "Limite para offloading: " << limiteArmazenamentoDescarregamento
                << endl;
//////////////////////////////////////////////////////////////////////
        log_objetoDescarregado.open(file_objetoDescarregado.str());

        log_objetoDescarregado
        << "time;escalonamento;server;carId, CoordX, CoordY, idObj;tempDetect;SizeObj;obDetect;"
                "prioridadeVideo;deadlineVideo;coordObjX;coordObjY;\n";

 ////////////////////////////////////////////////////////////////////
        log_objetoEnviado.open(file_objetoEnviado.str());

        log_objetoEnviado
        << "time;escalonamento;server;carId, CoordX, CoordY, idObj;tempDetect;SizeObj;obDetect;"
                "prioridadeVideo;deadlineVideo;coordObjX;coordObjY;\n";


////////////////////////////////////////////////////////////////////
    log_objetoCloud.open(file_objetoCloud.str());

    log_objetoCloud
       << "time;escalonamento;server;carId, CoordX, CoordY, idObj;tempDetect;SizeObj;obDetect;"
               "prioridadeVideo;deadlineVideo;coordObjX;coordObjY;\n";

    enviadosSet.clear();
   }
}

void SateliteUdpVideoStreamServer::finish() {
    //ApplicationBase::finish();
    if (log_objetoDescarregado.is_open())
        log_objetoDescarregado.close();

    if (log_objetoEnviado.is_open())
            log_objetoEnviado.close();

    if (log_objetoCloud.is_open())
        log_objetoCloud.close();
}

void SateliteUdpVideoStreamServer::escreveObjetoDescarregado(dataStruct *ds) {
        //cout << "atualizando log criação..." << endl;
        cModule *parentModule = getParentModule();
        //cout << "Servidor: " << parentModule->getFullName() << endl;

        ds->time = simTime().dbl();
        ds->serverConectado=parentModule->getFullName();


        if (!log_objetoDescarregado.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_objetoDescarregado.open(file_objetoDescarregado.str());
        }

        if (log_objetoDescarregado.is_open()) {


            log_objetoDescarregado << ""
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->serverConectado << ";"
                    << ds->carId << ";"
                    << ds->cordX << ";"
                    << ds->cordY << ";"
                    << ds->idObj << ";"
                    << ds->tempDetect << ";"
                    << ds->SizeObj << ";"
                    << ds->ObDetect << ";"
                    << ds->prioridadeVideo << ";"
                    << ds->deadlineVideo << ";"
                    << ds->coordObX << ";"
                    << ds->coordObY
                    <<";\n";

        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamServer::escreveObjetosEnviados(dataStruct *ds) {
        //cout << "atualizando log criação..." << endl;
        cModule *parentModule = getParentModule();
        //cout << "Servidor: " << parentModule->getFullName() << endl;

        ds->time = simTime().dbl();
        ds->serverConectado=parentModule->getFullName();


        if (!log_objetoEnviado.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_objetoEnviado.open(file_objetoEnviado.str());
        }

        if (log_objetoEnviado.is_open()) {


            log_objetoEnviado << ""
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->serverConectado << ";"
                    << ds->carId << ";" //carro que recebeu
                    << ds->cordX << ";"
                    << ds->cordY << ";"
                    << ds->idObj << ";" //objeto enviado
                    << ds->tempDetect << ";"
                    << ds->SizeObj << ";"
                    << ds->ObDetect << ";"
                    << ds->prioridadeVideo << ";"
                    << ds->deadlineVideo << ";"
                    << ds->coordObX << ";"
                    << ds->coordObY
                    <<";\n";

        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamServer::escreveObjetosCloud(dataStruct *ds) {
        //cout << "atualizando log criação..." << endl;
        cModule *parentModule = getParentModule();
        //cout << "Servidor: " << parentModule->getFullName() << endl;

        ds->time = simTime().dbl();
        ds->serverConectado=parentModule->getFullName();


        if (!log_objetoCloud.is_open()) {
    //        cout << "arquivo não estava previamente aberto" << endl;
            log_objetoCloud.open(file_objetoCloud.str());
        }

        if (log_objetoCloud.is_open()) {


            log_objetoCloud << ""
                    << ds->time << ";"
                    << escalonamento << ";"
                    << ds->serverConectado << ";"
                    << ds->carId << ";"
                    << ds->cordX << ";"
                    << ds->cordY << ";"
                    << ds->idObj << ";"
                    << ds->tempDetect << ";"
                    << ds->SizeObj << ";"
                    << ds->ObDetect << ";"
                    << ds->prioridadeVideo << ";"
                    << ds->deadlineVideo << ";"
                    << ds->coordObX << ";"
                    << ds->coordObY
                    <<";\n";

        } else {
            cout << "problema abrindo o arquivo!" << endl;
        }
}

void SateliteUdpVideoStreamServer::handleMessageWhenUp(cMessage *msg) {
    cout << "\n" << getParentModule()->getFullName() << " handleMessageWhenUp" << endl;
    cout << msg->getFullName() << " " << msg->isSelfMessage() << endl;

    if (msg->isSelfMessage()) {
        cout << "Mensagem 1 " << msg->getName() << endl;
        cout << "Mensagem 2 " << msg->getFullName() << endl;

        if(strcmp(msg->getFullName(), "VideoStreamTmr")==0){
            cout << "Mensagem " << msg->getFullName() << endl;
            createCloudPacket();
            delete msg;
        }

//        else{
//            requestStream();
//        }
    } else {
        cout << "Mensagem 1 " << msg->getName() << endl;
        cout << "Mensagem 2 " << msg->getFullName() << endl;
        if(strcmp(msg->getFullName(), "sendCloudData")==0){
            cout << "Mensagem " << msg->getFullName() << endl;
            socket_cld.processMessage(msg);
        }else{
            cout << "Mensagem " << msg->getFullName() << endl;
            socket_car.processMessage(msg);
        }
    }
}

void SateliteUdpVideoStreamServer::requestStream() {
    // QUando há o ping do veiculo para o servdor, tentar guardar o IP do carro para usar depois
    cout << "\n" << getParentModule()->getFullName() << " requestStream" << endl;
    int svrPort = par("serverPort");
    int localPort = par("localPort");
    const char *address = par("serverAddress");

    L3Address svrAddr;

    try {
        svrAddr = L3AddressResolver().resolve(address);
    } catch (const exception &e) {
        EV_WARN
       << "Endereço do servidor ainda não está disponível, tentando novamente mais tarde."
       << endl;
        EV_WARN << e.what() << endl;
        scheduleAt(simTime().dbl() + uniform(0.0, 0.1), selfMsg); // Reagendar para tentar novamente após um 'delay'
        return;
    }

    if (svrAddr.isUnspecified()) {
        EV_ERROR
                        << "Server address is unspecified, skip sending video stream request\n";
        return;
    }

    socket_car.setOutputGate(gate("socketOut"));
    socket_car.setCallback(this);

//    EV_INFO << "Requesting video stream from " << svrAddr << ":" << svrPort
//                   << "\n";
//    Packet *pk = new Packet("VideoStrmReq");
//    const auto &payload = makeShared<ByteCountChunk>(B(1)); //FIXME set packet length
//    pk->insertAtBack(payload);
//    socket_car.sendTo(pk, svrAddr, svrPort);
}

void SateliteUdpVideoStreamServer::socketDataArrived(UdpSocket *socket,
        Packet *packet) {
//    cout << "\n" << getParentModule()->getFullName() << " socketDataArrived" << endl;
    if(packet != nullptr)
        receiveStream(packet);
}

void SateliteUdpVideoStreamServer::socketErrorArrived(UdpSocket *socket,
        Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    cout << "\n" << getParentModule()->getFullName() << " socketErrorArrived" << endl;
    delete indication;
}

void SateliteUdpVideoStreamServer::socketClosed(UdpSocket *socket) {
    cout << "\n" << getParentModule()->getFullName() << " socketClosed" << endl;
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

double SateliteUdpVideoStreamServer::calculaTempoCon(double tempoEntrada)
{
    // dist =  distância carro antena
    // vel = velocidade do carro

//    return dist/hypot(vel.getX(), vel.getY());
//    return dist/vel;
    return simTime().dbl() - tempoEntrada;
}

double SateliteUdpVideoStreamServer::calculaMaxTransmissaoDados(double tempoCon, double dist, double ri)
//double SateliteUdpVideoStreamServer::calculaMaxTransmissaoDados(double capacidadeArmazenamento, double armazenamentoUtilizado, double ri)
{
    double rj = 30;
    if(strcmp(getParentModule()->getFullName(), "server1") == 0){
        rj = 40;
        return capacidadeArmazenamento-armazenamentoUtilizadoGnB;
    }else{
        rj = 40;
        return capacidadeArmazenamento-armazenamentoUtilizadoEnB;
    }

    cout << "ri: " << ri << " rj: " << rj << endl;

    // p * ganho =
    //return tempoCon * min(ri, rj);
    //return capacidadeArmazenamento-armazenamentoUtilizado;
}

//double SateliteUdpVideoStreamServer::calculaSIR(double p, double Gij, double I, double e)
double SateliteUdpVideoStreamServer::calculaSIR()
{
    // p = potência de transmissão do veículo
    // Gij = ganho do canal entre veículo e antena
    // I = interferência sofrida na transmissão de dados
    // e = potência do ruído

//    return (p*Gij)/(I+e);

    // TODO descobrir qual parâmetro corresponde à potência de ruído no arquivo .ini
    double e = -174;
    return 1/e;
}

double SateliteUdpVideoStreamServer::calculaCapCanal(double SIR)
{
    // Bj = largura de banda do canal (Hz)
    // SIR = relação de interferência do sinal (retorno de calculaSIR)
    // retorna o quanto o canal suporta de comunicação

//    return Bj * log2(1 + SIR);

    // TODO descobrir qual parâmetro corresponde à largura de banda no arquivo .ini
    double Bj;
    if(strcmp(getParentModule()->getFullName(), "server1") == 0) {
        Bj = 20;
    }else if (strcmp(getParentModule()->getFullName(), "server2")==0) {
        Bj = 10.5;
    }

    return Bj * log2(1 + SIR);
}



void SateliteUdpVideoStreamServer::createCloudPacket(){
        cout << "\n" << getParentModule()->getFullName() << " createCloudPacket" << endl;
        int cloudPort = par("cloudPort");
        L3Address cloudAddress = L3AddressResolver().resolve(par("cloudAddress"));

        cMessage *timer = new cMessage("VideoStreamTmr");
        StreamsFilesServer *d = &streams[timer->getId()];

        d->timer = timer;
        d->clientAddr = cloudAddress;
        d->clientPort = cloudPort;
        d->obSize = double(armazenamentoCloud / 1024 / 1024);
        d->bytesLeft = d->obSize;

        emit(reqStreamBytesSignal, d->obSize);
        sendCloudData(timer); // Inicia a transferência de dados para a cloud
}

void SateliteUdpVideoStreamServer::addToListInOrder(list<Video> *list,Video nv) {
    //Observação: Ordenar os dados por tamanho, independente do modelo,
    // descarrega mais dados
    if (list->size() != 0){
        auto it = list->begin();
        while ((it != list->end()) && (nv.obSize > it->obSize)){//((nv.deadline-(simTime().dbl()+nv.obSize/ri)) > (it->deadline-(simTime().dbl()+it->obSize/ri)))){//
            ++it;
        }
    //        cout << it->getId() << endl;
        list->insert(it, nv);
    } else {
        list->push_back(nv);
    }

}

void SateliteUdpVideoStreamServer::addListaCloud(list<Video> *list,Video nv) {
    int idRemover=-1;
    double deadlineRemove=0;
    for (Video nv : lista_edge){
       if(nv.getDeadline()  <=  simTime().dbl()){
           //cout << "Objetos na Cloud Antes: " <<  lista_cloud.size() << endl;

           addToListInOrder(&lista_cloud, nv);

           //cout << "Objetos na Cloud Depois: " <<  lista_cloud.size() << endl;
           idRemover=nv.getIdObj();
           deadlineRemove=nv.getDeadline();

           addToListInOrder(&lista_Videos, nv);

           cout << "Objeto Add a Cloud: " << idRemover << endl;
           cout << "tempo atual da Add na CLoud: " <<  simTime().dbl() << endl;
           cout << "Deadline do Objeto Add na Cloud: " << deadlineRemove << endl;

       }
    }
    //Removendo da Lista de Detecção da Edge
    if(lista_Videos.size() > 0){
      int idDeteccao=-1;
      int sizeEdgeAntes=-1;
      for (Video nv : lista_Videos){
          idDeteccao=nv.getIdObj();
          sizeEdgeAntes=lista_edge.size();
          //cout << "Objetos na Edge Antes: " <<  sizeEdgeAntes << endl;
          lista_edge.remove_if([idDeteccao](Video &nv) {
              return nv.getIdObj() == idDeteccao;
          });
          //if(lista_edge.size() < sizeEdgeAntes){
            //cout << "Objetos na Edge Antes: " <<  sizeEdgeAntes << endl;
          //cout << "Objetos na Edge Depois: " <<  lista_edge.size() << endl;
      }
    }
    lista_Videos.clear();

}

void SateliteUdpVideoStreamServer::enviarRequestPositiva(Packet *pk) {
//    cout << "**************************************************************" << endl;
//    cout << "********************   ENTROU AQUI!!   ***********************" << endl;
//    cout << "**************************************************************" << endl;
    cout << "\n" << getParentModule()->getFullName() << " enviarRequestPositiva" << endl;
    cModule *parentModule = getParentModule();

    auto stltChunk = pk->peekAtFront<SateliteMessage>();

    double distCarro = stltChunk.get()->getDistAntenna();
    double carVel = stltChunk.get()->getVel();
    double tempoInicial = stltChunk.get()->getTempoEntrada();
    int ri = stltChunk.get()->getRi();

    // Servidor envia mensagem de positivo
    int serverPort = par("serverPort");
    const char *address = par("serverAddress");
    // Tentando conexão com o servidor
    L3Address svrAddr = L3AddressResolver().resolve(stltChunk.get()->getClientAddr());
    EV_INFO << "Requesting video stream from " << svrAddr << ":" << serverPort
                   << endl;

    cout << "distancia carro/antenna: " << distCarro << endl;
    cout << "velocidade do carro: " << carVel << endl;

//    double tempoCon = calculaTempoCon(distCarro, carVel);
    double tempoCon = calculaTempoCon(tempoInicial);
    cout << "tempo de conectividade (T-Ti): " << tempoCon << endl;

    double maxTransmissaoDados = calculaMaxTransmissaoDados(tempoCon, distCarro, ri);



    //double maxTransmissaoDados= capacidadeArmazenamento-armazenamentoUtilizado;
    cout << "capacidade máxima de Armazenamento (MB): " << capacidadeArmazenamento << endl;
    cout << "capacidade máxima de transmissão de dados (MB): " << maxTransmissaoDados << endl;

    if (strcmp(parentModule->getFullName(), "server1") == 0){
        cout << "Armazenamento Utilizado gNB: " << armazenamentoUtilizadoGnB << endl;
        //double maxTransmissaoDados = capacidadeArmazenamento-armazenamentoUtilizadoEnB;

    }else if (strcmp(parentModule->getFullName(), "server2") == 0){
        cout << "Armazenamento Utilizado eNB: " << armazenamentoUtilizadoEnB << endl;
        //double maxTransmissaoDados = capacidadeArmazenamento-armazenamentoUtilizadoGnB;

    }
//    double sir = calculaSIR();
//    cout << "relação da interferência do sinal (SIR): " << sir << endl;
//
//    double capacidadeCanal = calculaCapCanal(sir);
//    cout << "capacidade do canal: " << capacidadeCanal << endl;

    socket_car.setOutputGate(gate("socketOut"));
    socket_car.setCallback(this);
    socket_car.setTos(stltChunk.get()->getTosEscolhido());


    //************Confirmando Recebimento do Pacote e Respondendo ao Pedido de Offloading**********

    auto confirmPacket = makeShared<ServerConnMessage>();
    confirmPacket->setConfirmacaoRequest(true);
    confirmPacket->setTempoCon(tempoCon);
    confirmPacket->setCapacidadeMax(maxTransmissaoDados);
    confirmPacket->setSrcName(getParentModule()->getFullName());
    confirmPacket->setChunkLength(B(32));

    // Montando pacote para dar um ping no servidor e dizer que a gente existe
    Packet *packetPositivo = new Packet("VideoStrmUpReqPositivo");
    const auto &payload = makeShared<ByteCountChunk>(B(32)); //FIXME set packet length

    packetPositivo->insertAtBack(confirmPacket);
    packetPositivo->insertAtBack(payload);

    //cout << "svrAddr: " << svrAddr << endl;
    //cout << "porta: " << stltChunk.get()->getPortaCarro() << endl;

    socket_car.sendTo(packetPositivo, svrAddr, stltChunk.get()->getPortaCarro());
    cout << "Mensagem do Servidor "<< "VideoStrmUpReqPositivo" << endl;

}

void SateliteUdpVideoStreamServer::enviarRequestNegativa(Packet *pk) {
    cout << "\n" << getParentModule()->getFullName() << " enviarRequestNegativa" << endl;
    cout << pk->getName() << endl;

    auto stltChunk = pk->peekAtFront<SateliteMessage>();

    double tempoInicial = stltChunk.get()->getTempoEntrada();
    double tempoCon = calculaTempoCon(tempoInicial);
    cout << "tempo de conectividade (T-Ti): " << tempoCon << endl;

    // Servidor envia mensagem de positivo
    int serverPort = par("serverPort");
    const char *address = par("serverAddress");
    // Tentando conexão com o servidor
    L3Address svrAddr = L3AddressResolver().resolve(stltChunk.get()->getClientAddr());

    socket_car.setOutputGate(gate("socketOut"));
    socket_car.setCallback(this);
    socket_car.setTos(stltChunk.get()->getTosEscolhido());

    // Confirmando Recebimento do Offloading e Respondendo ao Pedido
    auto confirmPacket = makeShared<ServerConnMessage>();
    confirmPacket->setConfirmacaoRequest(false);
    confirmPacket->setTempoCon(tempoCon);
    confirmPacket->setCapacidadeMax(0);
    confirmPacket->setSrcName(getParentModule()->getFullName());
    confirmPacket->setChunkLength(B(32));

    // Montando pacote para dar um ping no servidor e dizer que a gente existe
    Packet *packetNegativo = new Packet("VideoStrmUpReqNegativo");
    const auto &payload = makeShared<ByteCountChunk>(B(32)); //FIXME set packet length
    packetNegativo->insertAtBack(confirmPacket);
    packetNegativo->insertAtBack(payload);

    socket_car.sendTo(packetNegativo, svrAddr, stltChunk.get()->getPortaCarro());

    cout << "Mensagem do Servidor "<< "VideoStrmUpReqNegativo" << endl;
}



void SateliteUdpVideoStreamServer::receiveStream(Packet *pk) {
    cout << "\n" << getParentModule()->getFullName() << " receiveStream" << endl;
    cout << pk->getName() << endl;

    cModule *parentModule = getParentModule();
    // Verifica se o pacote recebido é um pacote de vídeo
    if (strcmp(pk->getName(), "VideoStrmPk") == 0) {
        if(!esvaziandoArmazenamento){
            auto stltChunk = pk->peekAtFront<SateliteMessage>();
            int serverPort = par("serverPort");
            const char *address = par("serverAddress");
            L3Address svrAddr =
                    L3AddressResolver().resolve(stltChunk->getClientAddr());
////////// Calcula o novo total de armazenamento utilizado após receber este pacote///
            std::string serverName = parentModule->getFullName();
            int tos = stltChunk->getTosEscolhido();
            int portaCarro = stltChunk->getPortaCarro();

            int IdCarro = stltChunk.get()->getId();
            double coordX = stltChunk.get()->getCoordX();
            double coordY = stltChunk.get()->getCoordY();



            if(stltChunk.get()->getIdObj() > -1){
                //int novoArmazenamentoUtilizado = armazenamentoUtilizado
                  //      + pk->getDataLength().get();
      ///////////////////Dados dos Objetos Enviados Pelos Veículos///////////
                cout << "Objeto Descarregado do CAV " << stltChunk.get()->getId() << endl;



                // ✅ AQUI está correto usar parentModule

                int idObj = totalObjetosDescarregados;
                double tempDetect = stltChunk.get()->getTempDetect();
                int ObDetect = stltChunk.get()->getObDetect();
                double SizeObj = stltChunk.get()->getObSize();
                int prioridadeObj = stltChunk.get()->getPrioridade();
                double deadlineObj = stltChunk.get()->getTempoDeVida();
                double coordObX = stltChunk.get()->getCoordObjX();
                double coordObY = stltChunk.get()->getCoordObjY();
                double timeDesObj = simTime().dbl();

                //Criando o objeto
                Video nv = Video(IdCarro, tempDetect, prioridadeObj, SizeObj, deadlineObj, coordX, coordY, idObj, ObDetect, SizeObj, coordObX, coordObY);

                if (deadlineObj <= simTime().dbl()){
                   //Enviado objeto pr cloud
                  //cout << "Objeto com Deadline Vencido" << endl;

                   addToListInOrder(&lista_cloud, nv);

                   //escrever no file cloud
                   dataStruct offCloud;

                   offCloud.serverConectado=parentModule->getFullName();
                   offCloud.carId=IdCarro;
                   offCloud.cordX=coordX;
                   offCloud.cordY=coordY;
                   offCloud.idObj = idObj;
                   offCloud.tempDetect=tempDetect;
                   offCloud.SizeObj = double(SizeObj / 1024 / 1024);
                   offCloud.ObDetect=ObDetect;
                   offCloud.prioridadeVideo = prioridadeObj;
                   offCloud.deadlineVideo = deadlineObj;
                   offCloud.coordObX=coordObX;
                   offCloud.coordObY=coordObY;

                   escreveObjetosCloud(&offCloud);
                }



    ///////////Criando o Video e Add na Lista de Detecção///////////////////////////////////////////////////////////////////////////


                if (lista_edge.size() == 0){
                    //cout << "Armazenando Objeto" << endl;
                    //Adicionando o objeto a lista vazia

                    //cout << "Recebendo Objeto do Veículo: " << IdCarro << endl;

                    addToListInOrder(&lista_edge, nv);
                    totalObjetosDescarregados += 1;

                    ////////// Calcula o novo total de armazenamento utilizado após receber este pacote///
                    if (strcmp(parentModule->getFullName(), "server2") == 0)
                        armazenamentoUtilizadoEnB += SizeObj;
                    else if (strcmp(parentModule->getFullName(), "server1") == 0)
                        armazenamentoUtilizadoGnB += SizeObj;

                    dataStruct offEdge;

                    offEdge.serverConectado=parentModule->getFullName();
                    offEdge.carId=IdCarro;
                    offEdge.cordX=coordX;
                    offEdge.cordY=coordY;
                    offEdge.timeFinal =timeDesObj;
                    offEdge.idObj = idObj;
                    offEdge.tempDetect=tempDetect;
                    offEdge.SizeObj = double(SizeObj / 1024 / 1024);
                    offEdge.ObDetect=ObDetect;
                    offEdge.prioridadeVideo = prioridadeObj;
                    offEdge.deadlineVideo = deadlineObj;
                    offEdge.coordObX=coordObX;
                    offEdge.coordObY=coordObY;


                    escreveObjetoDescarregado(&offEdge);

                }else{
                    for (Video ND : lista_edge){

                        double distEdge=sqrt(pow((ND.getCoordObjX()-coordObX),2) + pow((ND.getCoordObjY()-coordObY),2));

                        //Verificando se o objeto foi descarregado na
                        //mesma posição
                        if ((ND.getId() == IdCarro) && (ND.getobDetect() == ObDetect) && (ND.getobSize() == SizeObj) && (ND.getCoordObjX() == coordObX) && (ND.getCoordObjY() == coordObY)){
                           //cout << "Objeto Já Descarregado" << endl;
                           lista_cloud.push_back(nv);
                           //continue;
                           break;
                        }else if (((distEdge > 0) && (distEdge <= 3)) && (ND.getobSize() == SizeObj) && (ND.getobDetect() == ObDetect)){
                            //verificando se foi descarregado
                            //em posição diferente


                            // Envia Objeto na posição antiga pr cloud
                            //cout << "Objeto em posição diferente" << endl;

                            dataStruct offCloud;
                            offCloud.serverConectado=parentModule->getFullName();
                            offCloud.carId=ND.getId();
                            offCloud.cordX=ND.getCoordX();
                            offCloud.cordY=ND.getCoordY();
                            offCloud.idObj = ND.getIdObj();
                            offCloud.tempDetect=ND.getTimeCriacao();
                            offCloud.SizeObj = double(ND.getobSize() / 1024 / 1024);
                            offCloud.ObDetect=ND.getobDetect();
                            offCloud.prioridadeVideo = ND.getPrioridade();
                            offCloud.deadlineVideo = ND.getDeadline();
                            offCloud.coordObX=ND.getCoordObjX();
                            offCloud.coordObY=ND.getCoordObjY();

                            escreveObjetosCloud(&offCloud);

                            lista_cloud.push_back(ND);
                            //continue;
                            break;
                        }else{
                            //cout << "Armazenando o Objeto na " << parentModule->getFullName() << endl;

                            addToListInOrder(&lista_edge, nv);
                            totalObjetosDescarregados += 1;

                            //Atualizando o armazenamento
                            if (strcmp(parentModule->getFullName(), "server2") == 0)
                                armazenamentoUtilizadoEnB += SizeObj;
                            else if (strcmp(parentModule->getFullName(), "server1") == 0)
                                armazenamentoUtilizadoGnB += SizeObj;

                            dataStruct offEdge;
                            offEdge.serverConectado=parentModule->getFullName();
                            offEdge.carId=IdCarro;
                            offEdge.cordX=coordX;
                            offEdge.cordY=coordY;
                            offEdge.timeFinal =timeDesObj;
                            offEdge.idObj = idObj;
                            offEdge.tempDetect=tempDetect;
                            offEdge.SizeObj = double(SizeObj / 1024 / 1024);
                            offEdge.ObDetect=ObDetect;
                            offEdge.prioridadeVideo = prioridadeObj;
                            offEdge.deadlineVideo = deadlineObj;
                            offEdge.coordObX=coordObX;
                            offEdge.coordObY=coordObY;

                            escreveObjetoDescarregado(&offEdge);
                        }
                        // inviável para edge antes do deadline
                        if (ND.getDeadline() <= simTime().dbl()){
                            lista_cloud.push_back(ND);
                            continue;
                        }

                    }//For de verificação
                }//lista edge com objetos descarregados
                ////Identificando Objetos Não detectado  ////////////////////

                verificarObjetosNaoDetectados(
                        lista_edge,
                        IdCarro,
                        coordX,
                        coordY,
                        svrAddr,
                        tos,
                        portaCarro,
                        serverName
                );

                if(lista_cloud.size() > 0){
                   //cout << "Objetos Sendo Removidos da Lista Edge" << endl;
                   Video ND = lista_cloud.front();

                   dataStruct offCloud;

                   offCloud.serverConectado=parentModule->getFullName();
                   offCloud.carId=ND.getId();
                   offCloud.cordX=ND.getCoordX();
                   offCloud.cordY=ND.getCoordY();
                   offCloud.idObj = ND.getIdObj();
                   offCloud.tempDetect=ND.getTimeCriacao();
                   offCloud.SizeObj = double(ND.getobSize() / 1024 / 1024);
                   offCloud.ObDetect=ND.getobDetect();
                   offCloud.prioridadeVideo = ND.getPrioridade();
                   offCloud.deadlineVideo = ND.getDeadline();
                   offCloud.coordObX=ND.getCoordObjX();
                   offCloud.coordObY=ND.getCoordObjY();

                   escreveObjetosCloud(&offCloud);

                   //Atualizando Armazenamento
                   if (strcmp(parentModule->getFullName(), "server2") == 0){
                      armazenamentoUtilizadoEnB-=ND.getobSize();
                      //cout << "Atualizando o ArmazenamentoUtilizadoGnB" << endl;
                   }else if (strcmp(parentModule->getFullName(), "server1") == 0){
                       armazenamentoUtilizadoGnB-=ND.getobSize();
                       //cout << "Atualizando o ArmazenamentoUtilizadoGnB" << endl;
                   }
                   //Removendo da Lista Edge
                   int idRemover=ND.getIdObj();

                   lista_edge.remove_if([idRemover](Video &v) {
                       return v.getIdObj() == idRemover;
                   });

                   lista_cloud.remove_if([idRemover](Video &v) {
                       return v.getIdObj() == idRemover;
                   });
                   //cout << "Número de Objetos na Edge "<< lista_edge.size() << endl;
               }
            }else{//Veículo Não-Existente
                //cout << "Objeto ou Veículo Inexistente "<< stltChunk.get()->getId()/stltChunk.get()->getIdObj() << endl;
                //delete pk;
                ////Identificando Objetos Não detectado  ////////////////////

                verificarObjetosNaoDetectados(
                        lista_edge,
                        IdCarro,
                        coordX,
                        coordY,
                        svrAddr,
                        tos,
                        portaCarro,
                        serverName
                );
            }

            ///Enviando Objetos para Cloud ////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
            cMessage *timer = new cMessage("VideoStreamTmr");
            StreamsFilesServer *d = &streams[timer->getId()];
            d->timer = timer;
            d->idObj = totalObjetosDescarregados;//d->idObj;
            d->ObDetect = d->ObDetect;
            d->prioridade = d->prioridade;
            d->tempoDeVida = d->tempoDeVida;
            d->clientAddr = pk->getTag<L3AddressInd>()->getSrcAddress();
            d->clientPort = pk->getTag<L4PortInd>()->getSrcPort();
            //d->obSize = armazenamentoUtilizado;
            d->bytesLeft = d->obSize;

            //cout << "Enviando Mensagem de Fluxo" << endl;
            if (strcmp(parentModule->getFullName(), "server2") == 0){
                cout << "Armazenamento utilizado da eNB: " << capacidadeArmazenamento<< "/"
                                                                << armazenamentoUtilizadoEnB << endl;
                d->obSize = armazenamentoUtilizadoEnB;
            }else if (strcmp(parentModule->getFullName(), "server1") == 0){
                cout << "Armazenamento utilizado da gNB: " << capacidadeArmazenamento<< "/"
                <<  armazenamentoUtilizadoGnB << endl;
                d->obSize = armazenamentoUtilizadoGnB;
            }
            //cout << "armazenamento utilizado do servidor: " << capacidadeArmazenamento<< "/"
              //                                              << double (armazenamentoUtilizado /1024 /1024) << endl;
            // Verifica se o novo total excede o limite de armazenamento
            //d->obSize = armazenamentoUtilizado;
            d->bytesLeft = d->obSize;
            // Se não exceder, atualiza o armazenamento utilizado e processa o pacote
            EV_INFO << "Recebido pacote de fluxo de vídeo: " << pk->str() << endl;
            //armazenamentoUtilizado = armazenamentoUtilizado;
            emit(packetReceivedSignal, pk);

            //número do objeto descarregado
            totalObjetosDescarregados++;


        }else{
            cout << "servidor em modo offloading" << endl;
            armazenamento2 += pk->getDataLength().get();
            cout << "armazenamento2: " << armazenamento2 << endl;
            if (strcmp(parentModule->getFullName(), "server2") == 0){
                cout << "Capacidade de armazenamento da eNB/utilizado : " << capacidadeArmazenamento<< "/"
                                                                << armazenamentoUtilizadoEnB  << endl;
            }else if (strcmp(parentModule->getFullName(), "server1") == 0){
                cout << "Capacidade de armazenamento da gNB/utilizado : " << capacidadeArmazenamento<< "/"
                << armazenamentoUtilizadoGnB << endl;
            }
            //cout << "Armazenamento Da Edge Utilizado: " << armazenamentoUtilizado << endl;
//            enviarRequestNegativa(pk);
            // corrigir isso
            //lista_cloud.clear();
            enviarRequestPositiva(pk);
        }
    }else if (strcmp(pk->getName(), "VideoStrmUpReq") == 0) {
        if (strcmp(parentModule->getFullName(), "server2") == 0){

            if (armazenamentoUtilizadoEnB < limiteArmazenamentoDescarregamento) {
                enviarRequestPositiva(pk);
                cout << "Enviando Resposta do Offloading" << endl;
                cout << "Armazenamento Utilizado da EnB: " << armazenamentoUtilizadoEnB << endl;
            }else {
                cout << "Armazenamento Utilizado da EnB: " << armazenamentoUtilizadoEnB << endl;
                cout << "limite de armazenamento alcançado" << endl;
                esvaziandoArmazenamento = true;
                //enviarRequestNegativa(pk);

                if (!esvaziandoArmazenamento)
                    createCloudPacket();
                enviarRequestPositiva(pk);
            }
        }else if (strcmp(parentModule->getFullName(), "server1") == 0){

            if (armazenamentoUtilizadoGnB < limiteArmazenamentoDescarregamento) {
                enviarRequestPositiva(pk);
                cout << "Enviando Resposta do Offloading" << endl;
                cout << "Armazenamento Utilizado da GnB: " << armazenamentoUtilizadoGnB << endl;
            }else {
                cout << "Armazenamento Utilizado da GnB: " << armazenamentoUtilizadoGnB << endl;
                cout << "limite de armazenamento alcançado da GNB" << endl;
                esvaziandoArmazenamento = true;
                //enviarRequestNegativa(pk);

                if (!esvaziandoArmazenamento)
                    createCloudPacket();
                enviarRequestPositiva(pk);
            }
        }
    }
    // Deleta o pacote após o processamento
    delete pk;
    cout << " Saindo do receiveStream" << endl;
}

void SateliteUdpVideoStreamServer::verificarObjetosNaoDetectados(
        const std::list<Video>& lista_edge,
        int IdCarro,
        double coordX,
        double coordY,
        const L3Address& svrAddr,
        int tos,
        int portaCarro,
        const std::string& serverName){

    const double R2 = 300.0 * 300.0;

    if (lista_edge.empty())
        return;

    EV << "[DBG] server=" << serverName
       << " t=" << simTime()
       << " enviadosSet.size=" << enviadosSet.size()
       << " lista_edge.size=" << lista_edge.size()
       << endl;

    std::unordered_set<int> seenIds;
    std::unordered_set<int> enviadosNestaBusca;

    for (Video ND : lista_edge) {   // mantém assim se getters não forem const

        // --- DEBUG: duplicatas na lista ---
        if (seenIds.count(ND.getIdObj())) {
            EV << "[DUP IN LIST] obj=" << ND.getIdObj()
               << " aparece mais de uma vez na lista_edge" << endl;
        }
        seenIds.insert(ND.getIdObj());

        if (enviadosNestaBusca.count(ND.getIdObj()))
            continue;

        // 1️⃣ Deadline válido
        if (ND.getDeadline() <= simTime().dbl())
            continue;

        // 2️⃣ Não reenviar para o próprio veículo
        if (ND.getId() == IdCarro)
            continue;

        // 3️⃣ Distância (sem sqrt)
        double dx = ND.getCoordObjX() - coordX;
        double dy = ND.getCoordObjY() - coordY;
        if (dx*dx + dy*dy > R2)
            continue;

        // 4️⃣ Já foi enviado para este carro?
        uint64_t key = makeKey(IdCarro, ND.getIdObj());

        auto result = enviadosSet.insert(key);
        if (!result.second)
            continue;

        // Histórico (não remover)
        addToListInOrder(&lista_Enviados, ND);

        // Configura TOS
        socket_car.setTos(tos);

        // Monta mensagem
        auto confirmPacket = makeShared<ServerConnMessage>();
        confirmPacket->setConfirmacaoRequest(true);
        confirmPacket->setSrcName(serverName.c_str());
        confirmPacket->setChunkLength(B(32));
        confirmPacket->setIdObj(ND.getIdObj());
        confirmPacket->setTempDetect(ND.getTimeCriacao());
        confirmPacket->setObDetect(ND.getobDetect());
        confirmPacket->setObSize(ND.getobSize());
        confirmPacket->setPrioridade(ND.getPrioridade());
        confirmPacket->setTempoDeVida(ND.getDeadline());
        confirmPacket->setCoordObjX(ND.getCoordObjX());
        confirmPacket->setCoordObjY(ND.getCoordObjY());

        // Capacidade do servidor
        double capMB = (serverName == "server2")
            ? armazenamentoUtilizadoEnB / (1024.0 * 1024.0)
            : armazenamentoUtilizadoGnB / (1024.0 * 1024.0);

        confirmPacket->setCapacidadeMax(capMB);

        // Cria pacote
        Packet *packetObjEnviado = new Packet("VideoStrmUpReqPositivo");

        const auto &payload = makeShared<ByteCountChunk>(B(32));
        payload->addTag<CreationTimeTag>()
               ->setCreationTime(simTime());

        packetObjEnviado->insertAtBack(confirmPacket);
        packetObjEnviado->insertAtBack(payload);

        emit(packetSentSignal, packetObjEnviado);

        // Envia para o veículo
        socket_car.sendTo(packetObjEnviado, svrAddr, portaCarro);

        enviadosNestaBusca.insert(ND.getIdObj());

        // -------- dataStruct (NÃO REMOVER) --------
        dataStruct SendCAV;
        SendCAV.serverConectado = serverName;
        SendCAV.carId = ND.getId();
        SendCAV.cordX = ND.getCoordX();
        SendCAV.cordY = ND.getCoordY();
        SendCAV.idObj = ND.getIdObj();
        SendCAV.tempDetect = ND.getTimeCriacao();
        SendCAV.SizeObj = ND.getobSize() / (1024.0 * 1024.0);
        SendCAV.ObDetect = ND.getobDetect();
        SendCAV.prioridadeVideo = ND.getPrioridade();
        SendCAV.deadlineVideo = ND.getDeadline();
        SendCAV.coordObX = ND.getCoordObjX();
        SendCAV.coordObY = ND.getCoordObjY();

        escreveObjetosEnviados(&SendCAV);
        return;
    }
}

void SateliteUdpVideoStreamServer::sendCloudData(cMessage *timer) {
    cout << "\n" << getParentModule()->getFullName() << " sendCloudData" << endl;

    cModule *parentModule = getParentModule();

    auto it = streams.find(timer->getId());
    if (it == streams.end())
        throw cRuntimeError("Model error: Stream not found for timer");

    StreamsFilesServer *d = &(it->second);

    // generate and send a packet
    Packet *pkt = new Packet("VideoStrmPk");

    long pktLen = *packetLen;

    if (pktLen > d->bytesLeft)
        pktLen = d->bytesLeft;

    const auto &payload = makeShared<ByteCountChunk>(B(pktLen));
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    pkt->insertAtBack(payload);


    emit(packetSentSignal, pkt);
    socket_cld.sendTo(pkt, d->clientAddr, d->clientPort);

    d->bytesLeft -= pktLen;
    d->numPkSent++;


    if (strcmp(parentModule->getFullName(), "server2") == 0){
        armazenamentoUtilizadoEnB -= pktLen;
        // reschedule timer if there's bytes left to send
        if (armazenamentoUtilizadoEnB > 0) {
            cout << "dados faltantes para offloading: " << capacidadeArmazenamento-armazenamentoUtilizadoEnB << endl;
            simtime_t s_interval = (*sendInterval);
            scheduleAt(simTime() + s_interval, timer);
        } else {
            cout << "offloading para cloud terminado!" << endl;
            esvaziandoArmazenamento = false;
            armazenamentoUtilizadoEnB = 0;
            armazenamento2 = 0;
            delete timer;
        }
    }else if (strcmp(parentModule->getFullName(), "server1") == 0){
        armazenamentoUtilizadoEnB -= pktLen;
        // reschedule timer if there's bytes left to send
        if (armazenamentoUtilizadoGnB > 0) {
            cout << "dados faltantes para offloading: " << capacidadeArmazenamento-armazenamentoUtilizadoGnB << endl;
            simtime_t s_interval = (*sendInterval);
            scheduleAt(simTime() + s_interval, timer);
        } else {
            cout << "offloading para cloud terminado!" << endl;
            esvaziandoArmazenamento = false;
            armazenamentoUtilizadoGnB = 0;
            armazenamento2 = 0;
            delete timer;
        }
    }
}

void SateliteUdpVideoStreamServer::clearStreams() {
    // Talvez implementar algo aqui
//    cout << "clearStreams" << endl;
}

void SateliteUdpVideoStreamServer::handleStartOperation(
        LifecycleOperation *operation) {
    cout << "\n" << getParentModule()->getFullName() << " handlStartOperation" << endl;

    socket_car.setOutputGate(gate("socketOut"));
    socket_car.bind(localPort);
    socket_car.setCallback(this);

    socket_cld.setOutputGate(gate("socketOut"));
    socket_cld.bind(5000);
    socket_cld.setCallback(this);

//    simtime_t startTimePar = par("startTime");
//    simtime_t startTime = max(startTimePar, simTime());
//    scheduleAt(startTime, selfMsg);

    scheduleAt(simTime().dbl() + uniform(0.0, 0.2), selfMsg);
}

void SateliteUdpVideoStreamServer::handleStopOperation(
        LifecycleOperation *operation) {
//    cout << "handleStopOperation" << endl;

    cancelEvent(selfMsg);
    socket_car.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SateliteUdpVideoStreamServer::handleCrashOperation(
        LifecycleOperation *operation) {
//    cout << "handlCrashOperation" << endl;

    cancelEvent(selfMsg);
    if (operation->getRootModule() != getContainingNode(this)) // closes socket_car when the application crashed only
        socket_car.destroy(); //TODO  in real operating systems, program crash detected by OS and OS closes socket_cars of crashed programs.

}
