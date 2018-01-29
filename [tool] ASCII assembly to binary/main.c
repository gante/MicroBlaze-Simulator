//Created by: João Gante         November 2016 (last update)

//Bin to file: converts the binary string (handmade assembly) saved in ASCII
//             into an actual binary file file
//IMPORTANT: the ASCII binary string must end with an "a"



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


typedef int32_t bool;
#define false 0
#define true 1


const char *show_opcode(char x);
void to_LittleEndian(const char * original_bytes, char* new_bytes, int32_t size);

void binary_conversion(const char * in_filename, const char * out_filename);
void assembly_conversion(const char * in_filename, const char * out_filename);




int main(){

    //Program flow:
    // 1 - Prompts for input file name
    // 2 - Prompts for output file name
    // 3 - If the file exists, asks whether it is a ASCII binary, or a ASCII assembly
    // 4 - Decodes the stuff



    //::::::::::::::::::::::::::::::  1  ::::::::::::::::::::::::::::::

    bool valid_value;
    char buffer[100], in_filename[100];
    FILE *f;

    printf("\n-------------------------------------");
    printf("\nASCII to Microblaze binary translator\n");
    printf("-------------------------------------\n");

    do{
        printf("\nPlease type in the file to be read (type 0 to exit)\n>>> ");

        if(fgets (buffer , 100 , stdin) != 0){

            strcpy(in_filename, buffer);
            strtok(in_filename, "\n"); //removes the '\n' left from the stdin

            //If 0 was typed in, exits
            if(strcmp(in_filename, "0")==0){
                return(0);
            }

            f = fopen(in_filename, "r");

            //if the file exists, exits this loop
            if(f != NULL){
                valid_value = true;
            }
            else if(strcmp(in_filename, "\n")==0){
                printf("There are no files named '' :)\n");
                valid_value = false;
            }
            //otherwise, prints invalid filename
            else{
                printf("The file %s doesn't exist!\n", in_filename);
                valid_value = false;
            }
            fclose(f);


        }
    }while(valid_value == false);


    //::::::::::::::::::::::::::::::  2  ::::::::::::::::::::::::::::::


    char out_filename[100];
    printf("\nReading the file: %s", in_filename);


    do{
        printf("\nPlease type in the desired output file name(type 0 to exit)\n>>> ");

        if(fgets (buffer , 100 , stdin) != 0){

            strcpy(out_filename, buffer);
            strtok(out_filename, "\n"); //removes the '\n' left from the stdin

            //If 0 was typed in, exits
            if(strcmp(out_filename, "0")==0){
                return(0);
            }

            f = fopen(out_filename, "r");

            //if the file exists, asks for a new name (so as to avoid overwriting)
            if(f != NULL){
                printf("There is already a file with that name!\n");
                valid_value = false;
            }
            else if(strcmp(out_filename, "\n")==0){
                printf("There are no files named '' :)\n");
                valid_value = false;
            }
            //otherwise, prints invalid filename
            else{
                valid_value = true;
            }
            fclose(f);


        }
    }while(valid_value == false);


    //::::::::::::::::::::::::::::::  3  ::::::::::::::::::::::::::::::


    char file_type;
    printf("\nWriting to the file: %s", out_filename);
    printf("\nIs the file a binary ASCII or an assembly ASCII?");



    do{
        printf("\n(a for assembly, b for binary, 0 to quit the program)\n>>> ");

        file_type = getc(stdin);
        fflush(stdin);

        //If the input is 0, exits
        if(file_type == '0'){
            return(0);
        }

        if(file_type == 'a' || file_type == 'b'){
            valid_value = true;
        }
        else{
            printf("Please input a valid input!\n");
            valid_value = false;
        }

    }while(valid_value == false);



    //::::::::::::::::::::::::::::::  4  ::::::::::::::::::::::::::::::

    if(file_type == 'a'){
        printf("\nReading type: assembly");
        printf("\n\nStaring the conversion...");
        assembly_conversion(in_filename,out_filename);

    }
    else{
        printf("\nReading type: binary");
        printf("\n\nStaring the conversion...");
        binary_conversion(in_filename,out_filename);
    }



    return(0);
}




// Quick tool to printf bits, to be used as in "printf("%s", show_opcode(char))"
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




