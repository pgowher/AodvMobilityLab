#include "ManetMetrics.h"

#include <algorithm>
#include <cstring>

#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/HopLimitTag_m.h"

using namespace omnetpp;
using namespace inet;

Define_Module(ManetMetrics);

void ManetMetrics::initialize()
{
    initialHopLimit = par("initialHopLimit");
    pingTxSignal = registerSignal("pingTxSeq");
    pingRxSignal = registerSignal("pingRxSeq");
    rttSignal = registerSignal("rtt");
    packetToUpperSignal = registerSignal("packetSentToUpper");
    destinationDelaySignal = registerSignal("destinationDelay");
    destinationHopCountSignal = registerSignal("destinationHopCount");

    auto network = getParentModule();
    auto sourceApp = network->getSubmodule("host", 0)->getSubmodule("app", 0);
    auto destinationIpv4 = network->getSubmodule("host", 9)->getSubmodule("ipv4");
    sourceApp->subscribe(pingTxSignal, this);
    sourceApp->subscribe(pingRxSignal, this);
    sourceApp->subscribe(rttSignal, this);
    destinationIpv4->subscribe(packetToUpperSignal, this);
}

void ManetMetrics::receiveSignal(cComponent *, simsignal_t signalID, intval_t, cObject *)
{
    if (signalID == pingTxSignal)
        sent++;
    else if (signalID == pingRxSignal)
        repliesAtSource++;
}

void ManetMetrics::receiveSignal(cComponent *, simsignal_t signalID, const SimTime& value, cObject *)
{
    if (signalID == rttSignal)
        roundTripTime.collect(value.dbl());
}

void ManetMetrics::receiveSignal(cComponent *, simsignal_t signalID, cObject *value, cObject *)
{
    if (signalID != packetToUpperSignal)
        return;
    auto packet = dynamic_cast<Packet *>(value);
    if (packet == nullptr || strncmp(packet->getName(), "ping", 4) != 0 || strstr(packet->getName(), "reply") != nullptr)
        return;

    requestsAtDestination++;
    simtime_t delay = simTime() - packet->getCreationTime();
    destinationDelay.collect(delay.dbl());
    emit(destinationDelaySignal, delay);

    auto hopLimitTag = packet->findTag<HopLimitInd>();
    if (hopLimitTag != nullptr) {
        // Routers decrement TTL; add the final transmitter-to-destination link.
        int hops = initialHopLimit - hopLimitTag->getHopLimit() + 1;
        hopCount.collect(hops);
        emit(destinationHopCountSignal, hops);
    }

    if (previousDestinationArrival >= SIMTIME_ZERO)
        longestReceptionGap = std::max(longestReceptionGap, simTime() - previousDestinationArrival);
    previousDestinationArrival = simTime();
}

void ManetMetrics::finish()
{
    long destinationLoss = sent - requestsAtDestination;
    double deliveryRatio = sent == 0 ? 0 : 100.0 * requestsAtDestination / sent;

    recordScalar("ping packets sent by source", sent);
    recordScalar("ping packets received by destination", requestsAtDestination);
    recordScalar("ping replies received by source", repliesAtSource);
    recordScalar("ping packets lost before destination", destinationLoss);
    recordScalar("packet delivery ratio at destination (%)", deliveryRatio);
    recordScalar("average one-way delay at destination (s)", destinationDelay.getCount() ? destinationDelay.getMean() : 0);
    recordScalar("minimum one-way delay at destination (s)", destinationDelay.getCount() ? destinationDelay.getMin() : 0);
    recordScalar("maximum one-way delay at destination (s)", destinationDelay.getCount() ? destinationDelay.getMax() : 0);
    recordScalar("average hops to destination", hopCount.getCount() ? hopCount.getMean() : 0);
    recordScalar("minimum hops to destination", hopCount.getCount() ? hopCount.getMin() : 0);
    recordScalar("maximum hops to destination", hopCount.getCount() ? hopCount.getMax() : 0);
    recordScalar("average round-trip time (s)", roundTripTime.getCount() ? roundTripTime.getMean() : 0);
    recordScalar("longest destination reception gap (s)", longestReceptionGap.dbl());
}
