#include "pagetable.h"
#include <math.h>

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

std::vector<int> PageTable::sortedFrames()
{
    std::vector<int> frames;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        frames.push_back(it->second);
    }

    std::sort(frames.begin(), frames.end());

    return frames;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    int frame = -1; 
    // Find free frame
    // TODO: implement this!
    std::vector<int> frames = sortedFrames();

    for(int i = 0; i < frames.size(); i ++){
        if(i != frames[i]){
            frame = i;
            break;
        }
    }
    if(frame == -1){
        frame = frames.size();
    }
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    uint32_t n = (uint32_t)log2(_page_size);
    int page_number = virtual_address >> n;
    int page_offset = (_page_size - 1) & virtual_address;

    uint32_t frame_number;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if(_table.count(entry) > 0)
    {
        frame_number = _table[entry];
        address = (_page_size * frame_number) + page_offset;
    }

    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        //printf("%6d|%12d|%14d", sortedKeys[i]->pid, );
    }
}

int PageTable::getPageSize()
{
    return _page_size;
}