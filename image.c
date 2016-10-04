
#include "image.h"

char *input_filename, *output_filename;

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr, "usage: image <inputfile> [outputfile]\n");
        return EXIT_FAILURE;
    }
    input_filename = argv[1];
    if(argc < 3){
        /* No output file name is specified, generate a name (input.bmp => input_out.bmp) */
        output_filename = malloc(strlen(input_filename) + 5);
        strncpy(output_filename, input_filename, strlen(input_filename)-4);
        strcpy(output_filename+strlen(input_filename)-4, "_out");
        strcpy(output_filename+strlen(input_filename), (input_filename+strlen(input_filename)-4));
    }else{
        output_filename = argv[2];
    }

    printf("File written to '%s'\n", output_filename);
    return EXIT_SUCCESS;
}
