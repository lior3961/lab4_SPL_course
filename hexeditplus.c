//task 0 - (readelf -h abc) - to read the header file
//This command prints the ELF header, which contains the entry point address. 
//Entry point address: 0x80483b0

// task 0 - there are 29 sections - (readelf -S abc) to see the sections

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <elf.h>

typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    char display_mode;
} state;

typedef struct {
    char *name;
    void (*func)(state*);
} menu_item;

void toggle_debug_mode(state* s);
void set_file_name(state* s);
void set_unit_size(state* s);
void quit(state* s);
void not_implemented(state* s);
void load_into_memory(state* s);
void toggle_display_mode(state* s);
void memory_display(state* s);
void save_into_file(state* s);
void memory_modify(state* s);
void display_entry_point();
void get_file_size();




menu_item menu[] = {
    {"Toggle Debug Mode", toggle_debug_mode},
    {"Set File Name", set_file_name},
    {"Set Unit Size", set_unit_size},
    {"Load Into Memory", load_into_memory},
    {"Toggle Display Mode", toggle_display_mode},
    {"Memory Display", memory_display},
    {"Save Into File", save_into_file},
    {"Memory Modify", memory_modify},
    {"Quit", quit},
    {"Display Entry Point", display_entry_point},
    {"Length of file", get_file_size},
    {NULL, NULL}
};

void toggle_debug_mode(state* s) {
    if (s->debug_mode == 1) {
        printf("Debug flag now off\n");
        s->debug_mode = 0;
    } else {
        printf("Debug flag now on\n");
        s->debug_mode = 1;
    }
}

void set_file_name(state* s) {
    printf("Enter file name: ");
    fgets(s->file_name, sizeof(s->file_name), stdin);
    s->file_name[strcspn(s->file_name, "\n")] = 0; // Remove newline character
    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }

}void set_unit_size(state* s) {
    int size;
    printf("Enter unit size (1, 2, or 4): ");
    scanf("%d", &size);
    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size;
        if (s->debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
        }
    } else {
        printf("Invalid unit size\n");
    }
}

void quit(state* s) {
    if (s->debug_mode) {
        printf("quitting\n");
    }
    exit(0);
}

void load_into_memory(state* s) {
    if (strcmp(s->file_name, "") == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE *file = fopen(s->file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char input[100];
    unsigned int location;
    int length;

    printf("Please enter <location> <length>\n");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &location, &length);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: file_name='%s', location=0x%x, length=%d\n", s->file_name, location, length);
    }

    fseek(file, location, SEEK_SET);
    size_t read_size = length * s->unit_size;
    fread(s->mem_buf, s->unit_size, length, file);

    printf("Loaded %d units into memory\n", length);

    fclose(file);
}

void toggle_display_mode(state* s) {
    if (s->display_mode == 1) {
        printf("Display flag now off, decimal representation\n");
        s->display_mode = 0;
    } else {
        printf("Display flag now on, hexadecimal representation\n");
        s->display_mode = 1;
    }
}

void memory_display(state* s) {
    char input[100];
    unsigned int addr;
    int length;

    printf("Enter address and length\n");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &addr, &length);

    unsigned char* start_addr = (addr == 0) ? s->mem_buf : (unsigned char*)addr;

    if (s->display_mode) {
        printf("Hexadecimal\n===========\n");
        for (int i = 0; i < length; i++) {
            if (s->unit_size == 1) {
                printf("%#hhx\n", start_addr[i]);
            } else if (s->unit_size == 2) {
                printf("%#hx\n", ((uint16_t*)start_addr)[i]);
            } else if (s->unit_size == 4) {
                printf("%#x\n", ((uint32_t*)start_addr)[i]);
            }
        }
    } else {
        printf("Decimal\n=======\n");
        for (int i = 0; i < length; i++) {
            if (s->unit_size == 1) {
                printf("%#hhd\n", start_addr[i]);
            } else if (s->unit_size == 2) {
                printf("%#hd\n", ((uint16_t*)start_addr)[i]);
            } else if (s->unit_size == 4) {
                printf("%#d\n", ((uint32_t*)start_addr)[i]);
            }
        }
    }
}

