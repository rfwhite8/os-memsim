#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

enum DataType : uint8_t {FreeSpace, Char, Short, Int, Float, Long, Double};

typedef struct Variable {
    std::string name;
    DataType type;
    uint32_t virtual_address;
    uint32_t size;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
    uint32_t page_count;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    uint32_t _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    void print();
    void printVariable(uint32_t pid, std::string var_name, int physical_address);
    int processPid(int index);
    bool checkPid(uint32_t pid);
    bool checkVariable(uint32_t pid, std::string var_name);
    int numProcesses();
    uint32_t findSpace(uint32_t pid, uint32_t size, int page_size);
    uint32_t newPage(uint32_t pid);
    uint32_t getVirtualAddress(uint32_t pid, std::string var_name);
    DataType getDataType(uint32_t pid, std::string var_name);
    void deleteProcess(uint32_t pid);
};

#endif // __MMU_H_
