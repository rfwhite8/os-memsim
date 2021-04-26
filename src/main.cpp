// Peter Henkhaus and Ray White

#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"
#include <string.h>
#include <vector>

//starting
void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool creation);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    std::cin.clear();

    while (command.compare("exit") != 0) {
        std::vector<std::string> vector_command;

        char **segmented_command;
        splitString(command, ' ', vector_command);//splits string for error checking
        vectorOfStringsToArrayOfCharArrays(vector_command, &segmented_command);

        // Handle command
        if(strcmp(segmented_command[0], "create") == 0){
            int text_size = std::stoi(segmented_command[1]);
            int data_size = std::stoi(segmented_command[2]);
            createProcess(text_size, data_size, mmu, page_table);
        }else if(strcmp(segmented_command[0], "allocate") == 0){
            if(!mmu->checkPid(std::stoi(segmented_command[1]))){
                printf("error: process not found\n");
            }else {
                int pid = std::stoi(segmented_command[1]);
                std::string var_name = segmented_command[2];
                DataType type;
                if(strcmp(segmented_command[3], "char") == 0)
                {
                    type = DataType::Char;
                }
                else if(strcmp(segmented_command[3], "short") == 0)
                {
                    type = DataType::Short;
                }
                else if(strcmp(segmented_command[3], "int") == 0)
                {
                    type = DataType::Int;
                }
                else if(strcmp(segmented_command[3], "float") == 0)
                {
                    type = DataType::Float;
                }
                else if(strcmp(segmented_command[3], "long") == 0)
                {
                    type = DataType::Long;
                }
                else if(strcmp(segmented_command[3], "double") == 0)
                {
                    type = DataType::Double;
                }
                else
                {
                    type = DataType::Char;
                }
                int num_elements = std::stoi(segmented_command[4]);
                allocateVariable(pid, var_name, type, num_elements, mmu, page_table, false);
                //print Virtual Memory Adress
            }
        }else if(strcmp(segmented_command[0], "set") == 0){
            if(!mmu->checkPid(std::stoi(segmented_command[1]))){
                printf("error: process not found\n");
            } else {
                int pid = std::stoi(segmented_command[1]);
                std::string var_name = segmented_command[2];
                int offset = std::stoi(segmented_command[3]);
                DataType type = mmu->getDataType(pid, var_name);
                int var_size = 0;
                int datype = -1;
                if(type == DataType::Short)
                {
                    datype = 1;
                    var_size = 2;
                }
                else if(type == DataType::Int)
                {
                    datype = 2;
                    var_size = 4;
                }
                else if (type == DataType::Float)
                {
                    datype = 3;
                    var_size = 4;
                }
                else if(type == DataType::Long)
                {
                    datype = 4;
                    var_size = 8;
                }
                else if(type == DataType::Double)
                {
                    datype = 5;
                    var_size = 8;
                }
                else
                {
                    datype = 0;
                    var_size = 1;
                }
                for(int i = 4; i < vector_command.size(); i++)
                {
                    setVariable(pid, var_name, offset, segmented_command[i], mmu, page_table, memory);
                    offset += var_size;
                }
                //can do multiple values all at once
                }
        }else if(strcmp(segmented_command[0], "print") == 0 && segmented_command[1] != NULL){
            if(strcmp(segmented_command[1], "mmu") == 0){
                mmu->print();
            } else if(strcmp(segmented_command[1], "page") == 0){
                page_table->print();
            } else if(strcmp(segmented_command[1], "processes") == 0){
                for(int i = 0; i < mmu->numProcesses(); i++){
                    printf("%d\n", mmu->processPid(i));
                }
            } /*else if it is a PID and variable name
                print value
            }*/else {
                printf("error: print command not recognized\n");
            }
        }else if(strcmp(segmented_command[0], "free") == 0){
            if(!mmu->checkPid(std::stoi(segmented_command[1]))){
                printf("error: process not found\n");
            } else {
                int pid = std::stoi(segmented_command[1]);
                std::string var_name = segmented_command[2];
                freeVariable(pid, var_name, mmu, page_table);
            }
        }else if(strcmp(segmented_command[0], "terminate") == 0){
            if(!mmu->checkPid(std::stoi(segmented_command[1]))){
                printf("error: process not found\n");
            } else {
                int pid = std::stoi(segmented_command[1]);
                terminateProcess(pid, mmu, page_table);
            }
        }else{
            printf("error: command not recognized\n");
        }
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Clean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    int pid = mmu->createProcess();
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(pid, "<TEXT>", DataType::Char, text_size, mmu, page_table, true);
    allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table, true);
    allocateVariable(pid, "<STACK>", DataType::Char, 65536, mmu, page_table, true);
    //   - print pid
    printf("%d\n", pid);
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool creation)
{
    // TODO: implement this!
    int size = num_elements;
    if(type == DataType::Short)
    {
        size *= 2;
    }
    else if(type == DataType::Int || type == DataType::Float)
    {
        size *= 4;
    }
    else if(type == DataType::Long || type == DataType::Double)
    {
        size *= 8;
    }
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    int address = -1;
    while(address == -1)
    {
        address = mmu->findSpace(pid, size, page_table->getPageSize());
        //   - if no hole is large enough, allocate new page(s)
        if(address == -1)
        {
            uint32_t page_count = mmu->newPage(pid);
            page_table->addEntry(pid, page_count);
        }
    }
    //   - insert variable into MMU
    mmu->addVariableToProcess(pid, var_name, type, size, address);
    //   - print virtual memory address
    if(!creation)
    {
        printf("%ld\n", address);
    }
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    int physical_address = page_table->getPhysicalAddress(pid, mmu->getVirtualAddress(pid, var_name));
    physical_address += offset;
    std::cout << "physical_address: " << physical_address << std::endl;
    //   - insert `value` into `memory` at physical address
    void *temp = (void *)physical_address;
    temp = value;
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    mmu->deleteProcess(pid);
    //   - free all pages associated with given process
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: vector of strings - result will be stored here
*/
void splitString(std::string text, char d, std::vector<std::string>& result)
{
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        result.push_back(token);
    }
}

/*
   list: vector of strings to convert to an array of character arrays
   result: pointer to an array of character arrays when the vector of strings is copied to
*/
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result)
{
    int i;
    int result_length = list.size() + 1;
    *result = new char*[result_length];
    for (i = 0; i < list.size(); i++)
    {
        (*result)[i] = new char[list[i].length() + 1];
        strcpy((*result)[i], list[i].c_str());
    }
    (*result)[list.size()] = NULL;
}