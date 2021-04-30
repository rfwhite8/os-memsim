#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;
    proc->page_count = 0;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::deleteProcess(uint32_t pid){
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid){
            _processes.erase(_processes.begin() + i);
            break;
        }
    }
}

void Mmu::deleteVariable(uint32_t pid, std::string var_name){
    printf("hello0");
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid){
            printf("hello1");
            for (int a = 0; i < _processes[i]->variables.size(); i++){
                if(_processes[i]->variables[a]->name == var_name){
                    printf("number of variables: %d\n", _processes[i]->variables.size());
                    _processes[i]->variables.erase(_processes[i]->variables.begin() + a);
                    break;
                }
            }
            break;
        }
    }
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

int Mmu::processPid(int index){
    return _processes[index]->pid;
}

bool Mmu::checkPid(uint32_t pid){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            return true;
        }
    }
    return false;
}

bool Mmu::checkVariable(uint32_t pid, std::string var_name){

    Process* process_to_check;
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            process_to_check = _processes[i];
            break;
        }
    }

    for(int i = 0; i < process_to_check->variables.size(); i++){
        if(process_to_check->variables[i]->name == var_name){
            return true;
        }
    }
    return false;
}

int Mmu::numProcesses(){
    return _processes.size();
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            if(_processes[i]->variables[j]->type != DataType::FreeSpace)
            {
                printf(" %-5d| %-14s| %12u |%11u \n", _processes[i]->pid, _processes[i]->variables[j]->name.c_str(),
                _processes[i]->variables[j]->virtual_address, _processes[i]->variables[j]->size);
            }
        }
    }
}

void Mmu::printVariable(uint32_t pid, std::string var_name, int physical_address, void *memory)
{
    Process* process;
    Variable* variable;

    for(int i = 0; i < _processes.size(); i++)
    {
        if(pid == _processes[i]->pid)
        {
            process = _processes[i];
            break;
        }
    }
    for(int i = 0; i < process->variables.size(); i++)
    {
        if(var_name == process->variables[i]->name)
        {
            variable = process->variables[i];
            break;
        }
    }

    int data_size = 1;
    if(variable->type == DataType::Short)
    {
        data_size = 2;
    }
    else if(variable->type == DataType::Int || variable->type == DataType::Float)
    {
        data_size = 4;
    }
    else if(variable->type == DataType::Long || variable->type == DataType::Double)
    {
        data_size = 8;
    }

    for(int i = 0; i < 4*data_size && i < variable->size; i += data_size)
    {
        void *temp;
        temp = (void *)((char *)memory + physical_address + i);
        if(variable->type == DataType::Char)
        {
            char *valueptr = (char *)temp;
            std::cout << *valueptr;
        }
        else if(variable->type == DataType::Short)
        {
            short *valueptr = (short *)temp;
            std::cout << *valueptr;
        }
        else if(variable->type == DataType::Int)
        {
            int *valueptr = (int *)temp;
            std::cout << *valueptr;
        }
        else if(variable->type == DataType::Float)
        {
            float *valueptr = (float *)temp;
            std::cout << *valueptr;
        }
        else if(variable->type == DataType::Long)
        {
            long *valueptr = (long *)temp;
            std::cout << *valueptr;
        }
        else if(variable->type == DataType::Double)
        {
            double *valueptr = (double *)temp;
            std::cout << *valueptr;
        }
        if(i + data_size < variable->size)
        {
            std::cout << ", ";
        }
        if(i + data_size < variable->size && i/data_size == 3)
        {
            std::cout << "... [" << variable->size/data_size << " items]";
        }
    }
    std::cout << std::endl;
}

uint32_t Mmu::findSpace(uint32_t pid, uint32_t size, int page_size)
{
    int address = -1;
    int address_to_check = 0;
    Process* process_to_check;

    for(int i = 0; i < _processes.size(); i++)
    {
        if(pid == _processes[i]->pid)
        {
            process_to_check = _processes[i];
            break;
        }
    }

    for(int i = 1; i < process_to_check->variables.size(); i++)
    {
        if(process_to_check->variables[i]->virtual_address - address_to_check >= size)
        {
            address = address_to_check;
            break;
        }
        else
        {
            address_to_check = process_to_check->variables[i]->virtual_address + process_to_check->variables[i]->size;
        }
    }

    if(address == -1 && page_size*process_to_check->page_count - address_to_check >= size)
    {
        address = address_to_check;
    }

    return address;
}

uint32_t Mmu::newPage(uint32_t pid)
{
    Process* process;

    for(int i = 0; i < _processes.size(); i++)
    {
        if(pid == _processes[i]->pid)
        {
            process = _processes[i];
            break;
        }
    }

    process->page_count += 1;
    return process->page_count - 1;
}

uint32_t Mmu::getVirtualAddress(uint32_t pid, std::string var_name)
{
    Process* process;
    Variable* variable;

    for(int i = 0; i < _processes.size(); i++)
    {
        if(pid == _processes[i]->pid)
        {
            process = _processes[i];
            break;
        }
    }
    for(int i = 0; i < process->variables.size(); i++)
    {
        if(var_name == process->variables[i]->name)
        {
            variable = process->variables[i];
            break;
        }
    }

    return variable->virtual_address;
}

DataType Mmu::getDataType(uint32_t pid, std::string var_name)
{
    Process* process;
    Variable* variable;

    for(int i = 0; i < _processes.size(); i++)
    {
        if(pid == _processes[i]->pid)
        {
            process = _processes[i];
            break;
        }
    }
    for(int i = 0; i < process->variables.size(); i++)
    {
        if(var_name == process->variables[i]->name)
        {
            variable = process->variables[i];
            break;
        }
    }

    return variable->type;
}