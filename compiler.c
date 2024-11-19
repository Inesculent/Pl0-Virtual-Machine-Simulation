#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
oddsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym} token_type;


//Check if integer
int isInteger(char a);

//Check if identifier
int isIdent(char a);

//Add identifier
int addIdent(int index, int tokenCount, char ** tokenArray, char * str, int * tokenizedArray);

//Add number
int addNumber(int index, int tokenCount, char ** tokenArray, char * str, int * tokenizedArray);

void emit(char c [], int L, int M);
void factor();
void term();
void expression();
void condition();
void statement();
int var_declaration();
void const_declaration();
void procedure_declaration();
int block();
int program();
int symboltablecheck(char str [], int type);
void call();
int symbolExistsOnLevel(char str[], int type);



int token;


typedef struct{
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

typedef struct{
    char kind[4];
    int L;
    int M;
} code;

code code_table[500];
symbol symbol_table[500];

int symbolLoc = 0;
int loc = 0;
int line = 0;
int currLev = 0;

int curr_size = 1000;
char ** tokenArray;
int * tokenizedArray;
int codeIndex = 0;





int main(int argc, char *argv[]){

    tokenArray =  (char**)malloc(curr_size * sizeof(char*));
    tokenizedArray = (int*)malloc(curr_size * sizeof(int));

    //Read from file
    FILE *fp;
    int baseSize = 1;
    char *str = (char*)malloc(baseSize * sizeof(char));

    fp = fopen(argv[1], "r");


    //Open file
    if (fp == NULL){
        printf("There was an error opening the file\n");
    }
    //read from file
    else{
        char c;
        int flag = 0;
        while ((c = getc(fp)) != EOF){

            str[baseSize - 1] = c;
            baseSize++;
            str = (char*)realloc(str, baseSize * sizeof(char));
        }
    }
    //Add null terminator to end of the string
    str[baseSize-1] = '\0';

    printf("%s\n", str);

    //Close file
    fclose(fp);

    //Print the string
    //printf("%s\n", str);
            
    //Pad the string so we don't have issues with indexing when checking ahead
    str = (char*)realloc(str, (baseSize+10) * sizeof(char));
    for (int i = baseSize + 1; i < baseSize + 10; i++){
        str[i] = '!';
    }

    //repetitive, just to be safe
    str[baseSize] = '\0';

    //Index shows location of char within the string we read in, tokenCount shows what token we are at
    int index = 0;
    int tokenCount = 0;

    //Create an array for the Strings as well as an array storing the tokens in parallel


    while (str[index] != '\0'){

        //Start with the reserved words first

        //C, V, P, C, B, E, I, F, T, E, W, D

        //We need to check for these reserved words first and foremost

        //Skip over whitespace and invisible chars
        if (str[index] == ' ' || str[index] == '\n' || str[index] == '\t' || str[index] == 13){
            index++;
            continue;
        }

        //If we find comments, we loop until we get out of the comment
        if (str[index] == '/' && str[index+1] == '*'){
            index += 2;
            while(1){

                if (str[index] == '*' && str[index+1] == '/'){
                    break;
                }
                index++;
            }
            index += 2;
            continue;
        }

        //These are the "special cases" for reserved words. They can always be identsym so we need to make sure they are not

        //Do oddsym first just so we don't miss it, this is in place of skipsym

        if (str[index] == 'o'){
            if (str[index + 1] == 'd' && str[index + 2] == 'd' && isIdent(str[index + 3]) != 1){

                tokenArray[tokenCount] = (char*)malloc(3 * sizeof(char));
                
                strcpy(tokenArray[tokenCount], "odd"); 

            
                tokenizedArray[tokenCount] = 1;
                index += 3;
            }
        }
        else if (str[index] == 'b'){
            //beginsym
            if (str[index + 1] == 'e' && str[index + 2] == 'g' &&
                str[index + 3] == 'i' &&str[index + 4] == 'n' && isIdent(str[index + 5]) != 1){
                    
                tokenArray[tokenCount] = (char*)malloc(5 * sizeof(char));
                
                strcpy(tokenArray[tokenCount], "begin"); 

                tokenizedArray[tokenCount] = 21;
                index += 5;
                    
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
    
        else if (str[index] == 'c'){
            //constsym
            if (str[index + 1] == 'o' && str[index + 2] == 'n' &&
                str[index + 3] == 's' &&str[index + 4] == 't' && isIdent(str[index + 5]) != 1){
                
                tokenArray[tokenCount] = (char*)malloc(6 * sizeof(char));
                
                strcpy(tokenArray[tokenCount], "const"); 

                tokenizedArray[tokenCount] = 28;
                index = index + 5;

            }
            //callsym
            
            else if (str[index + 1] == 'a' && str[index + 2] == 'l' &&
            str[index + 3] == 'l' && isIdent(str[index + 4]) != 1){

                tokenArray[tokenCount] = (char*)malloc(5 * sizeof(char));
                
                strcpy(tokenArray[tokenCount], "call"); 
                tokenizedArray[tokenCount] = 27;
                index = index + 4;
            }
            
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);

            }

        }
        
        else if (str[index] == 'd'){
            //dosym
            if (str[index + 1] == 'o' && isIdent(str[index + 2]) != 2){
                tokenArray[tokenCount] = (char*)malloc(3* sizeof(char));
                strcpy(tokenArray[tokenCount], "do"); 

                tokenizedArray[tokenCount] = 26;
                index += 2;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
        else if (str[index] == 'e'){
            
            //elsesym

            /*
            if (str[index + 1] == 'l' && str[index + 2] == 's' && str[index + 3] == 'e' && isIdent(str[index + 4]) != 1){

                tokenArray[tokenCount] = (char*)malloc(5* sizeof(char));
                strcpy(tokenArray[tokenCount], "else"); 

                tokenizedArray[tokenCount] = 33;
                index += 4;
            }
            */
            //endsym
            if (str[index + 1] == 'n' && str[index + 2] == 'd' && isIdent(str[index + 3]) != 1){
                tokenArray[tokenCount] = (char*)malloc(4* sizeof(char));
                strcpy(tokenArray[tokenCount], "end"); 

                tokenizedArray[tokenCount] = 22;
                index += 3;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
        else if (str[index] == 'f'){

            //fisym
            if (str[index + 1] == 'i' && isIdent(str[index + 2]) != 1){
                tokenArray[tokenCount] = (char*)malloc(3* sizeof(char));
                strcpy(tokenArray[tokenCount], "fi"); 

                tokenizedArray[tokenCount] = 8;
                index += 2;

            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
        else if (str[index] == 'i'){
            //ifsym
            if (str[index + 1] == 'f' && isIdent(str[index + 2]) != 1){
                tokenArray[tokenCount] = (char*)malloc(3* sizeof(char));
                strcpy(tokenArray[tokenCount], "if"); 

                tokenizedArray[tokenCount] = 23;
                index += 2;

            }
            else{
                //identsym
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
    
        else if (str[index] == 'p'){
            //procsym
            
            if (str[index + 1] == 'r' && str[index + 2] == 'o' &&str[index + 3] == 'c'
             &&str[index + 4] == 'e' &&str[index + 5] == 'd' &&str[index + 6] == 'u' && str[index + 7] == 'r' && str[index + 8] == 'e' && isIdent(str[index + 9]) != 1){
                tokenArray[tokenCount] = (char*)malloc(10* sizeof(char));
                strcpy(tokenArray[tokenCount], "procedure"); 

                tokenizedArray[tokenCount] = 30;
                index += 9;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
    
        else if (str[index] == 'r'){
            //readsym
            if (str[index + 1] == 'e' && str[index + 2] == 'a' && str[index + 3] == 'd' && isIdent(str[index + 4]) != 1){ 
                tokenArray[tokenCount] = (char*)malloc(5* sizeof(char));
                strcpy(tokenArray[tokenCount], "read"); 

                tokenizedArray[tokenCount] = 32;
                index += 4;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }

        else if (str[index] == 't'){
            //thensym
            if (str[index + 1] == 'h' && str[index + 2] == 'e' && str[index + 3] == 'n' && isIdent(str[index + 4]) != 1){ 
                tokenArray[tokenCount] = (char*)malloc(5* sizeof(char));
                strcpy(tokenArray[tokenCount], "then"); 

                tokenizedArray[tokenCount] = 24;
                index += 4;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
        else if (str[index] == 'v'){
            //varsym
            if (str[index + 1] == 'a' && str[index + 2] == 'r' && isIdent(str[index + 3]) != 1){ 
                tokenArray[tokenCount] = (char*)malloc(4* sizeof(char));
                strcpy(tokenArray[tokenCount], "var"); 

                tokenizedArray[tokenCount] = 29;
                index += 3;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }
        else if (str[index] == 'w'){
            //whilesym
            if (str[index + 1] == 'h' && str[index + 2] == 'i'&& str[index + 3] == 'l'&& str[index + 4] == 'e' && isIdent(str[index + 5]) != 1){ 
                tokenArray[tokenCount] = (char*)malloc(6* sizeof(char));
                strcpy(tokenArray[tokenCount], "while"); 

                tokenizedArray[tokenCount] = 25;
                index += 5;
            }
            //writesym
            else if (str[index + 1] == 'r' && str[index + 2] == 'i'&& str[index + 3] == 't'&& str[index + 4] == 'e' && isIdent(str[index + 5]) != 1 ){ 
                tokenArray[tokenCount] = (char*)malloc(6* sizeof(char));
                strcpy(tokenArray[tokenCount], "write"); 

                tokenizedArray[tokenCount] = 31;
                index += 5;
            }
            //identsym
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
            }
        }


        //geq, leq, becomes 

        else if (str[index] == ':'){
            //becomessym
            if (str[index + 1] == '='){
                tokenArray[tokenCount] = (char*)malloc(3 * sizeof(char)); 
                strcpy(tokenArray[tokenCount], ":="); 
                tokenizedArray[tokenCount] = 20;
                index += 2;
            }
            //invalid
            else{
                tokenArray[tokenCount] = (char*)malloc(2 * sizeof(char)); 
                tokenArray[tokenCount][0] = str[index];
                tokenArray[tokenCount][1] = '\0';
                tokenizedArray[tokenCount] = -1;
                index++;
            }
        }
        //lessym
        else if (str[index] == '<'){
            //leqsym
            if (str[index +1] == '='){
                tokenArray[tokenCount] = (char*)malloc(3 * sizeof(char)); 
                strcpy(tokenArray[tokenCount], "<="); 
                tokenizedArray[tokenCount] = 12;
                index += 2;
            }
            //neqsym
            else if (str[index +1] == '>'){
                tokenArray[tokenCount] = (char*)malloc(3 * sizeof(char)); 
                strcpy(tokenArray[tokenCount], "<>"); 
                tokenizedArray[tokenCount] = 10;
                index += 2;
            }
            //lessym
            else{
                tokenArray[tokenCount] = (char*)malloc(2 * sizeof(char)); 
                tokenArray[tokenCount][0] = str[index];
                tokenArray[tokenCount][1] = '\0';
                tokenizedArray[tokenCount] = 11;
                index++;
            }
        }
        //gtrsym
        else if (str[index] == '>'){
            //geqsym
            if (str[index +1] == '='){
                tokenArray[tokenCount] = (char*)malloc(3 * sizeof(char)); 
                strcpy(tokenArray[tokenCount], ">="); 
                tokenizedArray[tokenCount] = 14;
                index += 2;
            }
            //gtrsym
            else{
                tokenArray[tokenCount] = (char*)malloc(2 * sizeof(char)); 
                tokenArray[tokenCount][0] = str[index];
                tokenArray[tokenCount][1] = '\0';
                tokenizedArray[tokenCount] = 13;
                index++;
            }
        }

        //Special symbols. Some of them are handled in the operator part 
        else if (str[index] == '+' || str[index] == '-' || str[index] == '*' || str[index] == '/'
        || str[index] == '(' || str[index] == ')' || str[index] == '=' || str[index] == ',' || str[index] == '.'
        || str[index] == ';'){


            tokenArray[tokenCount] = (char*)malloc(2 * sizeof(char)); 
            tokenArray[tokenCount][0] = str[index];
            tokenArray[tokenCount][1] = '\0';
            
            switch (str[index]){
                
                //plussym = 4
                case '+':
                    tokenizedArray[tokenCount] = 4;
                    break;

                //minussym = 5
                case '-':
                    tokenizedArray[tokenCount] = 5;
                    break;

                //multsym = 6
                case '*':
                    tokenizedArray[tokenCount] = 6;
                    break;

                //slashsym = 7
                case '/':
                    tokenizedArray[tokenCount] = 7;
                    break;
                //lparentsym = 15
                case '(':
                    tokenizedArray[tokenCount] = 15;
                    break;
                //rparentsym = 16
                case ')':
                    tokenizedArray[tokenCount] = 16;
                    break;                    
                //rparentsym = 16
                case '=':
                    tokenizedArray[tokenCount] = 9;
                    break;                    
                //commasym = 17
                case ',':
                    tokenizedArray[tokenCount] = 17;
                    break;                    
                //periodsym = 19
                case '.':
                    tokenizedArray[tokenCount] = 19;
                    break;
                //semicolonsym = 18
                case ';':
                    tokenizedArray[tokenCount] = 18;
                    break;
            }
            index++;
        }
        //If integer do this. Note how we MUST do this before Ident, such that all ints will get filtered out
        else if (isInteger(str[index]) == 1){
            index = addNumber(index, tokenCount, tokenArray, str,tokenizedArray);
        }
        //if ident do this
        else if (isIdent(str[index]) == 1){
            index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);
        }
        //invalid tokens
        else{
            printf("Error: Not a valid symbol ((%d))\n", str[index]);

            printf("HERE %c HERE", str[index]);
            
            exit(EXIT_FAILURE);
            /*
            tokenArray[tokenCount] = (char*)malloc(2 * sizeof(char)); 
            tokenArray[tokenCount][0] = str[index];
            tokenArray[tokenCount][1] = '\0';
            tokenizedArray[tokenCount] = -1;
            index++;

            */
        }

        tokenCount++;
        

        //Dynamically scale the memory
        if (tokenCount > curr_size/2){
            curr_size = curr_size*2;

            tokenArray = (char**)realloc(tokenArray, curr_size * sizeof(char*));
            tokenizedArray = (int*)realloc(tokenizedArray, curr_size * sizeof(int));
        }
    
    }

    //Print out the lexeme table

    
    /*
    printf("\nLexeme Table:\n\nLexeme          token type\n");

    for (int i = 0; i < tokenCount; i++){

        if (tokenizedArray[i] == -1){
            printf("%-15s Error: Not a valid symbol\n", tokenArray[i]);
            continue;
        }
        else if (tokenizedArray[i] == -2){
            printf("%-15s Error: Ident length too long\n", tokenArray[i]);
            continue;
        }
        else if (tokenizedArray[i] == -3){
            printf("%-15s Error: Too many digits\n", tokenArray[i]);
            continue;
        }

        printf("%-15s %d %d\n", tokenArray[i], tokenizedArray[i], i);
    }

    
    */
    
    
    /*
    printf("\nToken List:\n");

    //I am quite unsure why I implemented the loop backwards, but it works just as well
    for (int i = tokenCount; i > 0; i--){

        //The invalid ones that we skip over
        if (tokenizedArray[tokenCount - i] < 0){
            continue;
        }
        else if (tokenizedArray[tokenCount - i] == 3 || tokenizedArray[tokenCount - i] == 2){

            printf("%d %s ", tokenizedArray[tokenCount - i], tokenArray[tokenCount-i]);
        }
        else{
            printf("%d ", tokenizedArray[tokenCount - i]);
        }
    }

    */

    

    //Now to free memory




    //For constants, store kind, name, value
    //For variables, store kind, name, L, and M


    token = tokenizedArray[loc];

    program();

    //Set mark to 1 after execution

    printf("\n\nNo errors, program is syntactically correct\n\n");

    printf("Assembly Code:\n\n");

    printf("Line\tOP\tL\tM\n");
    for (int i = 0; i < line; i++){
        printf("%d\t%s\t%d\t%d\n", i, code_table[i].kind, code_table[i].L, code_table[i].M);
    }

    FILE *elf;
    elf = fopen("elf.txt", "w");

    for (int i = 0; i < line; i++){

        int outputType = 0;

        

        if (strcmp(code_table[i].kind,"ADD") == 0 || strcmp(code_table[i].kind,"SUB") == 0 || strcmp(code_table[i].kind,"MUL") == 0 ||
        strcmp(code_table[i].kind,"DIV") == 0 || strcmp(code_table[i].kind,"EQL") == 0 || strcmp(code_table[i].kind,"NEQ") == 0 ||
        strcmp(code_table[i].kind,"LSS") == 0 || strcmp(code_table[i].kind,"LEQ") == 0 || strcmp(code_table[i].kind,"GTR") == 0 ||
        strcmp(code_table[i].kind,"GEQ") == 0 || strcmp(code_table[i].kind,"ODD") == 0){
            outputType = 2;
        }
        else if(strcmp(code_table[i].kind,"LIT") == 0){
            outputType = 1;
        }
        else if(strcmp(code_table[i].kind,"RTN") == 0){
            outputType = 2;
        }
        else if(strcmp(code_table[i].kind,"LOD") == 0){
            outputType = 3;
        }
        else if(strcmp(code_table[i].kind,"STO") == 0){
            outputType = 4;
        }
        else if(strcmp(code_table[i].kind,"CAL") == 0){
            outputType = 5;
        }
        else if(strcmp(code_table[i].kind,"INC") == 0){
            outputType = 6;
        }
        else if(strcmp(code_table[i].kind,"JMP") == 0){
            outputType = 7;
        }
        else if(strcmp(code_table[i].kind,"JPC") == 0){
            outputType = 8;
        }
        else if(strcmp(code_table[i].kind,"SYS") == 0){
            outputType = 9;
        }


        fprintf(elf, "%d\t%d\t%d\n", outputType, code_table[i].L, code_table[i].M);
    }

    fclose(elf);

    /*

    printf("\n\nSymbol Table:\n\n");

    printf("Kind | Name\t       | Value | Level | Address | Mark\n---------------------------------------------------\n");
    
    for (int i = 0; i < symbolLoc; i++){
        printf("%5d| %15s |%6d |%6d |%8d |%4d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val,symbol_table[i].level,symbol_table[i].addr, symbol_table[i].mark);
    } 

    */


    //Freeing the memory
    for (int i = 0; i< tokenCount; i++){
        if (tokenArray[i] != NULL){
            free(tokenArray[i]);
        }

    }
    free(tokenArray);
    free(tokenizedArray);

}

//Get symbol
int symboltablecheck(char str [], int type){

    //Big thing we need to do is choose the correct one. 
    int val = -1;

    for (int i = 500; i >= 0; i--){
    //printf("HERE: %s\n", symbol_table[i].name);
        if (strcmp(str, symbol_table[i].name) == 0 && symbol_table[i].level <= currLev && symbol_table[i].kind == type){
            return(i);
        }

        //This is basically the "lesser" of the options, I don't think I even need it at all, but it shouldn't affect the program at all. 
        if (strcmp(str, symbol_table[i].name) == 0 && symbol_table[i].level <= currLev && symbol_table[i].kind == type){
            val = i;
        }

    }

    
    return val;

}


int symbolExistsOnLevel(char str[], int type){
    for (int i = 0; i < 500; i++){
        if (strcmp(str, symbol_table[i].name) == 0 && symbol_table[i].level == currLev){
            

            if (type == 3 && symbol_table[i].kind == 3){
                return (i);
            }

            if (type == 2 || type == 1 && symbol_table[i].kind == 2 || symbol_table[i].kind == 1){
                return(i);
            }


        }
    }
    return -1;
}


void procedure_declaration(){

    //Procedure ident; block; 


    while (token == procsym){
        

        
        loc++;
        token = tokenizedArray[loc];

        //Error handling 
        if (token != identsym){
            printf("Error: Incorrect symbol after procedure declaration - Identifier not found after procedure keyword");
            exit(EXIT_FAILURE);
        }

        if (symbolExistsOnLevel(tokenArray[loc], 3) != -1){
                printf("Error: symbol name has already been declared");


                exit(EXIT_FAILURE);
        }
        int jmpIndex = line;
        emit("JMP", 0, line*3);


        //Kind, name, level, mark

        symbol_table[symbolLoc].kind = 3;
        strcpy(symbol_table[symbolLoc].name, tokenArray[loc]);
        //printf("%s %s %d\n", symbol_table[symbolLoc].name,tokenArray[loc], symbolLoc);

        symbol_table[symbolLoc].level = currLev;
        symbol_table[symbolLoc].addr = jmpIndex*3;

        //Initialized to zero
        symbol_table[symbolLoc].mark = 0;

        symbolLoc++;

        loc++;
        token = tokenizedArray[loc];

        if (token != semicolonsym){
            printf("Error: Semicolon or comma missing");
            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];

        //Increment levels (can also be done in block, but then you'd have to initialize level to -1 at the start)
        currLev++;


        block();

        if (token != semicolonsym){
            printf("Error: Semicolon or comma missing");
        
        }
        loc++;
        token = tokenizedArray[loc];

        
        emit("RTN", 0, 0);
    


        currLev--;

        
    }
}


int program(){
    

    emit("JMP", 0, 3);

    block();

    if (token != periodsym){

        //printf("\ntoken %d %d\n", token, loc);
        printf("Error: program must end with period");
        exit(EXIT_FAILURE);
    }


    emit("SYS", 0, 3);
    
}

int block(){

    //Save a couple of values for reimplementation at the end of the block
    int dx = 3;
    int codeLoc = line;
 
    int prev_sx = symbolLoc;
            

    const_declaration();

    int numVars = var_declaration();

    
    procedure_declaration();

    

    //We need to subtract one owing to the nature of how the block is handled 
    code_table[codeLoc-1].M = line*3;

    emit("INC", 0, 3 + numVars);

    statement();

    //Before we do this, let's add back all the constants. This is because constants have a global scope and aren't deallocated in the same manner.

    //Probably won't need more than 30, I could dynamically allocate, but not doing for sake of time constraint
    symbol temp [30];
    int tempTable = 0;

    for (int i = prev_sx; i < symbolLoc; i++){

        if (symbol_table[i].kind == 2){
            
            temp[tempTable].addr = symbol_table[i].addr;
            temp[tempTable].kind = symbol_table[i].kind;
            temp[tempTable].val = symbol_table[i].val;
            temp[tempTable].level = symbol_table[i].level;
            strcpy(temp[tempTable].name, symbol_table[i].name);
        }
        tempTable++;
    }

    symbolLoc = prev_sx;

    for (int i = 0; i < tempTable; i++){
        symbol_table[symbolLoc].addr = temp[tempTable].addr;
        symbol_table[symbolLoc].kind = temp[tempTable].kind;
        symbol_table[symbolLoc].val = temp[tempTable].val;
        symbol_table[symbolLoc].level = temp[tempTable].level;
        strcpy(symbol_table[symbolLoc].name, temp[tempTable].name);

        symbolLoc++;
    }
}




void const_declaration(){


    if (token == constsym){
        do{
            loc++;
            token = tokenizedArray[loc];
            if (token != identsym){
                printf("Error: const, var, procedure must be followed by identifier");
                exit(EXIT_FAILURE);
            }

            //Assuming constants have global scope
            if (symboltablecheck (tokenArray[loc], 1) != -1 || symboltablecheck (tokenArray[loc], 2) != -1){
                printf("Error: symbol name has already been declared");


                exit(EXIT_FAILURE);
            }


            char tempName [20];

            strcpy(tempName, tokenArray[loc]);

            loc++;
            token = tokenizedArray[loc];

            if (token != eqsym){
                printf("Error: constants must be assigned with =");
                exit(EXIT_FAILURE);
            }
            loc++;
            token = tokenizedArray[loc];
            if (token != numbersym){
                printf("Error: constants must be assigned an integer value");
                exit(EXIT_FAILURE);
            }
            //Add to symbol table, kind, name, number, 0, 0
            symbol_table[symbolLoc].kind = 1;
            strcpy(symbol_table[symbolLoc].name, tempName);
            symbol_table[symbolLoc].val = atoi(tokenArray[loc]);

            symbolLoc++;
            loc++;
            token = tokenizedArray[loc];
        }
        while (token == commasym);



        if (token != semicolonsym){
            printf("Error: constant and variable declarations must be followed by a semicolon");
            exit(EXIT_FAILURE);
        }
        loc++;
        token = tokenizedArray[loc];

    }
}

int var_declaration(){

    int numVars = 0;

    if (token == varsym){
        do{
            numVars++;
            loc++;
            token = tokenizedArray[loc];

            if (token != identsym){
                printf("Error: const, var, procedure must be followed by identifier");
                exit(EXIT_FAILURE);
            }

            if (symbolExistsOnLevel(tokenArray[loc], 2) != -1 || symboltablecheck(tokenArray[loc], 1) != -1){
                printf("Error: symbol name has already been declared");

                exit(EXIT_FAILURE);
            }

            //Kind, name, level, mark
            symbol_table[symbolLoc].kind = 2;
            strcpy(symbol_table[symbolLoc].name, tokenArray[loc]);
            //printf("%s %s %d\n", symbol_table[symbolLoc].name,tokenArray[loc], symbolLoc);

            symbol_table[symbolLoc].level = currLev;
            symbol_table[symbolLoc].addr = numVars + 2;

            //Initialized to zero
            symbol_table[symbolLoc].mark = 0;
            symbolLoc++;

            loc++;
            token = tokenizedArray[loc];

        }
        while(token == commasym);

        if (token != semicolonsym){
            printf("Error: constant and variable declarations must be followed by a semicolon");
            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];
    }

    return numVars;
}

void statement(){

    if (token == callsym){
        call();
    }
 
    if (token == identsym){

        int symIdx = symboltablecheck(tokenArray[loc], 2);



        if (symIdx == -1){

            int testKind = symboltablecheck(tokenArray[loc], 3);
            int testKind1 = symboltablecheck(tokenArray[loc], 1);
            if (testKind != -1 || testKind1 != -1){
                printf("Error: Assignment to constant or procedure is not allowed");
                exit(EXIT_FAILURE);
            }
            
            printf("Error: undeclared identifier: %s", tokenArray[loc]);

            exit(EXIT_FAILURE);
        }
        

        loc++;
        token = tokenizedArray[loc];

        if (token != becomessym){
            printf("Error: assignment statements must use :=");
            exit(EXIT_FAILURE);
        }


        loc++;
        token = tokenizedArray[loc];

        expression();

        emit("STO", currLev - symbol_table[symIdx].level, symbol_table[symIdx].addr);

        //printf("STO, L, %d", symbol_table[symIdx].addr);
        return;
        
    }
    
    if (token == beginsym){

        do{
            loc++;
            token = tokenizedArray[loc];
            statement();
        }
        while (token == semicolonsym);


        if (token != endsym){
            printf("Error: begin must be followed by end");

            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];
        return;
    }

    if (token == ifsym){
        loc++;
        token = tokenizedArray[loc];
        condition();
        int jpcIdx = line;

        emit("JPC", 0, jpcIdx*3);
        
        //printf("JPC 0, %d", jpcIdx);
        

        if (token != thensym){
            printf("Error: if must be followed by then");
            exit(EXIT_FAILURE);
        }
        loc++;
        token = tokenizedArray[loc];

        statement();


        if (token != fisym){
            printf("Error: fi supposed to come after if/then");

            printf("%d %d %d", token, loc, tokenizedArray[loc + 1]);
            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];
        //code[jpcIdx].M = current code index
        code_table[jpcIdx].M = line*3;
        return;
    }

    //For whilesym
    if (token == whilesym){
        loc++;
        token = tokenizedArray[loc];
        int loopIdx = line;

        condition();

        if (token != dosym){
            printf("Error: while must be followed by do");
            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];

        int jpcIdx = line;
        
        emit("JPC", 0, jpcIdx*3);
     

        statement();

        emit("JMP", 0, loopIdx*3);


        code_table[jpcIdx].M = line*3;

        return;

    }
    if (token == readsym){

        loc++;
        token = tokenizedArray[loc];

        if (token != identsym){
            printf("Error: const, var, and read keywords must be followed by identifier");
            exit(EXIT_FAILURE);
        }

        int symIdx = symboltablecheck(tokenArray[loc], 2);

        if (symIdx == -1){
            printf("Error: undeclared identifier: %s", tokenArray[loc]);
            exit(EXIT_FAILURE);
        }
        //not a var
        if (symbol_table[symIdx].kind != 2){
            
            printf("Error: only variable values may be altered");
            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];
        //Read
        emit("SIN", 0, 2);

        //STO
        emit("STO", currLev - symbol_table[symIdx].level, symbol_table[symIdx].addr);
    

        return;

    }
    //To write
    if (token == writesym){
        loc++;
        token = tokenizedArray[loc];

        expression();

        emit("SYS", 0, 1);
 

        return;
    }


}

//Conditions
void condition(){

    if (token == oddsym){
        loc++;
        token = tokenizedArray[loc];

        expression();
        emit("ODD", 0, 11);
  

    }
    else{
        expression();

        if (token == eqsym){
            loc++;
            token = tokenizedArray[loc];
            expression();

            //EQL
            emit("EQL",0, 5 );


        }
        else if (token == neqsym){

            loc++;
            token = tokenizedArray[loc];
            expression();
            //NEQ
            emit("NEQ", 0, 6);
 
        }
        else if (token == lessym){
            loc++;
            token = tokenizedArray[loc];

            expression();
            //LSS
            emit("LSS",0, 7);

        }
        else if (token == leqsym){
            loc++;
            token = tokenizedArray[loc];

            expression();
            //LEQ
            emit("LEQ", 0, 8);
     

        }
        else if (token == gtrsym){
            loc++;
            token = tokenizedArray[loc];
            expression();

            emit("GTR", 0, 9);
  
        }
        else if (token == geqsym){
            loc++;
            token = tokenizedArray[loc];
            expression();

            emit("GEQ", 0, 10);
 
        }
        else{
            printf("Error: condition must contain comparison operator");
            exit(EXIT_FAILURE);
        }
    }
}

//Call (part of new grammar for HW4)
void call(){

    if (token == callsym){

        loc++;
        token = tokenizedArray[loc];

        //Error call must be followed by identifer
        if (token != identsym){
            printf("Error: Call must be followed by an identifier");
            exit(EXIT_FAILURE);
        }
        
        int i = symboltablecheck(tokenArray[loc], 3);

        //If we can't find procedure
        if (i == -1){
            
            //Check to see if there's a var or const so we can generate the correct error message
            int a = symboltablecheck(tokenArray[loc], 2);
            int b = symboltablecheck(tokenArray[loc], 3);

            if (a != -1 || b != -1){
                printf("Error: Call of a constant or variable is meaningless");
                exit(EXIT_FAILURE);
            }
            printf("Error: undeclared identifier");
            exit(EXIT_FAILURE);
        }

        if (symbol_table[i].kind == 3){
            
            //This is probably redundant since our symboltablecheck already checks for this. It is to be safe is all.
            if (currLev - symbol_table[i].level < 0){
                printf("Error: undeclared identifier");
                exit(EXIT_FAILURE);
            }
            emit("CAL", currLev - symbol_table[i].level, symbol_table[i].addr);
        }


        loc++;
        token = tokenizedArray[loc];

    }

}

void expression(){

    /*
    loc++;
    token = tokenizedArray[loc];
    */

    
    /*

    if (token == minussym || token == plussym){
        
 
        loc++;
        token = tokenizedArray[loc];

    }
    */

    term();

    while (token == plussym || token == minussym){
            if (token == plussym){
                loc++;
                token = tokenizedArray[loc];

                term();

                emit("ADD", 0, 1);
  
            }
            else{
                
                loc++;
                token = tokenizedArray[loc];

                term();

                emit("SUB",0, 2);

            }
        }

}

//Term
void term(){
    factor();
    //check for mult and slash sym
    while (token == multsym || token == slashsym){
        if (token == multsym){
            loc++;
            token = tokenizedArray[loc];
            factor();
            emit("MUL", 0, 3);

        }
        else if (token == slashsym){
            loc++;
            token = tokenizedArray[loc];
            factor();
            emit("DIV", 0, 4);

        }
    }
}

//Factor
void factor(){

    if (token == identsym){

        //We check for either const or var
        int s1 = symboltablecheck(tokenArray[loc] ,1);
        int s2 = symboltablecheck(tokenArray[loc], 2);

        int symIdx = 0;

        //If not const or var, then take it in as a procedure
        if (s1 == -1 && s2 == -1){
            symIdx = symboltablecheck(tokenArray[loc], 3);

        }
        else if (s1 == -1){
            symIdx = s2;
        }
        else if (s2 == -1){
            symIdx = s1;
        }
        //Take based on level
        else if (symbol_table[s1].level > symbol_table[s2].level){
            symIdx = s1;
        }
        else if (symbol_table[s1].level < symbol_table[s2].level){
            symIdx = s2;
        }
        //We don't need an error here cause it's impossible to have two on same level already



        //Print error statements

        if (symIdx == -1){
            printf("Error: undeclared identifier: %s", tokenArray[loc]);
            exit(EXIT_FAILURE);
        }
        else if (symbol_table[symIdx].kind == 3){
            printf("Error: Expression must not contain a procedure identifier");
            exit(EXIT_FAILURE);
        }



        if (symbol_table[symIdx].kind == 1){

            //printf("%s", symbol_table[symIdx].name);
            emit("LIT", 0, symbol_table[symIdx].val);


        }
        else if (symbol_table[symIdx].kind == 2){

            emit("LOD", currLev - symbol_table[symIdx].level, symbol_table[symIdx].addr);


        }
        

        loc++;
        token = tokenizedArray[loc];

    }
    else if (token == numbersym){


        emit ("LIT", 0, atoi(tokenArray[loc]));

        loc++;
        token = tokenizedArray[loc];
    }
    else if (token == lparentsym){
        loc++;
        token = tokenizedArray[loc];

        expression();

        if(token != rparentsym){
            printf("Error: right parenthesis must follow left parenthesis");
            exit(EXIT_FAILURE);
        }
        loc++;
        token = tokenizedArray[loc];
    }
    else{
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");

        //printf("%d %d", token, loc);
        exit(EXIT_FAILURE);
    }
}

void emit(char c [], int L, int M){

    strcpy(code_table[line].kind, c);
    code_table[line].L = L;
    code_table[line].M = M;

    line++;
}



//See if a number is an int
int isInteger(char a){
    if (a >= '0' && a <= '9'){
        return 1;
    }
    
    return 0;
}

//See if identifier
int isIdent(char a){
    if (isInteger(a) == 1 || (a >= 'a' && a <= 'z') ||(a >= 'A' && a <= 'Z') ){
        return 1;
    }
    return 0;
}

//Add identifier
int addIdent(int index, int tokenCount, char ** tokenArray, char * str, int * tokenizedArray){
    
    //This code is somewhat inefficient since we could in theory directly assign to the tokenArray, but this 
    //Implementation requires less memory accesses
    int inputTracker = 0;
    char tempArray [100];
    int runCounter = 0;

    //Read into tempArray of size 100. I assume we won't have values larger than this
    while(isIdent(str[index + runCounter])){
        tempArray[runCounter] = str[index + runCounter];
        runCounter++;
    }

    //Make sure to add null terminator to define as a string
    tempArray[runCounter] = '\0';

    //If we exceeded our limit, set as the negative number to identify the error. else, return the normal token
    if (runCounter > 11){

        printf("Error: Ident length too long\n");
        exit(EXIT_FAILURE);
        /*
        tokenizedArray[tokenCount] = -2;

        */
    }
    else{
        tokenizedArray[tokenCount] = 2;
    }

    //Allocate the memory needed
    tokenArray[tokenCount] = (char*)malloc(runCounter+1 * sizeof(char));

    //Transfer over the data until we reach null terminator
    for (int i = 0; i <= runCounter; i++){

        tokenArray[tokenCount][i] = tempArray[i];

        if (tempArray[i] == '\0'){
            tokenArray[tokenCount][i] = '\0';  //Had == instead of =
            break;
        }

    }

    //Add to the index how many chars we read in
    index += runCounter;
    
    //return that index
    return index;
}

//Exact same code as the addInd, but with isInteger and a limit of 5 instead of 11
int addNumber(int index, int tokenCount, char ** tokenArray, char * str, int * tokenizedArray){
    int inputTracker = 0;

    char tempArray [100];

    int runCounter = 0;

    while(isInteger(str[index + runCounter])){
        
        //Up to 5 inputs for the number token
        tempArray[runCounter] = str[index + runCounter];
        runCounter++;

    }

    tempArray[runCounter] = '\0';
    if (runCounter > 5){

        printf("Error: Error: Too many digits\n");
        exit(EXIT_FAILURE);
        /*
        tokenizedArray[tokenCount] = -3;

        */
    }
    else{
        tokenizedArray[tokenCount] = 3;
    }

    tokenArray[tokenCount] = (char*)malloc(runCounter+1 * sizeof(char));

    for (int i = 0; i <= runCounter; i++){

        tokenArray[tokenCount][i] = tempArray[i];

        if (tempArray[i] == '\0'){
            tokenArray[tokenCount][i] = '\0';
            break;
        }

    }
    index += runCounter;
    return index;
}
