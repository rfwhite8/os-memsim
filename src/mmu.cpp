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

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
            //std::cout << _processes[i]->pid << "|" << _processes[i]->variables[j]->name << "|" <<
                //_processes[i]->variables[j]->virtual_address << "|" << _processes[i]->variables[j]->size << std::endl;
            
            printf("%6d|%15s|%14u|%12u\n", _processes[i]->pid, _processes[i]->variables[j]->name.c_str(),
                _processes[i]->variables[j]->virtual_address, _processes[i]->variables[j]->size);
        }
    }
}

uint32_t findSpace(uint32_t pid, uint32_t size)
{
    uint32_t _max_size;
    std::vector<Process*> _processes;

    int address = -1;
    int address_to_check = 0;
    Process* process_to_check;

    for(int i = 0; i < _processes.size(); i++)
    {
        if(pid == _processes[i]->pid)
        {
            process_to_check = _processes[i];
        }
    }

    for(int i = 0; i < process_to_check->variables.size(); i++)
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

    if(address = -1 && _max_size - address_to_check >= size)
    {
        address = address_to_check;
    }

    return address;
}