void save_into_file(state* s) {
    if (strcmp(s->file_name, "") == 0) {
        printf("Error: file name is empty.\n");
        return;
    }

    FILE* file = fopen(s->file_name, "r+");
    if (file == NULL) {
        printf("Error: could not open file '%s'.\n", s->file_name);
        return;
    }

    char input[100];
    unsigned int src_addr;
    unsigned int target_loc;
    int length;

    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x %d", &src_addr, &target_loc, &length);

    unsigned char* src = (src_addr == 0) ? s->mem_buf : (unsigned char*)src_addr;

    if (fseek(file, 0, SEEK_END) != 0) {
        printf("Error: could not seek to end of file '%s'.\n", s->file_name);
        fclose(file);
        return;
    }
    long file_size = ftell(file);
    if (file_size < 0 || target_loc > (unsigned long)file_size) {
        printf("Error: target location is beyond file size.\n");
        fclose(file);
        return;
    }

    if (fseek(file, target_loc, SEEK_SET) != 0) {
        printf("Error: could not seek to target location in file '%s'.\n", s->file_name);
        fclose(file);
        return;
    }

    size_t num_bytes = length * s->unit_size;
    if (fwrite(src, s->unit_size, length, file) != length) {
        printf("Error: could not write data to file '%s'.\n", s->file_name);
    } else {
        printf("Saved %d units into file '%s'.\n", length, s->file_name);
    }

    fclose(file);
}

void memory_modify(state* s) {
    char input[100];
    unsigned int loc;
    unsigned int val;

    printf("Please enter <location> <val>\n");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x", &loc, &val);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: location=0x%x, val=0x%x\n", loc, val);
    }

    if (loc + s->unit_size > sizeof(s->mem_buf)) {
        printf("Error: location out of bounds.\n");
        return;
    }

    if (s->unit_size == 1) {
        *(uint8_t*)(s->mem_buf + loc) = (uint8_t)val;
    } else if (s->unit_size == 2) {
        *(uint16_t*)(s->mem_buf + loc) = (uint16_t)val;
    } else if (s->unit_size == 4) {
        *(uint32_t*)(s->mem_buf + loc) = (uint32_t)val;
    }
}

void display_entry_point() {
    
    char file_name[128];
    printf("Enter ELF file name: ");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0'; // Remove newline

    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Read ELF header (assuming 32-bit ELF for simplicity)
    Elf32_Ehdr elf_header;
    fread(&elf_header, sizeof(Elf32_Ehdr), 1, file);

    // Display entry point address
    printf("Entry point address (EIP): %#lx\n", (unsigned long)elf_header.e_entry);

    fclose(file);
}


void get_file_size() {
    char file_name[128];
    printf("Enter ELF file name: ");
    if (fgets(file_name, sizeof(file_name), stdin) == NULL) {
        perror("Error reading file name");
        return; // Return -1 to indicate error
    }

    // Remove newline character if present
    if (file_name[strlen(file_name) - 1] == '\n') {
        file_name[strlen(file_name) - 1] = '\0';
    }

    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return; // Return -1 to indicate error
    }

    // Seek to the end of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        fclose(file);
        return; // Return -1 to indicate error
    }

    // Get current position (which is the file size)
    long file_size = ftell(file);
    if (file_size == -1L) {
        perror("Error getting file size");
        fclose(file);
        return;
    }

    printf("Length of file : %ld\n", file_size);
    fclose(file);
}


int main() {
    state s = {0, "", 1, {0}, 0, 0};
    while (1) {
        if (s.debug_mode) {
            fprintf(stderr, "Debug: unit_size=%d, file_name='%s', mem_count=%zu\n", s.unit_size, s.file_name, s.mem_count);
        }
        printf("Choose action:\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }
        int choice;
        scanf("%d", &choice);
        getchar(); // Consume newline
        if (choice >= 0 && choice < sizeof(menu)/sizeof(menu[0]) - 1) {
            menu[choice].func(&s);
        } else {
            printf("Invalid choice\n");
        }
    }
    return 0;
}

//task 2 - The _start function is responsible for setting up the environment
// for the program before main() is called. the virtual address of start function here is 08048350.
//deep_thought Entry point address:  0x8048464
//deep_thought FileSize : 1676 (in decimal)
//the values of location is 0x8048464 and of length is 1676 bytes
// we can know that by using: readelf -h deep_thought (Entry point address), readelf -l deep_thought(size)
//task2:
// 1. ./deep_thought
// 2. readelf -s deep_thought | grep _start
// 3._ location - 08048350
// fix file :
//  ./hexeditplus
// 1 -> deep_thought
// 3 -> 0 52 (load elf)
// 2 -> 4
// 7 -> 18 08048350
// 2 -> 1
// 6 -> 0 0 52
// 8


//task 3:
// entry point of main - 0804844d , size - 23(16) = 35(10)
// offset  of main - 0804844d - 08048350 = FD(16) = 253(10)

