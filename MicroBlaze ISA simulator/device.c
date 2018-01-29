//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//


#include "device.h"
#include "menu.h"




//---------------------------------------------------------------------------------
// checks_if_runnable: checks if the files "device.txt", "filename.txt" and "misc.txt" exist;
//                      if they do, return true; else return false;
//
// IMPORTANT: future versions: check if the files hold correct values
//---------------------------------------------------------------------------------
bool checks_if_runnable(){

    FILE *f;
    bool is_everything_ok = true;

    //Checks for "data.txt"
    f = fopen("settings/data.txt","r");
    if(f == NULL){
        is_everything_ok = false;
    }
    fclose(f);

    //Checks for "inst.txt"
    f = fopen("settings/inst.txt","r");
    if(f == NULL){
        is_everything_ok = false;
    }
    fclose(f);

    //Checks for "filename.txt"
    f = fopen("settings/filename.txt","r");
    if(f == NULL){
        is_everything_ok = false;
    }
    fclose(f);

    //Checks for "misc.txt"
    f = fopen("settings/misc.txt","r");
    if(f == NULL){
        is_everything_ok = false;
    }
    fclose(f);



    return(is_everything_ok);
}


//---------------------------------------------------------------------------------
// initizalize_registers: sets all registers to 0
//---------------------------------------------------------------------------------
void initialize_registers(struct Registers *registers){

    int32_t i;
    for(i=0;i<32;i++){
        registers->r[i] = 0;
    }

    registers->rIMM = 0;

    registers->MSR[0] = false;
    registers->MSR[1] = false;

    registers-> PC = 0;

    return;
}


//---------------------------------------------------------------------------------
// load_filename: loads the .exe filename from "filename.txt"
//---------------------------------------------------------------------------------
bool load_filename(char * filename){

    FILE *f;

    f = fopen("settings/filename.txt", "r");

    //If the file doesn't exist
    if( fgets (filename , 100 , f) == NULL){
        printf("\n\nERROR loading the existing filename.txt !");
        return(false);
    }

    //Trims the end line, if it exists
    filename[strcspn(filename, "\r\n")] = 0;

    fclose(f);
    return(true);

}

//---------------------------------------------------------------------------------
// load_misc: loads the misc. options from "misc.txt"
//---------------------------------------------------------------------------------
bool load_misc(int32_t * signal, bool * print, bool * breakpoints){

    FILE *f;

    f = fopen("settings/misc.txt", "r");

    char buffer_mode[10], *p;
    int32_t signal_length, i;
    bool valid_signal;


    // for the print
    if( fgets (buffer_mode , 10 , f) != NULL){
        if(strcmp(buffer_mode, "no\n") == 0){
            *print = false;
        }
        else if(strcmp(buffer_mode, "yes\n") == 0){
            *print = true;
        }
        else{
            printf("\n\nERROR - invalid values in the misc.txt !");
            return(false);
        }
    }
    else{
        printf("\n\nERROR loading the existing misc.txt !");
        return(false);
    }

    // for the breakpoints
    if( fgets (buffer_mode , 10 , f) != NULL){
        if(strcmp(buffer_mode, "no\n") == 0){
            *breakpoints = false;
        }
        else if(strcmp(buffer_mode, "yes\n") == 0){
            *breakpoints = true;
        }
        else{
            printf("\n\nERROR - invalid values in the misc.txt !");
            return(false);
        }
    }
    else{
        printf("\n\nERROR loading the existing misc.txt !");
        return(false);
    }


    // for the signal
    if( fgets (buffer_mode , 10 , f) != NULL){

            signal_length = strlen(buffer_mode);
            valid_signal = true;

            for(i = 0; i < signal_length-1; i++){
                if(isdigit(buffer_mode[i]) == false){
                    valid_signal = false;
                }
            }

            if(valid_signal == false){
                printf("ERROR - invalid values in the misc.txt !\n\n");
                return(false);
            }
            else{
                *signal = strtol(buffer_mode, &p, 10);
            }
        }
        else{
            printf("ERROR loading the existing misc.txt !\n\n");
            return(false);
        }


    fclose(f);
    return(true);
}


