//----------------------------------------------------------------------------------------------------//
//																									  //
//				MicroBlaze ISA simulator				With 5-stage pipeline		   				  //
//							   	by: João Gante      Feb 2017									  	  //
//																									  //
//----------------------------------------------------------------------------------------------------//


#include "menu.h"



//---------------------------------------------------------------------------------
//clear_screen: clears the command line screen, by printing "\n" several times
//---------------------------------------------------------------------------------
void clear_screen(){

    int i;
    for(i = 0; i < 20; i++){
        printf("\n");
    }

}

//---------------------------------------------------------------------------------
//print_header: prints the header of the menus
//---------------------------------------------------------------------------------
void print_header(){
    printf("\n--------------------------------------------------------------------------------\n");
    printf("        MicroBlaze ISA simulator            Advanced Computer Architectures     \n");
}


//---------------------------------------------------------------------------------
//main_menu: prints the main menu and waits for an input; returns the selected option
//---------------------------------------------------------------------------------
char main_menu(bool valid_option){
    char menu_option;

    print_header();
    printf("                                  Main Menu                                     \n");
    printf("--------------------------------------------------------------------------------\n");

    printf("\nSelect an option:\n");
    printf("1 - Select a binary file\n");
    printf("2 - Configure the data memories\n");
    printf("3 - Configure the instruction memories\n");
    printf("4 - Miscellaneous\n");
    printf("5 - Run\n\n");

    printf("0 - Exit\n");

    printf("[Scroll up for the last run's results]\n");

    if(valid_option == false){
        printf("Please input a valid option!\n\n");
    }

    //Asks for an character
    fgets(&menu_option, 10, stdin);

    return(menu_option);
}


//---------------------------------------------------------------------------------
//ask_for_menu_option: asks for a valid option, in the main menu
//---------------------------------------------------------------------------------
char ask_for_menu_option(){

    bool valid_input = true;
    char menu_option;

    do{
        clear_screen();
        menu_option = main_menu(valid_input);

        if(menu_option >= '6' || menu_option < '0'){
            valid_input = false;
        }
        else{
            valid_input = true;
        }
    }while(valid_input == false);


    return(menu_option);
}


//---------------------------------------------------------------------------------
//switch_menu: switches into the right menu (device/binary/misc/break)
//---------------------------------------------------------------------------------
void switch_menu(char menu_option){

    switch(menu_option){
        case '1':
            menu_binary();
            break;

        case '2':
            menu_data();
            break;

        case '3':
            menu_inst();
            break;

        case '4':
            menu_misc();
            break;
        }


}


