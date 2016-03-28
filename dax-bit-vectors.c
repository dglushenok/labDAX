#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Vector.h"

int * gen_array(int size) {
	int num, *array, i;

//	srand(time(NULL));
	srand(42);

	array = malloc(size * sizeof(int));
	if (array == NULL) {
		perror("Error allocating memory for initial array: ");
		exit(1);
	}

	for (i=0;i<size;i++) {
		num = rand() * 2; // multiply by 2 to get negative values in array too
		array[i] = (num << 16) | rand();
	}

	return array;
}

vector DAX_load_data(int *array, int size) {
	int lines;
	struct timespec tstart, tend;
	vector valuesVec;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	valuesVec = vector_load_from_array(array, size, INTEGER, sizeof(int));
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to load %d integers from array using DAX library\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec), getVectorLength(valuesVec));

	return valuesVec;
}

vector DAX_in_range(vector valuesVec, int low, int high) {
	vector bitVec;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	bitVec = vector_in_range(valuesVec, &low, &high);
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to find numbers between %d and %d using DAX. There are %d such numbers\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec), low, high, bit_vector_count(bitVec));

	return bitVec;
}

long * regular_in_range(int *values, int numcount, int low, int high) {
	int i;
	long *bitmap;
	struct timespec tstart, tend;

	bitmap = calloc((numcount + sizeof(long) - 1) / sizeof(long), sizeof(long));

	clock_gettime(CLOCK_HIGHRES, &tstart);
	for (i=0; i<numcount; i++) {
		if (values[i] >= low && values[i] <= high) {
			bitmap[i / sizeof(long)] |= (1 << (i % sizeof(long)));
		}
	}
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to make bitmap of values between %d and %d using C-cycle.\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec), low, high);

	return bitmap;
}

vector DAX_bit_vectors_and(vector bitVec1, vector bitVec2) {
	vector resultsBitVec;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	resultsBitVec = bit_vector_and2(bitVec1, bitVec2);
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to join two bit-vectors using DAX.\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

	return resultsBitVec;
}

long * regular_bitmaps_and(long *regularBitMap1, long *regularBitMap2, int numcount) {
	int i;
	long *resultsRegularBitMap;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	resultsRegularBitMap = calloc((numcount + sizeof(long) - 1) / sizeof(long), sizeof(long));

	for (i=0; i<(numcount + sizeof(long) - 1) / sizeof(long); i++) {
		resultsRegularBitMap[i] = regularBitMap1[i] & regularBitMap2[i];
	}
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to join two bit-vectors using regular &.\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

	return resultsRegularBitMap;

}

int bitmap_1bit_count(long *bitmap, int numcount) {
	int i, n=0;

	for (i=0; i<(numcount + sizeof(long) - 1) / sizeof(long); i++) {
		while(bitmap[i] > 0) {
			if ((bitmap[i] & 1) == 1) {
				n++;
			}
			bitmap[i] >>= 1;
		}
	}

	return n;
}

int main(int argc, char **argv) {
	int *values1, *values2, numcount, low, high, rescount, i, n=0;
	long *regularBitMap1, *regularBitMap2, *resultsRegularBitMap;
	vector valuesVec1, valuesVec2, bitVec1, bitVec2, resultsBitVec;

	if (argc != 4) {
		printf("Need to specify range and array size, like \"a.out -1000 2000 1000000\"\n");
		exit(1);
	}

	// convert args
	low = atoi(argv[1]);
	high = atoi(argv[2]);
	numcount = atoi(argv[3]);

	// generate two arrays of random integers
	values1 = gen_array(numcount);
	values2 = gen_array(numcount);

	// fill DAX vectors using generated arrays
	valuesVec1 = DAX_load_data(values1, numcount);
	valuesVec2 = DAX_load_data(values2, numcount);

	// get DAX bit vectors of values in specified range
	bitVec1 = DAX_in_range(valuesVec1, low, high);
	bitVec2 = DAX_in_range(valuesVec2, low, high);

	// get actual DAX results as new vector
	resultsBitVec = DAX_bit_vectors_and(bitVec1, bitVec2);

	// get int array with values in specified range
	regularBitMap1 = regular_in_range(values1, numcount, low, high);
	regularBitMap2 = regular_in_range(values2, numcount, low, high);

	// get actual results using regular bitmaps
	resultsRegularBitMap = regular_bitmaps_and(regularBitMap1, regularBitMap2, numcount);

	// count DAX bits
	printf("DAX has found %d such numbers\n", bit_vector_count(resultsBitVec));

	// count C bits
	printf("Regular C has found %d such numbers\n", bitmap_1bit_count(resultsRegularBitMap, numcount));

	return 0;
}
