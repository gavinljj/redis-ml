#include <stdio.h>
#include <stdlib.h>

#include "layer.h"
#include "network.h"

void printDigit(unsigned char *d){
    for (int i = 0; i < 784; i++){
        printf("%c", d[i]>100 ? '*' : ' ');
        if(i%28 == 0){
            printf("\n");
        }
    }
}

int getInt32(unsigned char *buff, FILE *f) {
    if(fread(buff, 4, 1, f))
        return (buff[0]<<24 | (buff[1]<<16) | (buff[2]<<8) |buff[3] );
        
    return 0;
}

/*
 * args: training_data, cycles, mini_batch_size, eta,                test_data=None):
 */
int main (int argc, char**argv) {

    if (argc != 6) {
        printf("args: training_data, cycles, batch_size, learning_rate, num_layers\n");
        return 0;
    }

    char* trainingDataFile = argv[1];
    int cycles = atoi(argv[2]);
    int batchSize = atoi(argv[3]);
    float learningRate = atof(argv[4]);
    int nlayers = atoi(argv[5]);

    printf("trainingDataFile: %s\n", trainingDataFile);
    printf("cycles: %d\n", cycles);
    printf("batchSize: %d\n", batchSize);
    printf("learningRate: %.3f\n", learningRate);
    printf("nlayers: %d\n", nlayers);

    FILE *f;
    unsigned char buff[4];

    f = fopen(trainingDataFile,"r");
    if (f == NULL){
        printf("file missing\n");
    }

    int magic = getInt32(buff, f);
    printf("magic: %d\n", magic);

    int nimages = getInt32(buff, f);
    printf("nimages: %d\n", nimages);

    int rows = getInt32(buff, f);
    printf("rows: %d\n", rows);

    int cols = getInt32(buff, f);
    printf("cols: %d\n", cols);

    /* allocate training data */
    unsigned char* trainingData = calloc(nimages * rows * cols, sizeof(unsigned char));
    size_t rsize = fread(trainingData, 1, nimages * rows * cols, f);
    if (rsize != nimages * rows * cols){
        printf("error loading file to memory, read %zu bytes\n", rsize);
        return 0;
    }
    printf("success. read %zu bytes\n", rsize);
   
    f = fopen("./data/mnist/train-labels-idx1-ubyte","r");
    if (f == NULL){
        printf("file missing\n");
    }

    magic = getInt32(buff, f);
    printf("labels magic: %d\n", magic);

    nimages = getInt32(buff, f);
    printf("nlabels: %d\n", nimages);

    /* allocate training labels */
    unsigned char* trainingLabels = calloc(nimages, sizeof(unsigned char));
    rsize = fread(trainingLabels, 1, nimages, f);
    if (rsize != nimages){
        printf("error loading file to memory, read %zu bytes\n", rsize);
        return 0;
    }
    int l = 0;
    for (int i = 0; i < 7840; i+= 784){
        printf("\n\nlabel = %u\n", trainingLabels[l++]);
        printDigit(&trainingData[i]);
    }
    fclose(f);
    
    
    
    /* init the network */
    Network n = {};
    n.nlayers = nlayers;
    n.layers = malloc(n.nlayers * sizeof(Layer *));
    n.layers[0] = malloc(sizeof(Layer));
    n.layers[0]->a = malloc(sizeof(Matrix *));
    n.layers[0]->a->rows = rows * cols;
    n.layers[0]->a->cols = 1;
    n.layers[1] = Layer_Init(100, rows * cols, FULLY_CONNECTED, SIGMOID);
    n.layers[2] = Layer_Init(10, 100, FULLY_CONNECTED, SIGMOID);
    n.costDerivativeFunc = &CostSSEDeriv;
    

    return(0);
}
