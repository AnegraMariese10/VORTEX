#ifndef __INET_SATELITE_SateliteUdpEchoApp_H
#define __INET_SATELITE_SateliteUdpEchoApp_H

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

namespace inet {

/**
 * UDP application. See NED for more info.
 */
class INET_API SateliteUdpEchoApp : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    UdpSocket socket;
    int numEchoed;    // just for WATCH

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
};

} // namespace inet

#endif // ifndef __INET_SATELITE_SateliteUdpEchoApp_H

