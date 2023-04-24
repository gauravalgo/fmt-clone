#ifndef FORMAT_H_
#define FORMAT_H_

#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>
#include <iostream>
using namespace std;
namespace format{
    class FormatError: public std::runtime_error{
        public:
        FormatError(const std::string &message) : std::runtime_error(message){}
    };

class ArgFormatter;

template <typename Callback>
class ArgFormatterWithCallback;

//a sprintf like formator that automatically allocates enough storage to fit all the output
class Formatter {
    private:
    std::vector<char> buffer_;

    enum Type{
            CHAR,INT,UINT,LONG,ULONG,DOUBLE,LONG_DOUBLE,STRING,WSTRING,POINTER
    };
    struct Arg{
        Type type;
        union {
            int int_value;
            unsigned uint_value;
            double double_value;
            long long_value;
            unsigned long ulong_value;
            long double long_double_value;
            const char *string_value;
            const wchar_t *wstring_value;
            const void *pointer_value; 
        };
    
    explicit Arg(char value) : type(CHAR),int_value(value){}
    explicit Arg(int value) : type(INT),int_value(value){}
    explicit Arg(unsigned value) : type(UINT),uint_value(value){}
    explicit Arg(long value) : type(LONG),long_value(value){}
    explicit Arg(unsigned long value) :type(ULONG),ulong_value(value){}
    explicit Arg(double value) : type(DOUBLE),double_value(value){}
    explicit Arg(long double value) : type(LONG_DOUBLE),long_double_value(value){}
    explicit Arg(const char* value) :type(STRING),string_value(value){}
    explicit Arg(const wchar_t* value) : type(WSTRING),wstring_value(value){}
    explicit Arg(const void * value) : type(POINTER),pointer_value(value){}
};
    std::vector<Arg> args_;

    //Pointer to the part of the format string that has not been written yet.

    const char *format_;
    friend class ArgFormatter;
    
    void Add (const Arg &arg) {
        if(args_.empty())
        args_.reserve(10);
        args_.push_back(arg);
    }
    template<typename T>
    void FormatArg(const char*format, const T &arg,int width , int precision);

    void Format();

    public:
    Formatter(): format_(0) {std::cout<< "constructor is called" ;}

    ArgFormatter operator()(const char* format );

    template<typename Callback>
    ArgFormatterWithCallback<Callback> FormatWithCallback(const char* format);

    const char* c_str() const {return &buffer_[0];}
    std::size_t size() const {return buffer_.size() -1 ;}

    
};
class ArgFormatter{
    private:
    friend class Formatter;

    protected:
    mutable Formatter *formatter_;

   ArgFormatter(const ArgFormatter& other) :formatter_(other.formatter_){
    other.formatter_=0;
   }

   ArgFormatter& operator=(const ArgFormatter &other){
    formatter_ = other.formatter_;
    other.formatter_=0;
    return *this;
   }

   Formatter* FinishFormatting() const{
    Formatter *f = formatter_;
    if(f) {
        formatter_ = 0;
        f->Format();
    }
    return f;
   }
   public:
   explicit ArgFormatter(Formatter& f) : formatter_(&f){
   }
   ~ArgFormatter();
   friend const char* c_str(const ArgFormatter& af){
    return af.FinishFormatting()->c_str();
   }

   friend std::string str(const ArgFormatter& af){
    return af.FinishFormatting()->c_str();
   }

   ArgFormatter &operator<<(char value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
    ArgFormatter &operator<<(int value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
   ArgFormatter &operator<<(unsigned value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
    ArgFormatter &operator<<(long value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
    ArgFormatter &operator<<(unsigned long value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
     ArgFormatter &operator<<(double value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
    ArgFormatter &operator<<(long double value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
    ArgFormatter &operator<<(const char* value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
    ArgFormatter &operator<<(const void* value){
    formatter_->Add(Formatter::Arg(value));
    return *this;
   }
   //This method is not implemented intentionally to disallow output
   //of arbitary pointers . if you want to outpot apointer cast it to void*.

   template<typename T>
   ArgFormatter &operator<<(const T* value);

};
template<typename Callback>
class ArgFormatterWithCallback: public ArgFormatter {
    public:
    explicit ArgFormatterWithCallback(Formatter &f) : ArgFormatter(f) {}
    ~ArgFormatterWithCallback()
    {
        if(!formatter_) return;
        Callback callback;
        callback(*formatter_);
    }

};
inline ArgFormatter Formatter::operator()(const char *format)
{
    format_= format;
    return ArgFormatter(*this);
}

template <typename Callback>
ArgFormatterWithCallback<Callback>
Formatter::FormatWithCallback(const char *format){
    format_ = format;
    return ArgFormatterWithCallback<Callback>(*this);
}

class Format : public ArgFormatter {
  private:
  Formatter formatter_;

  //Do not implement
  Format(const Format&);
  Format& operator=(const Format&);
  public:
  explicit Format(const char* format) : ArgFormatter(formatter_){
    ArgFormatter::operator=(formatter_(format));
  }
  ~Format(){
    FinishFormatting();
  }
  
};

class Print : public ArgFormatter {
  private:
  Formatter formatter_;

  //do not implement 

  Print(const Print&);
  Print& operator=(Print&);
  public:
  explicit Print(const char* format ): ArgFormatter(formatter_){
    ArgFormatter::operator=(formatter_(format));
  }
  ~Print(){
    FinishFormatting();
    std::fwrite(formatter_.c_str(),1, formatter_.size(),stdout);
  }
};
}
namespace fmt  = format;
#endif // DEBUG