// Reorders from 1-2-3-4-... to 4-3-2-1-8-7-6-5-...
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


//Performs the binary ASCII -> pure binary conversion
//IMPORTANT: the ASCII sequence must end with an 'a' (to make things easy :D)
void binary_conversion(const char * in_filename, const char * out_filename){

    // Allocates space for the buffer
    char * binary_content = (char*) malloc(320000 * sizeof(char));
    int32_t number_of_chars, j;


    // Opens the file
    FILE* f = fopen(in_filename, "r");
    printf("\nReading %s...", in_filename);


    // Copies the file into the buffer, one word at a time (let's assume that it has <320000 chars)
    fgets (binary_content , 320000 , f);
    fclose(f);

    // Checks how many ASCII characters the file had and checks if it is a multiple of 32
    // (Because 1 instruction = 32 bits)
    for(j=0;j<320000;j++){
        if(binary_content[j] == 'a'){
            number_of_chars=j;
            j=320000;
        }
    }

    if(number_of_chars%32 != 0){
        printf("\nCAUTION - The number of elements in the extended binary file isn't a multiple of 32!\n");
    }


    // Converts 8 ASCII chars (with 0 or 1) into 1 char = 1 byte (don't forget: the MSB is the first to be read!)
    int32_t index;
    int32_t new_size_bytes = number_of_chars/8; //number of bytes of the new string
    char* new_binary = malloc(new_size_bytes * sizeof(char));

    printf("\nConverting into the new format...");

    for(j = 0; j<number_of_chars; j=j+8){
        index = j/8;
        new_binary[index] = 0x00;

        if(binary_content[j] == '1'){
            new_binary[index] = new_binary[index] | 0x80;
        }

        if(binary_content[j+1] == '1'){
            new_binary[index] = new_binary[index] | 0x40;
        }

        if(binary_content[j+2] == '1'){
            new_binary[index] = new_binary[index] | 0x20;
        }

        if(binary_content[j+3] == '1'){
            new_binary[index] = new_binary[index] | 0x10;
        }

        if(binary_content[j+4] == '1'){
            new_binary[index] = new_binary[index] | 0x08;
        }

        if(binary_content[j+5] == '1'){
            new_binary[index] = new_binary[index] | 0x04;
        }

        if(binary_content[j+6] == '1'){
            new_binary[index] = new_binary[index] | 0x02;
        }

        if(binary_content[j+7] == '1'){
            new_binary[index] = new_binary[index] | 0x01;
        }

    }

    printf("\nSaving the result...");


    // Opens the new binary file
    f = fopen(out_filename, "wb");

    // Inputs the new characters (saved as Little Endian, i.e.: 4-3-2-1-8-7-6-5-...)
    char* little_endian_bin = malloc(new_size_bytes * sizeof(char));
    to_LittleEndian(new_binary, little_endian_bin, new_size_bytes);
    fwrite(little_endian_bin, 1, new_size_bytes, f);

    // Closes the new binary file
    fclose(f);


    // Prints double checks (# of bytes, # of instructions (=bytes/4) and the opcodes)
    printf("\n\nDouble checking:");
    printf("\nNumber of bytes: %i (%i instructions)", new_size_bytes, (new_size_bytes/4));


    for(j=0;j<number_of_chars;j=j+32){
        //Extract the opcode, which is in 31:26 -> byte#4 in Little Endian
        printf("\nOpcode %i: ", ((j/32)+1));

        index = (j/8) + 3;
        printf("%s", show_opcode( (little_endian_bin[index] >> 2) & 0x3F )     );

    }




    printf("\n\nDone!\n");

    free(little_endian_bin);
    free(new_binary);
    free(binary_content);
}

//---------------------------------------------------------------------------------------------------------
// assembly stuff

//copys the 6 bit opcode into the intruction backbone
void copy_opcode(char * full_instruction, char * opcode){

    int i;

    for(i = 0; i < 6; i++){
        full_instruction[i] = opcode[i];
    }

}


