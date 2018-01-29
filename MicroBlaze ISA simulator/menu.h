//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//


#ifndef INC_MENU_H
#define INC_MENU_H

#include "general_includes.h"

void clear_screen();
void print_header();
char main_menu(bool valid_option);
char ask_for_menu_option();
void switch_menu(char menu_option);
void menu_data();
void menu_binary();
void menu_misc();
void menu_inst();
void to_memory_txt(const char* filename, char* new_filename);
void print_memory(char * main_memory, int32_t memory_size, char * memory_filename, bool bin);
bool check_memory_parameters(struct Memory_parameters * mem_par, bool data_memory);
bool load_memory_parameters(struct Memory_parameters * mem_par, FILE *f, bool data_memory);

#endif /* INC_MENU_H */
