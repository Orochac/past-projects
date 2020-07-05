/* Solution to comp10002 Assignment 1, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Oscar Rochanakij #1082645
   Dated:     17/09/19

*/

/* Formating and typesetting program
 * Removes whitespaces and uniformly prints text 
 * within specified margins
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// From Assignment 1 doc page
#define DEBUG 0
#if DEBUG
#define DUMP_STR(x) printf("line %d: %s = %s\n", __LINE__, #x, x)
#define DUMP_INT(x) printf("line %d: %s = %d\n", __LINE__, #x, x)
#else
#define DUMP_STR(x)
#define DUMP_INT(x)
#endif

#define MAXCHARS 999       /* Max character limit of a word */
#define MAXWORDS 8250      /* Max word limit of a document */
#define MAXHEADERS 5       /* Max level of headers */
#define LEFTMARGIN 4       /* 1 Position before the first character */
#define WIDTH 50           /* Width of a line in the program */
#define NEWLINEP 2         /* Newlines in paragraph */
#define NEWLINEB 1         /* Newlines in break */
#define TRUE 1
#define FALSE 0
#define INCMD 2            /* When in a line that begins with a command*/

#define CMDOP "pblwch"     /* All the operators for commands */
#define CMDTWOSPACE "plw"  /* Commands with two newlines succeeding */
#define CMDONESPACE "bc"   /* Commands with one newlines succeeding */
#define CH_CMD '.'         /* Character of a command */
#define CH_CMDL 'l'        /* Character of the .l command */
#define CH_CMDW 'w'        /* Character of the .w command */
#define CH_CMDC 'c'        /* Character of the .c command */
#define CH_CMDH 'h'        /* Character of the .h command */



typedef char word_t[MAXCHARS+1];

/*****************************************************************************/

 /* function prototypes */

int my_get_char();
int get_word(char word[], int length, int *incmd, int *newl);
int is_ws_char(char c, int *newl);
int check_cmd(char firstchar);
void format_line(char* word);
void read_text();
void print_hyphens(int lmargin, int width);
void print_nl(int lastcmd);
void print_text(word_t allwords[MAXWORDS], int textlen);
void print_margin(int lmargin);
void print_word(char* word);
void process_word(char* word, char* wordloc, 
                  int *incmd, int *newl, int *index);
int execute_cmd(int lastcmd, char* word, int *width, 
                int *lmargin, int headerm[5]);


/****************************************************************/

/* main program controls all the action 
 */

int 
main(int argc, char *argv[]) {
    int index = 0, incmd = FALSE, newl = TRUE;
    word_t word, allwords[MAXWORDS];

    /* loop through text one word at a time and store in array
     */
    while(get_word(word, MAXCHARS , &incmd, &newl) != EOF){
        /* test if word is a word and not in command then store
         */
        process_word(word, allwords[index], &incmd, &newl, &index);
    }
    /* Print what is stored in the array
     */
    print_text(allwords, index);

    /* All done!
     */
	return 0;
    
}
/****************************************************************/

/* Depending on the current location of the word it is either
 * saved to allwords at word location or removed, as such is
 * the case with words following .p or .b commands.
 */

void 
process_word(char* word, char* wordloc, 
                  int *incmd, int *newl, int *index){
    if(!*incmd && strcmp(word,"\0")){
        strcpy(wordloc, word);
        *index+=1;
    }
    if(*incmd && *newl==TRUE){
        *incmd = FALSE;
    }
    if(*incmd==TRUE){  
        strcpy(wordloc, word);
        *index+=1;
        *incmd=INCMD;
    }
     
}
/****************************************************************/

/* print the text formatted
 */

