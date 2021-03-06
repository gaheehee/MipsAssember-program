#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

/*
 * For debug option. If you want to debug, set 1.
 * If not, set 0.
 */
#define DEBUG 1

#define MAX_SYMBOL_TABLE_SIZE   1024
#define MEM_TEXT_START          0x00400000
#define MEM_DATA_START          0x10000000
#define BYTES_PER_WORD          4
#define INST_LIST_LEN           20

/******************************************************
 * Structure Declaration 
 *******************************************************/

typedef struct inst_struct {
    char *name;
    char *op;
    char type;
    char *funct;
} inst_t;

typedef struct symbol_struct {
    char name[32];
    uint32_t address;
} symbol_t;

enum section {     
    DATA = 0,
    TEXT,
    MAX_SIZE
};

/******************************************************
 * Global Variable Declaration
 *******************************************************/

inst_t inst_list[INST_LIST_LEN] = {       //  idx
    {"addiu",   "001001", 'I', ""},       //    0
    {"addu",    "000000", 'R', "100001"}, //    1
    {"and",     "000000", 'R', "100100"}, //    2
    {"andi",    "001100", 'I', ""},       //    3
    {"beq",     "000100", 'I', ""},       //    4
    {"bne",     "000101", 'I', ""},       //    5
    {"j",       "000010", 'J', ""},       //    6
    {"jal",     "000011", 'J', ""},       //    7
    {"jr",      "000000", 'R', "001000"}, //    8
    {"lui",     "001111", 'I', ""},       //    9
    {"lw",      "100011", 'I', ""},       //   10
    {"nor",     "000000", 'R', "100111"}, //   11
    {"or",      "000000", 'R', "100101"}, //   12
    {"ori",     "001101", 'I', ""},       //   13
    {"sltiu",   "001011", 'I', ""},       //   14
    {"sltu",    "000000", 'R', "101011"}, //   15
    {"sll",     "000000", 'R', "000000"}, //   16
    {"srl",     "000000", 'R', "000010"}, //   17
    {"sw",      "101011", 'I', ""},       //   18
    {"subu",    "000000", 'R', "100011"}  //   19
};

symbol_t SYMBOL_TABLE[MAX_SYMBOL_TABLE_SIZE]; // Global Symbol Table

uint32_t symbol_table_cur_index = 0; // For indexing of symbol table

/* Temporary file stream pointers */
FILE *data_seg;
FILE *text_seg;

/* Size of each section */
uint32_t data_section_size = 0;
uint32_t text_section_size = 0;

/******************************************************
 * Function Declaration
 *******************************************************/

void Eliminate(char* str, char ch) {
    for (; *str != '\0'; str++)//?????? ????????? ?????? ????????? ??????
    {
        if (*str == ch) {    //ch??? ?????? ????????? ???
            strcpy(str, str + 1);
            str--;
        }
    }
}

/* Change file extension from ".s" to ".o" */
char* change_file_ext(char *str) {
    char *dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

/* Add symbol to global symbol table */
void symbol_table_add_entry(symbol_t symbol)
{
    SYMBOL_TABLE[symbol_table_cur_index++] = symbol;
#if DEBUG
    printf("%s: 0x%08x\n", symbol.name, symbol.address);
#endif
}

/* Convert integer number to binary string */
char* num_to_bits(unsigned int num, int len) 
{
    char* bits = (char *) malloc(len+1);
    int idx = len-1, i;
    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        } else {
            bits[idx--] = '0';
        }
        num /= 2;
    }
    for (i = idx; i >= 0; i--){
        bits[i] = '0';
    }
    bits[len] = '\0';
    return bits;
}

