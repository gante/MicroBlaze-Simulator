//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//


#ifndef INC_DEVICE_H
#define INC_DEVICE_H

#include "general_includes.h"

bool checks_if_runnable();
void initialize_registers(struct Registers *registers);
bool load_filename(char * filename);
bool load_misc(int32_t * signal, bool * print, bool * breakpoints);
void initialize_aux(struct Auxiliary_variables * aux_mem, bool print);
bool load_parameters(struct Memory_parameters * data, struct Memory_parameters * inst);
bool initializes_memories(struct Memory * mem_ptr, struct Memory_parameters data_par,
                          struct Memory_parameters inst_par, const char * binary_filename);

#endif /* INC_DEVICE_H */
