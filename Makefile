all: neuron

neuron: neuron.c
	gcc $< -g -o $@ -lm

.PHONY: clean
clean:
	rm -f neuron