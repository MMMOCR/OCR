# OCR Project

## Setup
### Build

```make
make all
```

### Clean
```make
make clean
```

## Run

### GUI manual rotation
```sh
./gui/interface_rotate <path_to_image>
```

### Line detections
```sh
./utils/linesdetection <path_to_image>
```

### Image binarisation
```sh
./utils/imageutils <path_to_image>
```

### Solver
```sh
./solver/solver
``` 

### Neural network
```sh
./neuralnetwork/NN train <path_to_the_file_to_save_weights> <time_of_training>
./neuralnetwork/NN job <path_to_the_saved_weights> <first_digit_of_XOR><second_digit_of_XOR>
```
