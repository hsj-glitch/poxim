//
// Poxim-V C simulator example
// 
// (C) Copyright 2024 Bruno Otavio Piedade Prado
//
// This file is part of Poxim-V.
//
// Poxim-V is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Poxim-V is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Poxim-V.  If not, see <https://www.gnu.org/licenses/>.
//

// How to build and run:
// $ gcc -Wall -O3 nomesobrenome_123456789012_exemplo.c -o nomesobrenome_123456789012_exemplo.elf
// $ ./nomesobrenome_123456789012_exemplo.elf input.hex output.out

// Standard integer library
#include <stdint.h>
// Standard library
#include <stdlib.h>
// Standard I/O library
#include <stdio.h>

/**
 * Main function
 * @param argc  Number of command line arguments
 * @param argv  Command line arguments
 * @return      Returns the program execution status
 */
int main(int argc, char* argv[]) {
    // Outputting separator
    printf("--------------------------------------------------------------------------------\n");
    // Iterating over arguments
    for(uint32_t i = 0; i < argc; i++) {
        // Outputting argument
        printf("argv[%i] = %s\n", i, argv[i]);
    }
    // Opening input and output files using proper permissions
    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    
    if (input == NULL)
    {
        printf("Error opening input file\n");
        return 1;
    }
    // Setting memory offset to 0x80000000
	const uint32_t offset = 0x80000000;
    // Creating 32 registers initialized with zero and labels
    uint32_t x[32] = { 0 };
    const char* x_label[32] = { "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };
    // Creating pc register initialized with memory offset
    uint32_t pc = offset;
    // Creating 32 KiB memory for both data and instructions
    uint8_t* mem = (uint8_t*)(malloc(32 * 1024));
    // Reading memory contents from input hexadecimal file
    // Carregando cada linha do arquivo em uma posicao na memoria
    uint8_t byte = 0;
    fscanf(input, "%*c %X", &byte);
    uint32_t i = 0;
    while (fscanf(input, "%*c %X", &byte) != EOF)
    {
        mem[i] = (uint8_t)byte;
        ++i;
    }
    // Outputting separator
    printf("--------------------------------------------------------------------------------\n");
    // Setting run condition
    uint8_t run = 1;
    // Loop while condition is true
    while(run) {
        // Reading instruction from memory (4 byte alignment)
        const uint32_t instruction = ((uint32_t*)(mem))[(pc - offset) >> 2];
        // Retrieving instruction opcode (6:0)
        const uint8_t opcode = instruction & 0b1111111;
        // Retrieving instruction fields
        const uint8_t funct7 = instruction >> 25;
        const uint16_t imm = instruction >> 20;
        const uint8_t uimm = (instruction & (0b11111 << 20)) >> 20;
        const uint8_t rs2 = uimm;
        const uint8_t rs1 = (instruction & (0b11111 << 15)) >> 15;
        const uint8_t funct3 = (instruction & (0b111 << 12)) >> 12;
        const uint8_t rd = (instruction & (0b11111 << 7)) >> 7;
        const uint32_t imm20 = ((instruction >> 31) << 19) | (((instruction & (0b11111111 << 12)) >> 12) << 11) | (((instruction & (0b1 << 20)) >> 20) << 10) | ((instruction & (0b1111111111 << 21)) >> 21);
        const uint32_t uimm20 = (instruction >> 12) << 12;
        printf("\ninstruction: 0x%08x\nopcode: 0x%08x\nrd: 0x%08x\nfunct3: 0x%08x\nfunct7: 0x%08x\nimm: 0x%08x\nuimm: 0x%08x\nrs1: 0x%08x\nrs2: 0x%08x\nimm20: 0x%08x\nuimm20: 0x%08x\n",instruction, opcode, rd, funct3, funct7, imm, uimm, rs1, rs2, imm20, uimm20);
        // Checking instruction opcode
        switch(opcode) {
            // R type (0110011)
            case 0b0110011:
                // add (funct3 == 000 and funct == 0000000)
                if (funct3 == 0b000 && funct7 == 0b0000000) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:add   %s,%s,%s  %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1] + x[rs2]));
                    printf("0x%08x:add   %s,%s,%s  %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1] + x[rs2]));
                    // Executing behavior
                    x[rd] = x[rs1] + x[rs2];
                    break;
                }
                // sub (funct3 == 000 and funct == 0100000)
                else if (funct3 == 0b000 && funct7 == 0b0100000) {
                    // Executing behavior
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sub   %s,%s,%s  %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1] - x[rs2]));
                    printf("0x%08x:sub   %s,%s,%s  %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1] - x[rs2]));
                    // Executing behavior
                    x[rd] = x[rs1] - x[rs2];
                    break;
                }
            // I type (0010011)
            case 0b0010011:
                // slli (funct3 == 001 and funct7 == 0000000)
                if (funct3 == 0b001 && funct7 == 0b0000000) {
                    // Calculating operation data
                    const uint32_t data = x[rs1] << uimm;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:slli   %s,%s,%u  %s=0x%08x<<%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    printf("0x%08x:slli   %s,%s,%u  %s=0x%08x<<%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // addi (funct3 == 000)
                else if (funct3 == 0b000) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    printf("simm: 0x%08x\n", simm);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:addi   %s,%s,0x%04x  %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] + simm));
                    printf("0x%08x:addi   %s,%s,0x%04x  %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] + simm));
                    // Executing behavior
                    x[rd] = x[rs1] + simm;
                }
                // Breaking case
                break;
            // I type (1110011)
            case 0b1110011:
                // ebreak (funct3 == 000 and imm == 1)
                if(funct3 == 0b000 && imm == 1) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:ebreak\n", pc);
                    printf("0x%08x:ebreak\n", pc);
                    // Retrieving previous and next instructions
                    const uint32_t previous = ((uint32_t*)(mem))[(pc - 4 - offset) >> 2];
                    const uint32_t next = ((uint32_t*)(mem))[(pc + 4 - offset) >> 2];
                    // Halting condition
                    if(previous == 0x01f01013 && next == 0x40705013) run = 0;
                }
                break;
            // U tupe (0010111)
            case 0b0010111:
                // auipc
                // Setting field to proper value
                // Outputting instruction to file
                fprintf(output, "0x%08x:auipc   %s,0x%05x    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], uimm20, x_label[rd], uimm20, pc, (pc + uimm20));
                printf("0x%08x:auipc   %s,0x%05x    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], uimm20, x_label[rd], uimm20, pc, (pc + uimm20));
                // Executing behavior
                x[rd] = pc + uimm20;
                break;
            // J type (1101111)
            case 0b1101111:
                // jal
                // Performing sign extension in immediate field
                const uint32_t simm = (imm20 >> 20) ? (0xFFF00000) : (imm20);
                // Calculating operation address
                const uint32_t address = pc + (simm << 1);
                // Outputting instruction to console
                fprintf(output, "0x%08x:jal    %s,0x%05x    pc=0x%08x,%s=0x%08x\n", pc, x_label[rd], imm20, address, x_label[rd], pc + 4);
                printf("0x%08x:jal    %s,0x%05x    pc=0x%08x,%s=0x%08x\n", pc, x_label[rd], imm20, address, x_label[rd], pc + 4);
                // Updating register if not x[0] (zero)
                if(rd != 0) x[rd] = pc + 4;
                // Setting next pc minus 4
                pc = address - 4;
                // Breaking case
                break;
            // Unknown
            default:
                // Outputting error message
                printf("error: unknown instruction opcode at pc = 0x%08x\n", pc);
                // Halting simulation
                run = 0;
        }
        // Making sure x[0] is always 0
        x[0] = 0;
        // Incrementing pc by 4
        pc = pc + 4;
    }
    // Closing input and output files
    fclose(input);
    fclose(output);
    // Outputting separator
    printf("--------------------------------------------------------------------------------\n");
    // Returning success status
    return 0;
}
