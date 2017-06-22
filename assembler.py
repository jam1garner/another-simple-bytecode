import struct
import sys

opCodes = {'nop':0, 'lc':1, 'lcu':2, 'lv':3, 'str':4, 'push':5, 'pop':6, 'bin':7, 'bil':8, 'bie':9, 'big':0xA, 'b':0xB, 'cmp':0xC, 'sys':0xD, 'inc':0xE, 'dec':0xF, 'copy':0x10, 'swap':0x11, 'add':0x12, 'sub':0x13, 'mult':0x14, 'div':0x15, 'mod':0x16, 'or':0x17, 'and':0x18, 'xor':0x19, 'not':0x1A, 'ls':0x1B, 'rs':0x1C}

def getInstruction(instructionName):
    if not instructionName in opCodes.keys():
        raise NotImplementedError('Instruction: '+instructionName+' not valid')
    else:
        return opCodes[instructionName]

def parseInstructions(filename):
    #Read in lines and remove extra white space + ignore comments
    with open(filename, 'r') as f:
        lines = [line.strip() for line in f.readlines()]

    for i in range(len(lines)):
        semicolonPos = lines[i].find(';')
        if semicolonPos != -1:
            lines[i] = lines[i][:semicolonPos].strip()
    lines = [line for line in lines if len(line) != 0 and not line.isspace()]

    #parse and convert to instructions
    operations = []
    for line in lines:
        opcodeLength = line.find(' ')
        #if the opcode has no parameters (no space in the line)
        if opcodeLength == -1:
            operations.append([line])
        else:
            opcode = getInstruction(line[:opcodeLength])
            parameters = line[opcodeLength+1:].replace(' ','').split(',')
            operations.append([opcode]+parameters)
    return operations

def writeByteCode(filename, operations):
    with open(filename, 'wb') as f:
        f.write('ASMR') #Magic
        f.write(struct.pack('<L', len(operations))) #Op code count
        f.write(struct.pack('<L', 1)) #Little Endian
        f.write(struct.pack('<L', 0)) #Padding
        for operation in operations:
            f.write(chr(operation[0]))
            wroteValue = False
            if len(operation) >= 2:
                if operation[1][0] == 'r':
                    f.write(chr(int(operation[1][1:])))
                elif operation[1] == 'acc':
                    f.write(chr(0xA))
                elif operation[1] == 'br':
                    f.write(chr(0xB))
                elif operation[1] == 'c':
                    f.write(chr(0xC))
                elif operation[1] == 'ss':
                    f.write(chr(0xD))
                elif operation[1] == 'se':
                    f.write(chr(0xE))
                else:
                    f.write(chr(0xFF))
                    f.write(struct.pack('<H',int(operation[1], 0)))
                    wroteValue = True
                if not wroteValue and len(operation) == 2:
                    f.write(struct.pack('<H', 0));
            if len(operation) == 3 and not wroteValue:
                arg2Num = 0
                if operation[2][0] == 'r':
                    arg2Num = int(operation[2].lstrip('r'),0)
                elif operation[2] == 'acc':
                    arg2Num = 0xA
                elif operation[2] == 'br':
                    arg2Num = 0xB
                elif operation[2] == 'c':
                    arg2Num = 0xC
                elif operation[2] == 'ss':
                    arg2Num = 0xD
                elif operation[2] == 'se':
                    arg2Num = 0xE
                else:
                    arg2Num = int(operation[2] ,0)
                f.write(struct.pack('<H',arg2Num))

def main():
    instructions = []
    if len(sys.argv) > 1:
        instructions = parseInstructions(sys.argv[1])
        writeByteCode(sys.argv[1].replace('.asa', '') + '.asbc', instructions)
    else:
        instructions = parseInstructions('test.asa')
        writeByteCode('test.bin', instructions)

if __name__ == '__main__':
    main()