//---------------------------------------------------------------------------------
//menu_data: prints the data memories menu and waits for additional instructions;
//				if there is any previous configuration, reads it (otherwise, creates a default)
//   data memories -> main data memory + data cache
//---------------------------------------------------------------------------------
void menu_data(){

    // 1 - checks for previous configs   (FILE: data.txt)
    // 1a - if there is any, loads it
    // 1b - if there isn't, creates a default
    // 2 - displays the current settings and shows the menu
    // 3 - if there was any change, saves the file

    //KEY DIFFERENCE between data and inst memories:
    //  the inst main memory is not size limited (inst mem size = binary size)

    clear_screen();

    // :::::::::::::::::::::::::::::::::::::::: 1 ::::::::::::::::::::::::::::::::::::::::

    struct Memory_parameters * data_memory_ptr = malloc(6*sizeof(int32_t));

    bool write = false;
    bool error_loading = false;


    // Opens file
    FILE *f = fopen("settings/data.txt", "r");

    // Checks if it exists. If it doesn't/if it is wrong, create the default value, which must be saved in the future
    if(f != NULL){

        error_loading = load_memory_parameters(data_memory_ptr, f, true);
        if(error_loading) printf("\nWARNING: data.txt contained an incorrect config");

    }

    if(f == NULL || error_loading == true){

        //These default values [which are the same for both data and instruction memories]
        // are chosen so that it doesn't stall (due to the lack of executable instructions)
        // when there are no branches.

        data_memory_ptr->memory_size = 20;   //2^20 = 1048576 = 1 MB
        data_memory_ptr->cache_size = 8;     //2^8 = 256 B
        data_memory_ptr->cache_ways = 2;         //2-way cache
        data_memory_ptr->cache_blocksize = 5;    //2^5 = 32 B -> 8 words per block
        data_memory_ptr->memory_latency = 8;    //main memory latency = 8 clock cycles

        write = true;
    }

    fclose(f);


    // :::::::::::::::::::::::::::::::::::::::: 2 ::::::::::::::::::::::::::::::::::::::::
    bool valid_option = true;
    bool wrong_value = false;
    bool exit = false;
    char menu_option;
    char * buffer = malloc(10*sizeof(char));
    int temp_var = 0;

    // Enters the device menu
    do{
        print_header();
        printf("                                 Data Memory Menu                               \n");
        printf("--------------------------------------------------------------------------------\n");

        printf("\n\nSelect an option:\n");
        printf("1 - Change memory size (current = 2^%i Bytes)\n", data_memory_ptr->memory_size);
        printf("2 - Change memory latency (current = %i clock cycles)\n", data_memory_ptr->memory_latency);
        printf("3 - Change the number of cache ways (current = %i-way cache, 0 = no cache)\n", data_memory_ptr->cache_ways);
        printf("4 - Change cache size (current = 2^%i Bytes)\n", data_memory_ptr->cache_size);
        printf("5 - Change cache block size (current = 2^%i Bytes)\n", data_memory_ptr->cache_blocksize);

        printf("0 - Exit\n\n");

        if(valid_option == false){
            printf("Please input a valid option!\n\n");
        }
        if(wrong_value == true){
            printf("WARNING: wrong memory configuration (scroll up for more info)");
        }

        //Asks for an character
        fgets(&menu_option, 10, stdin);


        // menu_option = 0 -> exits
        // menu_option = 1-5 -> asks for a new value, checks if it is valid and saves it if it is a different value
        // menu_option = gibberish -> asks for a new menu_option input
        if(menu_option == '0'){
            exit = true;
            valid_option = true;
        }
        else if(menu_option == '1' || menu_option == '2' || menu_option == '3' ||
                menu_option == '4' || menu_option == '5'){
            valid_option = true;

            do{
                printf("\n\nOption %c selected, please input a new value\n", menu_option);

                if( fgets (buffer , 10 , stdin) != NULL){
                    temp_var = strtol(buffer, NULL, 10);
                }

                switch(menu_option){
                case '1':
                    data_memory_ptr->memory_size = temp_var;
                    break;
                case '2':
                    data_memory_ptr->memory_latency = temp_var;
                    break;
                case '3':
                    data_memory_ptr->cache_ways = temp_var;
                    break;
                case '4':
                    data_memory_ptr->cache_size = temp_var;
                    break;
                case '5':
                    data_memory_ptr->cache_blocksize = temp_var;
                    break;
                }

                wrong_value = check_memory_parameters(data_memory_ptr, true);
                write = true;

            }while(wrong_value == true);


            clear_screen();
        }
        else{
            valid_option = false;
            clear_screen();
        }


    }while(valid_option == false || exit == false);


    // :::::::::::::::::::::::::::::::::::::::: 3 ::::::::::::::::::::::::::::::::::::::::

    if(write){
        FILE *update = fopen("settings/data.txt", "w");
        if(update != NULL){
            fprintf(update, "%i\n", data_memory_ptr->memory_size);
            fprintf(update, "%i\n", data_memory_ptr->memory_latency);
            fprintf(update, "%i\n", data_memory_ptr->cache_ways);
            fprintf(update, "%i\n", data_memory_ptr->cache_size);
            fprintf(update, "%i\n", data_memory_ptr->cache_blocksize);
            fclose(update);
        }
        else{
            printf("\nWARNING: error saving file");
        }
    }

}

