#ifndef _H_aio_server_H_
#define _H_aio_server_H_

#include <list>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "native_aio.h"
#include "message.h"

class aio_server;
/*在所有异步请求返回 释放*/
class context
{
public:
    context(aio_server& srv);
    int send_message();
    int send_message_by_nowait(const std::string& data);
    /*close后不能再使用context*/
    int close();
    
private:
    int clientid_;
    int clientfd_;
    int incomplete_read_;
    int incomplete_write_;
    struct sockaddr_in clientaddr_;
    struct iocb iocb_read_;
    struct iocb iocb_write_;

    char read_buff[4096];

    std::list<std::string> write_msgs_;

    aio_server& srv_;

friend class aio_server;
};

class aio_server
{
public:
    aio_server();
    virtual ~aio_server();

    int start(short port);
    int stop();
    int cycle();

    virtual int process(context& ctx, const std::string& data) = 0;

private:
    
    int post_read(context* ctx);
    int post_write(context* ctx);

    int on_read(context* ctx, int64_t res);
    int on_write(context* ctx, int64_t res);

    void process_accept();
    void process_aio();

    void assert(int code);

private:
    int efd_;
    int epollfd_;
    int listenfd_;

    decoder decoder_;

    bool exit_;

friend class context;
};

#endif