//Converts the instruction into a 32 bit backbone, with most of the info
//(only things like registers and immediates are missing)
bool instruction_to_backbone(char * instruction, char * bit_backbone, bool * flags){

    //flags -> bool array with 5 elements
    //0- Imm, 1- Imm5, 2- Rd, 3- Ra, 4- Rb

    //Initializes the backbone as all zeros
    int i;
    for(i = 0; i < 32; i++){
        bit_backbone[i] = '0';
    }

    //converts everything to uppercase, just in case (got it? :D)
    for(i = 0; i < strlen(instruction); i++){
        instruction[i] = toupper(instruction[i]);
    }

    //starts the comparisons

    //---------------------------------------------------------
    //          Table 1
    //---------------------------------------------------------
    if(strcmp(instruction,"ADD")==0){
        copy_opcode(bit_backbone, "000000");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"RSUB")==0){
        copy_opcode(bit_backbone, "000001");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"ADDC")==0){
        copy_opcode(bit_backbone, "000010");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"RSUBC")==0){
        copy_opcode(bit_backbone, "000011");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"ADDK")==0){
        copy_opcode(bit_backbone, "000100");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"RSUBK")==0){
        copy_opcode(bit_backbone, "000101");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"ADDKC")==0){
        copy_opcode(bit_backbone, "000110");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"RSUBKC")==0){
        copy_opcode(bit_backbone, "000111");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"CMP")==0){
        copy_opcode(bit_backbone, "000101");
        bit_backbone[31] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"CMPU")==0){
        copy_opcode(bit_backbone, "000101");
        bit_backbone[30] = '1';
        bit_backbone[31] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"ADDI")==0){
        copy_opcode(bit_backbone, "001000");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"RSUBI")==0){
        copy_opcode(bit_backbone, "001001");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"ADDIC")==0){
        copy_opcode(bit_backbone, "001010");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"RSUBIC")==0){
        copy_opcode(bit_backbone, "001011");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"ADDIK")==0){
        copy_opcode(bit_backbone, "001100");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"RSUBIK")==0){
        copy_opcode(bit_backbone, "001101");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"ADDIKC")==0){
        copy_opcode(bit_backbone, "001110");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"RSUBIKC")==0){
        copy_opcode(bit_backbone, "001111");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"MUL")==0){
        copy_opcode(bit_backbone, "010000");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"MULH")==0){
        copy_opcode(bit_backbone, "010000");
        bit_backbone[31] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"MULHU")==0){
        copy_opcode(bit_backbone, "010000");
        bit_backbone[30] = '1';
        bit_backbone[31] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"MULHSU")==0){
        copy_opcode(bit_backbone, "010000");
        bit_backbone[30] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"MULI")==0){
        copy_opcode(bit_backbone, "011000");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BSRA")==0){
        copy_opcode(bit_backbone, "010001");
        bit_backbone[22] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"BSLA")==0){
        copy_opcode(bit_backbone, "010001");
        bit_backbone[21] = '1';
        bit_backbone[22] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"BSRL")==0){
        copy_opcode(bit_backbone, "010001");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"BSLL")==0){
        copy_opcode(bit_backbone, "010001");
        bit_backbone[21] = '1';
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BSRAI")==0){
        copy_opcode(bit_backbone, "011001");
        bit_backbone[22] = '1';
        flags[2] = true; flags[3] = true; flags[1] = true;
    }
    else if(strcmp(instruction,"BSLAI")==0){
        copy_opcode(bit_backbone, "011001");
        bit_backbone[21] = '1';
        bit_backbone[22] = '1';
        flags[2] = true; flags[3] = true; flags[1] = true;
    }
    else if(strcmp(instruction,"BSRLI")==0){
        copy_opcode(bit_backbone, "011001");
        flags[2] = true; flags[3] = true; flags[1] = true;
    }
    else if(strcmp(instruction,"BSLLI")==0){
        copy_opcode(bit_backbone, "011001");
        bit_backbone[21] = '1';
        flags[2] = true; flags[3] = true; flags[1] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"OR")==0){
        copy_opcode(bit_backbone, "100000");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"AND")==0){
        copy_opcode(bit_backbone, "100001");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"XOR")==0){
        copy_opcode(bit_backbone, "100010");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"ANDN")==0){
        copy_opcode(bit_backbone, "100011");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"SRA")==0){
        copy_opcode(bit_backbone, "100100");
        bit_backbone[31] = '1';
        flags[2] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"SRC")==0){
        copy_opcode(bit_backbone, "100100");
        bit_backbone[31] = '1';
        bit_backbone[26] = '1';
        flags[2] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"SRL")==0){
        copy_opcode(bit_backbone, "100100");
        bit_backbone[25] = '1';
        bit_backbone[31] = '1';
        flags[2] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"SEXT8")==0){
        copy_opcode(bit_backbone, "100100");
        bit_backbone[26] = '1';
        bit_backbone[25] = '1';
        flags[2] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"SEXT16")==0){
        copy_opcode(bit_backbone, "100100");
        bit_backbone[31] = '1';
        bit_backbone[26] = '1';
        bit_backbone[25] = '1';
        flags[2] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"ORI")==0){
        copy_opcode(bit_backbone, "101000");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"ANDI")==0){
        copy_opcode(bit_backbone, "101001");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"XORI")==0){
        copy_opcode(bit_backbone, "101010");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"ANDNI")==0){
        copy_opcode(bit_backbone, "101011");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"IMM")==0){
        copy_opcode(bit_backbone, "101100");
        flags[0] = true;
    }
    //---------------------------------------------------------
    //          Table 2
    //---------------------------------------------------------
    else if(strcmp(instruction,"LBU")==0){
        copy_opcode(bit_backbone, "110000");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"LHU")==0){
        copy_opcode(bit_backbone, "110001");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"LW")==0){
        copy_opcode(bit_backbone, "110010");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"SB")==0){
        copy_opcode(bit_backbone, "110100");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"SH")==0){
        copy_opcode(bit_backbone, "110101");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    else if(strcmp(instruction,"SW")==0){
        copy_opcode(bit_backbone, "110110");
        flags[2] = true; flags[3] = true; flags[4] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"LBUI")==0){
        copy_opcode(bit_backbone, "111000");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"LHUI")==0){
        copy_opcode(bit_backbone, "111001");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"LWI")==0){
        copy_opcode(bit_backbone, "111010");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"SBI")==0){
        copy_opcode(bit_backbone, "111100");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"SHI")==0){
        copy_opcode(bit_backbone, "111101");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    else if(strcmp(instruction,"SWI")==0){
        copy_opcode(bit_backbone, "111110");
        flags[2] = true; flags[3] = true; flags[0] = true;
    }
    //---------------------------------------------------------
    //          Table 3
    //---------------------------------------------------------
    else if(strcmp(instruction,"BR")==0){
        copy_opcode(bit_backbone, "100110");
        flags[4] = true;
    }
    else if(strcmp(instruction,"BRL")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[13] = '1';
        flags[4] = true; flags[2] = true;
    }
    else if(strcmp(instruction,"BRA")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[12] = '1';
        flags[4] = true;
    }
    else if(strcmp(instruction,"BRAL")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[12] = '1';
        bit_backbone[13] = '1';
        flags[4] = true; flags[2] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BEQ")==0){
        copy_opcode(bit_backbone, "100111");
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BNE")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[10] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLT")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[9] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLE")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[9] = '1';
        bit_backbone[10] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGT")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[8] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGE")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[8] = '1';
        bit_backbone[10] = '1';
        flags[4] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BRI")==0){
        copy_opcode(bit_backbone, "101110");
         flags[0] = true;
    }
    else if(strcmp(instruction,"BRLI")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[13] = '1';
        flags[0] = true; flags[2] = true;
    }
    else if(strcmp(instruction,"BRAI")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[12] = '1';
        flags[0] = true;
    }
    else if(strcmp(instruction,"BRALI")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[12] = '1';
        bit_backbone[13] = '1';
        flags[0] = true; flags[2] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BEQI")==0){
        copy_opcode(bit_backbone, "101111");
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BNEI")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[10] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLTI")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[9] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLEI")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[9] = '1';
        bit_backbone[10] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGTI")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[8] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGEI")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[8] = '1';
        bit_backbone[10] = '1';
        flags[0] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    //          Table 4
    //---------------------------------------------------------
    else if(strcmp(instruction,"BRD")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[11] = '1';
         flags[4] = true;
    }
    else if(strcmp(instruction,"BRLD")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[13] = '1';
        bit_backbone[11] = '1';
        flags[4] = true; flags[2] = true;
    }
    else if(strcmp(instruction,"BRAD")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[12] = '1';
        bit_backbone[11] = '1';
        flags[4] = true;
    }
    else if(strcmp(instruction,"BRALD")==0){
        copy_opcode(bit_backbone, "100110");
        bit_backbone[12] = '1';
        bit_backbone[13] = '1';
        bit_backbone[11] = '1';
        flags[4] = true; flags[2] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BEQD")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[6] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BNED")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[10] = '1';
        bit_backbone[6] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLTD")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[9] = '1';
        bit_backbone[6] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLED")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[9] = '1';
        bit_backbone[10] = '1';
        bit_backbone[6] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGTD")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[8] = '1';
        bit_backbone[6] = '1';
        flags[4] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGED")==0){
        copy_opcode(bit_backbone, "100111");
        bit_backbone[8] = '1';
        bit_backbone[10] = '1';
        bit_backbone[6] = '1';
        flags[4] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BRID")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[11] = '1';
        flags[0] = true;
    }
    else if(strcmp(instruction,"BRLID")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[13] = '1';
        bit_backbone[11] = '1';
        flags[0] = true; flags[2] = true;
    }
    else if(strcmp(instruction,"BRAID")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[12] = '1';
        bit_backbone[11] = '1';
        flags[0] = true;
    }
    else if(strcmp(instruction,"BRALID")==0){
        copy_opcode(bit_backbone, "101110");
        bit_backbone[12] = '1';
        bit_backbone[13] = '1';
        bit_backbone[11] = '1';
        flags[0] = true; flags[2] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"BEQID")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BNEID")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[10] = '1';
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLTID")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[9] = '1';
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BLEID")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[9] = '1';
        bit_backbone[10] = '1';
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGTID")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[8] = '1';
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    else if(strcmp(instruction,"BGEID")==0){
        copy_opcode(bit_backbone, "101111");
        bit_backbone[8] = '1';
        bit_backbone[10] = '1';
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    else if(strcmp(instruction,"RTSD")==0){
        copy_opcode(bit_backbone, "101101");
        bit_backbone[6] = '1';
        flags[0] = true; flags[3] = true;
    }
    //---------------------------------------------------------
    //---------------------------------------------------------
    //---------------------------------------------------------
    else{
        printf("\n\nERROR: no corresponding opcode translation!");
        return(false);
    }

    return(true);

}


