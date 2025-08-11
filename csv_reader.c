#include <stdio.h>
#include <string.h>
#include <conio.h>

int main() {
    
    FILE* fp = fopen("tests/dataset_22.csv", "r");

    if (!fp) printf("Can't open file\n");

    else {
        char buffer[127];

        int row = 0;
        int column = 0;

        while (fgets(buffer, sizeof(buffer), fp)) {
            column = 0;
            row++;

            if (row == 1) continue;

            char* value = strtok(buffer, ", ");

            while (value) {
                printf("%s\n", value);
                value = strtok(NULL, ",");
                column++;
            }

            printf("\n");
        }

        fclose(fp);
    }
    return 0;
}