/* Record .text section to output file */
void record_text_section(FILE *output) 
{
    uint32_t cur_addr = MEM_TEXT_START;
    char line[1024];

    /* Point to text_seg stream */
    rewind(text_seg);

    /* Print .text section */
    while (fgets(line, 1024, text_seg) != NULL) {
        char inst[0x1000] = {0};
        char op[32] = {0};
        char label[32] = {0};
        char type = '0';
        int i, idx = 0;
        int rs, rt, rd, imm, shamt, op1;
        int addr;

        rs = rt = rd = imm = shamt = addr = op1 = 0;
#if DEBUG
        printf("0x%08x: ", cur_addr);
#endif
        /* Find the instruction type that matches the line */
        /* blank */     // text_seg ??? ?????? ????????? ?????? ???????????? ??????

        char* temp;
        char _line[1024] = { 0 };
        strcpy(_line, line);
        temp = strtok(_line, "\t\n$, ()");


        
        //unsigned int rainbow;
        char* token[5];
        char* token_1[3];
        int count = 0;
		uint32_t address_1;
        while (temp != NULL) {      // token[0]??? instruction ?????? ???????????????
            token[count] = (char*)malloc(sizeof(char) * 10);
            strcpy(token[count], temp);
            //printf("%s ", token[count]);
            count++;
            temp = strtok(NULL, "\t\n$, ()");
        }



        if (strcmp(token[0], "la") != 0) {

	        while (!(strcmp(inst_list[idx].name, token[0]) == 0)) { 
                idx++;
            }
            if (strcmp(token[0], "addu") == 0 || strcmp(token[0], "and") == 0 || strcmp(token[0], "nor") == 0 || strcmp(token[0], "or") == 0 || strcmp(token[0], "sltu") == 0 || strcmp(token[0], "subu") == 0 || strcmp(token[0], "sll") == 0 ||strcmp(token[0], "jr") == 0 || strcmp(token[0], "srl") == 0 ) {
                type = 'R';
            }
            else if (strcmp(token[0], "j") == 0 || strcmp(token[0], "jal") == 0) {
                type = 'J';
            }
            else {
                type = 'I';
            }
        }
		else {
	
			i = 0;
			int count1=0;

			while (!(strcmp(SYMBOL_TABLE[i].name, token[2]) == 0)) {
				//printf("%x i:%d", SYMBOL_TABLE[i].address, i);
				count1 = count1 + (SYMBOL_TABLE[i + 1].address - SYMBOL_TABLE[i].address)/4;
				i++;

			}
			//printf("count : %d i:%d", count, i);
			//printf("%d", i);
			
            i = count;
			rewind(data_seg);
			uint32_t temp5;
			
            
            if (strstr(token[2], "0x")) {

				int temp_2;
				temp_2 = strtol(token[2], NULL, 16);
			    temp5 = temp_2;
			}
			else {
			
			    temp5 = atoi(token[2]);			
			}

			temp5 = MEM_DATA_START + count * 4; //?????????
			
            
			//lui
			rt = atoi(token[1]); imm = atoi(token[2]); rs = 0;
			fprintf(output, "%s%s%s%s\n", "001111", num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(temp5 >> 16, 16));
			//printf("%s%s%s%s\n", "001111", num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(temp5 >> 16, 16));

			//ori
			if (((temp5 << 16) >> 16) > 0) {

				rt = atoi(token[1]); rs = atoi(token[1]); imm = atoi(token[2]);
				fprintf(output, "%s%s%s%s", "001101", num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits((temp5 << 16) >> 16, 16));
				//printf("%s%s%s%s\n", "001101", num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits((temp5 << 16) >> 16, 16) );
			}
			
			

		}
	

        switch (type) {
            case 'R':
                /* blank */

                strcpy(op, inst_list[idx].op);

                if (strcmp(token[0], "addu") == 0 || strcmp(token[0], "and") == 0 || strcmp(token[0], "nor") == 0 || strcmp(token[0], "or") == 0 || strcmp(token[0], "sltu") == 0 || strcmp(token[0], "subu") == 0) {
                    
                    rd = atoi(token[1]); rs = atoi(token[2]); rt = atoi(token[3]);  // ?????????
                    fprintf(output, "%s%s%s%s%s%s", op, num_to_bits(rs, 5),num_to_bits(rt, 5), num_to_bits(rd, 5), num_to_bits(shamt, 5), inst_list[idx].funct);

                }
                else if (strcmp(token[0], "sll") == 0 || strcmp(token[0], "srl") == 0) {

                    rd = atoi(token[1]); rt = atoi(token[2]); shamt = atoi(token[3]);                  
                    fprintf(output, "%s%s%s%s%s%s",op, num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(rd, 5), num_to_bits(shamt, 5), inst_list[idx].funct);

                }
                else {

                    rs = atoi(token[1]); 
                    fprintf(output, "%s%s%s%s%s%s",op, num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(rd, 5), num_to_bits(shamt, 5), inst_list[idx].funct);

                }


#if DEBUG
                printf("op:%s rs:$%d rt:$%d rd:$%d shamt:%d funct:%s\n",
                        op, rs, rt, rd, shamt, inst_list[idx].funct);
#endif
                break;

            case 'I':
                /* blank */

                strcpy(op, inst_list[idx].op);

                if (strcmp(token[0], "addiu") == 0 || strcmp(token[0], "andi") == 0 || strcmp(token[0], "sltiu") == 0) {

				    if (strstr(token[3], "0x")) {
					    int temp_2;
					    temp_2 = strtol(token[3], NULL, 16);
					    sprintf(token[3], "%d", temp_2);
					    //printf("%s %s\n", token[0],token[3]);
				    }
				    
                    rt = atoi(token[1]); rs = atoi(token[2]); imm = atoi(token[3]);
                    fprintf(output, "%s%s%s%s",op, num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(imm,16));

                } 
                else if (strcmp(token[0], "sw") == 0 || strcmp(token[0], "lw") == 0) {

                    rt = atoi(token[1]); imm = atoi(token[2]); rs = atoi(token[3]);
                    fprintf(output, "%s%s%s%s",op, num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(imm, 16));

                } 
                else if(strcmp(token[0], "lui") == 0){
					
					if (strstr(token[2], "0x")) {
						int temp_2;
						temp_2 = strtol(token[2], NULL, 16);
						sprintf(token[2], "%d", temp_2);
						//printf("%s %s\n", token[0], token[2]);
					}
					
                    rt = atoi(token[1]); imm = atoi(token[2]); 
                    fprintf(output, "%s%s%s%s", op, num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(imm, 16));
                
                }
				else if (strcmp(token[0], "ori") == 0) {
					rt = atoi(token[1]); rs = atoi(token[2]); imm = atoi(token[3]);
					fprintf(output, "%s%s%s%s", op, num_to_bits(rs, 5), num_to_bits(rt, 5), num_to_bits(imm, 16));
				}

#if DEBUG
                printf("op:%s rs:$%d rt:$%d imm:0x%x\n",
                        op, rs, rt, imm);
#endif
                break;

            case 'J':
                /* blank */
				
				if (strcmp(token[0], "j") == 0) {
					
                    i = 0;
					
                    while (!(strcmp(SYMBOL_TABLE[i].name, token[1]) == 0)) {
						//printf("%s i:%d", SYMBOL_TABLE[i].name, i);
						i++;
					}

					address_1 = (SYMBOL_TABLE[i].address & 0b00001111111111111111111111111100) >> 2;

					strcpy(op, inst_list[idx].op);
					//printf("%s i:%d\n", token[0], i);
					fprintf(output, "%s%s", op, num_to_bits(address_1, 26));
					//printf("%s%s\n", op, num_to_bits(address_1, 26));
				}

				if (strcmp(token[0], "jal") == 0) {
					i = 0;
					while (!(strcmp(SYMBOL_TABLE[i].name, token[1]) == 0)) {
						//printf("%s i:%d", SYMBOL_TABLE[i].name, i);
						i++;
					}
					address_1 = (SYMBOL_TABLE[i].address & 0b00001111111111111111111111111100) >> 2;

					//(SYMBOL_TABLE[i].address & 0b00000011111111111111111111111111) << 2)
					//(cur_addr & 0b11110000000000000000000000000000)
					strcpy(op, inst_list[idx].op);
					//printf("%s i:%d\n", token[0], i);
					fprintf(output, "%s%s", op, num_to_bits(address_1, 26));
					//printf("%s%s\n", op, num_to_bits(address_1, 26));
				}				

#if DEBUG
                printf("op:%s addr:%i\n", op, addr);
#endif
                break;

            default:
                break;
        }
        fprintf(output, "\n");

        cur_addr += BYTES_PER_WORD;
    }
}

