CC = @gcc
CFLAGS = -Wall -g
LIBS = -lv4l2 -ljpeg 

SRCS_CAPTURE = main.c getjpeg.c yuv.c
SRCS_SERVER = server_recv_file.c
SRCS_CLIENT = client_send_file.c 

OBJS_CAPTURE = $(SRCS_CAPTURE:.c=.o)
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)

CAPTURE = capture
SERVER = server
CLIENT = client

all: $(CAPTURE) $(SERVER) $(CLIENT)
	@$(RM) *.o 

$(CAPTURE): $(OBJS_CAPTURE) 
	$(CC) $(CFLAGS) -o $(CAPTURE) $(OBJS_CAPTURE) $(LFLAGS) $(LIBS)

$(SERVER): $(OBJS_SERVER) 
	$(CC) $(CFLAGS) -o $(SERVER) $(OBJS_SERVER) $(LFLAGS) 

$(CLIENT): $(OBJS_CLIENT) 
	$(CC) $(CFLAGS) -o $(CLIENT) $(OBJS_CLIENT) $(LFLAGS) 

.c.o:
	$(CC) $(CFLAGS) -c $<  -o $@

clean:
	@$(RM) *.o *~ $(CAPTURE) $(SERVER) $(CLIENT)
