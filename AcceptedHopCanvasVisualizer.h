#ifndef __AODVMOBILITYLAB_ACCEPTEDHOPCANVASVISUALIZER_H
#define __AODVMOBILITYLAB_ACCEPTEDHOPCANVASVISUALIZER_H

#include "inet/visualizer/canvas/linklayer/DataLinkCanvasVisualizer.h"

class AcceptedHopCanvasVisualizer : public inet::visualizer::DataLinkCanvasVisualizer
{
  protected:
    virtual void subscribe() override;
    virtual void unsubscribe() override;

  public:
    virtual void receiveSignal(omnetpp::cComponent *source,
            omnetpp::simsignal_t signal, omnetpp::cObject *object,
            omnetpp::cObject *details) override;
};

#endif