/* Record .data section to output file */
void record_data_section(FILE *output)
{
    uint32_t cur_addr = MEM_DATA_START;
    char line[1024];

    /* Point to data segment stream */
    rewind(data_seg);

    /* Print .data section */
    while (fgets(line, 1024, data_seg) != NULL) {
        /* blank */
		
		char *temp;
		temp = strtok(line, "\t\n");
		while (temp != NULL)
		{
			if (!strstr(temp, ".")) { //.word??? ???????????? ????????? ???????????? ??????
				//printf("%s\n", temp);
				if (strstr(temp, "0x")) {
					fprintf(output, "%s\n", num_to_bits(strtol(temp, NULL, 16), 32));
				}
				else {
					fprintf(output, "%s\n", num_to_bits(atoi(temp), 32));
				}
			}
		temp = strtok(NULL, "\t\n");
		}


#if DEBUG
        printf("0x%08x: ", cur_addr);
        printf("%s", line);
#endif
        cur_addr += BYTES_PER_WORD;
    }
}


/* Fill the blanks */
void make_binary_file(FILE *output)
{
#if DEBUG
    char line[1024] = {0};
    rewind(text_seg);
    /* Print line of text segment */
    while (fgets(line, 1024, text_seg) != NULL) {
        printf("%s",line);
    }
    printf("text section size: %d, data section size: %d\n",
            text_section_size, data_section_size);
#endif

    /* Print text section size and data section size */
    /* blank */
	fprintf(output, "%s\n", num_to_bits(text_section_size,32));
	fprintf(output, "%s\n", num_to_bits(data_section_size,32));

    /* Print .text section */
    record_text_section(output);

    /* Print .data section */
    record_data_section(output);
}

