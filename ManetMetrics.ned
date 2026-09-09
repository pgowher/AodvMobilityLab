simple ManetMetrics
{
    parameters:
        int initialHopLimit = default(32);
        @display("i=block/cogwheel;p=42,315;is=s");
        @signal[destinationDelay](type=simtime_t);
        @signal[destinationHopCount](type=long);
        @statistic[destinationDelay](title="one-way delay at destination"; unit=s; record=vector,histogram; interpolationmode=none);
        @statistic[destinationHopCount](title="number of hops to destination"; record=vector,histogram; interpolationmode=none);
}
