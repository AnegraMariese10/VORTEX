#ifndef __INET_SATELITE_UDPVIDEOSTREAMCLI_H
#define __INET_SATELITE_UDPVIDEOSTREAMCLI_H

#include <omnetpp.h> // Essencial para qualquer módulo OMNeT++
// Se você for usar tipos específicos de mensagens ou classes de Simu5G/INET
// #include "simu5g/common/signals/LteControlInfo_m.h"
// #include "inet/common/signals/INetSignals.h

#include "inet/common/INETDefs.h"
#include "inet/common/INETMath.h"
#include "./video_model/video_class.cc"
#include "./objeto_model/objeto_class.cc"
#include "./data/data_struct.cc"



#include <map>
#include <list>
#include <unordered_set>
#include <fstream>

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/mobility/contract/IMobility.h"

#include <xgboost/c_api.h>

#include "./packet-mariese-cenario/SateliteMessage_m.h"
//#include "./packet-mariese-cenario/MessageObjeto_m.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/common/LteControlInfo_m.h" // Possivelmente para sinais de controle LTE/NR
#include "/home/anegramariese/src/Simu5G-1.1.0/src/common/LteCommon.h" // Possivelmente para sinais de controle LTE/NR
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/LtePhyUe.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/LtePhyUeD2D.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/phy/layer/NRPhyUe.h"
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/NRSchedulerGnbUl.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/LteSchedulerEnbUl.h" // Incluir o cabeçalho do gNB
#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/LteSchedulerEnb.h" // Incluir o cabeçalho do gNB
//#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduler/LteScheduler.h" // Incluir o cabeçalho do gNB
//#include "/home/anegramariese/src/Simu5G-1.1.0/src/stack/mac/scheduling_modules/LtePf.h" // Incluir o cabeçalho do gNB

using namespace std;

namespace inet {

/**
 * A "Realtime" VideoStream client application.
 *
 * Basic video stream application. Clients connect to server and get a stream of
 * video back.
 */
class INET_API SateliteUdpVideoStreamClient2 : public ApplicationBase, public UdpSocket::ICallback
{
  public:
    struct VideoStreamData
        {
            int videoId = -1;
            cMessage *timer = nullptr;    // self timer msg
            L3Address clientAddr;    // client address
            int clientPort = -1;    // client UDP port
            long videoSize = 0;    // total size of video
            int idObj = -1;
            int ObDetect=-1;
            long obSize = 0;    // total size of video
            long bytesLeft = 0;    // bytes left to transmit
            long numPkSent = 0;    // number of packets sent
            int prioridade = 0;
            int tempoDeVida = 0;
            double coordObX=-1;
            double coordObY=-1;

            friend std::ostream& operator<<(std::ostream &out,
                    const SateliteUdpVideoStreamClient2::VideoStreamData &d) {
                out << "client=" << d.clientAddr
                        << ":" << d.clientPort
                        << "  size="  << d.obSize
                        << "  pksent=" << d.numPkSent
                        << "  bytesleft=" << d.bytesLeft;
                return out;
            }
        };
    struct DadosArquivoControle {
        long tamanhoArquivo;
    };
    SateliteUdpVideoStreamClient2() {}
    ~SateliteUdpVideoStreamClient2();
  protected:
    // Prioridade do arquivo diz sobre quanto tempo de vida ele pode permanecer
    // 0 - Alta prioridade - 15s
    // 1 - Média prioridade - 30s
    // 2 - Baixa prioridade - 90s

    Video proximoVideo = Video();
    unsigned long long armazenamentoUtilizado = 0LL;
    unsigned long long maxArmazenamento = 0LL;

    //long

//    int prioridade = 0;
//    int tempoDeVida = 0;
    double startTime = 0;
    double minSize = 0.4 * 1024 * 1024; // 0.4mB
    double maxSize = 3 * 1024 * 1024; // 3mB

    double T_aging;          // valor inicial (fallback)
    double alphaAging;       // fator multiplicativo
    int minSamples;           // mínimo para estimar percentil

    std::vector<double> waitSamples;  // latências observadas


    int ri = uniform(4, 10); // taxa de transmissão de dados do carro
    int pktLen = ri * 0.006 * 1024 * 1024;
    string serverConectado = "";
    double distAntenaCon = -1;
    double timeInicioEnvio = 0;
    double tempoDeConectividade = 0;


    omnetpp::cModule *enbModule = nullptr; // Ponteiro para o módulo eNB
    omnetpp::cModule *gnbModule = nullptr; // Ponteiro para o módulo gNB
    cMessage *handoverCheckTimer;    // <-- Deve ter ;

    double distGNB;
    double distENB;
    double carVel;

//    int totalVideoArmazenados = 0;
    int totalVideosGerados = 0;
    int totalVideosEnviados = 0;

    list<Video> lista_videos;
    list<Video> lista_alta;
    list<Video> lista_media;
    list<Video> lista_baixa;

    list<Video> lista_deteccao;
    list<Video> lista_enviados;
    list<Video> lista_OutEnviados;
    list<Video> lista_DeadPerdido;