//---------------------------------------------------------------------------------
//menu_binary: prints the binary file menu and waits for additional instructions;
//				if there is any previsouly selected file, it reads its name
//---------------------------------------------------------------------------------
void menu_binary(){

    // 1 - checks for previous configs   (FILE: filename.txt)
    // 1a - if there is any, loads them
    // 2 - displays the current settings and shows the menu
    // 3 - if there was any change, saves the file

    clear_screen();

    // :::::::::::::::::::::::::::::::::::::::: 1 ::::::::::::::::::::::::::::::::::::::::
    char filename[100];
    bool previous_file = false;

    // Opens file
    FILE *f = fopen("settings/filename.txt", "r");
    // If it does exist, attempts to load binary file config
    if(f != NULL){
        if( fgets (filename , 100 , f) != NULL){
            previous_file = true;
        }
        else{
            printf("ERROR loading the existing filename.txt !\n\n");
        }
    }

    fclose(f);


    // :::::::::::::::::::::::::::::::::::::::: 2 ::::::::::::::::::::::::::::::::::::::::
    bool valid_option = true;
    bool valid_value;
    bool exit = false;
    char menu_option;
    char file_aux[100];
    FILE *update;


    // Enters the binary menu
    do{
        print_header();
        printf("                                Binary File Menu                                \n");
        printf("--------------------------------------------------------------------------------\n");

        if(previous_file){
            printf("\nCurrent settings:\n");
            printf("File to read = %s", filename);
        }
        else{
            printf("\nNo previous filename.txt !\n");
        }

        printf("\nSelect an option:\n");
        printf("1 - Change binary file\n");

        printf("0 - Exit\n\n");

        if(valid_option == false){
            printf("Please input a valid option!\n\n");
        }

        //Asks for an character
        fgets(&menu_option, 10, stdin);


        // menu_option = 0 -> exits
        // menu_option = 1 -> asks for a new value, checks if it is valid and saves it if it is a different value
        // menu_option = gibberish -> asks for a new menu_option input
        if(menu_option == '0'){
            exit = true;
            valid_option = true;
        }
        else if(menu_option == '1'){
            valid_option = true;

            do{
                printf("\n\nInput a new binary file name \n");


                if(fgets (file_aux , 100 , stdin) != 0){
                    valid_value = true;
                    strcpy(filename, file_aux);
                    previous_file = true;

                    // :::::::::::::::::::::::::::::::::::::::: 3 ::::::::::::::::::::::::::::::::::::::::

                    update = fopen("settings/filename.txt", "w");
                    if(update != NULL){
                        fprintf(update, "%s", filename);
                        fclose(update);
                    }

                }
                else{
                    printf("\nIncorrect file name!");
                    valid_value = false;
                }


            }while(valid_value == false);


            clear_screen();
        }
        else{
            valid_option = false;
            clear_screen();
        }


    }while(valid_option == false || exit == false);

}


