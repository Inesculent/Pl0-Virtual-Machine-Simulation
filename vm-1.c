#include <stdio.h>

//Project address space


int pas [500];
int ir [3];

//Base function provided by assignment
int base( int BP, int L){
    int arb = BP;	// arb = activation record base
    while ( L > 0)     //find base L levels down
    {
        arb = pas[arb];
        L--;
    }
    return arb;
}

//Main
int main(void){


    //FETCH
    FILE *fp;

    char name [30];

    scanf("%s", name);

    fp = fopen(name, "r");

    //Open file and read all data to the PAS
    if (fp == NULL){
        printf("There was an error opening the file\n");
    }
    else{
        int temp = 0;
        int holder;
        while(fscanf(fp, "%d", &holder) == 1){
            pas[temp] = holder;
            temp++;
        }
    }
    //Close file
    fclose(fp);

    

    //Initalize Varibles
    int PC = 0;
    int BP = 499;
    int SP = 500;

    //This is exclusively used to calcuate bars. It in NO way interacts with the actual stack. 500 is to be "safe", but probably don't need more than 50
    int tab = 0;
    int tracker [500];
    for (int i = 0; i<500; i++){
        tracker[i] = -1;
    }

    //Print initial values
    printf("\t\tPC	BP	SP	Stack\n");
    printf("Initial values: %d\t%d\t%d \n\n", PC, BP, SP);
    
    //Execute and continue while eop = 1
    int eop = 1;
    while(eop == 1){
        
        //Load into instruction register
        ir[0] = pas[PC];
        ir[1] = pas[PC+1];
        ir[2] = pas[PC+2];

        //For operation code
        switch(ir[0]){
            //LIT
            case 1:
                //Literal push from ir[2]
                SP = SP-1;
                pas[SP] = ir[2];
                printf("  LIT %d %d", ir[1], ir[2]);
                break;
            //RTN
            case 2:
                
                //Arithmetic/Logical Instructions
                switch(ir[2]){
                    //RTN - Returns from a subroutine and restores caller's AR
                    case 0:
                        printf("  RTN %d %d", ir[1], ir[2]);
                        SP = BP + 1;
                        BP = pas[SP - 2];
                        //When we jump, we have to decrease PC by 3 because it'll be incremented by 3 at the end
                        PC = pas[SP - 3] - 3;

                        //remove the end tab
                        tracker[tab] = -1;
                        tab--;
                        tracker[tab] = -1;
                        break;
                    //ADD
                    case 1:
                        printf("  ADD %d %d", ir[1], ir[2]);
                        pas[SP + 1] = pas[SP + 1] + pas[SP];
                        SP = SP +1;
                        break;
                    //SUB

                    case 2:
                        pas[SP + 1] = pas[SP +1] - pas[SP];
                        SP = SP + 1;
                        printf("  SUB %d %d", ir[1], ir[2]);
                        break;
                    //MUL
                    case 3:
                        pas[SP+1] = pas[SP+1]*pas[SP];
                        SP = SP + 1;
                        printf("  MUL %d %d", ir[1], ir[2]);
                        break;
                    //DIV
                    case 4:
                        pas[SP+1] = pas[SP+1]/pas[SP];
                        SP = SP + 1;
                        printf("  DIV %d %d", ir[1], ir[2]);
                        break;
                    //EQL
                    case 5:
                        pas[SP + 1] = pas[SP + 1] == pas[SP];
                        SP = SP + 1;
                        printf("  EQL %d %d", ir[1], ir[2]);
                        break;
                    //NEQ
                    case 6:
                        pas[SP + 1] = pas[SP + 1] != pas[SP];
                        SP = SP + 1;
                        printf("  NEQ %d %d", ir[1], ir[2]);
                        break;
                    //LSS
                    case 7:

                        pas[SP + 1] = pas[SP + 1] < pas[SP];
                        SP = SP + 1;
                        printf("  LSS %d %d", ir[1], ir[2]);
                        break;  
                    //LEQ
                    case 8:
                        pas[SP + 1] = pas[SP + 1] <= pas[SP];
                        SP = SP + 1;
                        printf("  LEQ %d %d", ir[1], ir[2]);
                        break;
                    //GTR
                    case 9:
                        pas[SP + 1] = pas[SP + 1] > pas[SP];
                        SP = SP + 1;
                        printf("  GTR %d %d", ir[1], ir[2]);
                        break;
                    //GEQ
                    case 10:
                        pas[SP + 1] = pas[SP + 1] >= pas[SP];
                        SP = SP + 1;
                        printf("  GEQ %d %d", ir[1], ir[2]);
                        break;
                    //ODD
                    case 11:
                        pas[SP] = pas[SP] % 2;
                        printf("  ODD %d %d", ir[1], ir[2]);
                        break;

                }
                break;
                //LOD
            case 3:
                SP = SP-1;
                pas[SP] = pas[base(BP, ir[1]) - ir[2]];
                printf("  LOD %d %d", ir[1], ir[2]);
                break;
            //STO
            case 4:
                pas[base(BP, ir[1])- ir[2]] = pas[SP];
                SP = SP + 1;
                printf("  STO %d %d", ir[1], ir[2]);

                        
                break;
            
            //CAL
            case 5:
                pas[SP - 1] = base(BP, ir[1]); //Static link
                pas[SP -2] = BP; //Dynamic Link
                pas[SP - 3] = PC + 3; //return address
                BP = SP-1;
                PC = ir[2] - 3;
                printf("  CAL %d %d", ir[1], ir[2]);
                break;
            
            //INC
            case 6:
                //Store the stack pointer (for the bars)
                tracker[tab] = SP;
                tab++;

                //Function for INC
                SP = SP - ir[2];
                printf("  INC %d %d", ir[1], ir[2]);

                break;
            
            //JMP
            case 7:
                //When we jump, we have to decrease PC by 3 because it'll be incremented by 3 at the end
                PC = ir[2] - 3;
                printf("  JMP %d %d", ir[1], ir[2]);
                break;

            //JPC
            case 8:
                printf("  JPC %d %d", ir[1], ir[2]);
                if (pas[SP] == 0){
                    //When we jump, we have to decrease PC by 3 because it'll be incremented by 3 at the end
                    PC = ir[2] - 3;
                }

                SP = SP + 1;
                break;
            
            //SYS
            case 9:
                switch(ir[2]){
                    //SOU
                    case 1:
                        printf("Output result is: %d\n", pas[SP]);
                        printf("  SOU %d %d", ir[1], ir[2]);
                        SP = SP + 1;
                        break;
                    //SIN
                    case 2:

                        printf("Please Enter an Integer: ");
                        SP = SP-1;
                        char c;
                        while((c= getchar()) != '\n' && c != EOF);
                        pas[SP] = getc(stdin) - '0';
                        //pas[SP] = getc(fp);

                        printf("  SIN %d %d", ir[1], ir[2]);
                        break;
                    //EOP
                    case 3:
                        printf("  EOP %d %d", ir[1], ir[2]);
                        eop = 0;
                        break;
                }
        }
        //Increment program counter
        PC += 3;

        printf("\t%d\t%d\t%d\t", PC, BP, SP);
        int temp = 1;
        //printf("TRACKER %d --- TEMP %d --- \n", tracker[0], tab);

        //Print out the line
        for (int i = 499; i >= SP; i--){

            printf("%d ", pas[i]);
            //Logic for bars
            if (tracker[temp] == i && temp != tab){
                printf("| ");
                temp += 1;
            }
        }
        printf("\n");
    }

    return 0;
}

