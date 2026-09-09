import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.inet.ManetRouter;
import inet.physicallayer.wireless.common.medium.UnitDiskRadioMedium;
import inet.visualizer.canvas.integrated.IntegratedMultiCanvasVisualizer;

// Ten-node MANET arranged as a short central path plus a longer upper bypass.
// Relay host[2] moves away at 20 s, breaking the central path. The protocol
// configurations demonstrate either AODV discovery or DSDV reconvergence.
network AodvMobilityNetwork
{
    parameters:
        int numHosts = default(10);
        @display("bgb=1100,720;bgi=background/grid");
        @figure[topologyNote](type=text; pos=550,53; anchor=c; font=Arial,11; color=#424242; text="Radio range = 250 m; SOURCE-to-DESTINATION distance = 880 m, so every delivered packet needs multiple hops");
    submodules:
        animationController: AnimationController {
            @display("p=782,90;is=s");
        }
        radioMedium: UnitDiskRadioMedium {
            @display("p=106,96;is=s");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=245,90;is=s");
        }
        routingTableRecorder: RoutingTableRecorder {
            @display("p=436,90;is=s");
        }
        visualizer: IntegratedMultiCanvasVisualizer {
            @display("p=610,90;is=s");
        }
        metrics: ManetMetrics {
            @display("p=966,96;is=s");
        }
        host[numHosts]: ManetRouter {
            @display("i=device/wifilaptop");
        }
    connections allowunconnected:
}

// Protocol-specific legends are kept separate so AODV and DSDV concepts are
// never conflated on the canvas.
network AodvProtocolNetwork extends AodvMobilityNetwork
{
    parameters:
        @figure[title](type=text; pos=550,25; anchor=c; font=Arial,20,bold; color=#17365D; text="AODV: on-demand discovery, link break, and route repair");
        @figure[legendTitle](type=text; pos=36,610; font=Arial,13,bold; text="AODV legend");
        @figure[rreq](type=line; points=38,638,83,638; lineColor=#0000FF; lineWidth=4);
        @figure[rreqText](type=text; pos=92,638; anchor=w; text="RREQ: one-hop reception; rebroadcasts form the flood");
        @figure[rrep](type=line; points=430,638,475,638; lineColor=#008000; lineWidth=4);
        @figure[rrepText](type=text; pos=484,638; anchor=w; text="RREP: one-hop unicast toward source");
        @figure[rerr](type=line; points=770,638,815,638; lineColor=#FF0000; lineWidth=4);
        @figure[rerrText](type=text; pos=824,638; anchor=w; text="RERR: broken active route");
        @figure[data](type=line; points=38,678,83,678; lineColor=#000000; lineWidth=5);
        @figure[dataText](type=text; pos=92,678; anchor=w; text="Black: ping forwarding hops and established multi-hop route");
        @figure[mobility](type=line; points=500,678,545,678; lineColor=#DAA520; lineWidth=3);
        @figure[mobilityText](type=text; pos=554,678; anchor=w; text="Gold: MOVING RELAY trail/velocity");
}

network DsdvProtocolNetwork extends AodvMobilityNetwork
{
    parameters:
        @figure[title](type=text; pos=550,25; anchor=c; font=Arial,20,bold; color=#17365D; text="DSDV: proactive table updates, route expiry, and reconvergence");
        @figure[implementationNote](type=text; pos=550,580; anchor=c; font=Arial,10; color=#555555; text="All nodes still update periodically; orange highlights only DESTINATION-origin information so its hop-by-hop propagation is readable");
        @figure[legendTitle](type=text; pos=36,610; font=Arial,13,bold; text="DSDV legend");
        @figure[hello](type=line; points=38,638,83,638; lineColor=#FF8C00; lineWidth=4);
        @figure[helloText](type=text; pos=92,638; anchor=w; text="DEST UPDATE: accepted one-hop advertisement/rebroadcast");
        @figure[drop](type=line; points=885,638,930,638; lineColor=#FF0000; lineWidth=4);
        @figure[dropText](type=text; pos=939,638; anchor=w; text="Genuine ping drop");
        @figure[data](type=line; points=38,678,83,678; lineColor=#000000; lineWidth=5);
        @figure[dataText](type=text; pos=92,678; anchor=w; text="Black: latest forward ping path; old path clears before next ping");
        @figure[mobility](type=line; points=500,678,545,678; lineColor=#DAA520; lineWidth=3);
        @figure[mobilityText](type=text; pos=554,678; anchor=w; text="Gold: relay movement; tables expire/reconverge without route requests");
}
