#include "aio_server.h"
#include "stdio.h"
#include <iostream>

class my_server : public aio_server
{
public:
    virtual int process(context& ctx, const std::string& data)
    {
        std::cout << data << std::endl;
        ctx.send_message_by_nowait(data);
        ctx.send_message_by_nowait(data);
    }
};

int main(int argc, char** argv)
{
    my_server srv;
    int ret = srv.start(5757);
    if ( ret != 0)
    {
        printf("start error ret:%d\n", ret);
        return -1;
    }
    printf("running......\n");
    srv.cycle();
    srv.stop();
    printf("stoped......\n");
    return 0;
}