/* Fill the blanks */
void make_symbol_table(FILE *input)
{
    char line[1024] = {0};
    uint32_t address = 0;
    enum section cur_section = MAX_SIZE;

    /* Read each section and put the stream */
    while (fgets(line, 1024, input) != NULL) {
        char *temp;
        char _line[1024] = {0};
        strcpy(_line, line);
        temp = strtok(_line, "\t\n");

        /* Check section type */
        if (!strcmp(temp, ".data")) { //.data??? address??? ??????????????? cur_section = DATA;??? ??????
            /* blank */
			address = 0;
			cur_section = DATA;
            data_seg = tmpfile();
            continue;
        }
        else if (!strcmp(temp, ".text")) {
            /* blank */
			address = 0;
			cur_section = TEXT;
            text_seg = tmpfile();
            continue;
        }

        /* Put the line into each segment stream */
        if (cur_section == DATA) {
            /* blank */
			if (strcmp(temp, ".data")) {    // ??? ????????? ???????????? ?????????,
				int check = 0;
				data_section_size += 4;
				while (temp != NULL) {
					if (strstr(temp, ":") && check == 0) {  //tempm??? :??? ?????????
						//??????
						symbol_t symbol;
                        Eliminate(temp, ':');
						strcpy(symbol.name, temp); 
						symbol.address = MEM_DATA_START + address;
						symbol_table_add_entry(symbol);

					}
					else if (strstr(temp, ".") && check == 0) {//.word ??????
						fprintf(data_seg, "%s", temp);
						check = 1;
					}
					else
					{
						fprintf(data_seg, "\t%s", temp); //????????? ????????? ???????????? tab+????????? ?????????
					}
					temp = strtok(NULL, "\t\n");
				}

				fprintf(data_seg, "\n");//????????? ??? ???????????? ?????????
			}
        }
        else if (cur_section == TEXT) {
            /* blank */

			if (strstr(_line, ":")) {    //????????? ?????? ??????
				symbol_t symbol;
                Eliminate(temp, ':');
				strcpy(symbol.name, temp);
				symbol.address = MEM_TEXT_START + address; //text ?????? ??? ???????????? address?????? ????????? ?????? ?????? ??????
				symbol_table_add_entry(symbol);

			}
			else {
                while (temp != NULL) {
                    if (strcmp(temp, "la") == 0) {
                        text_section_size += 4;
                    }
                    fprintf(text_seg, "%s\t", temp); //text??????
                    temp = strtok(NULL, "\t\n");
                }
				text_section_size += 4;//????????? ?????? ??????
                fprintf(text_seg, "\n");
			}
        }

        address += BYTES_PER_WORD;
    }
}

