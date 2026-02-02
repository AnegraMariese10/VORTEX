#ifndef __INET_SATELITE_UDPVIDEOSTREAMSVR_H
#define __INET_SATELITE_UDPVIDEOSTREAMSVR_H

//#include <omnetpp.h>
#include <omnetpp.h> // Essencial para qualquer módulo OMNeT++
// Se você for usar tipos específicos de mensagens ou classes de Simu5G/INET
// #include "simu5g/common/signals/LteControlInfo_m.h"
// #include "inet/common/signals/INetSignals.h
#include <map>
#include <list>
#include <fstream>

#include "inet/common/INETDefs.h"
#include "inet/common/INETMath.h"
#include "./video_model/video_class.cc"
#include "./objeto_model/objeto_class.cc"
#include "./data/data_struct.cc"


#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.h"

#include "./packet-mariese-cenario/SateliteMessage_m.h"
#include "./packet-mariese-cenario/CloudMessage_m.h"

#include <unordered_set>
#include <cstdint>





namespace inet {

/**
 * Stream VBR video streams to clients.
 *
 * Cooperates with UdpVideoStreamClient. UdpVideoStreamClient requests a strea
 * and SateliteUdpVideoStreamServer starts streaming to them. Capable of handling
 * streaming to multiple clients.
 */
class INET_API SateliteUdpVideoStreamServer: public ApplicationBase,
        public UdpSocket::ICallback {
public:
    struct StreamsFilesServer {
        cMessage *timer = nullptr;    // self timer msg
        L3Address clientAddr;  // client address
        int clientPort = -1;   // client UDP port
        int videoId = -1;
        int idObj = -1;
        int ObDetect=-1;
        int prioridade = 0;
        int tempoDeVida = 0;
        double coordObX=-1;
        double coordObY=-1;
        long obSize = 0;    // total size of video
        long bytesLeft = 0;    // bytes left to transmit
        long numPkSent = 0;    // number of packets sent

        friend std::ostream& operator<<(std::ostream &out,
                const StreamsFilesServer &d) {
            out << "client=" << d.clientAddr <<
                    ":" << d.clientPort << ""
                    "  size="<< d.obSize <<
                    "  pksent=" << d.numPkSent <<
                    "  bytesleft=" << d.bytesLeft;
            return out;
        }

    };

protected:

    cMessage *selfMsg = nullptr;
    cMessage *EnviarObjetoMSG = nullptr;

    // socket
    UdpSocket socket_car;
    UdpSocket socket_cld;

    int primeiraRequisicao = 0;
//    double distCarro = -1;
//    double carVel;
//    double tempoInicial;

    // parameters
    int localPort = -1;
    int serverPort;
    bool esvaziandoArmazenamento = false;

    list<Video> lista_alta;
    list<Video> lista_media;
    list<Video> lista_baixa;
    list<Video> lista_edge;
    list<Video> lista_cloud;
    list<Video> lista_Videos;
    list<Video> lista_Enviados;
    list<Video> lista_remocao;





    int totalObjetosDescarregados = 0;
    int totalObjetosEnviados = 0;

    int idRemover=-1;
    int obEncontrado=-1;
    double deadlineRemove=0;

    //int videoId = -1;
    //int idObj = -1;
    //int ObDetect=-1;
    //int prioridade = 0;
    //int tempoDeVida = 0;
    //double coordObX=-1;
    //double coordObY=-1;
    //long SizeObj = 0;    // total size of video


    typedef std::map<long int, StreamsFilesServer> FileStreamMap;
    FileStreamMap streams;

    // parametros de armazenamento e descarregamento para cloud
    bool ultrapassouLimite = false;
    unsigned long long armazenamentoUtilizadoEnB = 0LL;
    unsigned long long armazenamentoUtilizadoGnB = 0LL;
    unsigned long long armazenamentoCloud = 0LL;
    unsigned long long armazenamento2 = 0LL;
    unsigned long long capacidadeArmazenamento = 0LL;
    unsigned long long limiteArmazenamentoDescarregamento = 0LL;

    cPar *sendInterval = nullptr;
    cPar *packetLen = nullptr;
    cPar *obSize = nullptr;

    // statistics
    unsigned int numStreams = 0;    // number of video streams served
    unsigned long numPkSent = 0;    // total number of packets sent
    static simsignal_t reqStreamBytesSignal;   // length of video streams served

    // O servidor precisa captar a quantidade de pacotes que ele irá receber do Client, que enviará os vídeos através
    // do protocolo UDP
    // Para isso, ver o serviço do Client da aplicação original

    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void receiveStream(Packet *msg);
    virtual void requestStream();

    virtual void clearStreams();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication)
            override;
    virtual void socketClosed(UdpSocket *socket) override;

    virtual void createCloudPacket();
    virtual void sendCloudData(cMessage *timer);

    virtual double calculaTempoCon(double tempoEntrada);
//    virtual double calculaMaxTransmissaoDados(double tempoCon, double ri, double rj);

    //virtual double calculaMaxTransmissaoDados(double capacidadeArmazenamento, double armazenamentoUtilizado, double ri);
    virtual double calculaMaxTransmissaoDados(double tempoCon, double dist, double ri);
//    virtual double calculaSIR(double p, double Gij, double I, double e);
    virtual double calculaSIR();
    virtual double calculaCapCanal(double SIR);

    std::unordered_set<uint64_t> enviadosSet;

    std::unordered_set<int> objetosComRelayDecidido;

public:
    SateliteUdpVideoStreamServer() {
    }
    virtual ~SateliteUdpVideoStreamServer();

    void setValor(int novoValor);

private:
    ostringstream file_objetoDescarregado;
    ostringstream file_objetoEnviado;
    ostringstream file_objetoCloud;

    ofstream log_objetoDescarregado;
    ofstream log_objetoEnviado;
    ofstream log_objetoCloud;


    void enviarRequestPositiva(Packet *pk);
    void enviarRequestNegativa(Packet *pk);

    void addToListInOrder(list<Video> *list, Video nv);
    void addListaCloud(list<Video> *list, Video nv);
    void addToListInOrderCriacao(list<Video> *list, Video nv);
    //void EnviandoObjetoCar(Packet *pk, Video v);
    //void EnviandoObjetoCar(Packet *pk, int idObj, int obDetect, double sizeObj, int prioridade, int TempoDeVida, double coordObjX, double coordObjY);
    void escreveObjetoDescarregado(dataStruct* ds);
    void escreveObjetosEnviados (dataStruct* ds);
    void escreveObjetosCloud (dataStruct* ds);

    void verificarObjetosNaoDetectados(
            const std::list<Video>& lista_edge,
               int IdCarro,
               double coordX,
               double coordY,
               const L3Address& svrAddr,
               int tos,
               int portaCarro,
               const std::string& serverName);

    enum class ServerKind { UNKNOWN, SERVER1, SERVER2 };
    ServerKind serverKind = ServerKind::UNKNOWN;

    static constexpr double RELAY_RADIUS = 300.0;
    static constexpr double RELAY_R2 = RELAY_RADIUS * RELAY_RADIUS;



    // Função auxiliar para criar chave única (carro, objeto)
    static inline uint64_t makeKey(int carId, int objId) {
        return ( (uint64_t)(uint32_t)carId << 32 ) | (uint32_t)objId;
    }

};

} // namespace inet

#endif // ifndef __INET_UDPVIDEOSTREAMSVR_H
