//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef INET_APPLICATIONS_SATELITEUDPAPP_SATELITEUDPCLOUDSERVER_H_
#define INET_APPLICATIONS_SATELITEUDPAPP_SATELITEUDPCLOUDSERVER_H_

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.h"

#include "./packet-mariese-cenario/SateliteMessage_m.h"
#include "./packet-mariese-cenario/CloudMessage_m.h"

namespace inet {

class SateliteUdpCloudServer: public ApplicationBase,
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
    SateliteUdpCloudServer() {
    }
    virtual ~SateliteUdpCloudServer();

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_SATELITEUDPAPP_SATELITEUDPCLOUDSERVER_H_ */
