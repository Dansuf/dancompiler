#pragma once
#include <exception>
#include <string>

class CompilerException : public std::exception
{
  std::string message;

public:

  virtual const char* what() const throw() override
  {
    return message.c_str();
  }



  CompilerException(std::string message)
  {
    this->message = message;
  }
};
