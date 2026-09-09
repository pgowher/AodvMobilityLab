#ifndef __AODVMOBILITYLAB_MANETMETRICS_H
#define __AODVMOBILITYLAB_MANETMETRICS_H

#include <omnetpp.h>

class ManetMetrics : public omnetpp::cSimpleModule, public omnetpp::cListener
{
  protected:
    omnetpp::simsignal_t pingTxSignal;
    omnetpp::simsignal_t pingRxSignal;
    omnetpp::simsignal_t rttSignal;
    omnetpp::simsignal_t packetToUpperSignal;
    omnetpp::simsignal_t destinationDelaySignal;
    omnetpp::simsignal_t destinationHopCountSignal;

    long sent = 0;
    long repliesAtSource = 0;
    long requestsAtDestination = 0;
    int initialHopLimit = 32;
    omnetpp::simtime_t previousDestinationArrival = -1;
    omnetpp::simtime_t longestReceptionGap = 0;
    omnetpp::cStdDev destinationDelay;
    omnetpp::cStdDev hopCount;
    omnetpp::cStdDev roundTripTime;

  protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID,
                               omnetpp::intval_t value, omnetpp::cObject *details) override;
    virtual void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID,
                               const omnetpp::SimTime& value, omnetpp::cObject *details) override;
    virtual void receiveSignal(omnetpp::cComponent *source, omnetpp::simsignal_t signalID,
                               omnetpp::cObject *value, omnetpp::cObject *details) override;
};

#endif