//---------------------------------------------------------------------------------
//menu_misc: prints the miscelaneous menu and waits for additional instructions;
//				if there is any previsouly saved config, it reads it (otherwise, creates a default)
//---------------------------------------------------------------------------------
void menu_misc(){

    // 1 - checks for previous configs   (FILE: misc.txt)
    // 1a - if there is any, loads them
    // 1b - if there isn't, creates a default config
    // 2 - displays the current settings and shows the menu
    // 3 - if there was any change, saves the file

    clear_screen();

    // :::::::::::::::::::::::::::::::::::::::: 1 ::::::::::::::::::::::::::::::::::::::::
    bool write = false, valid_signal;
    char * buffer_signal = malloc(10*sizeof(char));  // --> for some reason this function is
    char * buffer_print = malloc(10*sizeof(char));   //     buggy if I don't use mallocs :v
    char * buffer_breakpoints = malloc(10*sizeof(char));
    int32_t signal_length, i;


    // Opens file
    FILE *f = fopen("settings/misc.txt", "r");
    // Checks if it exists. If it doesn't, create the default config, which must be saved in the future
    if (f == NULL){

        printf("(Creating a default misc.txt)\n\n");

        char default_signal[] = "100\n";
        strncpy(buffer_signal, default_signal, 9);
        buffer_signal[9] = '\0';

        char default_print[] = "yes\n";
        strncpy(buffer_print, default_print, 9);
        buffer_print[9] = '\0';

        char default_breakpoints[] = "yes\n";
        strncpy(buffer_breakpoints, default_breakpoints, 9);
        buffer_print[9] = '\0';

        write = true;
    }
    // If it does exist, attempts to load binary file config
    else{

        // for the print
        if( fgets (buffer_print , 10 , f) != NULL){
            if(strcmp(buffer_print, "yes\n") != 0 && strcmp(buffer_print, "no\n") != 0 ){
                printf("ERROR - invalid values in the misc.txt !\n\n");
            }
        }
        else{
            printf("ERROR loading the existing misc.txt !\n\n");
        }

        // for the breakpoints
        if( fgets (buffer_breakpoints , 10 , f) != NULL){
            if(strcmp(buffer_breakpoints, "yes\n") != 0 && strcmp(buffer_breakpoints, "no\n") != 0 ){
                printf("ERROR - invalid values in the misc.txt !\n\n");
            }
        }
        else{
            printf("ERROR loading the existing misc.txt !\n\n");
        }

        // for the signal
        if( fgets (buffer_signal , 10 , f) != NULL){

            signal_length = strlen(buffer_signal);
            valid_signal = true;

            for(i = 0; i < signal_length-1; i++){
                if(isdigit(buffer_signal[i]) == false){
                    valid_signal = false;
                }
            }

            if(valid_signal == false){
                printf("ERROR - invalid values in the misc.txt !\n\n");
            }
        }
        else{
            printf("ERROR loading the existing misc.txt !\n\n");
        }


    }
    fclose(f);


    // :::::::::::::::::::::::::::::::::::::::: 2 ::::::::::::::::::::::::::::::::::::::::
    bool valid_option = true;
    bool exit = false;
    char menu_option;

    // Enters the misc menu
    do{
        print_header();
        printf("                               Miscellaneous Menu                               \n");
        printf("--------------------------------------------------------------------------------\n");

        printf("\nSelect an option:\n");
        printf("\n1 - Change: life signal frequency? [Increase 10x]");
        printf("\n2 - Change: life signal frequency? [Decrease 10x]\ncurrent = %s", buffer_signal);
        printf("\n3 - Print all the memory writes to the std out?     current = %s", buffer_print);
        printf("4 - Print cycle-by-cycle machine state to a file?   current = %s", buffer_breakpoints);

        printf("\n0 - Exit\n\n");

        if(valid_option == false){
            printf("Please input a valid option!\n\n");
        }


        //Asks for an character
        fgets(&menu_option, 10, stdin);


        // menu_option = 0 -> exits
        // menu_option = 1 -> changes the signal frequency x10
        // menu_option = 2 -> changes the signal frequency /10
        // menu_option = 3 -> changes the print (yes/no)
        // menu_option = 4 -> changes the breakpoints (yes/no)
        // menu_option = gibberish -> asks for a new menu_option input
        if(menu_option == '0'){
            exit = true;
            valid_option = true;
        }
        else if(menu_option == '1'){
            valid_option = true;

            signal_length = strlen(buffer_signal);

            if(signal_length <= 9){
                //signal_length -1 -> here should be the \n
                buffer_signal[signal_length-1] = '0';
                buffer_signal[signal_length] = '\n';
                buffer_signal[signal_length+1] = '\0';
            }

            write = true;
            clear_screen();
        }
        else if(menu_option == '2'){
            valid_option = true;

            signal_length = strlen(buffer_signal);

            //signal_length -1 -> here should be the \n
            buffer_signal[signal_length-2] = '\n';
            buffer_signal[signal_length-1] = '\0';

            write = true;
            clear_screen();
        }
        else if(menu_option == '3'){
            valid_option = true;

            if(strcmp(buffer_print, "yes\n") == 0){
                strcpy(buffer_print, "no\n");
            }
            else{
                strcpy(buffer_print, "yes\n");
            }

            write = true;
            clear_screen();
        }
        else if(menu_option == '4'){
            valid_option = true;

            if(strcmp(buffer_breakpoints, "yes\n") == 0){
                strcpy(buffer_breakpoints, "no\n");
            }
            else{
                strcpy(buffer_breakpoints, "yes\n");
            }

            write = true;
            clear_screen();
        }
        else{
            valid_option = false;
            clear_screen();
        }



    }while(valid_option == false || exit == false);


    // :::::::::::::::::::::::::::::::::::::::: 3 ::::::::::::::::::::::::::::::::::::::::

    if(write){
        FILE *update = fopen("settings/misc.txt", "w");
        if(update != NULL){
            fprintf(update, "%s", buffer_print);
            fprintf(update, "%s", buffer_breakpoints);
            fprintf(update, "%s", buffer_signal);
            fclose(update);
        }
    }

}


