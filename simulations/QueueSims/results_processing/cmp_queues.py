"""
Generates charts to compare performance metrics.
"""

import numpy as np
import matplotlib.pyplot as plt


def barchart(mm1_vals, mg1_vals, title):
    n = len(mm1_vals)
    ind = np.arange(n)
    width = 0.35

    fig, ax = plt.subplots()
    rects1 = ax.bar(ind, mm1_vals, width, color="r")
    rects2 = ax.bar(ind + width, mg1_vals, width, color="y")

    ax.set_title(title)
    ax.set_xticks(ind + width)
    ax.set_xticklabels(["0.1", "0.2", "0.4", "0.6", "0.8"])

    ax.legend((rects1[0], rects2[0]), ("M/M/1", "M/G/1"), loc="upper left")


def main():
    mm1_l = (0.111244776046552, 0.248694933254686, 0.672186120383144,
             1.48966272094734, 3.994616657982)
    mg1_l = (0.107644762189048, 0.232743900858149, 0.577667697597301,
             1.20022768755203, 2.93897101783853)

    barchart(mm1_l, mg1_l, "Number of customers in system (L)")

    mm1_lq = (0.0111149013727673, 0.0496244365413576, 0.27044825918167,
              0.891147354668962, 3.19435533265994)
    mg1_lq = (0.00749361357817366, 0.0332726917450721, 0.177386849445469,
              0.59992475545888, 2.13789787944256)

    barchart(mm1_lq, mg1_lq, "Number of customers in queue (Lq)")

    mm1_w = (1.10969629456344, 1.24532814967966, 1.67814427742609,
             2.48532917455126, 4.99298828043815)
    mg1_w = (1.07358099344256, 1.16539332403288, 1.44378925145272,
             2.00109231577262, 3.67087771700752)

    barchart(mm1_w, mg1_w, "Time spent in system (w)")

    mm1_wq = (0.110873694723256, 0.248481995399075, 0.675184804682167,
              1.48671208230602, 3.99268069673231)
    mg1_wq = (0.0747354671885023, 0.166599842359951, 0.443345827651045,
              1.00017900272793, 2.67024217518531)

    barchart(mm1_wq, mg1_wq, "Time spent in queue (wq)")

    plt.show()


if __name__ == "__main__":
    main()
