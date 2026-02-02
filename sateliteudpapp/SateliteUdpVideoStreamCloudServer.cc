#include "SateliteUdpVideoStreamCloudServer.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/ipv4/Ipv4RoutingTable.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "./packet-mariese-cenario/ServerConnMessage_m.h"
#include "./packet-mariese-cenario/SateliteMessage_m.h"
#include <iostream>

using namespace std;
//using namespace inet;

Define_Module(SateliteUdpVideoStreamCloudServer);

void SateliteUdpVideoStreamCloudServer::initialize(int stage) {
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        localPort = par("localPort");
        selfMsg = new cMessage("UDPVideoStreamStart");
    }
}

SateliteUdpVideoStreamCloudServer::~SateliteUdpVideoStreamCloudServer() {
}

void SateliteUdpVideoStreamCloudServer::finish() {
//    ApplicationBase::finish();
}

void SateliteUdpVideoStreamCloudServer::socketDataArrived(UdpSocket *socket,
        Packet *packet) {
    cout << "\n" << getParentModule()->getFullName() << " socketDataArrived" << endl;
    receiveStream(packet);
}

void SateliteUdpVideoStreamCloudServer::handleMessageWhenUp(cMessage *msg) {
    cout << "\n" << getParentModule()->getFullName() << " handleMessageWhenUp" << endl;
    if (msg->isSelfMessage()) {
        cout << "Cloud Recebendo mensagem" << endl;
    } else {
        socket.processMessage(msg);
    }
}

void SateliteUdpVideoStreamCloudServer::receiveStream(Packet *msg) {
    cout << "\n" << getParentModule()->getFullName() << " receiveStream" << endl;
    cout << "Cloud está recebendo stream" << endl;

}

void SateliteUdpVideoStreamCloudServer::requestStream() {
    cout << "\n" << getParentModule()->getFullName() << " requestStream" << endl;
    cout << "Cloud Request stream" << endl;
}

void SateliteUdpVideoStreamCloudServer::socketErrorArrived(UdpSocket *socket,
        Indication *indication) {
    cout << "\n" << getParentModule()->getFullName() << " socketErrorArrived" << endl;
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void SateliteUdpVideoStreamCloudServer::socketClosed(UdpSocket *socket) {
    cout << "\n" << getParentModule()->getFullName() << " socketClosed" << endl;

    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void SateliteUdpVideoStreamCloudServer::handleStartOperation(
        LifecycleOperation *operation) {
    cout << "\n" << getParentModule()->getFullName() << " handleStartOperation" << endl;

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

void SateliteUdpVideoStreamCloudServer::clearStreams() {
}

void SateliteUdpVideoStreamCloudServer::handleStopOperation(
        LifecycleOperation *operation) {
    cout << "\n" << getParentModule()->getFullName() << " handleStopOperation" << endl;

    clearStreams();
    socket.setCallback(nullptr);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SateliteUdpVideoStreamCloudServer::handleCrashOperation(
        LifecycleOperation *operation) {
    cout << "\n" << getParentModule()->getFullName() << " handleCrashOperation" << endl;

    clearStreams();
    if (operation->getRootModule() != getContainingNode(this)) // closes socket when the application crashed only
        socket.destroy(); //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
    socket.setCallback(nullptr);
}

