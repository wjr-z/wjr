#ifndef GENERATE_FILE_H
#define GENERATE_FILE_H


/**************************************
*  Compiler options
**************************************/
#define _CRT_SECURE_NO_WARNINGS   /* Visual warning */


/**************************************
*  Include
**************************************/
#include <stdlib.h>   /* malloc, free */
#include <stdio.h>    /* printf */
#include <string.h>   /* memset */
#include <math.h>     /* log */


/**************************************
*  Constants
**************************************/
#define MB *(1<<20)
static size_t BUFFERSIZE = ((1 MB) - 1);
//#define BUFFERSIZE ((1 MB) - 1)
#define PROBATABLESIZE 4096
#define PRIME1   2654435761U
#define PRIME2   2246822519U


/**************************************
*  Text display
**************************************/
#define DISPLAY(...)         fprintf(stderr, __VA_ARGS__)
#define DISPLAYLEVEL(l, ...) if (displayLevel>=l) { DISPLAY(__VA_ARGS__); }
static int   displayLevel = 2;   /* 0 : no display;   1: errors;   2 : + result + interaction + warnings;   3 : + progression;   4 : + information */


/**************************************
*  Local variables
**************************************/
static char* g_programName;


/**************************************
*  Local functions
**************************************/
static unsigned int GEN_rand(unsigned int* seed) {
    *seed = ((*seed) * PRIME1) + PRIME2;
    return (*seed) >> 11;
}

static int usage(void) {
    DISPLAY("Usage :\n");
    DISPLAY("%s P%%\n", g_programName);
    DISPLAY("Exemple :\n");
    DISPLAY("%s 70%%\n", g_programName);
    return 0;
}

static int badusage(void) {
    DISPLAYLEVEL(1, "Incorrect parameters\n");
    if (displayLevel >= 1) usage();
    DISPLAY("Press enter to exit \n");
    getchar();
    exit(1);
}


static void generate(void* buffer, size_t buffSize, double p) {
    char table[PROBATABLESIZE] = { 0 };
    int remaining = PROBATABLESIZE;
    unsigned pos = 0;
    unsigned s = 0;
    char* op = (char*)buffer;
    char* oend = op + buffSize;
    unsigned seed = 1;

    if (p == 0.0) p = 0.005;
    DISPLAY("Generating %u KB with P=%.2f%%\n", (unsigned)(buffSize >> 10), p * 100);

    /* Build Table */
    while (remaining) {
        unsigned n = (unsigned)(remaining * p);
        unsigned end;
        if (!n) n = 1;
        end = pos + n;
        while (pos < end) table[pos++] = (char)s;
        s++;
        remaining -= n;
    }

    /* Fill buffer */
    while (op < oend) {
        const unsigned r = GEN_rand(&seed) & (PROBATABLESIZE - 1);
        *op++ = table[r];
    }
}


void createSampleFile(const char* filename, double p) {
    FILE* const foutput = fopen(filename, "wb");
    if (foutput == NULL) {
        perror("dataGenerator:");
        exit(1);
    }
    {   
    void* const buffer = malloc(BUFFERSIZE);
    generate(buffer, BUFFERSIZE, p);
    fwrite(buffer, 1, BUFFERSIZE, foutput);
    free(buffer);
    }
    fclose(foutput);
    DISPLAY("File %s generated\n", filename);
}

#endif