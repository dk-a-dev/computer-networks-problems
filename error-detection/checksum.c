#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CHECKSUM_SIZE 32

uint32_t calculateChecksum(const char* message) {
    uint32_t checksum = 0;
    while (*message) {
        checksum += (unsigned char)(*message);
        message++;
    }
    return checksum;
}

void printBinary(uint32_t value) {
    for (int i = CHECKSUM_SIZE - 1; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
}

int verifyChecksum(const char* message, uint32_t checksum) {
    uint32_t sum = calculateChecksum(message) + checksum;
    printf("Server: Calculated checksum: ");
    printBinary(sum);
    printf("\n");
    return sum == 0xFFFFFFFF; // Check if the result is all 1s
}

void clientFunction(const char* message, uint32_t* checksum) {
    uint32_t sum = calculateChecksum(message);
    *checksum = ~sum; // Invert the sum to get the checksum
    printf("Client: Message sent with checksum ");
    printBinary(*checksum);
    printf("\n");
}

int serverFunction(const char* message, uint32_t checksum) {
    int isValid = verifyChecksum(message, checksum);
    if (isValid) {
        printf("Server: Checksum is valid. Message: %s\n", message);
    } else {
        printf("Server: Checksum is incorrect. Invalid Message\n");
    }
    return isValid;
}

int main() {
    // Test Case 1
    printf("Test Case 1\n");
    const char* testMessage1 = "H";
    uint32_t checksum1;
    clientFunction(testMessage1, &checksum1);
    serverFunction(testMessage1, checksum1);
    printf("\n");

    // Test Case 2
    printf("Test Case 2\n");
    const char* testMessage2 = "He";
    uint32_t wrongChecksum = checksum1;
    wrongChecksum ^= 1; // Flip the least significant bit
    serverFunction(testMessage2, wrongChecksum);
    printf("\n");

    // Test Case 3
    printf("Test Case 3\n");
    const char* testMessage3 = "Goo";
    uint32_t checksum3;
    clientFunction(testMessage3, &checksum3);
    serverFunction(testMessage3, checksum3);
    printf("\n");

    // Test Case 4
    printf("Test Case 4\n");
    const char* testMessage4 = "Good";
    uint32_t wrongChecksum2 = checksum3;
    wrongChecksum2 ^= 2; // Flip the second least significant bit
    serverFunction(testMessage4, wrongChecksum2);
    printf("\n");

    return 0;
}