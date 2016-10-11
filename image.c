#include "image.h"

#define THREAD_COUNT 1

void *calculate_edge(void *ptr);

typedef struct
{
    unsigned char *image_data;
    unsigned char *destination_image;
    int start_x, start_y;
    int width;
    int height;
    int items;
    int components;
} image_data_t;

const double FILTER[3][3] = {
        {-1, -1, -1}, 
        {-1,  6, -1}, 
        {-1, -1, -1}
    };
const int TRESHOLD = 20;

int main(int argc, char *argv[]) {

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

    /* Load image to memory */
    int width, height, components;
    unsigned char *original_image, *destination_image;
    original_image = stbi_load(input_filename, &width, &height, &components, 1);
    if(original_image == NULL){
        perror("Could not load image");
        exit(EXIT_FAILURE);
    }

    int comp = 2;
    destination_image = malloc(height * width * comp);

    printf("Width: %d, Height: %d, Components: %d\n", width, height, components);

    pthread_t thread[THREAD_COUNT];
    image_data_t *image_data[THREAD_COUNT];

    int items = (width*height + THREAD_COUNT-1) / THREAD_COUNT;
    // initialize and start threads
    for(uint8_t i = 0; i<THREAD_COUNT; i++){
        image_data[i] = malloc(sizeof(image_data_t));
        image_data[i]->image_data = original_image;
        image_data[i]->destination_image = destination_image;
        image_data[i]->width = width;
        image_data[i]->height = height;
        if(i<THREAD_COUNT-1){
            image_data[i]->items = items;
        }else{
            image_data[i]->items = width*height - i*items;
        }
        image_data[i]->start_x = (i * items) % width;
        image_data[i]->start_y = (i * items) / width;
        image_data[i]->components = comp;

        int res = pthread_create(&thread[i], NULL, calculate_edge, (void*) image_data[i]);
        if(res)
        {
            fprintf(stderr, "Error creating threads: %d\n", res);
            exit(EXIT_FAILURE);
        }
    }
    
    // join threads
    for(uint8_t i = 0; i<THREAD_COUNT; i++){
        int res = pthread_join(thread[i], NULL);
        if(res){
            fprintf(stderr, "Error joining threads: %d\n", res);
            exit(EXIT_FAILURE);
        }
    }

    if(!stbi_write_bmp(output_filename, width, height, comp, destination_image)){
        perror("Could not save image");
        exit(EXIT_FAILURE);
    }

    stbi_image_free(original_image);
    free(destination_image);

    printf("File written to '%s'\n", output_filename);
    return EXIT_SUCCESS;
}

void *calculate_edge(void *ptr){
    image_data_t *img_data = ptr;
    int counter = 0;
    for(int y = img_data->start_y ; y < img_data->height ; y++){
        for(int x = (y > img_data->start_y ? 0 : img_data->start_x) ; x < img_data->width ; x++){
            unsigned char sum = 0;
            for(int i=0; i<3; i++){
                for(int j=0; j<3; j++){
                    int img_x = x - 1+i;
                    int img_y = (y-1+j)*img_data->width;
                    unsigned char img_x_y = *(img_data->image_data + img_x + img_y);
                    if(img_x < 0 || img_x >= img_data->width
                        || img_y < 0 || img_y >= img_data->height*img_data->width){
                        img_x_y = 0;
                    }
                    double res = FILTER[i][j] * img_x_y;
                    sum += res;
                }
            }
            
            int pos = img_data->components*x + img_data->components*y * img_data->width;
            for(int c=0; c<img_data->components; c++){
                *(img_data->destination_image + pos + c) = sum < TRESHOLD ? 0 : 255;
            }
            if(counter++ >= img_data->items) return 0;
        }
    }
    return 0;
}
