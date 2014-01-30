import os
import unittest

from hamcrest import assert_that, equal_to, contains

import zngr_solver


def filepath(filename):
    """
    Performs path resolution to the test data file.
    """
    return os.path.join(os.path.dirname(__file__), filename)


class ZNGRPuzzleTest(unittest.TestCase):
    def test_load_grid(self):
        grid = zngr_solver.load_grid(filepath("test_small_grid.csv"))

        assert_that(len(grid), equal_to(2))
        assert_that(grid[0], contains(1, 2, 3))
        assert_that(grid[1], contains(6, 5, 4))

    def test_solver_two_steps(self):
        grid = [[1, 2],
                [3, 4]]

        solver = zngr_solver.PuzzleSolver(grid)
        path = solver.find_maximal_path()

        assert_that(path.steps, equal_to("ru"))
        assert_that(path.value, equal_to(9))

    def test_solver_three_steps(self):
        grid = zngr_solver.load_grid(filepath("test_small_grid.csv"))

        solver = zngr_solver.PuzzleSolver(grid)
        path = solver.find_maximal_path()

        assert_that(path.steps, equal_to("rru"))
        assert_that(path.value, equal_to(18))

    def test_find_maximal_path_full_example(self):
        path = zngr_solver.find_maximal_path(filepath("test_grid.csv"))

        assert_that(path.steps, equal_to("rruuuuururru"))
        assert_that(path.value, equal_to(37))


if __name__ == '__main__':
    unittest.main()
