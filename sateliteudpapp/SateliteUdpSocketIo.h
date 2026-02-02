#ifndef __INET_SATELITE_UDPSOCKETIO_H
#define __INET_SATELITE_UDPSOCKETIO_H

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

namespace inet {

class INET_API SateliteUdpSocketIo : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    bool dontFragment = false;
    UdpSocket socket;
    int numSent = 0;
    int numReceived = 0;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *message) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void setSocketOptions();

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
};

} // namespace inet

#endif // ifndef __INET_SATELITE_UDPSOCKETIO_H

