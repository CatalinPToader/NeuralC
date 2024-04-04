OBJS 	= neuron.o network_config_checker.o
OUT 	= neuron network_config_checker
SOURCES = neuron.c network_config_checker.c

OBJS0	= neuron.o
SOURCE0	= neuron.c
OUT0	= neuron

OBJS1	= network_config_checker.o
SOURCE1	= network_config_checker.c
OUT1	= network_config_checker

CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: neuron network_config_checker

neuron: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0)

network_config_checker: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1)

neuron.o: $(SOURCE0)
	$(CC) $(FLAGS) $< 

network_config_checker.o: $(SOURCE1)
	$(CC) $(FLAGS) $< 

format:
	clang-format --style=Webkit -i $(SOURCES)

.PHONY: clean
clean:
	rm -f $(OBJS) $(OUT)
