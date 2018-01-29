//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//


#include "instruction_set.h"




//******************************************************************************************************
//NON PIPELINE functions
//(delete this section and complete the run_XXX functions for the pipeline version)

//---------------------------------------------------------------------------------
// get_instruction: gets an instruction from the instruction memory, given the PC
//                      (it also increments the PC!)
//---------------------------------------------------------------------------------
bool get_instruction(char* instruction, char* instruction_memory, int32_t* PC, int32_t max_PC){

    //PC must be smaller than max_PC and a multiple of 4
    if(*PC >= max_PC){
        return(false);
    }
    else if(*PC % 4 != 0){
        return(false);
    }

    int32_t index = *PC;

    instruction[0] = instruction_memory[index+0];
    instruction[1] = instruction_memory[index+1];
    instruction[2] = instruction_memory[index+2];
    instruction[3] = instruction_memory[index+3];

    *PC = *PC +4;

    return(true);
}


//---------------------------------------------------------------------------------
// assign_register: Assigns a real register to an alias, which is a pointer (for example: Rd = &r[25]).
//                  If a register's index is stored x:y (with x > y), starting index = y
//
//    IMPORTANT:   If Rd is atributed to r[0], return false! (because we can't write on r[0])
//---------------------------------------------------------------------------------
bool assign_register(int32_t ** alias, struct Registers * registers, char * bits, int32_t start_index, bool rzero){

    int32_t index;

    index = binary_string_to_int(bits, start_index, 5);

    //alias = pointer to a pointer to an element in the struct
    //printf("\nalias before pointing add=%i (index = %i)", *alias, index);
    *alias = &(*registers).r[index];
    //printf("\nalias r1 add=%i, value=%i", *alias, **alias);

    //If Rd has index == 0
    if(rzero && (index == 0)){
        return(true);
    }
    else{
        return(false);
    }

}


