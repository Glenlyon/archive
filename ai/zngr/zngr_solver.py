"""
Solves the ZNGR puzzle, for example see
http://www.brainbashers.com/showanswer.asp?ref=ZNGR

CSC 421 Assignment 1 Question 4
David Rusk
"""

import argparse
import csv


def find_maximal_path(filename):
    """
    Args:
      filename: str
        The input CSV file describing the puzzle grid.

    Returns: Path object
      The maximal path.
    """
    return PuzzleSolver(load_grid(filename)).find_maximal_path()


def load_grid(filename):
    """
    Reads a CSV file to create a 2D list representation of the puzzle grid.

    Example:
      Input:
        1, 2, 3
        4, 5, 6
      Output:
        [[1, 2, 3],
         [4, 5, 6]]
    """
    grid = []

    with open(filename) as filehandle:
        reader = csv.reader(filehandle)
        for row in reader:
            grid.append(map(int, row))

    return grid


class PuzzleSolver(object):
    """
    Solves the ZNGR puzzle by checking all possible paths and choosing the
    one with the highest value.
    """

    def __init__(self, grid):
        self.grid = grid
        self.num_rows = len(grid)
        self.num_columns = len(grid[0])

        self.finished_paths = []

    def find_maximal_path(self):
        """
        Returns: Path object
          The maximal path.
        """
        self.finished_paths = []

        # Start at bottom left
        self._recursive_search(self.num_rows - 1, 0, Path())

        return max(self.finished_paths, key=lambda path: path.value)

    def _has_right(self, column):
        return column < self.num_columns - 1

    def _has_up(self, row):
        return row > 0

    def _recursive_search(self, row, column, path):
        path.increment(self.grid[row][column])

        right_path, up_path = path.split()

        made_move = False

        if self._has_right(column):
            made_move = True
            right_path.move_right()
            self._recursive_search(row, column + 1, right_path)

        if self._has_up(row):
            made_move = True
            up_path.move_up()
            self._recursive_search(row - 1, column, up_path)

        if not made_move:
            assert row == 0 and column == self.num_columns - 1
            self.finished_paths.append(path)


class Path(object):
    """
    Stores the state of a path being searched.

    Properties:
      value: int
        The sum of the values encountered so far on this path.
      steps: str
        A string of characters representing the moves taken on this path.
        Characters can only be one of 'u' for up or 'r' for right.
    """

    def __init__(self, value=0, steps=""):
        self.steps = steps
        self.value = value

    def move_up(self):
        """
        Records an up move in the steps.
        """
        self.steps += "u"

    def move_right(self):
        """
        Records a right move in the steps.
        """
        self.steps += "r"

    def increment(self, value):
        """
        Increments the total value encountered on this path.
        """
        self.value += value

    def split(self):
        """
        Returns two paths: the current one and an exact copy.
        """
        return self, Path(value=self.value, steps=self.steps)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("grid_file", help="CSV file describing puzzle grid")

    args = parser.parse_args()

    path = find_maximal_path(args.grid_file)

    print "Maximal path:\n Value = %d\n Steps: %s" % (path.value, path.steps)


if __name__ == "__main__":
    main()
