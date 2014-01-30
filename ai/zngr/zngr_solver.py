"""
Solves the ZNGR puzzle, for example see
http://www.brainbashers.com/showanswer.asp?ref=ZNGR

CSC 421 Assignment 1 Question 4
David Rusk
"""

import argparse
import csv


def find_maximal_path(filename):
    grid = load_grid(filename)
    solver = PuzzleSolver(grid)
    return solver.find_maximal_path()


def load_grid(filename):
    grid = []

    with open(filename) as filehandle:
        reader = csv.reader(filehandle)
        for row in reader:
            grid.append(map(int, row))

    return grid


class PuzzleSolver(object):
    def __init__(self, grid):
        self.grid = grid
        self.num_rows = len(grid)
        self.num_columns = len(grid[0])

        self.finished_paths = []

    def find_maximal_path(self):
        self.finished_paths = []

        # Start at bottom left
        self.recursive_search(self.num_rows - 1, 0, Path())

        return max(self.finished_paths, key=lambda path: path.value)

    def has_right(self, column):
        return column < self.num_columns - 1

    def has_up(self, row):
        return row > 0

    def recursive_search(self, row, column, path):
        path.increment(self.grid[row][column])

        right_path, up_path = path.split()

        made_move = False

        if self.has_right(column):
            made_move = True
            right_path.move_right()
            self.recursive_search(row, column + 1, right_path)

        if self.has_up(row):
            made_move = True
            up_path.move_up()
            self.recursive_search(row - 1, column, up_path)

        if not made_move:
            assert row == 0 and column == self.num_columns - 1
            self.finished_paths.append(path)


class Path(object):
    def __init__(self, value=0, steps=""):
        self.steps = steps
        self.value = value

    def move_up(self):
        self.steps += "u"

    def move_right(self):
        self.steps += "r"

    def increment(self, value):
        self.value += value

    def split(self):
        return self, Path(value=self.value, steps=self.steps)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("grid_file", help="CSV file describing puzzle grid")

    args = parser.parse_args()

    path = find_maximal_path(args.grid_file)

    print "Maximal path:\n Value = %d\n Steps: %s" % (path.value, path.steps)


if __name__ == "__main__":
    main()