//---------------------------------------------------------------------------------
// run_instruction: the key function.
//                  Takes a 32 bit word instruction as an input and executes it.
//                  Read "lab 1" for the list of instructions
//---------------------------------------------------------------------------------
bool run_instruction(int32_t *PC, char * instruction_bits, struct Registers * registers, char* main_memory, int32_t memory_size, bool print){


    //Auxiliary pointers (not all of them will be used, but they are initialized so as to avoid compiler warnings)
    int32_t *Rd_ptr = 0;
    int32_t *Ra_ptr = 0;
    int32_t *Rb_ptr = 0;
    int32_t **Rd = &Rd_ptr;
    int32_t **Ra = &Ra_ptr;
    int32_t **Rb = &Rb_ptr;

    bool *I = &registers->MSR[0];
    bool *C = &registers->MSR[1];
    uint16_t *RIMM = &registers->rIMM;
    bool write_rzero= false;



    // ASSUMING THAT THE OPCODE IS IN THE 31:26
    // (reversing order, because the chars are read 31 -> 26 [see Lab1.pdf])
    char opcode[7];
    opcode[6]='\0'; // -> throughout the remaining of the code, the opcode will be used as a string!

    int32_t i;
    for(i=0;i<6;i++){
        opcode[i] = instruction_bits[31-i];
    }

    //printf("\nRunning instruction: PC = %i, opcode=%s", (*PC-4), opcode);

    //========================================================================================
    //          TABLE 1
    //========================================================================================
    //ADD Rd,Ra,Rb - opcode = 000000, Rd 25:21, Ra 20:16, Rb 15:11
    if(strcmp(opcode, "000000") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }


        int64_t temp = **Rb;
        temp += **Ra;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //RSUB Rd,Ra,Rb - opcode = 000001, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000001") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t temp = **Rb;
        temp += ~(**Ra) +1;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //ADDC Rd,Ra,Rb - opcode = 000010, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000010") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }


        int64_t temp = **Rb;
        temp += **Ra;

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //RSUBC Rd,Ra,Rb - opcode = 000011, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000011") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t temp = **Rb;
        temp += ~(**Ra);

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //ADDK Rd,Ra,Rb - opcode = 000100, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000100") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t temp = **Rb;
        temp += **Ra;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);


        *I = false; //By default, when it is not used

    }
    //RSUBK Rd,Ra,Rb - opcode = 000101, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000101") == 0 && instruction_bits[0] == '0' && instruction_bits[1] == '0'){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t temp = **Rb;
        temp += ~(**Ra) +1;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //ADDKC Rd,Ra,Rb - opcode = 000110, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000110") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t temp = **Rb;
        temp += **Ra;

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //RSUBKC Rd,Ra,Rb - opcode = 000111, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "000111") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t temp = **Rb;
        temp += ~(**Ra);

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //CMP Rd,Ra,Rb - opcode = 000101, Rd 25:21, Ra 20:16, Rb 15:11 (bit 0 = 1)
    else if(strcmp(opcode, "000101") == 0 && instruction_bits[0] == '1' && instruction_bits[1] == '0'){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = **Rb + ~(**Ra) +1;

        if(**Rb >= **Ra){
            //Rd[31] = 0     <- doing an "and" with 011111... = 7F FF FF FF
            **Rd = **Rd & 0x7FFFFFFF;
        }
        else{
            //Rd[31] = 1     <- doing an "or" with 10000... = 80 00 00 00
            **Rd = **Rd | 0x80000000;
        }

        *I = false; //By default, when it is not used

    }
    //CMPU Rd,Ra,Rb - opcode = 000101, Rd 25:21, Ra 20:16, Rb 15:11 (bits 0 and 1 = 1)
    else if(strcmp(opcode, "000101") == 0 && instruction_bits[0] == '1' && instruction_bits[1] == '1'){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = **Rb + ~(**Ra) +1;

        //signed -> unsigned: if the value is positive, it is unchanged; otherwise, adds UINT_MAX+1
        //(which is the same as treating the binary content of the integer as unsigned)
        uint32_t Ra_U = (uint32_t) **Ra;
        uint32_t Rb_U = (uint32_t) **Rb;

        if(Rb_U >= Ra_U){
            //Rd[31] = 0     <- doing an "and" with 011111... = 7F FF FF FF
            **Rd = **Rd & 0x7FFFFFFF;
        }
        else{
            //Rd[31] = 1     <- doing an "or" with 10000... = 80 00 00 00
            **Rd = **Rd | 0x80000000;
        }

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //ADDI Rd,Ra,IMM - opcode = 001000, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001000") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += **Ra;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //RSUBI Rd,Ra,IMM - opcode = 001001, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001001") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += ~(**Ra) +1;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //ADDIC Rd,Ra,IMM - opcode = 001010, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001010") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += **Ra;

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);


        *I = false; //By default, when it is not used

    }
    //RSUBIC Rd,Ra,IMM - opcode = 001011, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001011") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += ~(**Ra);

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        *C = check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //ADDIK Rd,Ra,IMM - opcode = 001100, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001100") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += **Ra;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //RSUBIK Rd,Ra,IMM - opcode = 001101, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001101") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += ~(**Ra) +1;

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //ADDICK Rd,Ra,IMM - opcode = 001110, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001110") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += **Ra;

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);


        *I = false; //By default, when it is not used

    }
    //RSUBICK Rd,Ra,IMM - opcode = 001111, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "001111") == 0){

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int64_t temp = IMM;
        temp += ~(**Ra);

        if(*C){
            temp = temp + 1;
        }

        //Rd <- temp, if there is no overflow. the value is corrected if there is any overflow
        check_overflow(temp, *Rd);

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //MUL Rd,Ra,Rb - opcode = 010000, Rd 25:21, Ra 20:16, Rb 15:11 (bits 0 and 1 = 0)
    else if(strcmp(opcode, "010000") == 0 && instruction_bits[1] == '0' && instruction_bits[0] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t mult;
        mult = **Rb;
        mult *= **Ra;

        // Gets the lower bits
        **Rd = (int32_t)mult;

        *I = false; //By default, when it is not used

    }
    //MULH Rd,Ra,Rb - opcode = 010000, Rd 25:21, Ra 20:16, Rb 15:11 (bit 1 = 0, bit 0 = 1)
    else if(strcmp(opcode, "010000") == 0 && instruction_bits[1] == '0' && instruction_bits[0] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int64_t mult;
        mult = **Rb;
        mult *= **Ra;

        // Gets the higher bits
        **Rd = (int32_t)(mult >> 32);

        *I = false; //By default, when it is not used

    }
    //MULHU Rd,Ra,Rb - opcode = 010000, Rd 25:21, Ra 20:16, Rb 15:11 (bit 1 = 1, bit 0 = 1)
    else if(strcmp(opcode, "010000") == 0 && instruction_bits[1] == '1' && instruction_bits[0] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        //signed -> unsigned: if the value is positive, it is unchanged; otherwise, adds UINT_MAX+1
        //(which is the same as treating the binary content of the integer as unsigned)
        uint32_t Ra_U = (uint32_t) **Ra;
        uint32_t Rb_U = (uint32_t) **Rb;

        int64_t mult;
        mult = Rb_U;
        mult *= Ra_U;

        // Gets the higher bits
        **Rd = (int32_t)(mult >> 32);

        *I = false; //By default, when it is not used

    }
    //MULHSU Rd,Ra,Rb - opcode = 010000, Rd 25:21, Ra 20:16, Rb 15:11 (bit 1 = 1, bit 0 = 0)
    else if(strcmp(opcode, "010000") == 0 && instruction_bits[1] == '1' && instruction_bits[0] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        //signed -> unsigned: if the value is positive, it is unchanged; otherwise, adds UINT_MAX+1
        //(which is the same as treating the binary content of the integer as unsigned)
        uint32_t Rb_U = (uint32_t) **Rb;

        int64_t mult;
        mult = Rb_U;
        mult *= **Ra;

        // Gets the higher bits
        **Rd = (int32_t)(mult >> 32);

        *I = false; //By default, when it is not used

    }
    //MULI Rd,Ra,IMM - opcode = 011000, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "011000") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);


        if(write_rzero){
            return(false);
        }


        int64_t mult;
        mult = IMM;
        mult *= **Ra;

        // Gets the lower bits
        **Rd = (int32_t)(mult);

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //BSRA Rd,Ra,Rb - opcode = 010001, Rd 25:21, Ra 20:16, Rb 15:11 (bit 10 = 0, bit 9 = 1)
    else if(strcmp(opcode, "010001") == 0 && instruction_bits[10] == '0' && instruction_bits[9] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        //Rb[4:0] to unsigned integer ("and" with 00...0011111)
        uint32_t shift_amount = **Rb & 0x0000001F;

        // If Ra is negative, the arithmetic shift must be padded with 1s
        // This is done by " value | ~(~0U >> shift_amount)"  [the inverse of a set of 1s, right shifted by "shift amount"]
        if (**Ra < 0)
            **Rd = (**Ra >> shift_amount) | (~(~0U >> shift_amount));
        else
            **Rd = (**Ra >> shift_amount);

        *I = false; //By default, when it is not used

    }
    //BSLA Rd,Ra,Rb - opcode = 010001, Rd 25:21, Ra 20:16, Rb 15:11 (bit 10 = 1, bit 9 = 1)
    else if(strcmp(opcode, "010001") == 0 && instruction_bits[10] == '1' && instruction_bits[9] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        //Rb[4:0] to unsigned integer ("and" with 00...0011111)
        uint32_t shift_amount = **Rb & 0x0000001F;

        // If Ra is negative, the MSB must be set to 1
        // else, the MSB must be set to 0
        if (**Ra < 0)
            **Rd = (**Ra << shift_amount) | (0x80000000);
        else
            **Rd = (**Ra << shift_amount) & (0x7FFFFFFF);

        *I = false; //By default, when it is not used

    }
    //BSRL Rd,Ra,Rb - opcode = 010001, Rd 25:21, Ra 20:16, Rb 15:11 (bit 10 = 0, bit 9 = 0)
    else if(strcmp(opcode, "010001") == 0 && instruction_bits[10] == '0' && instruction_bits[9] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        //Rb[4:0] to unsigned integer ("and" with 00...0011111)
        uint32_t shift_amount = **Rb & 0x0000001F;

        //Logical right-shift
        **Rd = (int32_t)((uint32_t) **Ra >> shift_amount);

        *I = false; //By default, when it is not used

    }
    //BSLL Rd,Ra,Rb - opcode = 010001, Rd 25:21, Ra 20:16, Rb 15:11 (bit 10 = 1, bit 9 = 0)
    else if(strcmp(opcode, "010001") == 0 && instruction_bits[10] == '1' && instruction_bits[9] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        //Rb[4:0] to unsigned integer ("and" with 00...0011111)
        uint32_t shift_amount = **Rb & 0x0000001F;

        //Logical left-shift
        **Rd = (int32_t)((uint32_t) **Ra << shift_amount);

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //BSRAI Rd,Ra,Rb - opcode = 011001, Rd 25:21, Ra 20:16, IMM 4:0 (bit 10 = 0, bit 9 = 1)
    else if(strcmp(opcode, "011001") == 0 && instruction_bits[10] == '0' && instruction_bits[9] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 5); //as it only 5 bits, it is an unsigned for sure

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = (uint32_t) IMM;

        // If Ra is negative, the arithmetic shift must be padded with 1s
        // This is done by " value | ~(~0U >> shift_amount)"  [the inverse of a set of 1s, right shifted by "shift amount"]
        if (**Ra < 0)
            **Rd = (**Ra >> shift_amount) | (~(~0U >> shift_amount));
        else
            **Rd = (**Ra >> shift_amount);

        *I = false; //By default, when it is not used

    }
    //BSLAI Rd,Ra,Rb - opcode = 011001, Rd 25:21, Ra 20:16, IMM 4:0 (bit 10 = 1, bit 9 = 1)
    else if(strcmp(opcode, "011001") == 0 && instruction_bits[10] == '1' && instruction_bits[9] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 5); //as it only 5 bits, it is an unsigned for sure

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = (uint32_t) IMM;

        // If Ra is negative, the MSB must be set to 1
        // else, the MSB must be set to 0
        if (**Ra < 0)
            **Rd = (**Ra << shift_amount) | (0x80000000);
        else
            **Rd = (**Ra << shift_amount) & (0x7FFFFFFF);

        *I = false; //By default, when it is not used

    }
    //BSRLI Rd,Ra,Rb - opcode = 011001, Rd 25:21, Ra 20:16, IMM 4:0 (bit 10 = 0, bit 9 = 0)
    else if(strcmp(opcode, "011001") == 0 && instruction_bits[10] == '0' && instruction_bits[9] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 5); //as it only 5 bits, it is an unsigned for sure

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = (uint32_t) IMM;

        //Logical right-shift
        **Rd = (int32_t)((uint32_t) **Ra >> shift_amount);

        *I = false; //By default, when it is not used

    }
    //BSLLI Rd,Ra,Rb - opcode = 011001, Rd 25:21, Ra 20:16, IMM 4:0 (bit 10 = 1, bit 9 = 0)
    else if(strcmp(opcode, "011001") == 0 && instruction_bits[10] == '1' && instruction_bits[9] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 5); //as it only 5 bits, it is an unsigned for sure

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = (uint32_t) IMM;

        //Logical left-shift
        **Rd = (int32_t)((uint32_t) **Ra << shift_amount);

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //OR Rd,Ra,Rb - opcode = 100000, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "100000") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = **Rb | **Ra;
        *I = false; //By default, when it is not used

    }
    //AND Rd,Ra,Rb - opcode = 100001, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "100001") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = **Rb & **Ra;
        *I = false; //By default, when it is not used

    }
    //XOR Rd,Ra,Rb - opcode = 100010, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "100010") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = **Rb ^ **Ra;
        *I = false; //By default, when it is not used

    }
    //ANDN Rd,Ra,Rb - opcode = 100011, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "100011") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = **Ra & ~(**Rb);
        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //SRA Rd,Ra - opcode = 100100, Rd 25:21, Ra 20:16  (bit 6 = 0, bit 5 = 0, bit 4 = 0, bit 0 = 1)
    else if(strcmp(opcode, "100100") == 0 && instruction_bits[6] == '0' && instruction_bits[5] == '0'
                                          && instruction_bits[4] == '0' && instruction_bits[0] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = 1;

        //Arithmetic right-shift
        if (**Ra < 0){
            **Rd = (**Ra >> shift_amount) | (~(~0U >> shift_amount));
        }
        else{
            **Rd = (**Ra >> shift_amount);
        }

        //If Ra[0] == 1, C = 1
        int32_t Ra_0 = **Ra & 0x00000001;
        if(Ra_0 == 1){
            *C = true;
        }
        else{
            *C = false;
        }


        *I = false; //By default, when it is not used

    }
    //SRC Rd,Ra - opcode = 100100, Rd 25:21, Ra 20:16  (bit 6 = 0, bit 5 = 1, bit 4 = 0, bit 0 = 1)
    else if(strcmp(opcode, "100100") == 0 && instruction_bits[6] == '0' && instruction_bits[5] == '1'
                                          && instruction_bits[4] == '0' && instruction_bits[0] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = 1;

        //Logical right-shift
        **Rd = (int32_t)((uint32_t) **Ra >> shift_amount);

        //Rd[31] <--- MSR[C]
        if(*C){
            **Rd = **Rd | 0x80000000;
        }
        else{
            **Rd = **Rd & 0x7FFFFFFF;
        }

        //If Ra[0] == 1, C = 1
        int32_t Ra_0 = **Ra & 0x00000001;
        if(Ra_0 == 1){
            *C = true;
        }
        else{
            *C = false;
        }


        *I = false; //By default, when it is not used

    }
    //SRL Rd,Ra - opcode = 100100, Rd 25:21, Ra 20:16  (bit 6 = 1, bit 5 = 0, bit 4 = 0, bit 0 = 1)
    else if(strcmp(opcode, "100100") == 0 && instruction_bits[6] == '1' && instruction_bits[5] == '0'
                                          && instruction_bits[4] == '0' && instruction_bits[0] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);

        if(write_rzero){
            return(false);
        }

        uint32_t shift_amount = 1;

        //Logical right-shift
        **Rd = (int32_t)((uint32_t) **Ra >> shift_amount);

        //If Ra[0] == 1, C = 1
        int32_t Ra_0 = **Ra & 0x00000001;
        if(Ra_0 == 1){
            *C = true;
        }
        else{
            *C = false;
        }


        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //SEXT8 Rd,Ra - opcode = 100100, Rd 25:21, Ra 20:16  (bit 6 = 1, bit 5 = 1, bit 4 = 0, bit 0 = 0)
    else if(strcmp(opcode, "100100") == 0 && instruction_bits[6] == '1' && instruction_bits[5] == '1'
                                          && instruction_bits[4] == '0' && instruction_bits[0] == '0'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);

        if(write_rzero){
            return(false);
        }

        //If Ra 7:0 is negative (Ra[7] = 1), Rd must be left-padded with 1s
        //(int32_t variable >> k) & 0x00000001 -> extracts the kth bit from char_variable
        int32_t sign = (**Ra >> 7) & 0x00000001;

        if(sign == 1){
            **Rd = **Ra | 0xFFFFFF00;
        }
        else{
            **Rd = **Ra & 0x000000FF;
        }

        *I = false; //By default, when it is not used

    }
    //SEXT16 Rd,Ra - opcode = 100100, Rd 25:21, Ra 20:16  (bit 6 = 1, bit 5 = 1, bit 4 = 0, bit 0 = 1)
    else if(strcmp(opcode, "100100") == 0 && instruction_bits[6] == '1' && instruction_bits[5] == '1'
                                          && instruction_bits[4] == '0' && instruction_bits[0] == '1'){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);

        if(write_rzero){
            return(false);
        }

        //If Ra 15:0 is negative (Ra[15] = 1), Rd must be left-padded with 1s
        //(int32_t variable >> k) & 0x00000001 -> extracts the kth bit from char_variable
        int32_t sign = (**Ra >> 15) & 0x00000001;

        if(sign == 1){
            **Rd = **Ra | 0xFFFF0000;
        }
        else{
            **Rd = **Ra & 0x0000FFFF;
        }

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //ORI Rd,Ra,IMM - opcode = 101000, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "101000") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        **Rd = **Ra | IMM;
        *I = false; //By default, when it is not used

    }
    //ANDI Rd,Ra,IMM - opcode = 101001, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "101001") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        **Rd = **Ra & IMM;
        *I = false; //By default, when it is not used

    }
    //XORI Rd,Ra,IMM - opcode = 101010, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "101010") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        **Rd = **Ra ^ IMM;
        *I = false; //By default, when it is not used

    }
    //ANDNI Rd,Ra,IMM - opcode = 101011, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "101011") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        **Rd = **Ra & ~(IMM);
        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //IMM IMM - opcode = 101100,  IMM 15:0
    else if(strcmp(opcode, "101100") == 0){

        //Load the IMM into 16 unsigned int (in this instruction, the flag I is ignored)
        *RIMM = (uint16_t)binary_string_to_int(instruction_bits, 0, 16);

        /*//If IMM[15] is 1 (i.e., uint16 > 32768),
        if(IMM >= 32768){
            //Will make the cast without any conversion and manually set the sign to 1
            IMM = IMM - 32768;
            *RIMM = (int16_t)IMM;
            *RIMM = *RIMM | 0x8000;
        }
        else{
            //Otherwise, can make the cast directly
            *RIMM = (int16_t)IMM;
        }*/

        *I = true; //The exception!

    }
    //========================================================================================
    //          TABLE 2
    //========================================================================================
    //Memory considerations: the memory size is expressed in Bytes. However, the words are 4 bytes longs, and this ISA
    // doesn't allow for misaligned loads. This means that when an half-word is loaded/written, the address must be a multiple
    // of 2 and, when a word is loaded/written, the address must be a multiple of 4.

    //Little Endian: "In little endian, you store the least significant byte in the smallest address."
    //Example: if we want to store 4 bytes, "90 AB 12 CD" (1 byte = 2 hex characters), it should looks like this
    // Address     Value
    //      0       CD      <--- least significant byte
    //      1       12
    //      2       AB
    //      3       90      <--- most significant byte
    //
    // bit storage (reading left to right):  7:0   15:8   23:16   31:24
    //                                       CD     12      AB      90              <----- they're kinda backwards!

    //LBU Rd,Ra,Rb - opcode = 110000, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "110000") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + **Rb;

        if(address > memory_size){
            return(false);
        }

        int32_t Byte_0 = ((int32_t) main_memory[address]) & 0x000000FF;

        **Rd = Byte_0;

        *I = false; //By default, when it is not used

    }

    //LHU Rd,Ra,Rb - opcode = 110001, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "110001") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + **Rb;

        if(address > memory_size){
            return(false);
        }

        //Checks for alignment
        if((address % 2) != 0){
            return(false);
        }

        int32_t Byte_0 = ((int32_t) main_memory[address]) & 0x000000FF;
        int32_t Byte_1 = (((int32_t) main_memory[address+1]) & 0x000000FF) << 8;

        **Rd = Byte_0 | Byte_1;

        *I = false; //By default, when it is not used

    }
    //LW Rd,Ra,Rb - opcode = 110010, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "110010") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + **Rb;

        if(address > memory_size){
            return(false);
        }

        //Checks for alignment
        if((address % 4) != 0){
            return(false);
        }

        int32_t Byte_0 = ((int32_t) main_memory[address]) & 0x000000FF;
        int32_t Byte_1 = (((int32_t) main_memory[address+1]) & 0x000000FF) << 8;
        int32_t Byte_2 = (((int32_t) main_memory[address+2]) & 0x000000FF) << 16;
        int32_t Byte_3 = (((int32_t) main_memory[address+3]) & 0x000000FF) << 24;

        **Rd = Byte_0 | Byte_1 | Byte_2 | Byte_3;

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //SB Rd,Ra,Rb - opcode = 110100, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "110100") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + **Rb;

        int32_t Byte_0 = **Rd & 0x000000FF;

        //If address == 0xFFFFFFC0, redirects to std out
        if(address == 0xFFFFFFC0 && print){
            print_mem_write(*PC, Byte_0, -1);
        }
        else{
            if(address > memory_size){
                return(false);
            }

            main_memory[address] = (char)Byte_0;
        }

        *I = false; //By default, when it is not used

    }

    //SH Rd,Ra,Rb - opcode = 110101, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "110101") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + **Rb;


        int32_t Byte_0 = **Rd & 0x000000FF;
        int32_t Byte_1 = (**Rd & 0x0000FF00) >> 8;

        //If address == 0xFFFFFFC0, redirects to std out
        if(address == 0xFFFFFFC0 && print){
            print_mem_write(*PC, Byte_0, -1);
            print_mem_write(*PC, Byte_1, -1);
        }
        else{
            if(address > memory_size){
                return(false);
            }

            //Checks for alignment
            if((address % 2) != 0){
                return(false);
            }

            main_memory[address] = (char)Byte_0;
            main_memory[address+1] = (char)Byte_1;
        }


        *I = false; //By default, when it is not used

    }
    //SW Rd,Ra,Rb - opcode = 110110, Rd 25:21, Ra 20:16, Rb 15:11
    else if(strcmp(opcode, "110110") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + **Rb;

        int32_t Byte_0 = **Rd & 0x000000FF;
        int32_t Byte_1 = (**Rd & 0x0000FF00) >> 8;
        int32_t Byte_2 = (**Rd & 0x00FF0000) >> 16;
        int32_t Byte_3 = (**Rd & 0xFF000000) >> 24;

        //If address == 0xFFFFFFC0, redirects to std out
        if(address == 0xFFFFFFC0 && print){
            print_mem_write(*PC, Byte_0, -1);
            print_mem_write(*PC, Byte_1, -1);
            print_mem_write(*PC, Byte_2, -1);
            print_mem_write(*PC, Byte_3, -1);
        }
        else{
            if(address > memory_size){
                return(false);
            }

            //Checks for alignment
            if((address % 4) != 0){
                return(false);
            }

            main_memory[address] = (char)Byte_0;
            main_memory[address+1] = (char)Byte_1;
            main_memory[address+2] = (char)Byte_2;
            main_memory[address+3] = (char)Byte_3;
        }

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //LBUI Rd,Ra,IMM - opcode = 111000, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "111000") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + IMM;

        if(address > memory_size){
            return(false);
        }

        int32_t Byte_0 = ((int32_t) main_memory[address]) & 0x000000FF;

        **Rd = Byte_0;

        *I = false; //By default, when it is not used

    }

    //LHUI Rd,Ra,IMM - opcode = 111001, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "111001") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + IMM;

        if(address > memory_size){
            return(false);
        }

        //Checks for alignment
        if((address % 2) != 0){
            return(false);
        }

        int32_t Byte_0 = ((int32_t) main_memory[address]) & 0x000000FF;
        int32_t Byte_1 = (((int32_t) main_memory[address+1]) & 0x000000FF) << 8;

        **Rd = Byte_0 | Byte_1;

        *I = false; //By default, when it is not used

    }
    //LWI Rd,Ra,IMM - opcode = 111010, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "111010") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + IMM;

        if(address > memory_size){
            return(false);
        }

        //Checks for alignment
        if((address % 4) != 0){
            return(false);
        }

        int32_t Byte_0 = ((int32_t) main_memory[address]) & 0x000000FF;
        int32_t Byte_1 = (((int32_t) main_memory[address+1]) & 0x000000FF) << 8;
        int32_t Byte_2 = (((int32_t) main_memory[address+2]) & 0x000000FF) << 16;
        int32_t Byte_3 = (((int32_t) main_memory[address+3]) & 0x000000FF) << 24;

        **Rd = Byte_0 | Byte_1 | Byte_2 | Byte_3;

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //SBI Rd,Ra,IMM - opcode = 110100, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "111100") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + IMM;

        int32_t Byte_0 = **Rd & 0x000000FF;

        //If address == 0xFFFFFFC0, redirects to std out
        if(address == 0xFFFFFFC0 && print){
            print_mem_write(*PC, Byte_0, -1);
        }
        else{
            if(address > memory_size){
                return(false);
            }

            main_memory[address] = (char)Byte_0;
        }

        *I = false; //By default, when it is not used

    }

    //SHI Rd,Ra,IMM - opcode = 111101, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "111101") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + IMM;

        int32_t Byte_0 = **Rd & 0x000000FF;
        int32_t Byte_1 = (**Rd & 0x0000FF00) >> 8;

        //If address == 0xFFFFFFC0, redirects to std out
        if(address == 0xFFFFFFC0 && print){
            print_mem_write(*PC, Byte_0, -1);
            print_mem_write(*PC, Byte_1, -1);
        }
        else{
            if(address > memory_size){
                return(false);
            }

            //Checks for alignment
            if((address % 2) != 0){
                return(false);
            }

            main_memory[address] = (char)Byte_0;
            main_memory[address+1] = (char)Byte_1;
        }

        *I = false; //By default, when it is not used

    }
    //SWI Rd,Ra,IMM - opcode = 111110, Rd 25:21, Ra 20:16, IMM 15:0
    else if(strcmp(opcode, "111110") == 0){
        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        int32_t address = **Ra + IMM;

        int32_t Byte_0 = **Rd & 0x000000FF;
        int32_t Byte_1 = (**Rd & 0x0000FF00) >> 8;
        int32_t Byte_2 = (**Rd & 0x00FF0000) >> 16;
        int32_t Byte_3 = (**Rd & 0xFF000000) >> 24;

        //If address == 0xFFFFFFC0, redirects to std out
        if(address == 0xFFFFFFC0 && print){
            print_mem_write(*PC, Byte_0, -1);
            print_mem_write(*PC, Byte_1, -1);
            print_mem_write(*PC, Byte_2, -1);
            print_mem_write(*PC, Byte_3, -1);
        }
        else{
            if(address > memory_size){
                return(false);
            }

            //Checks for alignment
            if((address % 4) != 0){
                return(false);
            }

            main_memory[address] = (char)Byte_0;
            main_memory[address+1] = (char)Byte_1;
            main_memory[address+2] = (char)Byte_2;
            main_memory[address+3] = (char)Byte_3;
        }


        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //          TABLE 3 and TABLE 4
    //========================================================================================
    //BR/BRD Rb - opcode = 100110, Rb 15:11 (bit 19 = 0, bit 18 = 0)
    else if(strcmp(opcode, "100110") == 0 && instruction_bits[19] == '0' && instruction_bits[18] == '0'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        assign_register(Rb, registers, instruction_bits, 11, false);

        *PC = *PC + **Rb;

        *I = false; //By default, when it is not used

    }
    //BRL/BRLD Rd,Rb - opcode = 100110, Rd 25:21 Rb 15:11 (bit 19 = 0, bit 18 = 1)
    else if(strcmp(opcode, "100110") == 0 && instruction_bits[19] == '0' && instruction_bits[18] == '1'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = *PC;
        *PC = *PC + **Rb;

        *I = false; //By default, when it is not used

    }
    //BRA/BRAD Rb - opcode = 100110, Rb 15:11 (bit 19 = 1, bit 18 = 0)
    else if(strcmp(opcode, "100110") == 0 && instruction_bits[19] == '1' && instruction_bits[18] == '0'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        assign_register(Rb, registers, instruction_bits, 11, false);

        *PC = **Rb;

        *I = false; //By default, when it is not used

    }
    //BRAL/BRALD Rd,Rb - opcode = 100110, Rd 25:21 Rb 15:11 (bit 19 = 1, bit 18 = 1)
    else if(strcmp(opcode, "100110") == 0 && instruction_bits[19] == '1' && instruction_bits[18] == '1'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(write_rzero){
            return(false);
        }

        **Rd = *PC;
        *PC = **Rb;

        *I = false; //By default, when it is not used

    }
    //========================================================================================
    //BEQ/BEQD Ra,Rb - opcode = 100111, Rd 20:16 Rb 15:11 (bit 23 = 0, bit 22 = 0, bit 21 = 0)
    else if(strcmp(opcode, "100111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '0' && instruction_bits[21] == '0'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(**Ra == 0){
            *PC = *PC + **Rb;
        }

        *I = false; //By default, when it is not used
    }
    //BNE/BNED Ra,Rb - opcode = 100111, Rd 20:16 Rb 15:11 (bit 23 = 0, bit 22 = 0, bit 21 = 1)
    else if(strcmp(opcode, "100111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '0' && instruction_bits[21] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(**Ra != 0){
            *PC = *PC + **Rb;
        }

        *I = false; //By default, when it is not used
    }
    //BLT/BLTD Ra,Rb - opcode = 100111, Rd 20:16 Rb 15:11 (bit 23 = 0, bit 22 = 1, bit 21 = 0)
    else if(strcmp(opcode, "100111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '1' && instruction_bits[21] == '0'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(**Ra < 0){
            *PC = *PC + **Rb;
        }

        *I = false; //By default, when it is not used
    }
    //BLE/BLED Ra,Rb - opcode = 100111, Rd 20:16 Rb 15:11 (bit 23 = 0, bit 22 = 1, bit 21 = 1)
    else if(strcmp(opcode, "100111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '1' && instruction_bits[21] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(**Ra <= 0){
            *PC = *PC + **Rb;
        }

        *I = false; //By default, when it is not used
    }
    //BGT/BGTD Ra,Rb - opcode = 100111, Rd 20:16 Rb 15:11 (bit 23 = 1, bit 22 = 0, bit 21 = 0)
    else if(strcmp(opcode, "100111") == 0 && instruction_bits[23] == '1' && instruction_bits[22] == '0' && instruction_bits[21] == '0'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(**Ra > 0){
            *PC = *PC + **Rb;
        }

        *I = false; //By default, when it is not used
    }
    //BGE/BGED Ra,Rb - opcode = 100111, Rd 20:16 Rb 15:11 (bit 23 = 1, bit 22 = 0, bit 21 = 1)
    else if(strcmp(opcode, "100111") == 0 && instruction_bits[23] == '1' && instruction_bits[22] == '0' && instruction_bits[21] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        assign_register(Rb, registers, instruction_bits, 11, false);

        if(**Ra >= 0){
            *PC = *PC + **Rb;
        }

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //BRI/BRID IMM - opcode = 101110, IMM 15:0 (bit 19 = 0, bit 18 = 0)
    else if(strcmp(opcode, "101110") == 0 && instruction_bits[19] == '0' && instruction_bits[18] == '0'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        *PC = *PC + IMM;

        *I = false; //By default, when it is not used
    }
    //BRLI/BRLID Rd, IMM - opcode = 101110, Rd 25:21 IMM 15:0 (bit 19 = 0, bit 18 = 1)
    else if(strcmp(opcode, "101110") == 0 && instruction_bits[19] == '0' && instruction_bits[18] == '1'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        **Rd = *PC;
        *PC = *PC + IMM;

        *I = false; //By default, when it is not used
    }
    //BRAI/BRAID IMM - opcode = 101110, IMM 15:0 (bit 19 = 1, bit 18 = 0)
    else if(strcmp(opcode, "101110") == 0 && instruction_bits[19] == '1' && instruction_bits[18] == '0'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        *PC = IMM;

        *I = false; //By default, when it is not used
    }
    //BRALI/BRALID Rd, IMM - opcode = 101110, Rd 25:21 IMM 15:0 (bit 19 = 1, bit 18 = 1)
    else if(strcmp(opcode, "101110") == 0 && instruction_bits[19] == '1' && instruction_bits[18] == '1'){

        if(instruction_bits[20] == '1'){
            //DELAYED
        }

        write_rzero = assign_register(Rd, registers, instruction_bits, 21, true);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(write_rzero){
            return(false);
        }

        **Rd = *PC;
        *PC = IMM;

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //BEQI/BEQID Ra, IMM - opcode = 101111, Rd 20:16 IMM 15:0 (bit 23 = 0, bit 22 = 0, bit 21 = 0)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '0' && instruction_bits[21] == '0'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(**Ra == 0){
            *PC = *PC + IMM;
        }

        *I = false; //By default, when it is not used
    }
    //BNEI/BNEID Ra, IMM - opcode = 101111, Rd 20:16 IMM 15:0 (bit 23 = 0, bit 22 = 0, bit 21 = 1)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '0' && instruction_bits[21] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(**Ra != 0){
            *PC = *PC + IMM;
        }

        *I = false; //By default, when it is not used
    }
    //BLTI/BLTID Ra, IMM - opcode = 101111, Rd 20:16 IMM 15:0 (bit 23 = 0, bit 22 = 1, bit 21 = 0)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '1' && instruction_bits[21] == '0'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(**Ra < 0){
            *PC = *PC + IMM;
        }

        *I = false; //By default, when it is not used
    }
    //BLEI/BLEID Ra, IMM - opcode = 101111, Rd 20:16 IMM 15:0 (bit 23 = 0, bit 22 = 1, bit 21 = 1)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[23] == '0' && instruction_bits[22] == '1' && instruction_bits[21] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(**Ra <= 0){
            *PC = *PC + IMM;
        }

        *I = false; //By default, when it is not used
    }
    //BGTI/BGTID Ra, IMM - opcode = 101111, Rd 20:16 IMM 15:0 (bit 23 = 1, bit 22 = 0, bit 21 = 0)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[23] == '1' && instruction_bits[22] == '0' && instruction_bits[21] == '0'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(**Ra > 0){
            *PC = *PC + IMM;
        }

        *I = false; //By default, when it is not used
    }
    //BGEI/BGEID Ra, IMM - opcode = 101111, Rd 20:16 IMM 15:0 (bit 23 = 1, bit 22 = 0, bit 21 = 1)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[23] == '1' && instruction_bits[22] == '0' && instruction_bits[21] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        if(**Ra >= 0){
            *PC = *PC + IMM;
        }

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //RSTD Ra, IMM - opcode = 101101, Rd 20:16 IMM 15:0 (bit 25 = 1)
    else if(strcmp(opcode, "101111") == 0 && instruction_bits[25] == '1'){

        if(instruction_bits[25] == '1'){
            //DELAYED
        }

        assign_register(Ra, registers, instruction_bits, 16, false);
        int32_t IMM = immediate_load(registers, instruction_bits, 0, 16);

        *PC = **Ra + IMM;

        *I = false; //By default, when it is not used
    }
    //========================================================================================
    //If it's not on the list -> error
    else{
        return(false);
    }



    return(true);
}



