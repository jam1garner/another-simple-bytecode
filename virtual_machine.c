#include <stdlib.h>
#include <stdio.h>

struct instruction{
    unsigned char opCode;
    unsigned char registerNum;
    short parameter;
};

int main(int argc, char* argv[]){
    if(argc <= 1){
        printf("Usage: virtual_machine.exe [filename]\n");
        return 0;
    }
    FILE* file = fopen(argv[1],"rb");
    struct instruction* instructions;
    fseek(file, 0x4, SEEK_SET);//Goto instruction count
    int instructionCount;
    fread(&instructionCount, sizeof(int), 1, file);
    fseek(file, 0x10, SEEK_SET);//goto instructions
    instructions = malloc(instructionCount * sizeof(struct instruction));
    fread(instructions, sizeof(struct instruction), instructionCount, file);
    int r[10]; //Registers, example: r0 is r[0]
    int acc = 0, branch = 0, stackPosition = 0, counter = 0, temp, a, b; //a and b are for comparing/math
    int stack[0x1000];
    int* var; //used for temporary memory access
    for(int c = 0; c < instructionCount; c++){
        //Find the values of registers a and b for any operation that uses the value of both
        if((instructions[c].opCode >= 0x11 && instructions[c].opCode <= 0x1C) || instructions[c].opCode == 0xC){
            if(instructions[c].registerNum >= 0 && instructions[c].registerNum <= 9)
                a = r[instructions[c].registerNum];
            else if(instructions[c].registerNum == 0xA)
                a = acc;
            else if(instructions[c].registerNum == 0xB)
                a = branch;
            else if(instructions[c].registerNum == 0xC)
                a = counter;
            else if(instructions[c].registerNum == 0xD)
                a = *stack;
            else if(instructions[c].registerNum == 0xE)
                a = *stack + (stackPosition * sizeof(int));
            //get b value
            if(instructions[c].parameter >= 0 && instructions[c].parameter <= 9)
                b = r[instructions[c].parameter];
            else if(instructions[c].parameter == 0xA)
                b = acc;
            else if(instructions[c].parameter == 0xB)
                b = branch;
            else if(instructions[c].parameter == 0xC)
                b = counter;
            else if(instructions[c].parameter == 0xD)
                b = *stack;
            else if(instructions[c].parameter == 0xE)
                b = *stack + (stackPosition * sizeof(int));
        }
        //evaluate based on opcode
        switch(instructions[c].opCode){
            case 0://nop
                break;
            case 1://load constant
                if(instructions[c].registerNum >= 0 || instructions[c].registerNum <= 9)
                    r[instructions[c].registerNum] = instructions[c].parameter;
                else if(instructions[c].registerNum == 0xA)
                    acc = instructions[c].parameter;
                break;
            case 2://load constant upper
                if(instructions[c].registerNum >= 0 || instructions[c].registerNum <= 9)
                    r[instructions[c].registerNum] = (r[instructions[c].registerNum] & 0xFF) + (instructions[c].parameter << 16);
                else if(instructions[c].registerNum == 0xA)
                    acc = (acc & 0xFF) + (instructions[c].parameter << 16);
                break;
            case 3://load variable
                if(instructions[c].parameter > 0xA){ //If it's a memory address
                    *var = instructions[c].parameter;
                }
                else{//if it's a register
                    if(instructions[c].parameter == 0xA){//use memory address from acc
                        var = (int*)acc;
                    }
                    else{//use memory address in rX register
                        var = (int*)r[instructions[c].parameter];
                    }
                }
                if(instructions[c].registerNum < 0xA)
                    r[instructions[c].registerNum] = *var;
                else
                    acc =  *var;
                break;
            case 4://Store variable
                if(instructions[c].parameter > 0xA){ //If it's a memory address
                    var = (int*)(int)instructions[c].parameter;
                }
                else{//if it's a register
                    if(instructions[c].parameter == 0xA)//use memory address from acc
                        var = (int*)acc;
                    else//use memory address in rX register
                        var = (int*)r[instructions[c].parameter];
                }
                if(instructions[c].registerNum < 0xA)
                    *var = r[instructions[c].registerNum];
                else
                    *var = acc;
                break;
            case 5://Push
                if(instructions[c].registerNum <= 0x9)
                    stack[stackPosition] = r[instructions[c].registerNum];
                else if (instructions[c].registerNum == 0xA)
                    stack[stackPosition] = acc;
                else if (instructions[c].registerNum == 0xB)
                    stack[stackPosition] = branch;
                else if (instructions[c].registerNum == 0xC)
                    stack[stackPosition] = counter;
                else if (instructions[c].registerNum == 0xD)
                    stack[stackPosition] = (int)&stack;
                else if (instructions[c].registerNum == 0xE)
                    stack[stackPosition] = (int)&stack + (sizeof(int)*stackPosition);
                else if (instructions[c].registerNum == 0xFF)
                    //No register (push a constant)
                    stack[stackPosition] = instructions[c].parameter;

                stackPosition++;
                break;
            case 6://Pop
                stackPosition--;
                if(instructions[c].registerNum <= 0x9)
                    r[instructions[c].registerNum] = stack[stackPosition];
                else if (instructions[c].registerNum == 0xA)
                    acc = stack[stackPosition];
                else if (instructions[c].registerNum == 0xB)
                    branch = stack[stackPosition];
                else if (instructions[c].registerNum == 0xC)
                    counter = stack[stackPosition];
                stack[stackPosition] = 0;
            case 7://branch if not equal
                if(branch != 0){
                    if(instructions[c].registerNum == 0xFF)
                        c = instructions[c].parameter - 2;
                    else
                        c = r[instructions[c].registerNum] - 2;
                }
                break;
            case 8://branch if less than
                if(branch < 0){
                    if(instructions[c].registerNum == 0xFF)
                        c = instructions[c].parameter - 2;
                    else
                        c = r[instructions[c].registerNum] - 2;
                }
                break;
            case 9://branch if equal to
                if(branch == 0){
                    if(instructions[c].registerNum == 0xFF)
                        c = instructions[c].parameter - 2;
                    else
                        c = r[instructions[c].registerNum] - 2;
                }
                break;
            case 0xA://branch if greater than
                if(branch > 0){
                    if(instructions[c].registerNum == 0xFF)
                        c = instructions[c].parameter - 2;
                    else
                        c = r[instructions[c].registerNum] - 2;
                }
                break;
            case 0xB://branch without condition
                if(instructions[c].registerNum == 0xFF)
                    c = instructions[c].parameter - 2;
                else
                    c = r[instructions[c].registerNum] - 2;
                break;
            case 0xC://compare registers
                if(a > b)
                    branch = 1;
                else if(a == b)
                    branch = 0;
                else
                    branch = -1;
                break;
            case 0xD:; //syscall - used for higher level operations such as user input and printing
                int syscall = 0xFFFF;
                if(instructions[c].registerNum == 0xFF)
                    syscall = instructions[c].parameter;
                else if(instructions[c].registerNum >= 0 && instructions[c].registerNum <= 9)
                    syscall = r[instructions[c].registerNum];
                switch(syscall){
                    case 0:
                        stackPosition--;
                        printf("%i",stack[stackPosition]);
                        stack[stackPosition] = 0;
                        break;
                    case 1:
                        stackPosition--;
                        printf("%c",stack[stackPosition]);
                        stack[stackPosition] = 0;
                        break;
                    case 2:;
                        int temp = 0;
                        scanf("%i", &temp);
                        stack[stackPosition] = temp;
                        stackPosition++;
                        break;
                    case 3:;
                        char tempChar = 0;
                        scanf("%c", &tempChar);
                        stack[stackPosition] = tempChar;
                        stackPosition++;
                        break;
                    case 4:
                        while (stackPosition > 0 && stack[stackPosition - 1] != 0) {
                            stackPosition--;
                            printf("%c",stack[stackPosition]);
                            stack[stackPosition] = 0;
                        }
                        if(stackPosition != 0)
                            stackPosition--;
                        break;
                    default:
                        printf("No such syscall \"%i\" exists\n", syscall);
                        break;
                }
                break;
            case 0xE://insrement counter
                counter += instructions[c].parameter;
                break;
            case 0xF://decrement counter
                counter -= instructions[c].parameter;
                break;
            case 0x10:;//copy register
                if(instructions[c].registerNum >= 0 && instructions[c].registerNum <= 9)
                    temp = r[instructions[c].registerNum];
                else if(instructions[c].registerNum == 0xA)
                    temp = acc;
                else if(instructions[c].registerNum == 0xB)
                    temp = branch;
                else if(instructions[c].registerNum == 0xC)
                    temp = counter;
                else if(instructions[c].registerNum == 0xD)
                    temp = (int)&stack;
                else if(instructions[c].registerNum == 0xE)
                    temp = (int)&stack + (sizeof(int) * stackPosition);

                if(instructions[c].parameter >= 0 && instructions[c].parameter <= 9)
                    r[instructions[c].parameter] = temp;
                else if(instructions[c].parameter == 0xA)
                    acc = temp;
                else if(instructions[c].parameter == 0xB)
                    branch = temp;
                else if(instructions[c].parameter == 0xC)
                    counter = temp;
                break;
            case 0x11://swap regissters
                if(instructions[c].registerNum >= 0 && instructions[c].registerNum <= 9)
                    r[instructions[c].registerNum] = b;
                else if(instructions[c].registerNum == 0xA)
                    acc = b;
                else if(instructions[c].registerNum == 0xB)
                    branch = b;
                else if(instructions[c].registerNum == 0xC)
                    counter = b;

                if(instructions[c].parameter >= 0 && instructions[c].parameter <= 9)
                    r[instructions[c].parameter] = a;
                else if(instructions[c].parameter == 0xA)
                    acc = a;
                else if(instructions[c].parameter == 0xB)
                    branch = a;
                else if(instructions[c].parameter == 0xC)
                    counter = a;
                break;
            case 0x12://add
                acc = a + b;
                break;
            case 0x13://subtract
                acc = a - b;
                break;
            case 0x14://multiply
                acc = a * b;
                break;
            case 0x15://divide
                acc = a / b;
                break;
            case 0x16://modulus
                acc = a % b;
                break;
            case 0x17://or
                acc = a | b;
                break;
            case 0x18://and
                acc = a & b;
                break;
            case 0x19://xor
                acc = a ^ b;
                break;
            case 0x1A://not
                acc = !a;
                break;
            case 0x1B://left shift
                acc = a << b;
                break;
            case 0x1C://right shift
                acc = a >> b;
        }
    }
    free(instructions);
    return 0;
}