/******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/

int main(int argc, char* argv[])
{
    FILE *input, *output;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Read the input file */
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    /* Create the output file (*.o) */
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if(change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = fopen(filename, "w");
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    /******************************************************
     *  Let's complete the below functions!
     *
     *  make_symbol_table(FILE *input)
     *  make_binary_file(FILE *output)
     *  ????????? record_text_section(FILE *output)
     *  ????????? record_data_section(FILE *output)
     *
     *******************************************************/
    make_symbol_table(input);
    make_binary_file(output);

    fclose(input);
    fclose(output);

    return 0;
}

// event??? ?????? receiver action
/*WHILE(1) {

    WHILE(segment ??????) {

        IF(segment??? in_order???) THEN

            IF(?????? segment??? ?????? ack ????????????) THEN   // segment??? in_order??? ????????????, ?????? segment??? ?????? ack??? ??? ??????????????????,

            wait 500msec                            // receiver??? 500msec ????????????.    

            // ???????????? ?????? segment ????????????, 2) immediate & cumelative ACK??????.
            IF(???????????? ?????? in_order segment ??????) send immediate & cumelative ACK

            // 500msec ???????????? ?????? in_order segment ?????????????????????, 1) Delayed ACK??????.
            ELSE send Delayed ACK

            ELSE(ack ???????????? ?????? ?????? in_order segment??? ??????) send immediate & cumelative ACK
            // ?????? segment??? ack??? ???????????? ??????, ?????? segment??? in_order??? ?????????,
            // ?????? 2) immediate & cumelative ACK??????.

            ELSE(segment??? out_order???) THEN

            IF(????????? gap ??????) send immediate & duplicate ACK   // segment??? out_order??? ???????????? ????????? gap??? ????????????,
                                                                 // 3)immediate & duplicate ACK??????.

            IF(gap ?????????) send immediate & cumulative ACK AND window slides      // segment??? out_order??? ???????????? ????????? gap??? ???????????????,
                                                                                  // 4)immediate & duplicate ACK??? ??? window slides?????????.

    }
}*/



