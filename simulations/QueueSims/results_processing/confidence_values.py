"""
A quick script where sample data can be just pasted in to calculate
the confidence interval.
"""

from confidence import mean_confidence_interval


def get_data():
    data = """
95.996131834
95.9219521225
95.534866072
109.0473648195
105.7706293973
90.0777207581
102.2625276454
81.8437166559
83.8528921808
115.8893974067
    """
    return [float(num) for num in data.split()]


def main():
    _, lower, upper = mean_confidence_interval(get_data())
    print "(%.14f, %.14f)" % (lower, upper)


if __name__ == "__main__":
    main()
