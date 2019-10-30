#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <cassert>
#include <string>

// Only compile debug blocks in debug mode
#ifdef NDEBUG
    #define DBLK(x)
#else
    #define DBLK(x) x
#endif

class MessageHandler
{
    
    public:
        
        typedef enum
        {
            INFO = 0,
            DEBUG_L1 = 1, // Suggestion: Sporadic occurrences
            DEBUG_L2 = 2, // Suggestion: More or less once every frame
            DEBUG_L3 = 3  // Suggestion: Multiple times per frame, inner loops
        } ReportLevelType;
        
        void report(const std::string& Message, const ReportLevelType Level = INFO)
        {
            if (Level == INFO)
            {
                std::cout << "[  MSG  ] " << Message << std::endl;
            }
            else
            {
                assert(Level == INFO);
            }
        }
        DBLK(
            void reportDebug(const std::string& Message, const ReportLevelType Level = DEBUG_L1)
            {
                if (Level == DEBUG_L1 || Level == DEBUG_L2 || Level == DEBUG_L3)
                {
                    std::cout << "[  DBG  ] " << Message << std::endl;
                }
            }
        )
};

#endif // MESSAGE_HANDLER_H
