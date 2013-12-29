package priorityqueue;

import org.omnetpp.queueing.Classifier;
import org.omnetpp.queueing.Delay;
import org.omnetpp.queueing.PassiveQueue;
import org.omnetpp.queueing.Server;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;

network PriorityQueue
{
    parameters:
        @display("i=block/users");
    submodules:
        arrival: Source {
            @display("p=32.0,132.0;i=block/arrival");
        }
        classifier: Classifier {
            @display("p=124.0,132.0");
        }
        highPriority: PassiveQueue {
            @display("p=270.0,39.0");
        }
        mediumPriority: PassiveQueue {
            @display("p=270.0,132.0");
        }
        lowPriority: PassiveQueue {
            @display("p=270.0,225.0");
        }
        server: Server {
            @display("p=448.0,132.0");
        }
        complete: Sink {
            @display("p=546.0,132.0;i=block/departure");
        }
        nullSink: Sink {
            @display("p=124.0,225.0");
        }
    connections:
        arrival.out --> classifier.in++;
        classifier.out++ --> highPriority.in++;
        classifier.out++ --> mediumPriority.in++;
        classifier.out++ --> lowPriority.in++;
        highPriority.out++ --> server.in++;
        mediumPriority.out++ --> server.in++;
        lowPriority.out++ --> server.in++;
        server.out --> complete.in++;
        classifier.rest --> nullSink.in++;
}
