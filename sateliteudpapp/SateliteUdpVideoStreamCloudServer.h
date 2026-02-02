#ifndef INET_APPLICATIONS_SATELITEUDPAPP_SATELITEUDPVIDEOSTREAMCLOUDSERVER_H_
#define INET_APPLICATIONS_SATELITEUDPAPP_SATELITEUDPVIDEOSTREAMCLOUDSERVER_H_

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.h"

#include "packet-mariese-cenario/CloudMessage_m.h"
#include "packet-mariese-cenario/SateliteMessage_m.h"

using namespace std;
using namespace inet;

class SateliteUdpVideoStreamCloudServer: public ApplicationBase,
        public UdpSocket::ICallback {
protected:

    cMessage *selfMsg = nullptr;

    // socket
    UdpSocket socket;

    // parameters
    int localPort = -1;
    int serverPort;

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

public:
    SateliteUdpVideoStreamCloudServer() {
    }
    virtual ~SateliteUdpVideoStreamCloudServer();

};


#endif /* INET_APPLICATIONS_SATELITEUDPAPP_SATELITEUDPVIDEOSTREAMCLOUDSERVER_H_ */