//******************************************************************************************************


//---------------------------------------------------------------------------------
// char_to_bits: Quick tool to printf bits, to be used as "printf("%s", char_to_bits(char))"
//---------------------------------------------------------------------------------
const char *char_to_bits(char x){
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}


//---------------------------------------------------------------------------------
// extract_bits: extracts the bits from a 32 bit word, storing them in a char array
//                  (Little Endian format -> the least significant byte in the smallest address)
//---------------------------------------------------------------------------------
void extract_bits(const char * instruction, char * bits){

    //Example:    char value   bit address [7]    [0]
    //              AA        ->            10101010

    //Since it is Little Endian: [keeps bit order within byte, stores LSB first]
    // - BYTE 0 has bits 7 to 0;
    // - BYTE 1 has bits 15 to 8;
    // - BYTE 2 has bits 23 to 16;
    // - BYTE 3 has bits 31 to 24;

    int32_t i;
    char bits_temp[32];

    //(char_variable >> k) & 0x01 -> extracts the kth bit from char_variable
    for(i = 0; i < 8; i ++){
        bits_temp[0 + i] = (instruction[0] >> i) & 0x01;
        bits_temp[8 + i] = (instruction[1] >> i) & 0x01;
        bits_temp[16 + i] = (instruction[2] >> i) & 0x01;
        bits_temp[24 + i] = (instruction[3] >> i) & 0x01;
    }

    //converts the 0 and 1 (numbers) to '0' and '1' (ASCII code, easier for comparison)
    for(i = 0; i < 32; i ++){
        bits[i] = bits_temp[i] + '0';
    }



    return;
}


