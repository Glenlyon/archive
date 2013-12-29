"""
Performs chi-square test for uniformity.
"""

import math
import sys

import numpy as np


def chi_square(random_numbers, num_classes=10):
    """
    Calculates the chi-square value of the provided random random_numbers
    using the specified number of classes.

    The returned chi-square value can be compared with tabulated critical
    values for a given level of significance.  If the value is less
    than the critical value, then the random numbers match the uniform
    distribution with the given level of significance.

    Args:
      random_numbers: iterable(float)
        The numbers being tested for uniformity.  They should be in the
        range [0.0, 1.0].
      num_classes: int
        The number of classes to split the numbers into.  For example,
        if num_classes=10, the classes will be 0.0-0.1, 0.1-0.2, ...
        up to 0.9-1.0.

    Returns:
      chi_square: float
        The chi-square value to compare with a critical value.
    """
    if len(random_numbers) < num_classes:
        raise ValueError("Cannot have fewer random numbers than classes.")

    # Make sure the numbers are in a numpy array.
    random_numbers = np.asarray(random_numbers)

    # We are expecting a uniform distribution with the same count in each
    # interval.
    step = 1.0 / num_classes
    expected_count = len(random_numbers) / num_classes

    _chi_square = 0
    for i in range(num_classes):
        lower_bound = i * step
        upper_bound = (i + 1) * step

        in_range = random_numbers[
            ((lower_bound <= random_numbers) &
             (random_numbers < upper_bound))]

        _chi_square += (float(np.square(len(in_range) - expected_count)) /
                        expected_count)

    return _chi_square


def main():
    if len(sys.argv) != 2:
        print "Usage: python %s <random_numbers_filename>" % sys.argv[0]
        sys.exit(1)

    with open(sys.argv[1], "rb") as filehandle:
        numbers = [float(num_str) for num_str in
                   filehandle.read().rstrip().split()]

    num_classes = int(round(math.sqrt(len(numbers))))
    print "Number of classes: %d" % num_classes

    print "Chi-square value: %f" % chi_square(numbers, num_classes=num_classes)


if __name__ == "__main__":
    main()