void 
print_text(word_t allwords[MAXWORDS], int textlen){
    /* start array at pos 0
     */
    int apos = 0, lmargin = LEFTMARGIN, width = WIDTH;
    /* program starts with paragraph, and headers 
     * set to 0.0.0.0.0
     */
    int prevcmd = NEWLINEP, headerm[]= {0, 0, 0, 0, 0}; 
    while(apos < textlen){
        /* start print position at zero
         */
        int pos = 0; 
        word_t word;
        strcpy(word, allwords[apos]);
        /* print margin if non command
         */
        if(!check_cmd(word[0])){
            print_margin(lmargin);
        }
        /* print word and newline if word is longer 
         * than the width and not cmd
         */
        if(strlen(word)> width && !check_cmd(word[0])){
            prevcmd=NEWLINEB;
            printf(" %s\n",word);
            apos++;
        }
        else{
            if(!check_cmd(word[0])){
                pos += strlen(word)+1;
            }
            /* print words until one more word places
             * position past width
             */
            while(pos<=width+1){
                
                /* commands control
                 */
                if(check_cmd(word[0])){
                    char cmdlet = word[1];

                    /* test if command letter is in 
                     * possible commands
                     */
                    if(strchr(CMDOP, cmdlet) != NULL){
                        prevcmd = execute_cmd(prevcmd, word,
                                &width, &lmargin, headerm);
                        apos++;
                        break;
                    }
                }
                prevcmd=0;

                /* handling for non column one '.'
                 */
                print_word(word);
                apos++;
                strcpy(word, allwords[apos]);
                /* check if next word flows over width
                 * +1 for the space char
                 */
                if(!check_cmd(word[0])){
                    pos += strlen(word)+1;
                }

                /* end line if end of text
                 */
                if(apos == textlen){
                    break;
                }
            }
        /* print \n if new line
         */
        if(!prevcmd){
            printf("\n");
            prevcmd = NEWLINEB; 
        }
        }
    }
}

/****************************************************************/
/* execute the various possible commands in the program and return
 * the amound of newlines following the command.
 */
int 
execute_cmd(int lastcmd, char* word, int *width, 
            int *lmargin, int headerm[5]){
    char cmdlet =  word[1];
    if(!lastcmd){
        printf("\n");
    }

    /* alter length
     */
    if(cmdlet==CH_CMDL){
        /* +3 is where the len is stored
         */
        *lmargin= atoi(word+3);
    }

    /* alter width
     */
    if(cmdlet==CH_CMDW){
        /* +3 is where the width is stored
         */
        *width = atoi(word+3);
    }

    /* formating for .p .w and .l commands
     */
    if(strchr(CMDTWOSPACE, cmdlet) != NULL){
        print_nl(lastcmd);
        return NEWLINEP;
    }

    /* print centered words
     */
    if(cmdlet==CH_CMDC){
        /* +4 is where the words are stored
         */
        int i, wordlen = strlen(word+4);
        print_margin(*lmargin);
        for(i=0; i<=((*width-wordlen)/2) ;i++){
            printf(" ");
        }
        printf("%s\n", word+4);
    }

    /*formatting for .c and .b commands
     */
    if(strchr(CMDONESPACE, cmdlet) != NULL){
        return NEWLINEB;
    }

    /* print header 
     */               
    if(cmdlet==CH_CMDH){
        /* Convert the text of the header to int
        */
        int hlvl = word[4]-'1', i;
        print_nl(lastcmd);

        /* print width of hyphens if header level is 1
         */
        if(!hlvl){
            print_hyphens(*lmargin, *width);
        }
        print_margin(*lmargin);
        headerm[hlvl]++;
        /* Set all succeeding header levels to 0.
         * the -1 one is due to index being one less
        */
        for(i=MAXHEADERS-1; hlvl<i;i--){
            headerm[i]=0;
        }
        i=0;
        /* Print the header dot points 
         */
        printf(" %d", headerm[i]);
        for(i=1;i<=hlvl;i++){
            printf(".%d", headerm[i]);
        }
        /* +5 is where the header is stored
         */
        printf("%s\n\n", word+5);
        return NEWLINEP;
    }
    
    
    return 0;
    }

/****************************************************************/
// From Assignment 1 doc page

/* An altered get char to deal with cross platform CR+LF problems
 */

int 
my_get_char() {
	int c;
	while ((c=getchar())=='\r') {
	}
	return c;
	}

/****************************************************************/

/* Get the next word in the text terminated by a whitespace
 */

