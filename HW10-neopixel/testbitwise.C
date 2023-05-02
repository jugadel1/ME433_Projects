#include <stdio.h>

int main(void){
    unsigned char chipadd = 0b01000000;
    unsigned char read = (chipadd | 0b00000001);

    for (int i = 0; i < 8; i++) {
        printf("%d", !!((read << i) & 0x80));
    }
    printf("\n");
}
