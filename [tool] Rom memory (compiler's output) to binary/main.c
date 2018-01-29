//Created by: João Gante         January 2017 (last update)

// Rom memory to binary: converts the rom memory (with the instruction sequence in a text file) into an actual binary file
//              IMPORTANT: the sequence ends with an "ffffffff"


//REQUIREMENTS: "rom.mem" file

//TRANSLATION EXAMPLE:
//b8080050 -> 1011 1000 0000 1000 0000 0000 0101 0000 in binary -> BRAI 80  (0x50 = 80 in decimal)


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef int32_t bool;
#define false 0
#define true 1


const char *show_opcode(char x);
void to_LittleEndian(const char * original_bytes, char* new_bytes, int32_t size);
char hex_to_byte( char* original_hex, int32_t index);
bool is_hex(char to_eval);
void print_instruction(char byte);



int main(){


    // Allocates space (1 MB = 1048576 B  (1 char = 1 B))
    char* binary_content = malloc(1048576 * sizeof(char));
    char* filtered_content = malloc(1048576 * sizeof(char));

    int32_t j;
    int32_t end = 0;

    //opens the file
    FILE* f = fopen("rom.mem", "r");

    printf("\nReading rom.mem...");

    //copies the file into the buffer (each element is a char)
    fread(binary_content, 1048576, 1, f);
    fclose(f);

    //At this point, we don't know where the end of the file is (it ends with 3 "\n")
    //Moreover, it still has the initial header (which ends with "@" plus 8 digits)
    bool is_endofline[2] = {false, false};
    int32_t j_mod2;
    int32_t starting_index = 0;

    for(j=0;j<1048576;j++){

        j_mod2 = j % 2;


        //If the next char is not a hex char (0-9 plus A-F) and the starting
        // index was already found, it's a possible end of file
        if(is_hex(binary_content[j]) == false && starting_index != 0){
                is_endofline[j_mod2] = true;
        }
        else is_endofline[j_mod2] = false;

        //two "\n" in a row -> end of file found
        if(is_endofline[0] && is_endofline[1]){
            end = j-2;
            j = 1048576;
        }

        //When it finds the @ -> sets the starting index (= current index + 10)
        if(binary_content[j] == '@'){
            starting_index = j+10;
        }

    }


    printf("\n End: %i", end );
    printf("\n Starting index: %i", starting_index );

    if(end == 0){
        printf("\nCAUTION - The final sequence (ffffffff) was not found!\n");
    }
    if(starting_index == 0){
        printf("\nCAUTION - If this file has an initial header, it was not found!\n");
    }


    //We know that the instructions start at "starting_index" and end at "end".
    //However, there are endline chars at the end of each line
    int32_t filtered_index = 0;
    int32_t j_mod9;

    for(j=starting_index;j<=end;j++){

        j_mod9 = (j-starting_index) % 9;

        if(j_mod9 != 8){
            filtered_content[filtered_index] = binary_content[j];
            filtered_index++;
        }
    }

    end = filtered_index;

    free(binary_content);

    /*printf("\nlast instruction:");
    for(j = filtered_index-8; j <= filtered_index; j++){
        printf("%c", filtered_content[j]);
    }*/


    //Because 1 inst = 8 hex
    if(end % 8 != 0){
        printf("\nCAUTION - The number of hex characters in the filtered rom file isn't a multiple  ");
        printf("of 8! (8 hex characters = 32-bit instruction)\n");
    }


    //converts 2 chars (with 0 through f) into 1 char = 1 byte (don't forget: the MSB is the first to be read!)
    int32_t index;
    int32_t new_size = end/2; //number of bytes of the new string -> number of hex characters/2
    char* new_binary = malloc(new_size * sizeof(char));

    printf("\nConverting into the new format...");

    //FOR DEBUGGING
    bool dbg = false;
    if(dbg) printf("\n\nDBG MODE: printing the instructions, 31->0");

    for(index = 0; index<(end/2); index++){

        new_binary[index] = hex_to_byte(filtered_content, index);

        if(dbg && index < 40){
            if(index%4==0) printf("\nInst #%i:", index/4);
            print_instruction(new_binary[index]);
        }

    }


    printf("\nSaving the result...");

    //opens the new binary file
    f = fopen("new_binary", "wb");

    //Inputs the new characters (saved as Little Endian, i.e.: 4-3-2-1-8-7-6-5-...)
    char* little_endian_bin = malloc(new_size * sizeof(char));
    to_LittleEndian(new_binary, little_endian_bin, new_size);
    fwrite(little_endian_bin, 1, new_size, f);

    //closes the file
    fclose(f);


    //Printf double checks (# of bytes, # of instructions (=bytes/4) and the opcodes)
    printf("\n\nDouble checking:");
    printf("\nNumber of bytes: %i (%i instructions)", new_size, (new_size/4));

    /*
    for(j=0;j<end;j=j+8){
        //Extract the opcode, which is in 31:26 -> byte#4 in Little Endian
        printf("\nOpcode %i: ", ((j/8)+1));

        index = (j/2) + 1;
        printf("%s", show_opcode( (little_endian_bin[index] >> 2) & 0x3F )     );

    }*/




    printf("\n\nDone!\n");

    free(little_endian_bin);
    free(new_binary);
    free(filtered_content);

    return(0);
}




//Quick tool to printf bits, to be used as "printf("%s", show_opcode(char))"
const char *show_opcode(char x){
    static char b[7];
    b[0] = '\0';

    int z;
    for (z = 32; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}




// reorders from 1-2-3-4-... to 4-3-2-1-8-7-6-5-...
void to_LittleEndian(const char * original_bytes, char* new_bytes, int32_t size){

    int32_t i;

    for(i = 0; i<size; i=i+4){
        new_bytes[i+0] = original_bytes[i+3];
        new_bytes[i+1] = original_bytes[i+2];
        new_bytes[i+2] = original_bytes[i+1];
        new_bytes[i+3] = original_bytes[i+0];
    }

    return;
}


//converts two ASCII hexes into a byte
char hex_to_byte( char* original_hex, int32_t index){

    char MS_hex = original_hex[(index*2)];
    char LS_hex = original_hex[(index*2) + 1];
    char output1, output2, final_output;


    switch(MS_hex){
        case '1':
            output1 = 0x10;
            break;
        case '2':
            output1 = 0x20;
            break;
        case '3':
            output1 = 0x30;
            break;
        case '4':
            output1 = 0x40;
            break;
        case '5':
            output1 = 0x50;
            break;
        case '6':
            output1 = 0x60;
            break;
        case '7':
            output1 = 0x70;
            break;
        case '8':
            output1 = 0x80;
            break;
        case '9':
            output1 = 0x90;
            break;
        case 'a':
            output1 = 0xA0;
            break;
        case 'b':
            output1 = 0xB0;
            break;
        case 'c':
            output1 = 0xC0;
            break;
        case 'd':
            output1 = 0xD0;
            break;
        case 'e':
            output1 = 0xE0;
            break;
        case 'f':
            output1 = 0xF0;
            break;
        default:
            output1 = 0x00;
            break;
    }


    switch(LS_hex){
        case '1':
            output2 = 0x01;
            break;
        case '2':
            output2 = 0x02;
            break;
        case '3':
            output2 = 0x03;
            break;
        case '4':
            output2 = 0x04;
            break;
        case '5':
            output2 = 0x05;
            break;
        case '6':
            output2 = 0x06;
            break;
        case '7':
            output2 = 0x07;
            break;
        case '8':
            output2 = 0x08;
            break;
        case '9':
            output2 = 0x09;
            break;
        case 'a':
            output2 = 0x0A;
            break;
        case 'b':
            output2 = 0x0B;
            break;
        case 'c':
            output2 = 0x0C;
            break;
        case 'd':
            output2 = 0x0D;
            break;
        case 'e':
            output2 = 0x0E;
            break;
        case 'f':
            output2 = 0x0F;
            break;
        default:
            output2 = 0x00;
            break;
    }

    final_output = output2 | output1;

    return(final_output);
}


//Checks if the char is a valid hex character
bool is_hex(char to_eval){

    bool result = false;
    int32_t j;

    //tests 0 through 9
    char test_char = '0';
    for(j = 0; j < 10; j++){
        if(to_eval == test_char){
            result = true;
        }
        test_char++;
    }

    //tests a through f
    test_char = 'a';
    for(j = 0; j < 6; j++){
        if(to_eval == test_char){
            result = true;
        }
        test_char++;
    }


    return(result);
}


//For debugging: prints the 32 bit instruction as zeros and ones
void print_instruction(char byte){

    char bits_temp[8];
    int i;

    //extracting the bits
    for(i = 0; i < 8; i ++){
        bits_temp[7 - i] = (byte >> i) & 0x01;
    }

    //printing the stuff
    for(i = 0; i < 8; i ++){
        bits_temp[i] += '0';
        printf("%c", bits_temp[i]);
    }

}