//---------------------------------------------------------------------------------
// binary_string_to_int: returns an integer, converted from a binary number saved as a char array
//---------------------------------------------------------------------------------
int32_t binary_string_to_int(char * bits, int32_t start_index, int32_t length){

    int32_t value = 0;
    int32_t i;

    for(i = 0; i<length; i++){

        if(bits[i + start_index] == '1'){
            //value = value + pow(2,i);
            value = value + (1 << i);
        }

    }

    return(value);
}


//---------------------------------------------------------------------------------
// immediate_load: returns the immediate, with sign extension
//---------------------------------------------------------------------------------
int32_t immediate_load(struct Registers * registers, char * bits, int32_t start_index, int32_t length){

    bool I = registers->MSR[0];

    uint32_t RIMM = (uint32_t)registers->rIMM; //16 bits -> 32 bits, so it can be left shifted later on
    int32_t IMM;

    //IMM <-- unsigned(IMM)     [this will be useful]
    IMM = binary_string_to_int(bits, start_index, length);

    //If MSR[I] == true, IMM<-rIMM(15:0)IMM(15:0)
    if(I == true){
        RIMM = (RIMM << 16);
        IMM = IMM | RIMM;
    }
    //Else, IMM<-sign extension(IMM(15:0))
    else{
        //if IMM has bit#15 = 1 (2^15 = 32768), its sign extension must be negative!
        // which is equivalent to setting 31:16 to 1 (left-padding with 1) - otherwise, do nothing
        if(IMM >= 32768){
            IMM = IMM | 0xFFFF0000;
        }
    }


    return(IMM);
}


