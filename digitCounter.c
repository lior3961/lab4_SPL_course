#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <elf.h>

int count_digits(const char *str);


int count_digits(const char *str) {
    int count = 0;
    while (*str != '\0') {  
        if (*str >= '0' && *str <= '9') {
            count++;
        }
        str++;
    }
    return count;
}

int main(int argc, char *argv[]) {
    // Check if a command-line argument is provided
    if (argc < 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    // Use the first command-line argument as the input string
    const char *input = argv[1];
    
    // Count the digits in the input string
    int digit_count = count_digits(input);
    
    // Print the result
    printf("Number of digits in the input string: %d\n", digit_count);
    
    return 0;
}

