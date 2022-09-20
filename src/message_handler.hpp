#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <atomic>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

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
            ERROR = 0,
            WARNING = 1,
            INFO = 2,
            DEBUG_L1 = 3,
            DEBUG_L2 = 4,
            DEBUG_L3 = 5
        } ReportLevelType;

        bool checkError()
        {
            return ErrorFlag.exchange(false);
        }

        void registerSource(const std::string& _Source, const std::string& _DisplayName)
        {
            SourceMap_.insert({_Source, _DisplayName});
        }
        
        void report(const std::string& _Source, const std::string& _Message, const ReportLevelType _Level = ERROR)
        {
            auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            std::stringstream Message;
            std::string L;

            switch (_Level)
            {
                case ERROR:
                    L = "[ E  ]";
                    break;
                case WARNING:
                    L = "[ W  ]";
                    break;
                case INFO:
                    L = "[ I  ]";
                    break;
                case DEBUG_L1:
                case DEBUG_L2:
                case DEBUG_L3:
                    L = "[ D" + std::to_string(_Level-2) + " ]";
                    break;
                default:
                    L = "[ ?  ]";

            }
            if (_Level <= Level_)
            {
                if (IsColored_)
                {
                    if (_Level == 0)
                    {
                        Message << "\033[31m";
                    }
                    else if (_Level == 1)
                    {
                        Message << "\033[33m";
                    }
                    else if (_Level == 2)
                    {
                        Message << "\033[32m";
                    }
                }
                Message << std::put_time(std::localtime(&t), "%Y-%m-%d %X") << "   ";

                if (SourceMap_.count(_Source) == 1)
                {
                    Message << L << "[ " << SourceMap_[_Source] << " ] " <<  _Message;
                }
                else
                {
                    Message << L << "[ " << _Source << ": source not found ] " <<  _Message;
                }
                if (IsColored_ && (_Level == 0 || _Level == 1) || _Level == 2)
                {
                    Message << "\033[0m";
                }
                if (_Level == 0)
                {
                    std::cerr << Message.str() << std::endl;
                    ErrorFlag = true;
                }
                else
                    std::cout << Message.str() << std::endl;
            }
        }

        void reportRaw(const std::string& _Message, const ReportLevelType _Level = INFO)
        {
            if (_Level <= Level_)
            {
                std::cout << _Message;
            }
        }

        void setColored(bool _IsColored) {IsColored_ = _IsColored;}
        void setLevel(ReportLevelType _Level) {Level_ = _Level;}

    private:

        std::atomic_bool ErrorFlag{false};
        std::atomic<ReportLevelType> Level_{DEBUG_L3};

        bool IsColored_{true};
        // Map source identifiers to display names
        std::unordered_map<std::string, std::string> SourceMap_;

};

#endif // MESSAGE_HANDLER_H