//---------------------------------------------------------------------------------
// print_mem_write: when a write is made into address "0xFFFFFFC0", redirects it to std output (as hexadecimal)
//                  otherwise, prints the written value plus written address combination
//---------------------------------------------------------------------------------
void print_mem_write(int32_t PC, char ch_to_print, int32_t address){

    if(address == -1){
        printf("\nPrinting directly to std out - hex:%x   ascii:%c (current PC @ IF=%x)", (ch_to_print & 0xFF),(ch_to_print & 0xFF), PC);
    }
    else{
        printf("\nMemory write: %2x | address: %x  (current PC @ IF= %x)", (ch_to_print & 0xFF), address, PC);
    }
}


//---------------------------------------------------------------------------------
// check_overflow: checks if there is over or underflow, and corrects the value
//                  returns true if there is a positive overflow (to set the C flag)
//---------------------------------------------------------------------------------
bool check_overflow(int64_t value, int32_t *Rd){

    *Rd = (int32_t)(value);

    //Positive overflow
    if (value > INT32_MAX){
        return(true);
    }
    //Negative overflow (C = 0!)
    else if (value < INT32_MIN){
        return(false);
    }
    else{
        return(false);
    }

}


//---------------------------------------------------------------------------------
// print_registers: prints the registers to the file
//---------------------------------------------------------------------------------
void print_registers(struct Registers registers, int32_t stalled, const char * instruction){

    int32_t PC = registers.PC;

    //Opens file at the end
    FILE *f = fopen("outputs/breakpoints_output.txt", "a");

    fprintf(f,"\n\n____________________________");
    fprintf(f,"\nprinting the registers right before fetching PC=0x%x ", PC);

    if(stalled != 0){
        if(stalled == 1){
            fprintf(f,"[stalled @ MEM]");
        }
        else if(stalled == 2){
            fprintf(f,"[stalled @ EX]");
        }
        else if(stalled == 3){
            fprintf(f,"[stalled @ ID]");
        }
        else if(stalled == 4){
            fprintf(f,"[stalled @ IF]");
        }
    }


    fprintf(f,":\n");

    int32_t i;
    char instruction_bits[32], instruction_printable[33];
    instruction_printable[32] = '\0';

    //FOR char[4] -> ascii zeroes and ones -----
    extract_bits(instruction, instruction_bits);
    //------------------------------------------

    for(i=0; i<32; i++){
        instruction_printable[i] = instruction_bits[31-i];
        fprintf(f,"r[%i]=%i, ", i, registers.r[i]);

        if(((i+1)%4) == 0) fprintf(f,"\n");
    }


    fprintf(f,"\nrIMM=%i, MSR[I]=%i, MSR[C]=%i", registers.rIMM, registers.MSR[0], registers.MSR[1]);
    fprintf(f,"\nLast instruction fetched:%s",instruction_printable);
    fclose(f);

    return;

}


