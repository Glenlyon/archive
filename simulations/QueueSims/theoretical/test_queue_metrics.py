"""
Tests for queue performance metric calculations.
"""

import unittest

from queue_metrics import (QueueMetricCalculator, MM1MetricCalculator,
                           MG1MetricCalculator)


class QueueMetricCalculatorTest(unittest.TestCase):
    def test_arrival_rate(self):
        calculator = QueueMetricCalculator(0.5, 2)
        self.assertEqual(calculator.arrival_rate, 1)


class MM1MetricCalculatorTest(unittest.TestCase):
    def setUp(self):
        self.calculator = MM1MetricCalculator(2.0 / 3, 3)

    def test_average_customers_in_system(self):
        self.assertAlmostEqual(self.calculator.average_customers_in_system(),
                               2.0)

    def test_average_customers_in_queue(self):
        self.assertAlmostEqual(self.calculator.average_customers_in_queue(),
                               4.0 / 3)

    def test_average_time_in_system(self):
        self.assertAlmostEqual(self.calculator.average_time_in_system(),
                               1.0)

    def test_average_time_in_queue(self):
        self.assertAlmostEqual(self.calculator.average_time_in_queue(),
                               2.0 / 3)

    def test_state_probability(self):
        self.assertAlmostEqual(self.calculator.state_probability(0), 1.0 / 3)
        self.assertAlmostEqual(self.calculator.state_probability(1), 2.0 / 9)
        self.assertAlmostEqual(self.calculator.state_probability(2), 4.0 / 27)
        self.assertAlmostEqual(self.calculator.state_probability(3), 8.0 / 81)


class MG1MetricCalculatorTest(unittest.TestCase):
    def setUp(self):
        self.calculator = MG1MetricCalculator(0.75, 2, 1.0 / 9)

    def test_average_customers_in_system(self):
        self.assertAlmostEqual(self.calculator.average_customers_in_system(),
                               2.375)

    def test_average_customers_in_queue(self):
        self.assertAlmostEqual(self.calculator.average_customers_in_queue(),
                               1.625)

    def test_average_time_in_system(self):
        self.assertAlmostEqual(self.calculator.average_time_in_system(),
                               1.583333333)

    def test_average_time_in_queue(self):
        self.assertAlmostEqual(self.calculator.average_time_in_queue(),
                               1.083333333)


if __name__ == '__main__':
    unittest.main()
