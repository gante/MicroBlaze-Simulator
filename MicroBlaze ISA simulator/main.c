//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//


#include "general_includes.h"
#include "menu.h"
#include "device.h"
#include "instruction_set.h"



// simulator flowchart:
// 1 - loads the starting screen
// 1 a) - loads the binary file
// 1 b) - sets up the data memory
// 1 c) - sets up the instruction memory
// 1 d) - miscellaneous
// 2 - initializes the simulated device (memories, registers, ...)
// 3 - executes the binary file
// 4 - returns the data memory (as a text file)


int main(){

    bool execution = true;
    char menu_option;

    while(execution){

        // :::::::::::::::::::::::::::::::::::::::: 1 ::::::::::::::::::::::::::::::::::::::::
        // (function code at "menu.c")

        // Repeatedly calls the menu until it gets a valid option
        menu_option = ask_for_menu_option();

        // If the selected option was 0, exits
        if(menu_option == '0'){
            execution = false;
        }
        // If the selected option was 1, 2, 3 or 4, switches into the correct sub-menu
        else if(menu_option > '0' && menu_option < '5'){
            switch_menu(menu_option);
        }

        // If the selected option was 5, runs the simulation.
        // Options 1, 2 and 3 must have been executed at least once in the past (including previous executions)
        else{

            // :::::::::::::::::::::::::::::::::::::::: 2 ::::::::::::::::::::::::::::::::::::::::
            // (function code at "instruction_set.c")

            // Checks if the needed files exist
            if(checks_if_runnable()){

                //Needed variables
                int32_t life_signal;
                int32_t *signal_ptr = &life_signal;

                struct Registers registers;
                struct Registers *registers_pointer = &registers;

                struct Memory_parameters data_mem, inst_mem;
                struct Memory_parameters * data_mem_ptr = & data_mem;
                struct Memory_parameters * inst_mem_ptr = & inst_mem;

                struct Memory memory;
                struct Memory * memory_ptr = & memory;

                bool status, print, breakpoints;
                bool *print_ptr = &print;
                bool *breakpoints_ptr = &breakpoints;

                char filename[100];


                //Creates the registers and initializes them
                initialize_registers(registers_pointer);


                //Loads the data and instruction memories' parameters
                status = load_parameters(data_mem_ptr, inst_mem_ptr);
                if(status == false) return(1);


                //Loads the misc file
                status = load_misc(signal_ptr, print_ptr, breakpoints_ptr);
                if(status == false) return(1);


                //Loads the binary file name
                status = load_filename(filename);
                if(status == false) return(1);



                //Initializes the memory
                status = initializes_memories(memory_ptr, data_mem, inst_mem, filename);
                if(status == false) return(1);

                //Resets the breakpoints file
                FILE *f = fopen("outputs/breakpoints_output.txt", "w");
                fclose(f);



                // :::::::::::::::::::::::::::::::::::::::: 3 ::::::::::::::::::::::::::::::::::::::::
                // (function code at "instruction_set.c")

                //Initializes the auxiliary memory structure
                struct Auxiliary_variables auxi_mem;
                struct Auxiliary_variables *aux_mem = &auxi_mem;
                initialize_aux(aux_mem, *print_ptr);


                //Other auxiliary variables for the main cycle
                bool reading_binary = true;
                int32_t total_cycles = 0, cycles_stalled = 0, is_stall;
                int32_t *is_stall_ptr = &is_stall;



                // "while" loop reading the instruction memory, 32 bits at a time ("clock_tick")
                // It processes the pipeline in inverse order, as it is better for stall management
                // a) WB - Writes the RD
                // b) MEM - Memory access and PC calculation
                // c) EX - Executes the operation & gets the memory address
                // d) ID - Instruction decode and operand fetch
                // e) IF - Instruction fetch and PC increment

                printf("\n\n**** STARTING TO RUN ****\n");


                //**********************************************
                //NON PIPELINE version [aux variables]
                //(delete this section for the pipeline version)

                int32_t max_PC = memory_ptr->inst_parameters.memory_size;
                int32_t data_size = memory_ptr->data_parameters.memory_size;
                union dword instruction;
                char instruction_bits[32];

                //**********************************************



                while(reading_binary){

                    total_cycles++;
                    if(total_cycles%life_signal == 0){
                        printf("\n[Running clock cycle = %i]", total_cycles);
                    }

                    //**********************************************
                    //NON PIPELINE version
                    //(delete this section and uncomment the following one for the pipeline version)

                    //fetches the instruction
                    status = get_instruction(instruction.as_char, memory_ptr->inst_memory, &registers_pointer->PC, max_PC);
                    if(status == false) printf("\nERROR: something went wrong when FETCHING the last instruction, exiting...");

                    extract_bits(instruction.as_char, instruction_bits);

                    if(instruction.as_int == -1207959552){ // <----- BRI 0 (stopping condition)
                        break;
                    }

                    //runs the instruction
                    status = run_instruction(&registers_pointer->PC, instruction_bits, registers_pointer, memory_ptr->data_memory, data_size, *print_ptr);
                    if(status == false){
                        printf("\nERROR: something went wrong when RUNNING the last instruction, exiting...");
                        break;
                    }

                    //stores the current register file in an external file
                    if(breakpoints){
                        print_registers(registers, 0 ,instruction.as_char);
                    }
                    //**********************************************

                    /*
                    //Runs a clock cycle
                    reading_binary = clock_tick(registers_pointer, memory_ptr, aux_mem, is_stall_ptr);
                    if(is_stall != 0) cycles_stalled++;


                    //Saves the breakpoint to a file if needed
                    if(breakpoints){
                        print_registers(registers,is_stall,aux_mem->if_inst.as_char);
                    }*/


                }

                printf("\n\n**** FINISHED ****");
                printf("\nSummary:");
                printf("\nTotal cycles: %i", total_cycles);
                printf("\nTotal stalls: %i", cycles_stalled);


                // :::::::::::::::::::::::::::::::::::::::: 4 ::::::::::::::::::::::::::::::::::::::::

                char memory_filename[100];
                to_memory_txt(filename, memory_filename);

                print_memory(memory_ptr->data_memory, memory_ptr->data_parameters.memory_size, memory_filename, false);


            }
            // If the files are missing, abort
            else{
                printf("\n\nERROR: You must configure your system first!");
            }
        }
    }

    return 0;
}