//---------------------------------------------------------------------------------
// clock_tick:  [[[[[FOR THE PIPELINED VERSION]]]]]
//              The key function.
//              Executes one clock cycle
//    *** Returns whether the program will be stopped or not ***
//---------------------------------------------------------------------------------
bool clock_tick(struct Registers * registers, struct Memory * mem_ptr, struct Auxiliary_variables * aux_mem, bool * stall){

    // It processes the pipeline in inverse order [WB -> MEM -> EX -> ID -> IF]
    // COMBINATORIAL STEP:
    //   a) WB - Writes the register file
    //   b) MEM - Memory access and PC calculation
    //   c) EX - Executes the operation & gets the memory address
    //   d) ID - Instruction decode and operand fetch
    //   e) IF - Instruction fetch and PC increment
    // SEQUENTIAL STEP:
    //   f) Pipeline registers update

    int32_t stalled = 0;
    bool stalled_bool;


    //::::::::::: a :::::::::::
    // a) WB - Writes the register file
    run_wb(registers, aux_mem);
    //this one never stalls :D

    //::::::::::: b :::::::::::
    // b) MEM - Memory access and PC calculation
    stalled_bool = run_mem(registers, aux_mem, mem_ptr);
    if(stalled_bool){
        stalled = 1;
        stalled_bool = false;
    }

    //::::::::::: c :::::::::::
    // c) EX - Executes the operation & gets the memory address
    if(stalled == 0){
        int32_t data_mem_size = mem_ptr->data_parameters.memory_size;
        stalled_bool = run_ex(registers, aux_mem, data_mem_size);
    }
    if(stalled_bool){
        stalled = 2;
        stalled_bool = false;
    }

    //::::::::::: d :::::::::::
    // d) ID - Instruction decode and operand fetch
    if(stalled == 0) stalled_bool = run_id(registers, aux_mem);
    if(stalled_bool){
        stalled = 3;
        stalled_bool = false;
    }

    //::::::::::: e :::::::::::
    // e) IF - Instruction fetch and PC increment
    if(stalled == 0) stalled_bool = run_if(registers, aux_mem, mem_ptr);
    if(stalled_bool){
        stalled = 4;
        stalled_bool = false;
    }

    //::::::::::: f :::::::::::
    //f) Moves the instruction
    update_auxiliary_variables(registers, aux_mem, stalled);


    *stall = stalled;


    // Stopping condition: BRI 0
    // Opcode: 101110 (the rest are zeros)
    // as integer: -1207959552

    int32_t instruction_at_WB_stage;

    if(instruction_at_WB_stage == -1207959552){
        return false;
    }
    else{
        return true;
    }


}



