#include "AcceptedHopCanvasVisualizer.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/common/packet/Packet.h"

using namespace omnetpp;
using namespace inet;

Define_Module(AcceptedHopCanvasVisualizer);

void AcceptedHopCanvasVisualizer::subscribe()
{
    // Deliberately use two different observation boundaries:
    //  - sentToLower identifies the MAC that performs this transmission;
    //  - sentToUpper identifies a MAC that accepted the received frame.
    // This produces physical one-hop arrows without drawing overhearing radios.
    visualizationSubjectModule->subscribe(packetSentToLowerSignal, this);
    visualizationSubjectModule->subscribe(packetSentToUpperSignal, this);
}

void AcceptedHopCanvasVisualizer::unsubscribe()
{
    auto subject = findModuleFromPar<cModule>(par("visualizationSubjectModule"), this);
    if (subject != nullptr) {
        subject->unsubscribe(packetSentToLowerSignal, this);
        subject->unsubscribe(packetSentToUpperSignal, this);
    }
}

void AcceptedHopCanvasVisualizer::receiveSignal(cComponent *source,
        simsignal_t signal, cObject *object, cObject *details)
{
    Enter_Method("%s", cComponent::getSignalName(signal));

    auto module = check_and_cast<cModule *>(source);
    auto packet = check_and_cast<Packet *>(object);

    if (signal == packetSentToLowerSignal) {
        if (!isLinkStart(module))
            return;

        // A forwarded packet may preserve chunk identity. Always replace the
        // previous transmitter here so the next arrow starts at this hop.
        mapChunks(packet->peekAt(b(0), packet->getTotalLength()),
                [&] (const Ptr<const Chunk>& chunk, int id) {
                    if (getLastModule(id) != nullptr)
                        removeLastModule(id);
                });

        auto node = getContainingNode(module);
        auto networkInterface = getContainingNicModule(module);
        if (nodeFilter.matches(node) && interfaceFilter.matches(networkInterface)
                && packetFilter.matches(packet)) {
            mapChunks(packet->peekAt(b(0), packet->getTotalLength()),
                    [&] (const Ptr<const Chunk>& chunk, int id) {
                        setLastModule(id, module);
                    });
        }
    }
    else if (signal == packetSentToUpperSignal) {
        if (!isLinkEnd(module))
            return;

        auto receivingNode = getContainingNode(module);
        auto networkInterface = getContainingNicModule(module);
        if (nodeFilter.matches(receivingNode)
                && interfaceFilter.matches(networkInterface)
                && packetFilter.matches(packet)) {
            mapChunks(packet->peekAt(b(0), packet->getTotalLength()),
                    [&] (const Ptr<const Chunk>& chunk, int id) {
                        auto transmittingMac = getLastModule(id);
                        if (transmittingMac != nullptr) {
                            auto transmittingNode = getContainingNode(transmittingMac);
                            if (transmittingNode != receivingNode)
                                updateLinkVisualization(transmittingNode, receivingNode, packet);
                        }
                    });
        }
    }
}
