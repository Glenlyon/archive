"""
Reads system state information and creates a comparative histogram with
the theoretical values.
"""

import collections
import sys

import matplotlib.pyplot as plt
import numpy as np

from queue_metrics import MM1MetricCalculator


def parse_states(filename):
    counter = collections.defaultdict(int)

    first_line = True
    with open(filename, "rb") as filehandle:
        for line in filehandle:
            if first_line:
                # ignore header
                first_line = False
                continue

            num_customers = int(line.strip().split(",")[1])
            counter[num_customers] += 1

    # scale down to percentages instead of raw counts
    total = sum(counter.values())
    for state, count in counter.iteritems():
        counter[state] = float(count) / total

    return counter


def create_chart(simulated_states, theoretical_states):
    # +1 because want to start at 0 and include max value
    ind = np.arange(max(simulated_states.keys()) + 1)

    def convert_to_array(state_dict):
        arr = np.zeros(len(ind))
        for state, value in state_dict.iteritems():
            arr[state] = value

        return arr

    simulated_values = convert_to_array(simulated_states)
    theoretical_values = convert_to_array(theoretical_states)

    width = 0.35

    fig, ax = plt.subplots()
    rects1 = ax.bar(ind, simulated_values, width, color="r")
    rects2 = ax.bar(ind + width, theoretical_values, width, color="y")

    ax.set_xticks(ind + width)
    ax.set_xticklabels(ind)

    ax.legend((rects1[0], rects2[0]), ("Simulated", "Theoretical"))

    plt.show()


def main():
    if len(sys.argv) != 3:
        print "Usage: python %s <state_file> <utilization>" % sys.argv[0]
        sys.exit(1)

    simulated_states = parse_states(sys.argv[1])
    calculator = MM1MetricCalculator(float(sys.argv[2]), 1.0)

    theoretical_states = {}
    for state in simulated_states:
        theoretical_states[state] = calculator.state_probability(state)

    create_chart(simulated_states, theoretical_states)


if __name__ == "__main__":
    main()
