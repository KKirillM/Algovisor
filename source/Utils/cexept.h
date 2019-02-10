#pragma once

#include <string>

namespace fo2base
{          
    inline void findAndReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
    {
        size_t pos = 0;
        while((pos = str.find(oldStr, pos)) != std::string::npos)
        {
            str.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
    }

    //----------------------------------------------------------------------------
    /// Базовый класс exceptions 
    //----------------------------------------------------------------------------
    class CExcept : public std::exception
    {
    public:
        explicit CExcept() : m_str("") 
        {
        }

        explicit CExcept(const std::string& txt) : m_str(txt) 
        {
            findAndReplace(m_str,"%","%%");
        }

        virtual ~CExcept() throw()
        {
        }

        virtual const char *what() const throw()
        { 
            return m_str.c_str();
        }

        virtual std::string GetMessage() const 
        { 
            return m_str; 
        }

    protected:
        std::string m_str;	// stored message string
    };
}
