#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <string>

class MessageHandler
{
    
    public:
        
        typedef enum
        {
            INFO = 0,
            DEBUG = 1,
            DEBUG_2 = 2,
            DEBUG_3 = 3,
            DEBUG_4 = 4,
            DEBUG_5 = 5
        } ReportLevelType;
        
        void report(std::string Message, const ReportLevelType Level = INFO)
        {
            std::cout << "[  MSG  ] " << Message << std::endl;
        }
    
};

#endif // MESSAGE_HANDLER_H
