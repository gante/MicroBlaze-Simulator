//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//

#ifndef INC_GENERAL_H
#define INC_GENERAL_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>


typedef int32_t bool;
#define false 0
#define true 1


//Struct with the register file [32*32bit registers + 16 bit rIMM + 32 bit PC + 2 binary flags]
struct Registers{
    int32_t  r[32]; // "32 general purpose registers of 32 bits (r0 to r31), with the value of register r0 being fixed at zero (all writes to register r0 are discarded)"
    uint16_t rIMM;  // "1 Special Purpose Register (rIMM) of 16 bits for immediate (constant) storage (see the IMM instruction);"
    bool MSR[2];    // "1 Machine Status Register (MSR), holding the immediate flag (I) and the carry flag (C) fields;"
    int32_t PC;     // The program counter
};
//The C flag assumes there is ONLY POSITIVE OVERFLOW


//Union for the instruction. Allows the quick conversion of char[4] <-> int32_t
union dword{

    char as_char[4];
    int32_t as_int;

};


//Struct with the memory parameters (will be useful throughout the program)
struct Memory_parameters{

    int32_t memory_size;    //total memory = 2^memory_size Bytes    10 <= value <= 25
    int32_t cache_size;     //cache memory = 2^cache_size Bytes     4 <= value <= min((memory_size-2), 14)
                            //                                -> the cache is at most 1/4 of the main memory

    int32_t cache_ways;         //0, 1, 2 or 4   (0 = no cache)
    int32_t cache_blocksize;    //cache block size = 2^cache_blocksize Bytes
                                // 2 <= value <= (cache_size - log2(cache_ways))
                                // -> i.e. it cant have less than 1 block per way

    int32_t memory_latency;     //in clock cycles                   0 <= value <= 100

    //int32_t memory_throughput -> ignored this parameter, for the sake of simplicity.
    //   Furthermore, if the throughput is over 32 bits per cycle (which is quite low),
    //   this will unlikely be a bottleneck :D

};

//Struct with the cache information
struct Cache{

    //info for quick access
    int16_t total_number_blocks;
    int16_t index_bits;
    int16_t word_select_bits;

    //actual info
    char * content;

    //for each block, there's one of these
    bool * valid_bit;
    int16_t * last_in; //(only the first one will be up to date, in many-way caches)
    int32_t * tag;

};

//Struct with all the memories and their parameters
struct Memory{

    //Data:
    char * data_memory;
    struct Cache data_cache;
    struct Memory_parameters data_parameters;

    //Instruction:
    char * inst_memory;
    struct Cache inst_cache;
    struct Memory_parameters inst_parameters;
};


//Struct with all the required auxiliary variables
struct Auxiliary_variables{

    /*
    Example of variables used in the pipeline version
    [you can do your own version :D]

    bool print;                 //Flags that controls the std out of memory writes

    union dword wb_inst;    //binary instruction at WB
    union dword mem_inst;   //binary instruction at MEM
    union dword ex_inst;    //binary instruction at EX
    union dword id_inst;    //binary instruction at ID
    union dword if_inst;    //binary instruction at IF

    int32_t stall_register[4];  //Data Hazard: Indicates which register will be used to store new values at
                                //[3] = @WB, [2] = @MEM, [1] = @EX, [0] = @ID
    int32_t stall_MSR_C;        //[counter, 0 = no stall] For the "carry" flag
    int32_t stall_MSR_I;        //[counter, 0 = no stall] For the "immediate" flag
    int32_t stall_main_memory;  //Structural Hazard: How many cycles are left before the (main) memory is available again
    int32_t stall_inst_memory;  //Structural Hazard: How many cycles are left before the (inst) memory is available again
    bool stall_PC;              //Control Hazard: Indicates whether the PC is unknown or not

    int32_t id_operand1;   //Future operand 1
    int32_t id_operand2;   //Future operand 2
    bool id_delayed;       //For delayed branches: indicates whether we should run the next instruction or not

    int32_t ex_result;     //Result of the ALU operation (@ex)
    int32_t ex_operand1;   //Operand 1
    int32_t ex_operand2;   //Operand 2
    int32_t ex_branch;     //New PC (@ex)

    int32_t mem_branch;    //New PC (@ mem)
    int32_t mem_result;    //Result of the ALU operation (@mem)
    int32_t mem_stall_data;     //stall temporary memory
    int32_t mem_stall_address;  //stall temporary memory
    bool unlock_stall_PC;  //Unlocks the "stall_PC" flag

    int32_t wb_result;     //Result of the ALU operation (@wb)


    //MSR temporary variable:
    // -2: can't change MSR -> NOOP
    // -1: doesn't updates MSR -> only sets I to false
    // 00: sets C to false (and resets I to false)
    // 01: sets C to true (and resets I to false)
    // 10: sets I to true (and keeps C as false)
    // 11: sets I to true (and keeps C as true)
    int32_t ex_MSR;        //New MSR (@ex)
    int32_t mem_MSR;       //New MSR (@mem)
    int32_t wb_MSR;        //New MSR (@wb)
    */

};





#endif /* INC_GENERAL_H */
