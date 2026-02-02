#include "SateliteUdpCloudServer.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/ipv4/Ipv4RoutingTable.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include <iostream>

using namespace std;

namespace inet {


Define_Module(SateliteUdpCloudServer);

void SateliteUdpCloudServer::initialize(int stage) {
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        localPort = par("localPort");
        selfMsg = new cMessage("UDPVideoStreamStart");
    }
}

SateliteUdpCloudServer::~SateliteUdpCloudServer() {
}

void SateliteUdpCloudServer::finish() {
//    ApplicationBase::finish();
}

void SateliteUdpCloudServer::socketDataArrived(UdpSocket *socket,
        Packet *packet) {
    receiveStream(packet);
}

void SateliteUdpCloudServer::handleMessageWhenUp(cMessage *msg) {
    if (msg->isSelfMessage()) {
        cout << "Recebendo mensagem" << endl;
    } else {
        socket.processMessage(msg);
    }
}


void SateliteUdpCloudServer::receiveStream(Packet *msg) {
    cout << "Recebendo stream" << endl;

}

void SateliteUdpCloudServer::requestStream() {
    cout << "Request stream" << endl;
}


void SateliteUdpCloudServer::socketErrorArrived(UdpSocket *socket,
        Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void SateliteUdpCloudServer::socketClosed(UdpSocket *socket) {
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void SateliteUdpCloudServer::handleStartOperation(
        LifecycleOperation *operation) {
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    socket.bind(localPort);

    scheduleAt(simTime().dbl(), selfMsg);

    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket.setTimeToLive(timeToLive);
    int dscp = par("dscp");
    if (dscp != -1)
        socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(-1);

}

void SateliteUdpCloudServer::clearStreams() {
}

void SateliteUdpCloudServer::handleStopOperation(
        LifecycleOperation *operation) {
    clearStreams();
    socket.setCallback(nullptr);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SateliteUdpCloudServer::handleCrashOperation(
        LifecycleOperation *operation) {
    clearStreams();
    if (operation->getRootModule() != getContainingNode(this)) // closes socket when the application crashed only
        socket.destroy(); //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
    socket.setCallback(nullptr);
}

}
