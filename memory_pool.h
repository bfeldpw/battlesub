#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <array>
#include <stdexcept>
#include <vector>

#include "common.h"

template<class T, int S>
class MemoryPool
{
    public:
        
        MemoryPool()
        {
            FreeItems_.reserve(S);
            for (auto i = 0u; i < S; ++i )
            {
                FreeItems_.push_back(&Items_[i]);
            }
        }

        T* allocate()
        {
            if (FreeItems_.size() > 0)
            {
                T* Item = FreeItems_.back();
                FreeItems_.pop_back();
                
                DBLK(GlobalMessageHandler.reportDebug("Item allocated from memory pool. Items: " +
                                                      std::to_string(FreeItems_.size()), MessageHandler::DEBUG_L2);)
                
                return Item;
            }
            else
            {
                GlobalErrorHandler.reportError("Exceeding memory pool limits.");
                return &Dummy_;
            }
        }

        void free(T* Item)
        {
            if (FreeItems_.size() < S)
            {
                FreeItems_.push_back(Item);
                DBLK(GlobalMessageHandler.reportDebug("Item destroyed from memory pool. Items: " +
                                                      std::to_string(FreeItems_.size()), MessageHandler::DEBUG_L2);)
                Item = nullptr;
            }
            else
            {
                GlobalErrorHandler.reportError("Memory pool freeing not possible, all items already freed.");
            }
        }

    private:
        
        std::vector<T*>     FreeItems_;
        std::array<T, S>    Items_;
        T                   Dummy_;
        
};

#endif // MEMORY_POOL_H
