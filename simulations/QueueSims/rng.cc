#include <cstdlib>
#include <fstream>

#include "cmersennetwister.h"

using namespace std;

void writeToFile(const char *filename, double *numbers, int length) {
    ofstream outputFile;
    outputFile.open(filename, ofstream::out);

    int i;
    for (i = 0; i < length; i++) {
        outputFile << numbers[i] << endl;
    }

    outputFile.close();
}

int main() {
    int quantity = 1000000;
    double *numbers = (double *) malloc(sizeof(double) * quantity);

    cMersenneTwister rng;

    int i;
    for (i = 0; i < quantity; i++) {
        numbers[i] = rng.doubleRand();
    }

    writeToFile("random_numbers", numbers, quantity);

    free(numbers);

    return 0;
}
