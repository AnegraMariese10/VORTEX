#ifndef __INET_SATELITE_SateliteUdpSink_H
#define __INET_SATELITE_SateliteUdpSink_H

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

namespace inet {

/**
 * Consumes and prints packets received from the Udp module. See NED for more info.
 */
class INET_API SateliteUdpSink : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    enum SelfMsgKinds { START = 1, STOP };

    UdpSocket socket;
    int localPort = -1;
    L3Address multicastGroup;
    simtime_t startTime;
    simtime_t stopTime;
    cMessage *selfMsg = nullptr;
    int numReceived = 0;

  public:
    SateliteUdpSink() {}
    virtual ~SateliteUdpSink();

  protected:
    virtual void processPacket(Packet *msg);
    virtual void setSocketOptions();

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    virtual void processStart();
    virtual void processStop();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
};

} // namespace inet

#endif // ifndef __INET_SATELITE_SateliteUdpSink_H

