#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Vector.h"

vector DAX_load_data(char *filename) {
	int lines;
	struct timespec tstart, tend;
	vector valuesVec;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	valuesVec = vector_load_from_file(filename, INTEGER, sizeof(int));
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to load %d integers from text file using DAX library\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) -
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec), getVectorLength(valuesVec));

	return valuesVec;
}

int * regular_load_data(char *filename, int lines) {
	int *values, i;
	struct timespec tstart, tend;
	FILE *fd;

	clock_gettime(CLOCK_HIGHRES, &tstart);
	fd = fopen(filename, "r");
	values = malloc(lines * sizeof(int));
	for (i=0; i<lines; i++) {
		fscanf(fd, "%d", &values[i]);
	}
	fclose(fd);
	clock_gettime(CLOCK_HIGHRES, &tend);
	fprintf(stderr, "%.6f seconds to load %d integers from text file using fscanf()\n",
		(tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
		(tstart.tv_sec + 1.0e-9*tstart.tv_nsec), lines);

	return values;
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
	char *filename;
	int *values, *results, numcount, low, high, rescount;
	vector valuesVec, bitVec, resultsVec;

	if (argc != 4) {
		printf("Need to specify file name and range, like \"a.out file.txt -1000 2000\"\n");
		exit(1);
	}

	// convert args
	filename = argv[1];
	low = atoi(argv[2]);
	high = atoi(argv[3]);

	// fill DAX vector using text file
	valuesVec = DAX_load_data(filename);

	// fill int array using text file
	numcount = getVectorLength(valuesVec); // easy way to get number of lines in file
	values = regular_load_data(filename, numcount);

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
