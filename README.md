
# Neural Network in C
A small personal project aimed at creating neural networks in C to better understand how they work.


## Installation & running

```bash
git clone https://github.com/CatalinPToader/NeuralC
cd neural-c
mkdir build && cd build
cmake ..
make
./neuron
```

 Network checker

```bash
./network_config_checker examples/example.network
```

## TODO:
- Saving trained networks to file
- Loading trained networks from files
- Loading networks from network descriptor files
- Training checkpoints
- Interactive mode (load, save, train, cancel training, run without restarting)