//---------------------------------------------------------------------------------
//menu_inst: prints the instruction memories menu and waits for additional orders;
//				if there is any previous configuration, reads it (otherwise, creates a default)
//   instruction memories -> main inst memory + inst cache
//---------------------------------------------------------------------------------
void menu_inst(){

    // 1 - checks for previous configs   (FILE: inst.txt)
    // 1a - if there is any, loads it
    // 1b - if there isn't, creates a default
    // 2 - displays the current settings and shows the menu
    // 3 - if there was any change, saves the file

    //KEY DIFFERENCE between data and inst memories:
    //  the inst main memory is not size limited (inst mem size = binary size)

    clear_screen();

    // :::::::::::::::::::::::::::::::::::::::: 1 ::::::::::::::::::::::::::::::::::::::::

    struct Memory_parameters * inst_memory_ptr = malloc(6*sizeof(int32_t));

    bool write = false;
    bool error_loading = false;


    // Opens file
    FILE *f = fopen("settings/inst.txt", "r");

    // Checks if it exists. If it doesn't/if it is wrong, create the default value, which must be saved in the future
    if(f != NULL){

        error_loading = load_memory_parameters(inst_memory_ptr, f, true);
        if(error_loading) printf("\nWARNING: inst.txt contained an incorrect config");

    }

    if(f == NULL || error_loading == true){

        //These default values [which are the same for both data and instruction memories]
        // are chosen so that it doesn't stall (due to the lack of executable instructions)
        // when there are no branches.

        inst_memory_ptr->memory_size = 20;   //2^20 = 1048576 = 1 MB -> this will be overwritten, so, NP
        inst_memory_ptr->cache_size = 8;     //2^8 = 256 B
        inst_memory_ptr->cache_ways = 2;         //2-way cache
        inst_memory_ptr->cache_blocksize = 5;    //2^5 = 32 B -> 8 words per block
        inst_memory_ptr->memory_latency = 8;    //main memory latency = 8 clock cycles

        write = true;
    }

    fclose(f);


    // :::::::::::::::::::::::::::::::::::::::: 2 ::::::::::::::::::::::::::::::::::::::::
    bool valid_option = true;
    bool wrong_value = false;
    bool exit = false;
    char menu_option;
    char * buffer = malloc(10*sizeof(char));
    int temp_var = 0;

    // Enters the device menu
    do{
        print_header();
        printf("                             Instruction Memory Menu                            \n");
        printf("--------------------------------------------------------------------------------\n");

        printf("\n\nSelect an option:\n");
        printf("2 - Change memory latency (current = %i clock cycles)\n", inst_memory_ptr->memory_latency);
        printf("3 - Change the number of cache ways (current = %i-way cache, 0 = no cache)\n", inst_memory_ptr->cache_ways);
        printf("4 - Change cache size (current = 2^%i Bytes)\n", inst_memory_ptr->cache_size);
        printf("5 - Change cache block size (current = 2^%i Bytes)\n", inst_memory_ptr->cache_blocksize);

        printf("0 - Exit\n\n");

        if(valid_option == false){
            printf("Please input a valid option!\n\n");
        }
        if(wrong_value == true){
            printf("WARNING: wrong memory configuration (scroll up for more info)");
        }

        //Asks for an character
        fgets(&menu_option, 10, stdin);


        // menu_option = 0 -> exits
        // menu_option = 2-5 -> asks for a new value, checks if it is valid and saves it if it is a different value
        // menu_option = gibberish -> asks for a new menu_option input
        if(menu_option == '0'){
            exit = true;
            valid_option = true;
        }
        else if( menu_option == '2' || menu_option == '3' ||
                menu_option == '4' || menu_option == '5'){
            valid_option = true;

            do{
                printf("\n\nOption %c selected, please input a new value\n", menu_option);

                if( fgets (buffer , 10 , stdin) != NULL){
                    temp_var = strtol(buffer, NULL, 10);
                }

                switch(menu_option){
                case '1':
                    inst_memory_ptr->memory_size = temp_var;
                    break;
                case '2':
                    inst_memory_ptr->memory_latency = temp_var;
                    break;
                case '3':
                    inst_memory_ptr->cache_ways = temp_var;
                    break;
                case '4':
                    inst_memory_ptr->cache_size = temp_var;
                    break;
                case '5':
                    inst_memory_ptr->cache_blocksize = temp_var;
                    break;
                }

                wrong_value = check_memory_parameters(inst_memory_ptr, true);
                write = true;

            }while(wrong_value == true);


            clear_screen();
        }
        else{
            valid_option = false;
            clear_screen();
        }


    }while(valid_option == false || exit == false);


    // :::::::::::::::::::::::::::::::::::::::: 3 ::::::::::::::::::::::::::::::::::::::::

    if(write){
        FILE *update = fopen("settings/inst.txt", "w");
        if(update != NULL){
            fprintf(update, "%i\n", inst_memory_ptr->memory_size);
            fprintf(update, "%i\n", inst_memory_ptr->memory_latency);
            fprintf(update, "%i\n", inst_memory_ptr->cache_ways);
            fprintf(update, "%i\n", inst_memory_ptr->cache_size);
            fprintf(update, "%i\n", inst_memory_ptr->cache_blocksize);
            fclose(update);
        }
        else{
            printf("\nWARNING: error saving file");
        }
    }

}


