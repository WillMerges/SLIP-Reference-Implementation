/*******************************************************************************
*
*  Name: main.cpp
*
*  Purpose: Test the SLIP implementation.
*
*  Author: Will Merges
*
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "slip.h"

// ANSI escape sequences
#define BLUE  "\033[1;34m"
#define RESET "\033[0m"

int main() {
    SLIPEncoder<1024> encoder;
    SLIPDecoder<1024> decoder;

    // create a buffer of ramped 8-bit integers
    uint8_t data[255];
    for(uint8_t i = 0; i < 255; i++) {
        data[i] = i;
    }

    // encode into a slip frame
    slip_buffer_t* encoded = encoder.encode(data, 255);
    if(NULL == encoded) {
        printf("Encoder returned NULL\n");
        exit(-1);
    }

    printf("encoded: \n");
    for(size_t i = 0; i < encoded->len; i++) {
        // print escapes colorfully
        if(SLIP_ESC == encoded->data[i]) {
            printf("%s%02x ", BLUE, encoded->data[i]);
        } else {
            printf("%02x %s", encoded->data[i], RESET);
        }

        if(i % 16 == 15 || i == encoded->len - 1) {
            printf("\n");
        }
    }
    printf("\n");

    slip_buffer_t* decoded;
    for(size_t i = 0; i < encoded->len; i++) {
        decoded = decoder.push(encoded->data[i]);

        if(i < encoded->len - 1 && NULL != decoded) {
            printf("decoded frame too early\n");
            break;
        }
    }

    if(NULL == decoded) {
        printf("decoded is NULL\n");
        exit(-1);
    }

    printf("decoded: \n");
    for(size_t i = 0; i < decoded->len; i++) {
        printf("%02x ", decoded->data[i]);

        if(i % 16 == 15 || i == decoded->len - 1) {
            printf("\n");
        }
    }
    printf("\n");

    // encode an entire struct
    typedef struct {
        int a;
        int b;
        bool c;
    } test_t;

    test_t test_dat = {1, 2, true};
    encoded = encoder.encode<test_t>(&test_dat);

    if(NULL == encoded) {
        printf("failed to encode struct\n");
        exit(-1);
    }

    // first push some garbage data
    uint8_t garbage[5] = {0x1, 0x2, 0x3, 0x4, 0x5};
    for(size_t i = 0; i < 5; i++) {
        decoded = decoder.push(garbage[i]);

        if(NULL != decoded) {
            printf("decoded frame when shouldn't have\n");
            exit(-1);
        }
    }

    // now push the actual encoded frame
    for(size_t i = 0; i < encoded->len; i++) {
        decoded = decoder.push(encoded->data[i]);

        if(i < encoded->len - 1 && NULL != decoded) {
            printf("decoded frame too early\n");
            break;
        }
    }

    if(sizeof(test_t) != decoded->len) {
        printf("decoded struct size is incorrect\n");
        exit(-1);
    }

    test_t* ptr = (test_t*)decoded->data;
    if(ptr->a != 1 || ptr->b != 2 || ptr->c != true) {
        printf("decoded struct data is incorrect\n");
        exit(-1);
    }

    printf("done\n");
}
