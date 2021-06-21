#ifndef BASIC_ERROR_HANDLER_H
#define BASIC_ERROR_HANDLER_H

#include <string>

class basic_error_handler
{
public:
    virtual ~basic_error_handler() = default;
    virtual void addError(const std::string& error) = 0;
};
#endif // BASIC_ERROR_HANDLER_H