    list<Video> lista_InDead;
    list<Video> lista_OutDead;

    list<Video> lista_conexao;

    std::unordered_set<int> vencidosSet;  // guarda idObj já marcado como vencido
    std::list<Video> vencidosIds;

    BoosterHandle booster{nullptr};
    bool mlEnabled = true;
    double mlThr = 0.20; // você pode começar com 0.20 (pela sua tabela de thresholds)

    // objetos

    int numObj = 0;
    list<Objeto> objetos;

    double capacidadeMaxEnvio = 0;

    int prioridadeEnviar = 0;
    int totalPacotes = 0;

    // state
    int sendingState = 0;

    UdpSocket socket;
    cMessage *selfMsg = nullptr;
    cMessage *gerarVideoMsg = nullptr;
    cMessage *enviarProximoVideoMsg = nullptr;

    simtime_t interval;
    cPar *sendInterval = nullptr;

    std::string server1AddrStr;
    std::string server2AddrStr;

    L3Address destinationAddress;

    inet::L3Address server1Address; // NOVA VARIAVEL
    inet::L3Address server2Address; // NOVA VARIAVEL

    inet::L3Address resolveServerAddress(const char *serverName);


    string antenaEnvio;

    simtime_t inicioJanela;
    simtime_t duracaoJanela;

    std::list<Video> listaAtual;
    std::list<Video> listaProxima;

    double T_max;          // segundos
    double alphaTx;        // opcional (>=1.0)
    int minSamplesTx;       // mínimo de amostras
    int maxSamplesTx;     // para não crescer infinito
    std::vector<double> txSamples;

    void updateTmaxTx();

    int localPort;
    int destinationPort;
    int primeiraRequisicao = 0;
    cPar *packetLen = nullptr;
    typedef std::map<long int, VideoStreamData> VideoStreamMap;
//    long videoSize = 0;
    bool naArea = false;
    double tempoInicial = -1;

    long totalDadosEnviar = 0;

    VideoStreamMap streams;

    unsigned int numStreams = 0;    // number of video streams served
    unsigned long numPkSent = 0;    // total number of packets sent
    static simsignal_t reqStreamBytesSignal;    // length of video streams served


  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void handleMessageWhenUp(cMessage *msg) override;

    // O carro irá enviar os pacotes de vídeo
    // Para o carro fazer o envio, ele deve fazer a criação dos pacotes de vídeo e fazer o envio para o servidor
    // Para isso, verificar as coisas que estão no servidor da aplicação original
    virtual void sendVideoPacket(cMessage *timer);
    virtual void processStream(Packet *msg);

    double percentile(std::vector<double>& v, double p);




    void updateTAging();
    void onObjectSent(Video v);
//    virtual void pingServer();
    virtual void conexaoServer(short tosEscolhido = -2);
//    cMessage* createVideoStreamReqPacket();
//    SateliteMessage* createVideoStreamReqPacket();
    Packet* createVideoStreamReqPacket(short tosEscolhido, double dist, double vel, double tempoEntrada);

    virtual void clearStreams();

    // ApplicationBase:
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    BoosterHandle xgbBooster = nullptr;
    std::string model_path;
    //double TaxaDeTransmissao(int ueMacId);

  private:
    int quantidadeTempoDeVida(int prioridadeArquivo);
    virtual void gerarVideo();
    void addToListInOrder(list<Video> *list, Video nv);
    void atualizaListaMaiorPrioridade();
    virtual list<Objeto> identfyObjects(int numObj, double videoSize);

    Video getVideoMaiorPrioridade(list<Video> lista);
    Video getVideoMaiorPrioridade(list<Video> lista, double capacidadeMax);


    //double calculateMLScore(VideoObject* obj);

    void defineProximoVideoDead();
    void defineProximoVideo(double capacidadeMax);

    void rotinaDistanciaAntena();
    const char* distanciaAntena(const char *tipoAntena, const char *servidorEscolhido, int tos, int distanciaMaximaAntena, double *dist, double *vel);

    long calculaTotalDadosSize();
    long calculaTotalDadosSize(int prioridade);



    //WRITE FILE
    ostringstream file_criacao;
    ostringstream file_conexao;
    ostringstream file_final;
    ostringstream file_ObjEnviados;
    ostringstream file_cloud;
    ostringstream file_selecao;

    ofstream log_criacao;
    ofstream log_conexao;
    ofstream log_final;
    ofstream log_ObjEnviados;
    ofstream log_cloud;
    ofstream log_selecao;

    void escreveLogCriacao(dataStruct* ds);
    void escreveLogConexao(dataStruct* ds);
    void escreveLogFinal(dataStruct* ds);
    void escreveLogObjEnviados(dataStruct *ds);
    void escreveLogCloud(dataStruct* ds);

    void escreveLogSelecao(dataStruct* ds);


    //BoosterHandle h_booster;
};

} // namespace inet

#endif // ifndef __INET_UDPVIDEOSTREAMCLI_H
