"""
Calculate the confidence interval of sample data.
"""

import math

import scipy.stats


def sample_mean(numbers):
    return sum(numbers) / len(numbers)


def sample_variance(numbers):
    var = 0
    for number in numbers:
        var += number ** 2

    n = len(numbers)
    var -= n * sample_mean(numbers) ** 2

    return var / (n - 1)


def sample_stddev(numbers):
    return math.sqrt(sample_variance(numbers))


def mean_confidence_interval(data, confidence=0.95):
    """
    Args:
      data:
        The sample data.
      confidence: float

    Returns:
      mean: float
        Mean value of the sample data
      lower: float
        Lower bound of the confidence interval
      upper: float
        Upper bound of the confidence interval
    """
    mean = sample_mean(data)
    stddev = sample_stddev(data)

    n = len(data)
    width = (abs(scipy.stats.t.ppf((1 - confidence) / 2, n - 1)) * stddev /
             math.sqrt(n))

    return mean, mean - width, mean + width
