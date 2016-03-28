#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Vector.h"

int * gen_array(int size) {
	int num, *array, i;

//      srand(time(NULL));
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

int * regular_in_range(int *values, int numcount, int low, int high) {
	int *results, n, i;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	results = malloc(numcount * sizeof(int));
	n=0;
	for (i=0; i<numcount; i++) {
		if (values[i] >= low && values[i] <= high) {
			results[n] = values[i];
			n++;
		}
	}
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to find numbers between %d and %d using C-cycle. There are %d such numbers\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec), low, high, n);

	return results;
}

int DAX_display_results(vector resultsVec) {
	int i;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
//	for (i=0; i<bit_vector_count(bitVec); i++) {
//		printf("%d\n", getElement(resultsVec, i));
//	}
	vector_display(resultsVec);
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to display results using DAX.\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
	vector_destroy(resultsVec);

	return 0;
}

int regular_display_results(int *results, int rescount) {
	int i;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	for (i=0; i<rescount; i++) {
		printf("%d\n", results[i]);
	}
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to display results from array.\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

	return 0;
}

vector DAX_get_results_vector(vector valuesVec, vector bitVec) {
	vector resultsVec;
	struct timespec tstart, tend;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	resultsVec = vector_extract(valuesVec, bitVec);
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to get results as new vector using DAX.\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

	return resultsVec;
}

int main(int argc, char **argv) {
	int *values, *results, numcount, low, high, rescount;
	vector valuesVec, bitVec, resultsVec;

	if (argc != 4) {
		printf("Need to specify range and array size, like \"a.out -1000 2000 1000000\"\n");
		exit(1);
	}

	// convert args
	low = atoi(argv[1]);
	high = atoi(argv[2]);
	numcount = atoi(argv[3]);

	// generate array of random integers
	values = gen_array(numcount);

	// fill DAX vector using generated array
	valuesVec = DAX_load_data(values, numcount);

	// get DAX bit vector of values in specified range
	bitVec = DAX_in_range(valuesVec, low, high);

	// get actual DAX results as new vector
	resultsVec = DAX_get_results_vector(valuesVec, bitVec);

	// get int array with values in specified range
	results = regular_in_range(values, numcount, low, high);

	// extract results using bit vector and print them
//	DAX_display_results(resultsVec);

	// print results from given int array
//	rescount = bit_vector_count(bitVec); // easy way to get results count
//	regular_display_results(results, rescount);

	return 0;
}
