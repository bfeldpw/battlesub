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
        
        void report(const std::string& _Message, const ReportLevelType _Level = INFO)
        {
            if (_Level == INFO)
            {
                std::cout << "[  MSG  ] " << _Message << std::endl;
            }
            else
            {
                assert(_Level == INFO);
            }
        }
        DBLK(
            void reportDebug(const std::string& _Message, const ReportLevelType _Level = DEBUG_L1)
            {
                if (_Level <= Level_)
                {
                    std::cout << "[  DBG  ] " << _Message << std::endl;
                }
            }
            void reportDebugRaw(const std::string& _Message, const ReportLevelType _Level = DEBUG_L1)
            {
                if (_Level <= Level_)
                {
                    std::cout << _Message;
                }
            }
        )

        void setLevel(ReportLevelType _Level) {Level_ = _Level;}

    private:

        ReportLevelType Level_ = DEBUG_L3;
};

#endif // MESSAGE_HANDLER_H
