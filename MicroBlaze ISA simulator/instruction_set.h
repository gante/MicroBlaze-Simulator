//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//

#ifndef INC_ISA_H
#define INC_ISA_H

#include "general_includes.h"


//******************************************************************************************************
//NON PIPELINE functions
bool get_instruction(char* instruction, char* instruction_memory, int32_t* PC, int32_t max_PC);
bool assign_register(int32_t ** alias, struct Registers * registers, char * bits, int32_t start_index, bool rzero);
bool run_instruction(int32_t *PC, char * instruction_bits, struct Registers * registers, char* main_memory, int32_t memory_size, bool print);
//******************************************************************************************************

const char *char_to_bits(char x);
void extract_bits(const char * instruction, char * bits);
int32_t binary_string_to_int(char * bits, int32_t start_index, int32_t length);
int32_t immediate_load(struct Registers * registers, char * bits, int32_t start_index, int32_t length);
void print_mem_write(int32_t PC, char ch_to_print, int32_t address);
bool check_overflow(int64_t value, int32_t *Rd);
void print_registers(struct Registers registers, int32_t stalled, const char * instruction);
bool clock_tick(struct Registers * registers, struct Memory * mem_ptr, struct Auxiliary_variables * aux_mem, bool * stall);
void get_instruction_main(char* instruction, char* instruction_memory, int32_t PC);
bool get_instruction_cache(char * instruction, struct Memory * mem_ptr, int32_t PC);
void load_inst_cache_block(struct Memory * mem_ptr, int32_t PC);
bool run_if(struct Registers * registers, struct Auxiliary_variables * aux_mem, struct Memory * mem_ptr);
bool run_id(struct Registers * registers, struct Auxiliary_variables * aux_mem);
bool run_ex(struct Registers * registers, struct Auxiliary_variables * aux_mem, int32_t data_mem_size);
bool get_data_cache(char * data, struct Memory * mem_ptr, int32_t address, int32_t bytes_to_move, bool store);
void get_data_main(char * data, char * data_memory, int32_t address, int32_t bytes_to_move, bool store);
void load_data_cache_block(struct Memory * mem_ptr, int32_t address);
bool run_mem(struct Registers * registers, struct Auxiliary_variables * aux_mem, struct Memory * mem_ptr);
void run_wb(struct Registers * registers, struct Auxiliary_variables * aux_mem);
void update_auxiliary_variables(struct Registers * registers, struct Auxiliary_variables * aux_mem, int32_t stalled);



#endif /* INC_ISA_H */