//---------------------------------------------------------------------------------
// initialize_aux: initializes the Auxiliary_memory structure
//---------------------------------------------------------------------------------
void initialize_aux(struct Auxiliary_variables * aux_mem, bool print){



    /*
    //initialize_aux for the example variables in general_includes.h

    //variable initializations
    aux_mem->print = print;


    //fixed initializations
    aux_mem->stall_register[0] = -1;
    aux_mem->stall_register[1] = -1;
    aux_mem->stall_register[2] = -1;
    aux_mem->stall_register[3] = -1;
    aux_mem->stall_MSR_C = 0;
    aux_mem->stall_MSR_I = 0;
    aux_mem->stall_PC = false;
    aux_mem->stall_inst_memory = 0;
    aux_mem->stall_main_memory = 0;

    aux_mem->id_operand1 = 0;
    aux_mem->id_operand2 = 0;
    aux_mem->id_delayed = false;

    aux_mem->ex_result = 0;
    aux_mem->ex_operand1 = 0;
    aux_mem->ex_operand2 = 0;
    aux_mem->ex_branch = -1;
    aux_mem->ex_MSR = -2;

    aux_mem->mem_result = 0;
    aux_mem->mem_branch = -1;
    aux_mem->mem_MSR = -2;
    aux_mem->unlock_stall_PC = false;

    aux_mem->wb_result = 0;
    aux_mem->wb_MSR = -1;

    //The instructions will treated as NOOP if they are all zeros (equivalent to "ADD R0 R0 R0")
    aux_mem->wb_inst.as_int = 0;
    aux_mem->mem_inst.as_int = 0;
    aux_mem->ex_inst.as_int = 0;
    aux_mem->id_inst.as_int = 0;
    aux_mem->if_inst.as_int = 0;

    */
}


//---------------------------------------------------------------------------------
// load_parameters: load the memories' parameters (stored in data.txt and inst.txt)
//---------------------------------------------------------------------------------
bool load_parameters(struct Memory_parameters * data, struct Memory_parameters * inst){

    bool error;
    FILE *f;


    //   1   - data memory

    f = fopen("settings/data.txt", "r");

    //If the file doesn't exist
    if( f == NULL){
        printf("\n\nERROR loading data.txt !");
        return(false);
    }
    //If the file exists
    else{
        error = load_memory_parameters(data, f, true);
    }

    fclose(f);


    //   2   - inst memory

    f = fopen("settings/inst.txt", "r");


    //If the file doesn't exist
    if( f == NULL){
        printf("\n\nERROR loading inst.txt !");
        return(false);
    }
    //If the file exists
    else{
        error = load_memory_parameters(inst, f, false);
    }

    fclose(f);


    // final tweak: converts some variables into their true value :D
    //              (example: data->memory_size into 2^data->memory_size)
    data->memory_size = pow(2,data->memory_size);
    data->cache_blocksize = pow(2,data->cache_blocksize);
    data->cache_size = pow(2,data->cache_size);
    inst->cache_blocksize = pow(2,inst->cache_blocksize);
    inst->cache_size = pow(2,inst->cache_size);


    if(error)return(false);
    else return(true);
}