//---------------------------------------------------------------------------------
// get_instruction_main: gets an instruction from the main instruction memory, given the PC
//---------------------------------------------------------------------------------
void get_instruction_main(char* instruction, char* instruction_memory, int32_t PC){

    int32_t index = PC;

    instruction[0] = instruction_memory[index+0];
    instruction[1] = instruction_memory[index+1];
    instruction[2] = instruction_memory[index+2];
    instruction[3] = instruction_memory[index+3];

}


//---------------------------------------------------------------------------------
// get_instruction_cache: tries to get an instruction from the instruction cache, given the PC
//---------------------------------------------------------------------------------
bool get_instruction_cache(char * instruction, struct Memory * mem_ptr, int32_t PC){

    bool success = false;
    int32_t i;

    uint32_t index_bits = mem_ptr->inst_cache.index_bits;
    uint32_t wordselect_bits = mem_ptr->inst_cache.word_select_bits;
    uint32_t ways = mem_ptr->inst_parameters.cache_ways;


    //gets the index/tag  (PC "AND" index/tag mask)
    uint32_t index, index_mask, index_block;
    index_mask = ((1 << index_bits) - 1) << wordselect_bits;
    index = PC & index_mask;
    index_block = index >> wordselect_bits;


    uint32_t tag, tag_mask;
    tag_mask = ((1 << (32 - index_bits - wordselect_bits)) - 1) << (index_bits + wordselect_bits);
    tag = PC & tag_mask;

    //searches for the tag, using the index, within each way
    for(i = 0; i < ways; i++){

        //first of all, the block must be valid
        if(mem_ptr->inst_cache.valid_bit[index_block]){

            //if the block is valid, compares the tag
            if(tag == mem_ptr->inst_cache.tag[index_block]){

                //if the tag was found, copies the info
                success = true;

                uint32_t wordselect, wordselect_mask;
                wordselect_mask = ((1 <<  wordselect_bits) - 1);
                wordselect = PC & wordselect_mask;

                index += wordselect; // gets the right cache address to load from

                instruction[0] = mem_ptr->inst_cache.content[index+0];
                instruction[1] = mem_ptr->inst_cache.content[index+1];
                instruction[2] = mem_ptr->inst_cache.content[index+2];
                instruction[3] = mem_ptr->inst_cache.content[index+3];

            }
        }

        //if no tag was found so far, searches the next way
        //(one extra "index_mask" away)
        index += (((1 << index_bits)) << wordselect_bits);
        index_block = index >> wordselect_bits;
    }



    return(success);
}

//---------------------------------------------------------------------------------
// load_inst_cache_block: loads the cache block, given the PC (bonus: also gets the needed inst)
//---------------------------------------------------------------------------------
void load_inst_cache_block(struct Memory * mem_ptr, int32_t PC){

    int32_t i;

    //gets the index/ tag  (PC "AND" index/tag mask)
    uint32_t index_bits = mem_ptr->inst_cache.index_bits;
    uint32_t wordselect_bits = mem_ptr->inst_cache.word_select_bits;
    uint32_t index, index_mask, index_block;

    index_mask = ((1 << index_bits) - 1) << wordselect_bits;
    index = (PC & index_mask);
    index_block = index >> wordselect_bits;

    uint32_t tag, tag_mask;
    tag_mask = ((1 << (32 - index_bits - wordselect_bits)) - 1) << (index_bits + wordselect_bits);
    tag = PC & tag_mask;

    //for debug
    /*if(PC == 80){

        printf("\nindex bits =%i", index_bits);
        printf("\nword select bits =%i", wordselect_bits);

        printf("\n index_mask= %i", index_mask );
        printf("\nindex 80 =%i", 80 & index_mask);
        printf("\nindex 84 =%i", 84 & index_mask);

        printf("\n tag_mask= %i", tag_mask );
        printf("\ntag 80 =%i", 80 & tag_mask);
        printf("\ntag 84 =%i", 84 & tag_mask);
        //They should have the same index and tag, for the default cache
    }*/


    //gets in the appropriate way
    uint16_t max_ways = mem_ptr->inst_parameters.cache_ways;
    uint16_t way_in = mem_ptr->inst_cache.last_in[index_block] + 1;
    if(way_in == max_ways) way_in -= max_ways;

    //updates the last way in
    mem_ptr->inst_cache.last_in[index_block] = way_in;

    //gets the corresponding addresses for that way
    uint32_t cache_index = index;
    for(i = 0; i < way_in; i++){
        cache_index += (((1 << index_bits)) << wordselect_bits);
        index_block = cache_index >> wordselect_bits;
    }

    //updates the valid bit
    mem_ptr->inst_cache.valid_bit[index_block] = true;



    //loads the right block to the right cache way
    uint32_t bytes_to_copy = pow(2,wordselect_bits);
    uint32_t starting_address = index + tag;

    for(i = 0; i < bytes_to_copy; i++){
        mem_ptr->inst_cache.content[cache_index + i] = mem_ptr->inst_memory[starting_address + i];
    }

    //updates the valid bit
    mem_ptr->inst_cache.valid_bit[index_block] = true;

    //copies the tag
    mem_ptr->inst_cache.tag[index_block] = tag;


}

//---------------------------------------------------------------------------------
// run_if: runs the instruction fetch (returns whether it needs to stall or not)
//---------------------------------------------------------------------------------
bool run_if(struct Registers * registers, struct Auxiliary_variables * aux_mem, struct Memory * mem_ptr){

    //Summary:
    //1 - Checks if it needs to wait for the correct PC (control hazard)
    //2 - Loads the 32 bit instruction from the main memory/cache, using the PC as address
    //3 - Increments the PC
    //[Return: bool value indicating whether the IF needs a stall or not]



    return(false);
}



//---------------------------------------------------------------------------------
// run_id: runs the instruction decode (returns whether it needs to stall or not)
//---------------------------------------------------------------------------------
bool run_id(struct Registers * registers, struct Auxiliary_variables * aux_mem){

    //Summary:
    //1 - Decodes the instruction
    //2 - Checks whether the operands are available    -> does not stall on overwrites
    //3 - Saves the values in the ALU "next" variables
    //4 - Flags the RD register as future stall material
    //[Return: bool value indicating whether the IF needs a stall or not]



    return false;

}