//---------------------------------------------------------------------------------
//to_memory: adds "_memory.txt" to a string
//---------------------------------------------------------------------------------
void to_memory_txt(const char* filename, char* new_filename){

    //Creates a copy
    char filename_copy[100];
    strcpy(filename_copy, filename);

    //adds the new termination
    strcpy(new_filename, filename_copy);
    strcat(new_filename, "_memory.txt");

    return;
}

//---------------------------------------------------------------------------------
// print_memory: prints the memory content into a file
//---------------------------------------------------------------------------------
void print_memory(char * main_memory, int32_t memory_size, char * memory_filename, bool bin){

    int32_t i;

    // Opens file ("outputs/file")
    char file_to_open[100];
    strcpy(file_to_open,  "outputs/");
    strcat(file_to_open, memory_filename);
    FILE *f = fopen(file_to_open, "w");

    // Writes the memory into the file
    if (f != NULL){

        //bin = false -> print in hexadecimal, one word per line
        if(bin == false){

            for(i=0;i<memory_size;i++){
                fprintf(f, "%x ", (main_memory[i] & 0xFF));

                if(((i+1)%4) == 0) fprintf(f, "\n");
            }

        }
        //bin = true -> print in binary, one word per line
        else{
            char bits[8];
            int32_t j;

            for(i=0;i<memory_size;i++){

                //char -> binary
                for(j = 0; i < 8; i ++){
                    //inverses the order, so that the bit representation is 7:0 (and not 0:7, the string's default)
                    bits[7-j] = ((main_memory[i] >> j) & 0x01) + '0';
                }

                fprintf(f, "%s ", bits);

                if((i%4) == 0 && i>0) fprintf(f, "\n");
            }
        }

    }
    else{
        printf("\n\nERROR - unable to save the memory into a file!");
    }


    fclose(f);


    return;
}


