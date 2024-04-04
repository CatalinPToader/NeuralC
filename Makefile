all: neuron

neuron: neuron.c
	gcc $< -o $@ -lm

.PHONY: clean
clean:
	rm -f neuron