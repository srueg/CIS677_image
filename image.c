#include "image.h"


int main(int argc, char *argv[]) {

    const double FILTER[3][3] = {{-0.0625, -0.0625, -0.0625}, {-0.0625, 0.5, -0.0625}, {-0.0625, -0.0625, -0.0625}};
    const int TRESHOLD = 50;

    char *input_filename, *output_filename;

    if(argc < 2){
        fprintf(stderr, "usage: image <inputfile> [outputfile]\n");
        exit(EXIT_FAILURE);
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

    int width, height, components;
    unsigned char *original_image, *destination_image;
    original_image = stbi_load(input_filename, &width, &height, &components, 1);
    if(original_image == NULL){
        perror("Could not load image");
        exit(EXIT_FAILURE);
    }

    destination_image = malloc(height * width *3);

    printf("Width: %d, Height: %d, Components: %d\n", width, height, components);

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            unsigned char sum = 0;
            for(int i=0; i<3; i++){
                for(int j=0; j<3; j++){
                    double res = FILTER[i][j] * *(original_image + x + y*width);
                    sum += res;
                }
            }
            
            int pos = 3*x + 3*y * width;
            for(int c=0; c<3; c++){
                *(destination_image + pos + c) = sum < TRESHOLD ? 0 : 255;
            }
        }
    }

    if(!stbi_write_bmp(output_filename, width, height, 3, destination_image)){
        perror("Could not save image");
        exit(EXIT_FAILURE);
    }

    stbi_image_free(original_image);
    free(destination_image);

    printf("File written to '%s'\n", output_filename);
    return EXIT_SUCCESS;
}
