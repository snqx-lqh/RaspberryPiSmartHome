#.o文件存放路径
O_PATH_DIR = ./build
#源文件路径
SUBDIR = .

SOURCE_PATH = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
OBJ_PATH    = $(patsubst %.c,$(O_PATH_DIR)/%.o, $(notdir $(SOURCE_PATH)))

CFLAGS	= -Wall
LDLIBS  = -lwiringPi
CC	    = cc

main:  $(OBJ_PATH)
	$(CC) -o $@  $(OBJ_PATH) $(CFLAGS) $(LDLIBS)

$(O_PATH_DIR)/%.o: $(SUBDIR)/%.c 
	$(CC) -c $< $(CFLAGS) $(LDLIBS) -o $@

.PHONY: clean
clean:
	rm $(OBJ_PATH) 
	rm main