//Decodes the register, returning its value (or -1, if there was an error)
int decode_register(const char * register_assembly){

    //the first char must be a R
    if(toupper(register_assembly[0]) != 'R'){
        return(-1);
    }

    char reg[3];
    reg[0] = register_assembly[1];
    reg[1] = register_assembly[2];
    reg[2] = register_assembly[3];

    int value = 0;

    //if the 3rd element is a \0  r<32
    if(iscntrl(reg[2])){
        if( isdigit(reg[0]) && isdigit(reg[1])){
            value += reg[1] - 48;
            value += 10 * (reg[0] - 48);
            if(value < 32){
                return(value);
            }
        }
    }

    //if the 2rd element is a \0, r<10
    if(iscntrl(reg[1])){
        if( isdigit(reg[0])){
            value += reg[0] - 48;
            if(value < 10){
                return(value);
            }
        }
    }


    //If it reaches here, an error occurred!
    return(-1);
}


//inputs the decoded register into the binary instruction
void input_register(char * binary_instruction, const int value, const int position){

    int i = 0;

    if(value & 16) binary_instruction[position + i] = '1'; i++;
    if(value & 8) binary_instruction[position + i] = '1'; i++;
    if(value & 4) binary_instruction[position + i] = '1'; i++;
    if(value & 2) binary_instruction[position + i] = '1'; i++;
    if(value & 1) binary_instruction[position + i] = '1';

}


