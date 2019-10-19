#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>

class ErrorHandler
{
    
    public:
        
        bool checkError()
        {
            bool ErrorState = ErrorFlag;
            
            ErrorFlag = false;
            return ErrorState;
        }
        
        void reportError(const std::string& Message)
        {
            std::cerr << "[ ERROR ] " << Message << std::endl;
            ErrorFlag = true;
        }
    
    private:
        
        bool ErrorFlag = false;
        
};

#endif // ERROR_HANDLER_H