//---------------------------------------------------------------------------------
//check_memory_parameters: checks if the values are within acceptable range
//          "bool data_memory" -> true if data memory, false if instruction memory
//---------------------------------------------------------------------------------
bool check_memory_parameters(struct Memory_parameters * mem_par, bool data_memory){

    //code: 0 -> ok, 1 -> too small, 2 -> too big, 3 -> incorrect value
    int parameter[6] = {0,0,0,0,0,0};

    //total memory = 2^memory_size Bytes    10 <= value <= 25
    if(mem_par->memory_size < 10){
        parameter[0] = 1;
    }
    else if(mem_par->memory_size > 25){
        parameter[0] = 2;
    }


    //cache ways: 0, 1, 2 or 4   (0 = no cache)
    if(mem_par->cache_ways != 0 && mem_par->cache_ways != 1 && mem_par->cache_ways != 2 && mem_par->cache_ways != 4){
        parameter[1] = 3;
    }

    //cache memory = 2^cache_size Bytes     4 <= value <= min((memory_size-2), 14)
    int min;
    if(mem_par->memory_size - 2 > 14) min = 14;
    else min = mem_par->memory_size - 2;

    if(mem_par->cache_size < 4){
        parameter[2] = 1;
    }
    else if(mem_par->cache_size > min){
        parameter[2] = 2;
    }



    //cache block size = 2^cache_blocksize Bytes   2 <= value <= (cache_size - log2(cache_ways))
    int log2_cacheways = 0;

    if(mem_par->cache_ways == 1) log2_cacheways = 0;
    else if(mem_par->cache_ways == 2) log2_cacheways = 1;
    else if(mem_par->cache_ways == 4)log2_cacheways = 2;

    if(mem_par->cache_blocksize < 2){
        parameter[3] = 1;
    }
    else if(mem_par->cache_blocksize > (mem_par->cache_size - log2_cacheways)){
        parameter[3] = 2;
    }

    //memory latency: in clock cycles                   0 <= value <= 100
    if(mem_par->memory_latency < 0){
        parameter[4] = 1;
    }
    else if(mem_par->memory_latency > 1000){
        parameter[4] = 2;
    }


    //little cheat: if cacheways == 0 -> temporarily puts the other values as valid ones
    else if(mem_par->cache_ways == 0){
        parameter[2] = 0;
        parameter[3] = 0;
    }

    //prints an error message if needed
    bool error = false;
    int i;
    char * parameter_name = malloc(20*sizeof(char));

    for(i = 0; i < 6; i++){

        if(parameter[i] != 0){
            if(error == false){
                printf("\nERROR: incorrect configuration of the ");
                if(data_memory) printf("DATA memories!");
                else printf("INSTRUCTION memories!");
            }

            if(i == 0){
                strcpy(parameter_name, "Memory size");
            }
            else if(i == 1){
                strcpy(parameter_name, "# of cache ways");
            }
            else if(i == 2){
                strcpy(parameter_name, "Cache size");
            }
            else if(i == 3){
                strcpy(parameter_name, "Cache block size");
            }
            else if(i == 4){
                strcpy(parameter_name, "Memory latency");
            }
            else if(i == 5){
                strcpy(parameter_name, "Memory throughput");
            }


            printf("\n The %s is ", parameter_name);

            if(parameter[i] == 1){
                printf("too small!");
            }
            else if(parameter[i] == 2){
                printf("too big!");
            }
            else if(parameter[i] == 3){
                printf("incorrect! (must be 0, 1, 2 or 4)");
            }

            error = true;
        }

    }


    return error;
}


//---------------------------------------------------------------------------------
//load_memory_parameters: loads the memory parameters from a file
//          "bool data_memory" -> true if data memory, false if instruction memory
//---------------------------------------------------------------------------------
bool load_memory_parameters(struct Memory_parameters * mem_par, FILE *f, bool data_memory){

    bool error = false;
    char buffer[10];


    //memory size
    if( fgets (buffer , 10 , f) != NULL){
        mem_par->memory_size = strtol(buffer, NULL, 10);
    }
    else{
        error = true;
    }

    //memory latency
    if( fgets (buffer , 10 , f) != NULL){
        mem_par->memory_latency = strtol(buffer, NULL, 10);
    }
    else{
        error = true;
    }


    //cache ways
    if( fgets (buffer , 10 , f) != NULL){
        mem_par->cache_ways = strtol(buffer, NULL, 10);
    }
    else{
        error = true;
    }

    //cache size
    if( fgets (buffer , 10 , f) != NULL){
        mem_par->cache_size = strtol(buffer, NULL, 10);
    }
    else{
        error = true;
    }

    //cache block size
    if( fgets (buffer , 10 , f) != NULL){
        mem_par->cache_blocksize = strtol(buffer, NULL, 10);
    }
    else{
        error = true;
    }


    //checks them values
    if(!error){
        //check_memory_paremeters retuns if there was an error
        error = check_memory_parameters(mem_par, data_memory);
    }

    return error;
}

