"""
Calculates theoretical values of queue performance metrics.
"""

from __future__ import division


class QueueMetricCalculator(object):
    """
    Base class for calculating queue metrics.
    """

    def __init__(self, utilization, service_rate):
        self.arrival_rate = utilization * service_rate
        self.service_rate = service_rate
        self.utilization = utilization

    def average_customers_in_system(self):
        """
        The long-run time-average number of customers in the system.
        """
        raise NotImplementedError()

    def average_customers_in_queue(self):
        """
        The long-run time-average number of customers in the queue.
        """
        raise NotImplementedError()

    def average_time_in_system(self):
        """
        The long-run average time spent in the system per customer.
        """
        raise NotImplementedError()

    def average_time_in_queue(self):
        """
        The long-run average time spent in the queue per customer.
        """
        raise NotImplementedError()

    def print_report(self):
        print "Arrival rate: %f" % self.arrival_rate
        print "Serviced rate: %f" % self.service_rate
        print "Utilization: %f" % self.utilization
        print "Average customers in system: %f" % self.average_customers_in_system()
        print "Average customers in queue: %f" % self.average_customers_in_queue()
        print "Average time in system: %f" % self.average_time_in_system()
        print "Average time in queue: %f" % self.average_time_in_queue()

    def state_probability(self, num_customers):
        """
        The probability of the system having the specified number of
        customers.
        """
        raise NotImplementedError()


class MM1MetricCalculator(QueueMetricCalculator):
    """
    Calculates M/M/1 queue metrics.
    """

    def average_customers_in_system(self):
        return self.utilization / (1 - self.utilization)

    def average_customers_in_queue(self):
        return self.utilization ** 2 / (1 - self.utilization)

    def average_time_in_system(self):
        return 1 / (self.service_rate - self.arrival_rate)

    def average_time_in_queue(self):
        return self.utilization / (self.service_rate * (1 - self.utilization))

    def state_probability(self, num_customers):
        return (1 - self.utilization) * self.utilization ** num_customers


class MG1MetricCalculator(QueueMetricCalculator):
    """
    Calculates M/G/1 queue metrics.
    """

    def __init__(self, utilization, service_rate, variance):
        super(MG1MetricCalculator, self).__init__(utilization, service_rate)
        self.variance = variance

    def average_customers_in_system(self):
        return self.average_customers_in_queue() + self.utilization

    def average_customers_in_queue(self):
        return ((self.utilization ** 2) *
                (1 + self.variance * self.service_rate ** 2) /
                (2 * (1 - self.utilization)))

    def average_time_in_system(self):
        return self.average_time_in_queue() + 1 / self.service_rate

    def average_time_in_queue(self):
        return (self.arrival_rate *
                (1 / self.service_rate ** 2 + self.variance) /
                (2 * (1 - self.utilization)))


class PriorityQueueMetricCalculator(QueueMetricCalculator):
    def __init__(self, utilization, service_rate, num_queues):
        super(PriorityQueueMetricCalculator, self).__init__(utilization, service_rate)
        self.num_queues = num_queues

        # Calculate "per queue" metrics.  All priorities have equal
        # probabilities.
        self.queue_utilization = utilization / num_queues
        self.queue_service_rate = service_rate / num_queues
        self.queue_arrival_rate = self.arrival_rate / num_queues