int 
get_word(char word[], int length, int *incmd, int *newl){
    /* Start the index at 0, set whole line to 0 (only for .c and .h)
     */
    int current_char, index = 0, wholel=FALSE;
    current_char = my_get_char();
    /* test end of file, command or only whitespace
     */
    if (current_char==EOF) {
		return EOF;
	}

    /* format comands to be stored with values for width, margin ect.
       if the command is at the start of a new line
     */
    if (*newl && check_cmd(current_char)) {
        /* ignore rest of line past nn except if CH_CMD == c or h
         */
        word[index++] = current_char;

        /* set current_char to the command char
         */
        current_char = my_get_char();
        word[index++] = current_char;

        /* if the command is .c or .h continue to store words
         */
        if(current_char!=CH_CMDC && current_char!=CH_CMDH){
            *incmd = TRUE;
            /* new line false untill next \n
             */
            *newl = FALSE;

            /* store the nn for .l and .w commands
             */
            if(current_char == CH_CMDL|| current_char==CH_CMDW){ 
                int j = 0;
                /* 3 is length of space and two digits _nn
                 */
                for(j=0;j<3;j++){
                    current_char = my_get_char();

                    /* break if single digit number
                    */
                    if(current_char=='\n'){
                        *newl = INCMD;
                        break;
                    }
                    word[index++] = current_char;
                }
            }
            
            word[index] = '\0';            
            return 0;
        }
        else{
            /* As this is either a .c or .h 
             */
            wholel = TRUE;
        }

	}
    *newl = FALSE; 
    /* For words starting with '.' not in column 1
     * put a space infront to not be mistaken for cmd
     */
    if (check_cmd(current_char)) {
        word[index++] = ' ';
    }

    /* test for double whitespace
     */
    if (is_ws_char(current_char, &*newl)){
        word[index] = '\0';
        return 0;
    }
    word[index++] = current_char;
    int j=0;
    /* itterate through word intill whitespace or eof
     */
    while((current_char = my_get_char()) != EOF  || j<length){
        if (is_ws_char(current_char, &*newl)){
            /* continue adding to word if in .c or .h command
             */
            if(*newl || wholel == FALSE){
                break;
            }
        }
        j++;
        word[index++] = current_char;
    }
    word[index] = '\0';
    if(wholel){
        format_line(word);
    }
	return 0;
    
}

/****************************************************************/
/* Format a line grabbed by the .c or .h commands to remove 
 * double spaces and tabs
 */
void
format_line(char* word){
    int i=0, j=0, last_ws = FALSE, null=FALSE, wschar =FALSE;
    char curchar;
    word_t templine;
    /* Loop through each char in the line testing for
     * double spaces and tabs.
     */
    while(word[i]!='\0'){
        curchar = word[i];
        wschar = is_ws_char(curchar, &null);
        if(!wschar){
            templine[j] = curchar;
            j++;
            last_ws =FALSE;
        }
        else {
            if(!last_ws){
                templine[j] = ' ';
                j++;
                last_ws =TRUE;
            }
        }
        i++;
    }
    templine[j]='\0';
    strcpy(word, templine);
}
/****************************************************************/
/* Check for new line and if the current character is a 
 * whitespace character
 */

int
is_ws_char(char c, int *newl){
    if (c == '\t'){
        return TRUE;
    }
    if (c == ' '){
        return TRUE;
    }
    if(c=='\n'){
        *newl = TRUE;
        return TRUE;
    }
    return FALSE;
}


/****************************************************************/
/* Print spaces the length of the current margin
 */

void print_margin(int lmargin){
    int i;
    for(i=1; i<lmargin;i++){
        printf(" ");
    }
}
/****************************************************************/
/* Print a newline if there was not paragraph break just prior
 */

void print_nl(int lastcmd){
    if(lastcmd != NEWLINEP){
        printf("\n");
    }
}

/****************************************************************/
/* print a width of - characters with a margin 
 */

void print_hyphens(int lmargin, int width){
    int i;
    print_margin(lmargin);
    printf(" ");
    for(i=0; i<width;i++){
        printf("-");
    }
    printf("\n");
}

/****************************************************************/
/* print a word with or without a precceeding space
 */
void print_word(char* word){
    if(word[0] == ' '){
        printf("%s",word);
    }
    else{
        printf(" %s",word);
    } 
}
/****************************************************************/
/* Return 1 if word is command otherwise return 0
 */
int check_cmd(char firstchar){
    if(firstchar == CH_CMD){
            return TRUE;
        }
    return FALSE;
}

/****************************************************************/
// Program end; algorithms are fun