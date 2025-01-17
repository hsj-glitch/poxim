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
// gcc -Wall -O3 daniellima_202200059689_poximv1.c -o daniellima_202200059689_poximv1.elf
// $ ./daniellima_202200059689_poximv1.elf input.hex output.out

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
    fscanf(input, "%*c %hhX", &byte);
    uint32_t i = 0;
    while (fscanf(input, "%*c %hhX", &byte) != EOF)
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
        const uint16_t bimm = ((instruction >> 31) << 11) | ((instruction >> 7) & 0b1) << 10 | (((instruction >> 25) & 0b111111) << 4) | (((instruction >> 8) & 0b1111) << 0);
        const uint32_t imm20 = ((instruction >> 31) << 19) | (((instruction & (0b11111111 << 12)) >> 12) << 11) | (((instruction & (0b1 << 20)) >> 20) << 10) | ((instruction & (0b1111111111 << 21)) >> 21);
        const uint32_t uimm20 = (instruction >> 12) << 12;
        printf("\ninstruction: 0x%08x\nopcode: 0x%08x\nrd: 0x%08x\nfunct3: 0x%08x\nfunct7: 0x%08x\nimm: 0x%08x\nuimm: 0x%08x\nrs1: 0x%08x\nrs2: 0x%08x\nbimm: 0x%08x\nimm20: 0x%08x\nuimm20: 0x%08x\n",instruction, opcode, rd, funct3, funct7, imm, uimm, rs1, rs2, bimm, imm20, uimm20);
        // Checking instruction opcode
        switch(opcode) {
            // R type (0110011)
            // case (0110011) instructions: (add, sub, xor, or, and, sll, srl, sra, slt, sltu) (mul, mulh, mulhsu, mulhu, div, divu, rem, remu)
            case 0b0110011:
                // add (funct3 == 000 and funct == 0000000)
                // 0x????????:add    rd,rs1,rs2     rd=0x????????+0x????????=0x????????
                if (funct3 == 0b000 && funct7 == 0b0000000) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:add    %s,%s,%s    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]+x[rs2]));
                    printf("0x%08x:add    %s,%s,%s    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]+x[rs2]));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] + x[rs2];
                }
                // sub (funct3 == 000 and funct == 0100000)
                // 0x????????:sub    rd,rs1,rs2     rd=0x????????-0x????????=0x????????
                else if (funct3 == 0b000 && funct7 == 0b0100000) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sub    %s,%s,%s    %s=0x%08x-0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]-x[rs2]));
                    printf("0x%08x:sub    %s,%s,%s    %s=0x%08x-0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]-x[rs2]));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] - x[rs2];
                }
                // xor (funt3 == 100 and funct7 == 0000000)
                // 0x????????:xor    rd,rs1,rs2     rd=0x????????^0x????????=0x????????
                else if (funct3 == 0b100 && funct7 == 0b0000000) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:xor    %s,%s,%s     %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]^x[rs2]));
                    printf("0x%08x:xor    %s,%s,%s     %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]^x[rs2]));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] ^ x[rs2];
                }
                // or (funct3 == 110 and funct7 == 0000000)
                // 0x????????:or     rd,rs1,rs2     rd=0x????????|0x????????=0x????????
                else if (funct3 == 0b110 && funct7 == 0b0000000) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:or     %s,%s,%s     %s=0x%08x|0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]|x[rs2]));
                    printf("0x%08x:or     %s,%s,%s     %s=0x%08x|0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]|x[rs2]));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] | x[rs2];
                }
                // and (funct3 == 111 and funct7 == 0000000)
                // 0x????????:and    rd,rs1,rs2     rd=0x????????&0x????????=0x????????
                else if (funct3 == 0b111 && funct7 == 0b0000000) {
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:and    %s,%s,%s     %s=0x%08x&0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]&x[rs2]));
                    printf("0x%08x:and    %s,%s,%s     %s=0x%08x&0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (x[rs1]&x[rs2]));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] & x[rs2];
                }
                // sll (funct3 == 001 and funct7 == 0000000)
                // 0x????????:sll    rd,rs1,rs2     rd=0x????????<<u5=0x????????
                else if (funct3 == 0b001 && funct7 == 0b0000000) {
                    // Calculating operation data
                    const uint32_t data = x[rs1] << (x[rs2] & 0b11111);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sll    %s,%s,%s    %s=0x%08x<<%u=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], (x[rs2] & 0b11111), data);
                    printf("0x%08x:sll    %s,%s,%s    %s=0x%08x<<%u=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], (x[rs2] & 0b11111), data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // srl (funct3 == 101 and funct7 == 0000000)
                // 0x????????:srl    rd,rs1,rs2     rd=0x????????>>u5=0x????????
                else if (funct3 == 0b101 && funct7 == 0b0000000) {
                    // Retrieving shift value
                    const uint8_t shamt = (uint8_t) x[rs2] & 0b11111;
                    // Calculating operation data
                    const uint32_t data = x[rs1] >> shamt;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:srl    %s,%s,%s    %s=0x%08x>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], shamt, data);
                    printf("0x%08x:srl    %s,%s,%s    %s=0x%08x>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], shamt, data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // sra (funct3 == 101 and funct7 == 0100000)
                // 0x????????:sra    rd,rs1,rs2     rd=0x????????>>>u5=0x????????
                else if (funct3 == 0b101 && funct7 == 0b0100000) {
                    // Retrieving shift value
                    const uint8_t shamt = (uint8_t) x[rs2] & 0b11111;
                    // Calculating operation data
                    const uint32_t data = (int32_t) x[rs1] >> shamt;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sra    %s,%s,%s    %s=0x%08x>>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], shamt, data);
                    printf("0x%08x:sra    %s,%s,%s    %s=0x%08x>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], shamt, data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // slt (funct3 == 010 and funct7 == 0000000)
                // 0x????????:slt    rd,rs1,rs2     rd=(0x????????<0x????????)=u1
                else if (funct3 == 0b010 && funct7 == 0b0000000) {
                    // Calculating operation data
                    const uint32_t data = ((int32_t) x[rs1]) < ((int32_t) x[rs2]);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:slt    %s,%s,%s     %s=(0x%08x<0x%08x)=%u", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    printf("0x%08x:slt    %s,%s,%s     %s=(0x%08x<0x%08x)=%u", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // sltu (funct3 == 011 and funct7 == 0000000)
                // 0x????????:sltu   rd,rs1,rs2     rd=(0x????????<0x????????)=u1
                else if (funct3 == 0b011 && funct7 == 0b0000000) {
                    // Calculating operation data
                    const uint32_t data = x[rs1] < x[rs2];
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sltu   %s,%s,%s     %s=(0x%08x<0x%08x)=%u", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    printf("0x%08x:sltu   %s,%s,%s     %s=(0x%08x<0x%08x)=%u", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // mul (funct3 == 000 and funct7 == 0000001)
                // 0x????????:mul    rd,rs1,rs2     rd=0x????????*0x????????=0x????????
                else if (funct3 == 0b000 && funct7 == 0b0000001) {
                    // Calculating operation data
                    uint64_t data = x[rs1] * x[rs2];
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:mul    %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (uint32_t)data);
                    printf("0x%08x:mul    %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], (uint32_t)data);
                    // Executing behavior
                    if(rd != 0) x[rd] = (uint32_t) data;
                }
                // mulh (funct3 == 001 and funct7 == 0000001)
                // 0x????????:mulh   rd,rs1,rs2     rd=0x????????*0x????????=0x????????
                else if (funct3 == 0b001 && funct7 == 0b0000001) {
                    // Getting signed rs values
                    int32_t srs1, srs2;
                    srs1 = (int32_t) x[rs1];
                    srs2 = (int32_t) x[rs2];
                    // Calculating operation data
                    int64_t data = (int64_t)srs1 * (int64_t)srs2;
                    int32_t hbits = (int32_t) (data >>32);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:mulh   %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], hbits);
                    printf("0x%08x:mulh   %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], hbits);
                    // Executing behavior
                    if(rd != 0) x[rd] = hbits;
                }
                // mulhsu (funct3 == 010 and funct7 == 0000001)
                // 0x????????:mulhsu rd,rs1,rs2     rd=0x????????*0x????????=0x????????
                else if (funct3 == 0b010 && funct7 == 0b0000001) {
                    // Getting signed rs1 value
                    int32_t srs1;
                    srs1 = (int32_t) x[rs1];
                    // Calculating operation data
                    int64_t data = (int64_t)srs1 * (int64_t)x[rs2];
                    int32_t hbits = (int32_t) (data >>32);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:mulhsu %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], hbits);
                    printf("0x%08x:mulhsu %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], hbits);
                    // Executing behavior
                    if(rd != 0) x[rd] = hbits;
                }
                // mulhu (funct3 == 011 and funct7 == 0000001)
                // 0x????????:mulhu  rd,rs1,rs2     rd=0x????????*0x????????=0x????????
                else if (funct3 == 0b011 && funct7 == 0b0000001) {
                    // Calculating operation data
                    uint64_t data = (uint64_t) x[rs1] *(uint64_t) x[rs2];
                    uint32_t hbits = (uint32_t) (data >>32);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:mulhu  %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], hbits);
                    printf("0x%08x:mulhu  %s,%s,%s    %s=0x%08x*0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], hbits);
                    // Executing behavior
                    if(rd != 0) x[rd] = hbits;
                }
                // div (funct3 == 100 and funct7 == 0000001)
                // 0x????????:div    rd,rs1,rs2     rd=0x????????/0x????????=0x????????
                else if (funct3 == 0b100 && funct7 == 0b0000001) {
                    // Getting signed rs values
                    int32_t srs1, srs2;
                    srs1 = (int32_t) x[rs1];
                    srs2 = (int32_t) x[rs2];
                    // Calculating operation data
                    int32_t data = 0xFFFFFFFF;
                    if (srs2 != 0) data = (int32_t) (srs1 / srs2);
                    // Outputting instruction to file
                    fprintf(output,"0x%08x:div    %s,%s,%s     %s=0x%08x/0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    printf("0x%08x:div    %s,%s,%s     %s=0x%08x/0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    if(rd != 0) x[rd] = data;
                }
                // divu (funct3 == 101 and funct7 == 0000001)
                // 0x????????:divu   rd,rs1,rs2     rd=0x????????/0x????????=0x????????
                else if (funct3 == 0b101 && funct7 == 0b0000001) {
                    // Calculating operation data
                    uint32_t data = 0xFFFFFFFF;
                    if (x[rs2] != 0) data = (int32_t) (x[rs1] / x[rs2]);
                    // Outputting instruction to file
                    fprintf(output,"0x%08x:divu   %s,%s,%s     %s=0x%08x/0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    printf("0x%08x:divu   %s,%s,%s     %s=0x%08x/0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    if(rd != 0) x[rd] = data;
                }
                // rem (funct3 == 110 and funct7 == 0000001)
                // 0x????????:rem    rd,rs1,rs2     rd=0x????????%0x????????=0x????????
                else if (funct3 == 0b110 && funct7 == 0b0000001) {
                    // Getting signed rs values
                    int32_t srs1, srs2;
                    srs1 = (int32_t) x[rs1];
                    srs2 = (int32_t) x[rs2];
                    // Calculating operation data
                    int32_t data = x[rs1];
                    if (srs2 != 0) data = (int32_t) (srs1 % srs2);
                    // Outputting instruction to file
                    fprintf(output,"0x%08x:rem    %s,%s,%s     %s=0x%08x%%0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    printf("0x%08x:rem    %s,%s,%s     %s=0x%08x%%0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    if(rd != 0) x[rd] = data;
                }
                // remu (funct3 == 111 and funct7 == 0000001)
                // 0x????????:remu   rd,rs1,rs2     rd=0x????????%0x????????=0x????????
                else if (funct3 == 0b111 && funct7 == 0b0000001) {
                    // Calculating operation data
                    uint32_t data = x[rs1];
                    if (x[rs2] != 0) data = (int32_t) (x[rs1] % x[rs2]);
                    // Outputting instruction to file
                    fprintf(output,"0x%08x:remu   %s,%s,%s     %s=0x%08x%%0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    printf("0x%08x:remu   %s,%s,%s     %s=0x%08x%%0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], x_label[rs2], x_label[rd], x[rs1], x[rs2], data);
                    if(rd != 0) x[rd] = data;
                }
                break;
            // I type (0010011)
            // case (0010011) instructions: (addi, xori, ori, andi, slli, srli, srai, slti, sltiu)
            case 0b0010011:
                // slli (funct3 == 001 and funct7 == 0000000)
                if (funct3 == 0b001 && funct7 == 0b0000000) {
                    // Calculating operation data
                    const uint32_t data = x[rs1] << uimm;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:slli   %s,%s,%u    %s=0x%08x<<%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    printf("0x%08x:slli   %s,%s,%u    %s=0x%08x<<%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // srli (funct3 == 101 and funct7 == 0000000)
                // 0x????????:srli   rd,rs1,u5      rd=0x????????>>u5=0x????????
                else if (funct3 == 0b101 && funct7 == 0b0000000) {
                    // Calculating operation data
                    const uint32_t data = x[rs1] >> uimm;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:srli   %s,%s,%u    %s=0x%08x>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    printf("0x%08x:srli   %s,%s,%u    %s=0x%08x>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // srai (funct3 == 101 and funct7 == 0b0100000)
                // 0x????????:srai   rd,rs1,u5      rd=0x????????>>>u5=0x????????
                else if (funct3 == 0b101 && funct7 == 0b0100000) {
                    // Calculating operation data
                    const uint32_t data = (int32_t) x[rs1] >> uimm;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:srai   %s,%s,%u    %s=0x%08x>>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    printf("0x%08x:srai   %s,%s,%u    %s=0x%08x>>>%u=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], imm, data);
                    // Updating register if not x[0] (zero)
                    if(rd != 0) x[rd] = data;
                }
                // slti (funct3 == 010)
                // 0x????????:slti   rd,rs1,0x???   rd=(0x????????<0x????????)=u1
                else if (funct3 == 0b010) {
                    
                }
                // addi (funct3 == 000)
                else if (funct3 == 0b000) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:addi   %s,%s,0x%03x    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] + simm));
                    printf("0x%08x:addi   %s,%s,0x%03x    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] + simm));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] + simm;
                }
                // xori (funct3 == 100)
                // 0x????????:xori   rd,rs1,0x???   rd=0x????????^0x????????=0x????????
                else if (funct3 == 0b100) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:xori   %s,%s,0x%03x    %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] ^ simm));
                    printf("0x%08x:xori   %s,%s,0x%03x    %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] ^ simm));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] ^ simm;
                }
                // ori (funct3 == 110)
                // 0x????????:ori    rd,rs1,0x???   rd=0x????????|0x????????=0x????????
                else if (funct3 == 0b110) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:ori    %s,%s,0x%03x    %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] | simm));
                    printf("0x%08x:ori    %s,%s,0x%03x    %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] | simm));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] | simm;
                }
                // andi (funct3 == 111)
                // 0x????????:andi   rd,rs1,0x???   rd=0x????????&0x????????=0x????????
                else if (funct3 == 111) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:andi   %s,%s,0x%03x    %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] & simm));
                    printf("0x%08x:andi   %s,%s,0x%03x    %s=0x%08x^0x%08x=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x_label[rd], x[rs1], simm, (x[rs1] & simm));
                    // Executing behavior
                    if(rd != 0) x[rd] = x[rs1] & simm;
                }
                break;
            // I type (0000011)
            // case (0000011) instructions: (lb, lh, lw, lbu, lhu)
            case 0b0000011:
                // lb (funct3 == 000)
                // 0x????????:lb     rd,0x???(rs1)  rd=mem[0x????????]=0x????????
                if (funct3 == 0b000) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Calculating 8 bit data with sign extension
                    uint32_t data = (mem[address - offset] >> 7) ? (0xFFFFFF00 | mem[address - offset]) : (mem[address - offset]);
                    // Loading rd register with memory value
                    if (rd != 0) x[rd] = data;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:lb     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                    printf("0x%08x:lb     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                }
                // lh (funct3 == 001)
                // 0x????????:lh     rd,0x???(rs1)  rd=mem[0x????????]=0x????????
                else if (funct3 == 0b001) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Calculating 16 bit data
                    uint16_t halfword = (mem[address - offset]) | (mem[(address - offset) + 1] << 8);
                    // Performing sign extension
                    uint32_t data = (halfword >> 15) ? (0xFFFF0000 | halfword) : (halfword);
                    // Loading rd register with memory value
                    if (rd != 0) x[rd] = data;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:lh     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                    printf("0x%08x:lh     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                }
                // lw (funct3 == 010)
                // 0x????????:lw     rd,0x???(rs1)  rd=mem[0x????????]=0x????????
                else if (funct3 == 0b010) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Calculating 32 bit data
                    uint32_t data = (mem[address - offset]) | (mem[(address - offset) + 1] << 8) | (mem[(address - offset) + 2] << 16) | (mem[(address - offset) + 3] << 24);
                    // Loading rd register with memory value
                    if (rd != 0) x[rd] = data;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:lw     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                    printf("0x%08x:lw     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                }
                // lbu (funct3 == 100)
                // 0x????????:lbu    rd,0x???(rs1)  rd=mem[0x????????]=0x????????
                else if (funct3 == 0b100) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Calculating 8 bit data
                    uint32_t data = (mem[address - offset]);
                    // Loading rd register with memory value
                    if (rd != 0) x[rd] = data;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:lbu    %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                    printf("0x%08x:lbu     %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                }
                // lhu (funct3 == 101)
                // 0x????????:lhu    rd,0x???(rs1)  rd=mem[0x????????]=0x????????
                else if (funct3 == 0b101) {
                    // Performing sign extension in immediate field
                    const uint32_t simm = (imm >> 11) ? (0xFFFFF000 | imm) : (imm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Calculating 16 bit data
                    uint32_t data = (mem[address - offset]) | (mem[(address - offset) + 1] << 8);
                    // Loading rd register with memory value
                    if (rd != 0) x[rd] = data;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:lhu    %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                    printf("0x%08x:lhu    %s,0x%03x(%s)  %s=mem[0x%08x]=0x%08x\n", pc, x_label[rd], imm, x_label[rs1], x_label[rd], address, data);
                }
                break;
            // S type (0100011)
            // case (0100011) instructions: (sb, sh, sw)
            case 0b0100011:
                // sb (funct3 == 000)
                if (funct3 == 0b000) {
                    // Retrieving full immediate camp
                    uint32_t simm = (funct7 << 5) | rd;
                    // Performing sign extension in immediate field
                    simm = (simm >> 11) ? (0xFFFFF000 | simm) : (simm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Calculating 8 bit data
                    uint8_t data = (x[rs2] & 0xFF);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sb    %s,0x%03x(%s)    mem[0x%08x]=0x%08x\n", pc, x_label[rs2], simm, x_label[rs1], address, data);
                    printf("0x%08x:sb     %s,0x%03x(%s)    mem[0x%08x]=0x%08x\n", pc, x_label[rs2], simm, x_label[rs1], address, data);
                    // Executing behavior
                    mem[address - offset] = (x[rs2] & 0xFF);
                }
                // sh (funct3 == 001)
                else if (funct3 == 0b001) {
                    // Retrieving full immediate camp
                    uint32_t simm = (funct7 << 5) | rd;
                    // Performing sign extension in immediate field
                    simm = (simm >> 11) ? (0xFFFFF000 | simm) : (simm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sh     %s,0x%03x(%s)    mem[0x%08x]=0x%08x\n", pc, x_label[rs2], simm, x_label[rs1], address, x[rs2]);
                    printf("0x%08x:sh     %s,0x%03x(%s)    mem[0x%08x]=0x%08x\n", pc, x_label[rs2], simm, x_label[rs1], address, x[rs2]);
                    // Executing behavior
                    mem[address - offset] = (x[rs2] & 0xFF);
                    mem[(address - offset)+1] = ((x[rs2] >> 8) & 0xFF);
                }
                // sw (funct3 == 010)
                // 0x????????:sw     rs2,0x???(rs1) mem[0x????????]=0x????????
                else if (funct3 == 0b010) {
                    // Retrieving full immediate camp
                    uint32_t simm = (funct7 << 5) | rd;
                    // Performing sign extension in immediate field
                    simm = (simm >> 11) ? (0xFFFFF000 | simm) : (simm);
                    // Calculating memory address
                    uint32_t address = x[rs1] + simm;
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:sw     %s,0x%03x(%s)    mem[0x%08x]=0x%08x\n", pc, x_label[rs2], simm, x_label[rs1], address, x[rs2]);
                    printf("0x%08x:sw     %s,0x%03x(%s)    mem[0x%08x]=0x%08x\n", pc, x_label[rs2], simm, x_label[rs1], address, x[rs2]);
                    // Executing behavior
                    mem[address - offset] = (x[rs2] & 0xFF);
                    mem[(address - offset)+1] = ((x[rs2] >> 8) & 0xFF);
                    mem[(address - offset)+2] = ((x[rs2] >> 16) & 0xFF);
                    mem[(address - offset)+3] = ((x[rs2] >> 24) & 0xFF);
                }
                break;
            // B type (1100011)
            // case (1100011) instructions: (beq, bne, blt, bge, bltu, bgeu)
            case 0b1100011:
                // beq (funct3 == 000)
                if (funct3 == 0b000) {
                    // Performing sign extension
                    uint32_t sbimm = (bimm >> 11) ? (0xFFFFF000 | (bimm << 1)) : (bimm << 1);
                    // Calculating branch address
                    uint32_t address = (x[rs1] == x[rs2]) ? (pc + sbimm) : (pc + 4);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:beq    %s,%s,0x%03x    (0x%08x==0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] == x[rs2]), address);
                    printf("0x%08x:beq    %s,%s,0x%03x    (0x%08x==0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] == x[rs2]), address);
                    // Executing behavior
                    pc = ((address == pc) ? (address) : (address - 4));
                }
                // bne (funct3 == 001)
                else if (funct3 == 0b001) {
                    // Performing sign extension
                    uint32_t sbimm = (bimm >> 11) ? (0xFFFFF000 | (bimm << 1)) : (bimm << 1);
                    // Calculating branch address
                    uint32_t address = (x[rs1] != x[rs2]) ? (pc + sbimm) : (pc + 4);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:bne    %s,%s,0x%03x    (0x%08x!=0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] != x[rs2]), address);
                    printf("0x%08x:bne    %s,%s,0x%03x    (0x%08x!=0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] != x[rs2]), address);
                    // Executing behavior
                    pc = ((address == pc) ? (address) : (address - 4));
                }
                // blt (funct3 == 100)
                // 0x????????:blt    rs1,rs2,0x???  (0x????????<0x????????)=u1->pc=0x????????
                else if (funct3 == 0b100) {
                    // Performing sign extension
                    uint32_t sbimm = (bimm >> 11) ? (0xFFFFF000 | (bimm << 1)) : (bimm << 1);
                    // Calculating branch address
                    uint32_t address = (x[rs1] < x[rs2]) ? (pc + sbimm) : (pc + 4);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:blt    %s,%s,0x%03x    (0x%08x<0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] < x[rs2]), address);
                    printf("0x%08x:blt    %s,%s,0x%03x    (0x%08x<0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] < x[rs2]), address);
                    // Executing behavior
                    pc = ((address == pc) ? (address) : (address - 4));
                }
                //bge (funct3 == 101)
                else if (funct3 == 0b101) {
                    // Performing sign extension
                    uint32_t sbimm = (bimm >> 11) ? (0xFFFFF000 | (bimm << 1)) : (bimm << 1);
                    // Calculating branch address
                    uint32_t address = (x[rs1] >= x[rs2]) ? (pc + sbimm) : (pc + 4);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:bge    %s,%s,0x%03x    (0x%08x>=0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] >= x[rs2]), address);
                    printf("0x%08x:bge    %s,%s,0x%03x    (0x%08x>=0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] >= x[rs2]), address);
                    // Executing behavior
                    pc = ((address == pc) ? (address) : (address - 4));
                }
                // bltu (funct3 == 110)
                else if (funct3 == 0b110) {
                    // Calculating branch address
                    uint32_t address = (x[rs1] < x[rs2]) ? (pc + (bimm << 1)) : (pc + 4);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:blt    %s,%s,0x%03x    (0x%08x<0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] < x[rs2]), address);
                    printf("0x%08x:blt    %s,%s,0x%03x    (0x%08x<0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] < x[rs2]), address);
                    // Executing behavior
                    pc = ((address == pc) ? (address) : (address - 4));
                }
                //bgeu (funct3 == 111)
                else if (funct3 == 0b111) {
                    // Calculating branch address
                    uint32_t address = (x[rs1] >= x[rs2]) ? (pc + (bimm << 1)) : (pc + 4);
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:bge    %s,%s,0x%03x    (0x%08x>=0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] >= x[rs2]), address);
                    printf("0x%08x:bge    %s,%s,0x%03x    (0x%08x>=0x%08x)=%d->pc=0x%08x\n", pc, x_label[rs1], x_label[rs2], bimm, x[rs1], x[rs2], (x[rs1] >= x[rs2]), address);
                    // Executing behavior
                    pc = ((address == pc) ? (address) : (address - 4));
                }
                break;
            // I type (1110011)
            // case (1110011) instructions: (ecall, ebreak)
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
            // U type (0110111)
            case 0b0110111:
                // lui
                // Executing behavior
                if(rd != 0) x[rd] = uimm20;
                // Outputting instruction to file
                fprintf(output, "0x%08x:lui    %s,0x%05x    %s=0x%08x\n", pc, x_label[rd], uimm20 >> 12, x_label[rd], uimm20);
                printf("0x%08x:lui    %s,0x%05x    %s=0x%08x\n", pc, x_label[rd], uimm20 >> 12, x_label[rd], uimm20);
                break;
            // U type (0010111)
            case 0b0010111:
                // auipc
                // Outputting instruction to file
                fprintf(output, "0x%08x:auipc  %s,0x%05x    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], uimm20 >> 12, x_label[rd], uimm20, pc, (pc + uimm20));
                printf("0x%08x:auipc  %s,0x%05x    %s=0x%08x+0x%08x=0x%08x\n", pc, x_label[rd], uimm20 >> 12, x_label[rd], uimm20, pc, (pc + uimm20));
                // Executing behavior
                if(rd != 0) x[rd] = pc + uimm20;
                break;
            // J type (1101111)
            case 0b1101111:
                // jal
                // Performing sign extension in immediate field
                const uint32_t simm = (imm20 >> 19) ? (0xFFF00000 | imm20) : (imm20);
                // Calculating operation address
                const uint32_t address = pc + (simm << 1);
                // Outputting instruction to console
                fprintf(output, "0x%08x:jal    %s,0x%05x    pc=0x%08x,%s=0x%08x\n", pc, x_label[rd], imm20, address, x_label[rd], pc + 4);
                printf("0x%08x:jal    %s,0x%05x    pc=0x%08x,%s=0x%08x\n", pc, x_label[rd], imm20, address, x_label[rd], pc + 4);
                if(rd != 0) x[rd] = pc + 4; // Updating register if not x[0] (zero)
                // Setting next pc minus 4
                pc = address - 4;
                break;
            //I type (1100111)
            case 0b1100111:
                // jalr (funct3 == 000)
                // 0x????????:jalr   rd,rs1,0x???   pc=0x????????+0x????????,rd=0x????????
                if (funct3 == 0b000) {
                    // Performing sign extension
                    const uint32_t simm = ((imm >> 11) ? (0xFFFFF000 | imm) : ((imm >> 1) << 1));
                    // Outputting instruction to file
                    fprintf(output, "0x%08x:jalr   %s,%s,0x%03x   pc=0x%08x+0x%08x,%s=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x[rs1], simm, x_label[rd], (pc + 4));
                    printf("0x%08x:jalr   %s,%s,0x%03x   pc=0x%08x+0x%08x,%s=0x%08x\n", pc, x_label[rd], x_label[rs1], imm, x[rs1], simm, x_label[rd], (pc + 4));
                    // Executing behavior
                    if (rd != 0) x[rd] = pc + 4; //Updating register if not x[0] (zero)
                    pc = (x[rs1] + simm) - 4;
                }
                break;
            default:
                // Outputting error message
                printf("error: unknown instruction opcode at pc = 0x%08x\n", pc);
                // Halting simulation
                run = 0;
        }
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
