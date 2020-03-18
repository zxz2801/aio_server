#ifndef _H_message_H_
#define _H_message_H_

#include <stdio.h>
#include <string>
#include <sstream>
#include <list>

#define PKG_END "\r\n"
#define PKG_END_LEN 2

// class message
// {
// public:
//     message (char *data, size_t len);
//     virtual ~message();

//     /**
//      * 业务数据
//     */
//     char* data();

//     /**
//      * 业务数据长度
//     */
//     size_t size();

//     /**
//      * 网络数据包数据
//     */
//     char* raw();

//     /**
//      * 网路数据包长度
//     */
//     size_t raw_size();
// private:
//     char *data_;
//     size_t len_;
//     size_t capacity_;
// };

// class encoder
// {
// public:
//     // size_t encode(message* msg);
//     int encode(const std::string& msg, std::string& data);
// };



class decoder
{
private:
    enum Step
    {
        Step1 = 0,
        Step2 = 1,
        Step3 = 3,
    };
public:
    int append(char *date, size_t len);
    int append(const std::string& data);
    // message* fetch();
    bool fetch(std::string& data);

private:
    // std::list<message*> data_;
    // message *current_msg_;
    std::list<std::string> msgs_;
    std::string incomplete_msg_;
};

#endif // !_H_message_H_