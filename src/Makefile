HOM_COMMON=/homenlunzhu/svn/yinzhong/trunk/common


MY_INC_PATH+=$(HOM_COMMON)/include

MY_LIB_PATH+=$(HOM_COMMON)b

CC = gcc
CXX = g++
CFLAGS = -g -fPIC -Wall -O2 -pipe -fno-ident -MMD -D_GNU_SOURCE


LIB_STATIC+= 
LIB_DYNAMIC+=


INC     = $(patsubst %, -I%,  $(MY_INC_PATH))
LIBPATH = $(patsubst %, -L%,  $(MY_LIB_PATH))
LIB     = $(LIBPATH) -Wl,-dn  $(patsubst %, -l%, $(LIB_STATIC)) -Wl,-dy $(patsubst %, -l%, $(LIB_DYNAMIC))

          
TARGET=aio_server
          
SOURCE+=$(wildcard *.cpp)
SOURCE+=$(wildcard *.cc)
SOURCE+=$(wildcard *.c)

OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(SOURCE))))
    
    
all:$(TARGET)

$(TARGET):$(OBJS)
	@echo -e Compling $< ...
	$(CXX) -o $@ $^ $(CFLAGS) $(LIB)
%.o: %.cpp
	@echo -e  Compling $< ...
	$(CXX) $(INC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	@echo -e  Compling $< ...
	$(CC) $(INC) $(CFLAGS) -c -o $@ $<
clean:
	@rm -f $(OBJS) $(TARGET)
	@rm -f *.d