/*cwnd = 1 MSS
ssthresh = 64 KB
dupACKcount = 0

Slow Start :                    // Slow Start                          

    while (1) {

        IF(????????? ACK??????) {          // ????????? ACK??? ????????????, cwnd = cwnd + MSS??? ?????? 
                                      // cwmd??? exponentially?????? ??????????????? ????????? segment(s)??? ??????. 
            cwnd = cwnd + MSS
            dupACKcount = 0
            transmit ????????? segment(s)
        }

        IF(timeout??????) {             // timeout??? ????????????, ssthresh??? cwnd??? ????????? ???????????????
                                      // cwnd??? 1MSS??? ????????? ??? missing segment??? retransmit???.
            ssthresh = cwnd / 2
            cwnd = 1 MSS
            dupACKcount = 0
            retransmit missing segment
        }

        IF(duplicate ACK ??????) { dupACKcount++ }    // duplicate ACK??? ????????????, dupACKcount 1 ??????.

        IF(cwmd >= ssthresh) { go Congestion avoidance } //cwmd??? ssthresh?????? ????????? ??????,Congestion avoidance??? ??????.

        IF(dupACKcount == 3) {            // duplicate ACK ????????? 3?????????, ssthresh??? cwnd??? ???????????? ???????????????
                                          // 3 dulicate ACKs??? ???????????? ????????? ?????? ????????? ?????? ????????????  
            ssthresh = cwnd / 2           // cwnd??? 1 MSS??? ?????? ssthresh + 3 * MSS ??? ??????.
            cwnd = ssthresh + 3 * MSS     // ??? ??? missing segment??? retransmit????????? Fast Recovery??? ??????.
            retransmit missing segment

            go Fast Recovery
         }
    }
    

Congestion avoidance :          // Congestion avoidance
    
    while (1) {

        IF(????????? ACK??????) {          // ????????? ACK??? ????????????, cwnd = cwnd + MSS??? ?????? Slow start??? ?????????
                                      // cwmd??? linearly?????? ??????????????? ????????? segment(s)??? ??????. 
            cwnd = cwnd + MSS * (MSS / cwnd)
            dupACKcount = 0
            transmit ????????? segment(s)

        }

        IF(duplicate ACK ??????) { dupACKcount++ }    // duplicate ACK??? ????????????, dupACKcount 1 ??????.

        IF(dupACKcount == 3) {            // duplicate ACK ????????? 3?????????, ssthresh??? cwnd??? ???????????? ???????????????
                                          // 3 dulicate ACKs??? ???????????? ????????? ?????? ????????? ?????? ????????????  
            ssthresh = cwnd / 2           // cwnd??? 1 MSS??? ?????? ssthresh + 3 * MSS ??? ??????.
            cwnd = ssthresh + 3 * MSS     // ??? ??? missing segment??? retransmit????????? Fast Recovery??? ??????.
            retransmit missing segment

            go Fast Recovery
        }

        IF(timeout??????) {             // timeout??? ????????????, ssthresh??? cwnd??? ????????? ???????????????
                                      // cwnd??? 1MSS??? ????????? ??? missing segment??? retransmit???.
            ssthresh = cwnd / 2       // ????????? Slow start??? ??????.
            cwnd = 1 MSS
            dupACKcount = 0
            retransmit missing segment

            go Slow start
        }
    }
    


Fast recovery : 

    while (1) {

        IF(duplicate ACK ??????) {        // duplicate ACK??? ????????????, cwnd??? cwnd + MSS  ????????? ????????? ???
                                    // ????????? segment(s)??? ??????.
            cwnd = cwnd + MSS
            transmit ????????? segment(s)
        }

        IF(timeout) {                   // timeout?????? ssthresh??? cwnd??? ????????? ??????????????? 
                                        // cwnd??? 1??? ????????? ??? missing segment??? retransmit???.
            ssthresh = cwnd / 2         // ??? ??? Slow start??? ??????.
            cwnd = 1
            dupACKcount = 0
            retransmit missing segment

            go Slow start
        }

        IF(????????? ACK??????) {          // ????????? ACK??? ????????????, cwnd??? ssthresh????????? ????????????
                                      // dupACKcount??? 0?????? ??????????????? ??? 
            cwnd = ssthresh           // Congestion avoidance??? ??????.
            dupACKcount = 0

            go Congestion avoidance
        }
    }*/
    