//Decodes and inputs the immediate value
bool decode_input_immediate(char * binary_instruction, const char * immediate_assembly, bool five_imm){

    //16 bit: max value = 32767, min value -32768 [2^(16-1)]
    //5 bit: max value = 15, min value -16 [2^(5-1)]

    // 1 - decodes the number
    int value = 0, i;
    int imm_length = strlen(immediate_assembly);

    if(five_imm){
        //if it has 4 or more characters = error
        if(imm_length >= 4){
            return(false);
        }

        //if it is negative
        if(immediate_assembly[0] == '-'){
            for(i = 1; i < imm_length; i++){
                if(isdigit(immediate_assembly[i])){
                    value += (immediate_assembly[i]-48) * ceil(pow(10,(imm_length-i-1)));
                }
                else{
                    return(false);
                }
            }
            value = -value;
        }
        //otherwise, must be positive
        else{
            for(i = 0; i < imm_length; i++){
                if(isdigit(immediate_assembly[i])){
                    value += (immediate_assembly[i]-48) * ceil(pow(10,(imm_length-i-1)));
                }
                else{
                    return(false);
                }
            }
        }

        if(value > 15 || value < (-16)){
            return(false);
        }


    }
    else{
        //if it has 7 or more characters = error
        if(imm_length >= 7){
            return(false);
        }

        //if it is negative
        if(immediate_assembly[0] == '-'){
            for(i = 1; i < imm_length; i++){
                if(isdigit(immediate_assembly[i])){
                    value += (immediate_assembly[i]-48) * ceil(pow(10,(imm_length-i-1)));
                }
                else{
                    return(false);
                }
            }
            value = -value;
        }
        //otherwise, must be positive
        else{
            for(i = 0; i < imm_length; i++){
                if(isdigit(immediate_assembly[i])){
                    value += (immediate_assembly[i]-48) * ceil(pow(10,(imm_length-i-1)));
                }
                else{
                    return(false);
                }
            }
        }


        if(value > 65535 || value < (-32768)){
            return(false);
        }
        else if(value > 32767){
            printf("\nWARNING: using 16 bit IMM with value above 32767 (can only be used in IMM)");
        }

    }


    // 2 - inputs the number

    if(five_imm){

        //pos = 27 is for the sign
        int position = 28;
        int i = 0;

        if(value >= 0){
            if(value & 8) binary_instruction[position + i] = '1'; i++;
            if(value & 4) binary_instruction[position + i] = '1'; i++;
            if(value & 2) binary_instruction[position + i] = '1'; i++;
            if(value & 1) binary_instruction[position + i] = '1';
        }
        else{
            value = value + 16;
            binary_instruction[27] = '1';
            if(value & 8) binary_instruction[position + i] = '1'; i++;
            if(value & 4) binary_instruction[position + i] = '1'; i++;
            if(value & 2) binary_instruction[position + i] = '1'; i++;
            if(value & 1) binary_instruction[position + i] = '1';
        }

    }
    else{

        //pos = 16 is for the sign
        int position = 17;
        int i = 0;

        if(value >= 0){
            if(value & 32768) binary_instruction[16] = '1';
            if(value & 16384) binary_instruction[position + i] = '1'; i++;
            if(value & 8192) binary_instruction[position + i] = '1'; i++;
            if(value & 4096) binary_instruction[position + i] = '1'; i++;
            if(value & 2048) binary_instruction[position + i] = '1'; i++;
            if(value & 1024) binary_instruction[position + i] = '1'; i++;
            if(value & 512) binary_instruction[position + i] = '1'; i++;
            if(value & 256) binary_instruction[position + i] = '1'; i++;
            if(value & 128) binary_instruction[position + i] = '1'; i++;
            if(value & 64) binary_instruction[position + i] = '1'; i++;
            if(value & 32) binary_instruction[position + i] = '1'; i++;
            if(value & 16) binary_instruction[position + i] = '1'; i++;
            if(value & 8) binary_instruction[position + i] = '1'; i++;
            if(value & 4) binary_instruction[position + i] = '1'; i++;
            if(value & 2) binary_instruction[position + i] = '1'; i++;
            if(value & 1) binary_instruction[position + i] = '1';
        }
        else{
            value = value + 32768;
            binary_instruction[16] = '1';
            if(value & 16384) binary_instruction[position + i] = '1'; i++;
            if(value & 8192) binary_instruction[position + i] = '1'; i++;
            if(value & 4096) binary_instruction[position + i] = '1'; i++;
            if(value & 2048) binary_instruction[position + i] = '1'; i++;
            if(value & 1024) binary_instruction[position + i] = '1'; i++;
            if(value & 512) binary_instruction[position + i] = '1'; i++;
            if(value & 256) binary_instruction[position + i] = '1'; i++;
            if(value & 128) binary_instruction[position + i] = '1'; i++;
            if(value & 64) binary_instruction[position + i] = '1'; i++;
            if(value & 32) binary_instruction[position + i] = '1'; i++;
            if(value & 16) binary_instruction[position + i] = '1'; i++;
            if(value & 8) binary_instruction[position + i] = '1'; i++;
            if(value & 4) binary_instruction[position + i] = '1'; i++;
            if(value & 2) binary_instruction[position + i] = '1'; i++;
            if(value & 1) binary_instruction[position + i] = '1';
        }
    }



    //If it reached here, everything went ok
    return(true);
}