//---------------------------------------------------------------------------------
// run_ex: runs the execution stage (returns whether it needs to stall or not)
//---------------------------------------------------------------------------------
bool run_ex(struct Registers * registers, struct Auxiliary_variables * aux_mem, int32_t data_mem_size){

    //Summary:
    //1 - Gets the operation and executes it;
    //2 - Stores the results in the appropriate structures
    //[Return: bool value indicating whether the EX needs a stall or not]



    return false;

}


//---------------------------------------------------------------------------------
// get_data_cache: tries to get the data from the data cache, given the address
//---------------------------------------------------------------------------------
bool get_data_cache(char * data, struct Memory * mem_ptr, int32_t address, int32_t bytes_to_move, bool store){

    bool success = false;
    int32_t i, j;

    uint32_t index_bits = mem_ptr->data_cache.index_bits;
    uint32_t wordselect_bits = mem_ptr->data_cache.word_select_bits;
    uint32_t ways = mem_ptr->data_parameters.cache_ways;


    //gets the index/tag  (PC "AND" index/tag mask)
    uint32_t index, index_mask, index_block;
    index_mask = ((1 << index_bits) - 1) << wordselect_bits;
    index = address & index_mask;
    index_block = index >> wordselect_bits;

    uint32_t tag, tag_mask;
    tag_mask = ((1 << (32 - index_bits - wordselect_bits)) - 1) << (index_bits + wordselect_bits);
    tag = address & tag_mask;


    //DBG:
    //uint32_t index_dbg = (4864 & index_mask);
    //uint32_t tag_dbg = (4864 & tag_mask);
    //if(index == index_dbg && tag_dbg == tag && address != 4864) printf("\n key block is accessed (address = %i)", address);

    //searches for the tag, using the index, within each way
    for(i = 0; i < ways; i++){

        //first of all, the block must be valid
        if(mem_ptr->data_cache.valid_bit[index_block]){

            //if the block is valid, compares the tag
            if(tag == mem_ptr->data_cache.tag[index_block]){

                //if the tag was found, copies the info
                success = true;

                uint32_t wordselect, wordselect_mask;
                wordselect_mask = ((1 <<  wordselect_bits) - 1);
                wordselect = address & wordselect_mask;

                index += wordselect; // gets the right cache address to load from

                //Only needs to get the data if it is a load instruction
                if(store == false){
                    for(j = 0; j < bytes_to_move; j++){
                        data[(bytes_to_move-1)-j] = mem_ptr->data_cache.content[index+j];
                    }
                }
                //If it is a store instruction, stores the data in the cache AND in the main memory
                else{
                    for(j = 0; j < bytes_to_move; j++){
                         mem_ptr->data_cache.content[index+j] = data[(bytes_to_move-1)-j];
                    }

                    get_data_main(data, mem_ptr->data_memory,address, bytes_to_move, true);
                }
            }
        }

        //if no tag was found so far, searches the next way
        //(one extra "index_mask" away)
        index += (((1 << index_bits)) << wordselect_bits);
        index_block = index >> wordselect_bits;
    }



    return(success);


}



//---------------------------------------------------------------------------------
// get_data_main: gets/writes the data from/to the main data memory, given the address
//---------------------------------------------------------------------------------
void get_data_main(char * data, char * data_memory, int32_t address, int32_t bytes_to_move, bool store){

    int32_t i;

    //storing procedure
    if(store){
        for(i = 0; i < bytes_to_move; i++){
            data_memory[address + i] = data[(bytes_to_move-1)-i];
        }
    }

    //loading procedure
    else{
        for(i = 0; i < bytes_to_move; i++){
            data[(bytes_to_move-1)-i] = data_memory[address + i];
        }
    }


}


//---------------------------------------------------------------------------------
// load_data_cache_block: loads the desired block into the data cache
//---------------------------------------------------------------------------------
void load_data_cache_block(struct Memory * mem_ptr, int32_t address){


    int32_t i;

    //gets the index/ tag  (PC "AND" index/tag mask)
    uint32_t index_bits = mem_ptr->data_cache.index_bits;
    uint32_t wordselect_bits = mem_ptr->data_cache.word_select_bits;
    uint32_t index, index_mask, index_block;

    index_mask = ((1 << index_bits) - 1) << wordselect_bits;
    index = (address & index_mask);
    index_block = index >> wordselect_bits;

    uint32_t tag, tag_mask;
    tag_mask = ((1 << (32 - index_bits - wordselect_bits)) - 1) << (index_bits + wordselect_bits);
    tag = address & tag_mask;


    //DBG:
    //uint32_t index_dbg = (4864 & index_mask);
    //uint32_t tag_dbg = (4864 & tag_mask);
    //if(index == index_dbg && tag_dbg == tag && address != 4864) printf("\n key block is loaded (address = %i)", address);
    //if(index == index_dbg && tag_dbg != tag && address != 4864) printf("\n key block is unloaded (address = %i)", address);



    //gets in the appropriate way
    uint16_t max_ways = mem_ptr->data_parameters.cache_ways;
    uint16_t way_in = mem_ptr->data_cache.last_in[index_block] + 1;
    if(way_in == max_ways) way_in -= max_ways;

    //updates the last way in
    mem_ptr->data_cache.last_in[index_block] = way_in;

    //gets the corresponding addresses for that way
    uint32_t cache_index = index;
    for(i = 0; i < way_in; i++){
        cache_index += (((1 << index_bits)) << wordselect_bits);
        index_block = cache_index >> wordselect_bits;
    }

    //updates the valid bit
    mem_ptr->data_cache.valid_bit[index_block] = true;



    //loads the right block to the right cache way
    uint32_t bytes_to_copy = pow(2,wordselect_bits);
    uint32_t starting_address = index + tag;

    for(i = 0; i < bytes_to_copy; i++){
        mem_ptr->data_cache.content[cache_index + i] = mem_ptr->data_memory[starting_address + i];
    }

    //updates the valid bit
    mem_ptr->data_cache.valid_bit[index_block] = true;

    //copies the tag
    mem_ptr->data_cache.tag[index_block] = tag;


}

//---------------------------------------------------------------------------------
// run_mem: runs the memory access stage (returns whether it needs to stall or not)
//---------------------------------------------------------------------------------
bool run_mem(struct Registers * registers, struct Auxiliary_variables * aux_mem, struct Memory * mem_ptr){

    //Summary:
    //1 - If the current instruction is a memory access one, accesses the memory
    //2 - If the current instruction is a control one, updates the PC
    //[Return: bool value indicating whether the IF needs a stall or not]


    return false;
}


//---------------------------------------------------------------------------------
// run_wb: runs the write back stage
//---------------------------------------------------------------------------------
void run_wb(struct Registers * registers, struct Auxiliary_variables * aux_mem){

    //Summary:
    //1 - Checks whether we need to store a RD or not
    //2 - Stores the register
    //3 - Updates the MSR


}

//---------------------------------------------------------------------------------
// update_auxiliary_variables: prepares the aux_mem for the next clock cycle
//---------------------------------------------------------------------------------
void update_auxiliary_variables(struct Registers * registers, struct Auxiliary_variables * aux_mem, int32_t stalled){

    //Summary:
    //1 - Updates the MSR
    //2 - Updates the stall conditions
    //3 - Updates the pipeline stages


    //stalled:
    // 0 - no stalls
    // 1 - stalled at MEM stage
    // 2 - stalled at EX stage
    // 3 - stalled at ID stage
    // 4 - stalled at IF stage


}
