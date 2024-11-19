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
int block();
int program();
int symboltablecheck(char str []);



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

        //Special special case ODD

        if (str[index] == 'o'){
            //beginsym
            if (str[index + 1] == 'd' && str[index + 2] == 'd' &&isIdent(str[index + 3]) != 1){
                    
                tokenArray[tokenCount] = (char*)malloc(4 * sizeof(char));
                
                strcpy(tokenArray[tokenCount], "odd"); 

                tokenizedArray[tokenCount] = 1;
                index += 3;

                tokenCount++;
                continue;
                    
            }
            else{
                index = addIdent(index, tokenCount, tokenArray, str, tokenizedArray);

                tokenCount++;
                continue;
            }
        }

        //These are the "special cases" for reserved words. They can always be identsym so we need to make sure they are not

        

        if (str[index] == 'b'){
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

            /*
            else if (str[index + 1] == 'a' && str[index + 2] == 'l' &&
            str[index + 3] == 'l' && isIdent(str[index + 4]) != 1){

                tokenArray[tokenCount] = (char*)malloc(5 * sizeof(char));
                
                strcpy(tokenArray[tokenCount], "call"); 
                tokenizedArray[tokenCount] = 27;
                index = index + 4;
            }
            */
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
            tokenArray[tokenCount] = (char*)malloc(2 * sizeof(char)); 
            tokenArray[tokenCount][0] = str[index];
            tokenArray[tokenCount][1] = '\0';
            tokenizedArray[tokenCount] = -1;
            index++;
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

    for (int i = 0; i < symbolLoc; i++){

            symbol_table[i].mark = 1;
    }


    
    //Print out the assembly code and symbol table
    printf("Assembly Code:\n\n");

    printf("Line\tOP\tL\tM\n");
    for (int i = 0; i < line; i++){
        printf("%d\t%s\t%d\t%d\n", i, code_table[i].kind, code_table[i].L, code_table[i].M);
    }

    printf("\n\nSymbol Table:\n\n");

    printf("Kind | Name\t       | Value | Level | Address | Mark\n---------------------------------------------------\n");
    
    for (int i = 0; i < symbolLoc; i++){
        printf("%5d| %15s |%6d |%6d |%8d |%4d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val,symbol_table[i].level,symbol_table[i].addr, symbol_table[i].mark);
    } 


    //Freeing the memory
    for (int i = 0; i< tokenCount; i++){
        if (tokenArray[i] != NULL){
            free(tokenArray[i]);
        }

    }
    free(tokenArray);
    free(tokenizedArray);

}


//Linear search to find the symbol in symbol table
int symboltablecheck(char str []){

    for (int i = 0; i < 500; i++){
        //printf("HERE: %s\n", symbol_table[i].name);
        if (strcmp(str, symbol_table[i].name) == 0){
            return(i);
        }
    }
    return -1;

}


//Program defined as block "."
int program(){

    //Always emit JMP first
    emit("JMP", 0, 3);
    line++;
    block();

    if (token != periodsym){

        //printf("\ntoken %d %d\n", token, loc);
        printf("Error: program must end with period");
        exit(EXIT_FAILURE);
    }


    emit("EOP", 0, 3);
    line++;
}

//Block defined as const-declaration, var-declaration, statement
int block(){
    const_declaration();

    int numVars = var_declaration();

    emit("INC", 0, 3 + numVars);

    line++;

    statement();
}

//For constant declaration
void const_declaration(){

    //See if the token is a constant
    if (token == constsym){
        //Repeat while we have comma
        do{
            loc++;
            token = tokenizedArray[loc];

            //Checks for errors
            if (token != identsym){
                printf("Error: const, var, and read keywords must be followed by identifier");
                exit(EXIT_FAILURE);
            }

            if (symboltablecheck (tokenArray[loc]) != -1){
                printf("Error: symbol name has already been declared");
                exit(EXIT_FAILURE);
            }

            //Increment, copy into symbol table
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


        //If we don't end in a semicolon, error
        if (token != semicolonsym){
            printf("Error: constant and variable declarations must be followed by a semicolon");
            exit(EXIT_FAILURE);
        }
        //Increment the location
        loc++;
        token = tokenizedArray[loc];

    }
}

//Var declaration, very similar to the const in nature
int var_declaration(){

    int numVars = 0;

    if (token == varsym){
        do{
            //Increment and have some checks
            numVars++;
            loc++;
            token = tokenizedArray[loc];

            if (token != identsym){
                printf("Error: const, var, and read keywords must be followed by identifier");
                exit(EXIT_FAILURE);
            }

            if (symboltablecheck(tokenArray[loc]) != -1){
                printf("Error: symbol name has already been declared");
                exit(EXIT_FAILURE);
            }

            //Kind, name, level, mark
            symbol_table[symbolLoc].kind = 2;
            strcpy(symbol_table[symbolLoc].name, tokenArray[loc]);
            //printf("%s %s %d\n", symbol_table[symbolLoc].name,tokenArray[loc], symbolLoc);

            symbol_table[symbolLoc].level = 0;
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

//Statement
void statement(){
 
    if (token == identsym){

        int symIdx = symboltablecheck(tokenArray[loc]);


        //Checks for errors
        if (symIdx == -1){
            printf("Error: undeclared identifier: %s", tokenArray[loc]);

            exit(EXIT_FAILURE);
        }
        if (symbol_table[symIdx].kind != 2){
            printf("Error: only variable values may be altered");
            exit(EXIT_FAILURE);
        }  
        //Increment

        loc++;
        token = tokenizedArray[loc];

        if (token != becomessym){
            printf("Error: assignment statements must use :=");
            exit(EXIT_FAILURE);
        }


        loc++;
        token = tokenizedArray[loc];

        expression();

        emit("STO", 0, symbol_table[symIdx].addr);
        line++;
        //printf("STO, L, %d", symbol_table[symIdx].addr);
        return;
        
    }
    
    //For beginsym
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
    //For if
    if (token == ifsym){
        loc++;
        token = tokenizedArray[loc];
        condition();
        int jpcIdx = line;

        emit("JPC", 0, jpcIdx*3);
        line++;
        //printf("JPC 0, %d", jpcIdx);
        
        //Check for then
        if (token != thensym){
            printf("Error: if must be followed by then");
            exit(EXIT_FAILURE);
        }
        loc++;
        token = tokenizedArray[loc];

        statement();


        //Check for fi
        if (token != fisym){
            printf("Error: fi supposed to come after if/then");

            exit(EXIT_FAILURE);
        }

        loc++;
        token = tokenizedArray[loc];
        //code[jpcIdx].M = current code idx
        code_table[jpcIdx].M = line*3;
        return;
    }
    
    //While loops
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

        //Emit JPC and JMP
        emit("JPC", 0, jpcIdx*3);
        line++;
        statement();
        emit("JMP", 0, loopIdx*3);

        line++;
        //Go back and update our code AFTER the loop
        code_table[jpcIdx].M = line*3;

        return;

    }
    //For read
    if (token == readsym){

        loc++;
        token = tokenizedArray[loc];

        if (token != identsym){
            printf("Error: const, var, and read keywords must be followed by identifier");
            exit(EXIT_FAILURE);
        }

        int symIdx = symboltablecheck(tokenArray[loc]);

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
        line++;
        //STO
        emit("STO", 0, symbol_table[symIdx].addr);
        line++;

        return;

    }
    //For write
    if (token == writesym){
        loc++;
        token = tokenizedArray[loc];

        expression();

        emit("SOU", 0, 1);
        line++;

        return;
    }


}
//Conditions
void condition(){

    //Odd, eqsym, etc. Pretty self explanatory
    if (token == oddsym){
        loc++;
        token = tokenizedArray[loc];

        expression();
        emit("ODD", 0, 11);
        line++;

    }
    else{
        expression();

        if (token == eqsym){
            loc++;
            token = tokenizedArray[loc];
            expression();

            //EQL
            emit("OPR",0, 5 );
            line++;

        }
        else if (token == neqsym){

            loc++;
            token = tokenizedArray[loc];
            expression();
            //NEQ
            emit("OPR", 0, 6);
            line++;
        }
        else if (token == lessym){
            loc++;
            token = tokenizedArray[loc];

            expression();
            //LES
            emit("OPR",0, 7);
            line++;
        }
        else if (token == leqsym){
            loc++;
            token = tokenizedArray[loc];

            expression();
            //LEQ
            emit("OPR", 0, 8);
            line++;

        }
        else if (token == gtrsym){
            loc++;
            token = tokenizedArray[loc];
            expression();
            //GTR
            emit("OPR", 0, 9);
            line++;
        }
        else if (token == geqsym){
            loc++;
            token = tokenizedArray[loc];
            expression();
            //GEQ
            emit("OPR", 0, 10);
            line++;
        }
        else{
            printf("Error: condition must contain comparison operator");
            exit(EXIT_FAILURE);
        }
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

    //Expression defined as term, etc
    term();

    //For plus and minus
    while (token == plussym || token == minussym){
            if (token == plussym){
                loc++;
                token = tokenizedArray[loc];

                term();

                emit("OPR", 0, 1);
                line++;
            }
            else{
                
                loc++;
                token = tokenizedArray[loc];

                term();

                emit("OPR",0, 2);
                line++;
            }
        }

}

void term(){

    //term defined as factor
    factor();

    //Check for mult and divide
    while (token == multsym || token == slashsym){
        if (token == multsym){

            loc++;
            token = tokenizedArray[loc];
            factor();
            
            emit("OPR", 0, 3);
            line++;

        }
        else if (token == slashsym){
            loc++;
            token = tokenizedArray[loc];
            factor();
            emit("OPR", 0, 4);
            line++;
        }
    }
}

void factor(){

    //Chck the final symbols

    if (token == identsym){
        int symIdx = symboltablecheck(tokenArray[loc]);

        if (symIdx == -1){
            printf("Error: undeclared identifier: %s", tokenArray[loc]);
            exit(EXIT_FAILURE);
        }
        if (symbol_table[symIdx].kind == 1){

            //printf("%s", symbol_table[symIdx].name);
            emit("LIT", 0, symbol_table[symIdx].val);
            line++;

        }
        else if (symbol_table[symIdx].kind == 2){

            emit("LOD", 0, symbol_table[symIdx].addr);
            line++;
        }

        loc++;
        token = tokenizedArray[loc];

    }
    //If a number, then emit literal
    else if (token == numbersym){


        emit ("LIT", 0, atoi(tokenArray[loc]));
        line++;
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
    //If none of the things
    else{
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");

        //printf("%d %d", token, loc);
        exit(EXIT_FAILURE);
    }
}

//We use this to "emit" to the assembly array.

void emit(char c [], int L, int M){

    strcpy(code_table[line].kind, c);
    code_table[line].L = L;
    code_table[line].M = M;
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
    //Increment the index
    index += runCounter;
    return index;
}
