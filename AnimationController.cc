#include "AnimationController.h"

using namespace omnetpp;

Define_Module(AnimationController);

void AnimationController::initialize()
{
    // Qtenv's built-in animation draws WirelessSignal/sendDirect delivery
    // from the radio medium to every candidate radio. Those graphics describe
    // simulator internals, not AODV hops, and obscure the INET visualizers.
    // Disable them only on this network's inspector; INET canvas figures are
    // unaffected and continue to show RREQ/RREP/RERR and the selected route.
    getParentModule()->setBuiltinAnimationsAllowed(false);
}
