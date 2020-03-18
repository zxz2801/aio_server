#include "message.h"

int decoder::append(char *data, size_t len)
{
    return append(std::string(data, len));
}

int decoder::append(const std::string& data)
{
    incomplete_msg_ = incomplete_msg_+ data;

    std::size_t found = incomplete_msg_.find_first_of(PKG_END);

    while(found != std::string::npos)
    {
        msgs_.push_back("");
        msgs_.back() = incomplete_msg_.substr(0, found);

        incomplete_msg_ = incomplete_msg_.substr(found+PKG_END_LEN);
        found = incomplete_msg_.find_first_of(PKG_END);
    }

    return 0;
}

bool decoder::fetch(std::string& data)
{
    if (msgs_.empty())
    {
        return false;
    }

    data = msgs_.front();
    msgs_.pop_front();

    return true;
}