//Interprets the flags and the split assembly, adding the result to the binary instruction
bool flags_decode(char * binary_instruction, char * split_assembly[], const bool * flags){

    //IMPORTANT: char * split_assembly is a pointer with 4 elements to 4 different char strings!

    //bool array with 5 elements
    //0- Imm, 1- Imm5, 2- Rd, 3- Ra, 4- Rb

    int current_split_assembly = 1; //0 was the opcode

    //It always goes like this: RD -> RA -> RB/IMM/IMM5 (mutually exclusive)
    if(flags[2]){

        //decodes the register:
        int RD = decode_register(split_assembly[current_split_assembly]);

        if(RD == -1){
            return(false);
        }

        //inputs the register into the bitstream
        input_register(binary_instruction, RD, 6);

        current_split_assembly++;
    }

    //Now RA
    if(flags[3]){

        //decodes the register:
        int RA = decode_register(split_assembly[current_split_assembly]);

        if(RA == -1){
            return(false);
        }

        //inputs the register into the bitstream
        input_register(binary_instruction, RA, 11);

        current_split_assembly++;
    }

    //Now IMM/RB/IMM5
    if(flags[0]){

        bool error_imm;

        error_imm = decode_input_immediate(binary_instruction, split_assembly[current_split_assembly], false);


        if(error_imm == false){
            return(false);
        }

    }
    else if(flags[4]){

        //decodes the register:
        int RB = decode_register(split_assembly[current_split_assembly]);

        if(RB == -1){
            return(false);
        }

        //inputs the register into the bitstream
        input_register(binary_instruction, RB, 16);

    }
    else if(flags[1]){

        bool error_imm;

        error_imm = decode_input_immediate(binary_instruction, split_assembly[current_split_assembly], true);

        if(error_imm == false){
            return(false);
        }

    }


    //If it reaches here, everything went ok
    return(true);

}