//---------------------------------------------------------------------------------
// initializes_memories: initializes all the needed memories (data/instruction, main/cache)
//                      ALSO reads the binary and copies it into the instruction main memory
//---------------------------------------------------------------------------------
bool initializes_memories(struct Memory * mem_ptr, struct Memory_parameters data_par,
                          struct Memory_parameters inst_par, const char * binary_filename){

    // 1 - copies the memory parameters
    // 2 - creates the main memories (data/instruction)
    // 3 - loads the binary into the instruction memory
    // 4 - creates the caches (data/instruction)

    int32_t blocks_per_ways, log_2, ways, i;


    //::::::::::: 1 :::::::::::
    // 1 - copies the memory parameters
    mem_ptr->data_parameters = data_par;
    mem_ptr->inst_parameters = inst_par;


    //::::::::::: 2 :::::::::::
    // 2 - creates the main memories (data/instruction)

    //Creates the data memory and initializes it
    int32_t data_memory_bytes = data_par.memory_size;
    mem_ptr->data_memory = malloc(data_memory_bytes);
    memset(mem_ptr->data_memory, 0, data_memory_bytes);

    //Opens the binary file ----> needed to get the instruction memory size
    FILE *binary = fopen(binary_filename, "rb");
    if(binary == NULL){
        printf("\n\nERROR loading the desired binary file!");
        return(false);
    }

    // Creates the instruction memory
    fseek(binary, 0L, SEEK_END);
    int32_t binary_size_bytes = ftell(binary);
    fseek(binary, 0L, SEEK_SET);
    mem_ptr->inst_parameters.memory_size = binary_size_bytes;
    mem_ptr->inst_memory = malloc(binary_size_bytes);




    //::::::::::: 3 :::::::::::
    // 3 - loads the binary into the instruction memory
    fread(mem_ptr->inst_memory, binary_size_bytes, 1, binary);
    fclose(binary);


    //Copies the instruction memory into the data memory
    // (since the "premade" Xilinx code assumes that some data is alongside the instructions)
    //memcpy(mem_ptr->data_memory, mem_ptr->inst_memory, binary_size_bytes); //<--- does not respect endianness

    //IMPORTANT:Endianness :D
    char buffer[4];
    for(i = 0; i < binary_size_bytes; i += 4){
        //for each dword (4 bytes)
        buffer[3] = mem_ptr->inst_memory[i];
        buffer[2] = mem_ptr->inst_memory[i+1];
        buffer[1] = mem_ptr->inst_memory[i+2];
        buffer[0] = mem_ptr->inst_memory[i+3];

        mem_ptr->data_memory[i] = buffer[0];
        mem_ptr->data_memory[i+1] = buffer[1];
        mem_ptr->data_memory[i+2] = buffer[2];
        mem_ptr->data_memory[i+3] = buffer[3];
    }


    //::::::::::: 4 :::::::::::
    // 4 - creates the caches (data/instruction)

    // ------- data
    ways = mem_ptr->data_parameters.cache_ways;

    if(ways > 0){

        //Gets the number of blocks (= total size / block size)
        mem_ptr->data_cache.total_number_blocks = mem_ptr->data_parameters.cache_size / mem_ptr->data_parameters.cache_blocksize;

        //Gets the number of bits for the word select (depends on the log2(block_size))
        log_2 = log2(mem_ptr->data_parameters.cache_blocksize);
        mem_ptr->data_cache.word_select_bits = log_2;

        //Gets the number of bits for the index (depends on the log2(blocks_per_way))
        //  blocks_per_ways = total_blocks / number_of_ways
        blocks_per_ways = mem_ptr->data_cache.total_number_blocks / ways;
        log_2 = log2(blocks_per_ways);
        mem_ptr->data_cache.index_bits = log_2;

        //Initializes the cache content
        mem_ptr->data_cache.content = malloc(mem_ptr->data_parameters.cache_size * sizeof(char));


        //Initializes the valid_bit, tag and last_in variables
        mem_ptr->data_cache.valid_bit = malloc(mem_ptr->data_cache.total_number_blocks*sizeof(bool));
        mem_ptr->data_cache.tag = malloc(mem_ptr->data_cache.total_number_blocks*sizeof(int32_t));
        mem_ptr->data_cache.last_in = malloc(mem_ptr->data_cache.total_number_blocks*sizeof(int16_t));

        //sets the valid_bit to false and the last in to -1
        for(i = 0; i < mem_ptr->data_cache.total_number_blocks; i++){
            mem_ptr->data_cache.valid_bit[i] = false;
            mem_ptr->data_cache.last_in[i] = -1;

        }
    }

    // ------- instruction
    ways = mem_ptr->inst_parameters.cache_ways;

    if(ways > 0){

        //Gets the number of blocks (= total size / block size)
        mem_ptr->inst_cache.total_number_blocks = mem_ptr->inst_parameters.cache_size/mem_ptr->inst_parameters.cache_blocksize;

        //Gets the number of bits for the word select (depends on the log2(block_size))
        log_2 = log2(mem_ptr->inst_parameters.cache_blocksize);
        mem_ptr->inst_cache.word_select_bits = log_2;

        //Gets the number of bits for the index (depends on the log2(blocks_per_way))
        //  blocks_per_ways = total_blocks / number_of_ways
        blocks_per_ways = mem_ptr->inst_cache.total_number_blocks / ways;
        log_2 = log2(blocks_per_ways);
        mem_ptr->inst_cache.index_bits = log_2;

        //for debug :D
        /*printf("\ncache size =%i", mem_ptr->inst_parameters.cache_size);
        printf("\nblock size =%i", mem_ptr->inst_parameters.cache_blocksize);
        printf("\nways =%i", ways);
        printf("\ntotal blocks =%i", mem_ptr->inst_cache.total_number_blocks);
        printf("\nblocks per way =%i", blocks_per_ways);
        printf("\nlog2 blocks per way =%i", log_2);*/

        //Initializes the cache content
        mem_ptr->inst_cache.content = malloc(mem_ptr->inst_parameters.cache_size * sizeof(char));

        //Initializes the valid_bit, tag and last_in variables
        mem_ptr->inst_cache.valid_bit = malloc(mem_ptr->inst_cache.total_number_blocks*sizeof(bool));
        mem_ptr->inst_cache.tag = malloc(mem_ptr->inst_cache.total_number_blocks*sizeof(int32_t));
        mem_ptr->inst_cache.last_in = malloc(mem_ptr->inst_cache.total_number_blocks*sizeof(int16_t));

        //sets the valid_bit to false and the last in to -1
        for(i = 0; i < mem_ptr->inst_cache.total_number_blocks; i++){
            mem_ptr->inst_cache.valid_bit[i] = false;
            mem_ptr->inst_cache.last_in[i] = -1;

        }
    }

    return(true);
}

