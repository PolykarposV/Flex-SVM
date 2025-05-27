#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cfu.h>
#include <inttypes.h>
#include <perf.h>
#include <console.h>
#include <stdint.h>
#include <samples.h>
#include <labels.h>
#include <weights_q.h>
#include <biases_q.h>

#define SV_create_env(a,b)    opcode_R(0x33, 0x7, 0x1, a, b)
#define SV_calc4(a,b)     opcode_R(0x33, 0x0, 0x1, a, b)
#define SV_res4(a,b)     opcode_R(0x33, 0x1, 0x1, a, b)
#define SV_calc8(a,b)     opcode_R(0x33, 0x2, 0x1, a, b)
#define SV_res8(a,b)     opcode_R(0x33, 0x3, 0x1, a, b)
#define SV_calc16(a,b)     opcode_R(0x33, 0x4, 0x1, a, b)
#define SV_res16(a,b)     opcode_R(0x33, 0x5, 0x1, a, b)

#ifdef OPT_LINK_CODE_IN_SRAM
void donut(void) __attribute__((section(".ramtext")));
#else
void donut(void);
#endif

#define num_features 6
#define num_samples 62
#define num_classes 3
#define combinations (num_classes * (num_classes-1))/2


void print_binary(const char *label, int32_t value) {
    printf("%s: ", label);
    for (int bit = 31; bit >= 0; bit--) {
        printf("%d", (value >> bit) & 1);
    }
    printf("\n");
}

int argmax(const int *array, int length) {
    int max_index = 0;
    for (int i = 1; i < length; ++i) {
        if (array[i] > array[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

void get_class_pair(int classifier_idx, int* c1, int* c2) {
    // All the class pairs (c1, c2) for 6 classes
    int pairs[15][2] = {
        {0, 1}, {0, 2},// {0, 3}, {0, 4}, {0, 5},
        {1, 2}//, {1, 3}, {1, 4}, {1, 5},
       // {2, 3}, {2, 4}, {2, 5},
       // {3, 4}, {3, 5},
       // {4, 5}
    };
        *c1 = pairs[classifier_idx][0];
        *c2 = pairs[classifier_idx][1];
}

void donut() {
    int predictions[num_samples];
    int c1,c2;
    int a,b;
    int decision;

    for (int i = 0; i < num_samples; i++) {
	int votes[num_classes] = {0};
	SV_create_env(0,0);
	for (int j = 0; j < combinations; j++) {
            a = 0; // Register for features
            b = 0; // Register for weights

            // Constructing Operand A
            a |= ((samples[i][0] & 0xF));
            a |= ((samples[i][0] & 0xF) << 4);
            a |= ((samples[i][1] & 0xF) << 8);
            a |= ((samples[i][1] & 0xF) << 12);
            a |= ((samples[i][2] & 0xF) << 16);
            a |= ((samples[i][2] & 0xF) << 20);
            a |= ((samples[i][3] & 0xF) << 24);
            a |= ((samples[i][3] & 0xF) << 28);


            // Constructing Operand B
            b |= ((weights_q[j][0] & 0xFF));
            b |= ((weights_q[j][1] & 0xFF) << 8);
            b |= ((weights_q[j][2] & 0xFF) << 16);
            b |= ((weights_q[j][3] & 0xFF) << 24);

            SV_calc8(a, b);

            a = 0;
            b = 0;

            // Constructing Operand A
            a |= ((samples[i][4] & 0xF));
            a |= ((samples[i][4] & 0xF) << 4);
            a |= ((samples[i][5] & 0xF) << 8);
            a |= ((samples[i][5] & 0xF) << 12);
            a |= ((15 & 0xF) << 16);
            a |= ((15 & 0xF) << 20);


            // Constructing Operand B
            b |= ((weights_q[j][4] & 0xFF));
            b |= ((weights_q[j][5] & 0xFF) << 8);
            b |= ((biases_q[j] & 0xFF) << 16);

	    decision = SV_res8(a,b);
               get_class_pair(j,&c1,&c2);
               if (decision >= 0) {
                   votes[c1]++;
               } else {
                   votes[c2]++;
               }
        }
        predictions[i] = argmax(votes,num_classes);
    }

    int correct = 0;

    for (int i = 0; i < num_samples; i++) {
        if (predictions[i] == labels[i]) {
            correct++;
        }
    }
    printf("%d correct predictions out of %d\n", correct, num_samples);
}