//Performs the assembly ASCII -> pure binary conversion
void assembly_conversion(const char * in_filename, const char * out_filename){

    bool error = true;

    //bool array with 5 elements
    //0- Imm, 1- Imm5, 2- Rd, 3- Ra, 4- Rb
    bool flags[5];

    //32 bit binary instruction variable, to be reused
    char binary_instruction[33];

    //full binary instruction, assuming up to 10000 instructions
    char * binary_content = (char*) malloc(320000 * sizeof(char));
    binary_content[0] = '\0';


    //opens the file with said filename, in read mode
    FILE *f = fopen(in_filename,"r");

    //Defines the buffers
    char assembly[100];
    char *split_assembly[4], *buffer;
    char split_assembly_0[10]; split_assembly[0] = split_assembly_0;
    char split_assembly_1[10]; split_assembly[1] = split_assembly_1;
    char split_assembly_2[10]; split_assembly[2] = split_assembly_2;
    char split_assembly_3[10]; split_assembly[3] = split_assembly_3;


    //Reads the file, line by line
    int i, current_line = 0, len_aux;
    bool dbg = true;
    while(fgets(assembly, 100, f) != 0){

        //increments current_line
        current_line++;

        //1 - splits the assembly into keywords

        //checks for a '\n'. if it exists, remove it
        len_aux = strlen(assembly);
        if(assembly[len_aux-1] == '\n') assembly[len_aux-1] = '\0';

        if(dbg) printf("\nDBG - Read assembly: %s (l=%i)\n", assembly, current_line);

        //does the splitting
        i = 0;
        strcpy(split_assembly[i],strtok(assembly, " ")); //the 1st one is always valid
        if(dbg) printf("T%d: %s| ", i, split_assembly[i]);

        buffer = strtok(0, " ");
        while(buffer != 0){
            i++;
            strcpy(split_assembly[i],buffer);
            buffer = strtok(0, " ");
            if(dbg) printf("T%d: %s| ", i, split_assembly[i]);
        }



        //2 - use the first keyword as opcode
        for(i = 0; i < 5; i++){
            flags[i] = false;
        }
        error = instruction_to_backbone(split_assembly[0], binary_instruction, flags);

        if(error == false){
            printf("\n ERROR: an error occurred decoding the opcode (line = %d)", current_line);
            break;
        }
        if(dbg) printf("Opcode| ");

        //3 - use the flags to decode the rest
        error = flags_decode(binary_instruction, split_assembly, flags);

        if(error == false){
            printf("\n ERROR: an error occurred decoding the register/immediate (line = %d)", current_line);
            break;
        }
        if(dbg) printf("Decode| ");

        //4 - add the decoded instruction into the bitstream
        binary_instruction[32] = '\0';
        strcat(binary_content, binary_instruction);
        if(dbg) printf("Copy| ");

    }

    fclose(f);

    if(error == false){
        free(binary_content);
        return;
    }




    //-------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    //From here on, it's pretty much like the binary ASCII -> pure binary translation
    //(main difference: the source is the "binary_content" variable, not the file)


    int32_t number_of_chars, j;

    number_of_chars = strlen(binary_content);

    if(number_of_chars%32 != 0){
        printf("\nCAUTION - The number of elements in the extended binary file isn't a multiple of 32!\n");
    }


    // Converts 8 ASCII chars (with 0 or 1) into 1 char = 1 byte (don't forget: the MSB is the first to be read!)
    int32_t index;
    int32_t new_size_bytes = number_of_chars/8; //number of bytes of the new string
    char* new_binary = malloc(new_size_bytes * sizeof(char));

    printf("\nConverting into the new format...");

    for(j = 0; j<number_of_chars; j=j+8){
        index = j/8;
        new_binary[index] = 0x00;

        if(binary_content[j] == '1'){
            new_binary[index] = new_binary[index] | 0x80;
        }

        if(binary_content[j+1] == '1'){
            new_binary[index] = new_binary[index] | 0x40;
        }

        if(binary_content[j+2] == '1'){
            new_binary[index] = new_binary[index] | 0x20;
        }

        if(binary_content[j+3] == '1'){
            new_binary[index] = new_binary[index] | 0x10;
        }

        if(binary_content[j+4] == '1'){
            new_binary[index] = new_binary[index] | 0x08;
        }

        if(binary_content[j+5] == '1'){
            new_binary[index] = new_binary[index] | 0x04;
        }

        if(binary_content[j+6] == '1'){
            new_binary[index] = new_binary[index] | 0x02;
        }

        if(binary_content[j+7] == '1'){
            new_binary[index] = new_binary[index] | 0x01;
        }

    }

    printf("\nSaving the result...");


    // Opens the new binary file
    f = fopen(out_filename, "wb");

    // Inputs the new characters (saved as Little Endian, i.e.: 4-3-2-1-8-7-6-5-...)
    char* little_endian_bin = malloc(new_size_bytes * sizeof(char));
    to_LittleEndian(new_binary, little_endian_bin, new_size_bytes);
    fwrite(little_endian_bin, 1, new_size_bytes, f);

    // Closes the new binary file
    fclose(f);


    // Prints double checks (# of bytes, # of instructions (=bytes/4) and the opcodes)
    printf("\n\nDouble checking:");
    printf("\nNumber of bytes: %i (%i instructions)", new_size_bytes, (new_size_bytes/4));


    for(j=0;j<number_of_chars;j=j+32){
        //Extract the opcode, which is in 31:26 -> byte#4 in Little Endian
        printf("\nOpcode %i: ", ((j/32)+1));

        index = (j/8) + 3;
        printf("%s", show_opcode( (little_endian_bin[index] >> 2) & 0x3F )     );

    }




    printf("\n\nDone!\n");

    free(little_endian_bin);
    free(new_binary);
    free(binary_content);
}
