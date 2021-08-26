# gegelati-codeGen-performance
This Repository stores scripts and sources file to compare the inference between the native functions in gegelati and generated code.

To compile the target you need to install [gegelati](https://github.com/gegelati/gegelati) with the code gen activated
If you want to install gegelati on a different location use this [link](https://codimd.math.cnrs.fr/EvaYqdItQeW-9hVh1LtcVA?view#)
use the following commands 

```shell
git clone https://github.com/gegelati/gegelati-codeGen-performance.git
mkdir gegelati-codeGen-performance/bin
cd gegelati-codeGen-performance/bin
cmake ..
cmake --build . --target InferenceCompare
```

The results can be saved in a csv file if filename is given as the first parameter when calling the binary.
If no filename are given, results are printed on the standard output.
The following command start the measures.

```shell
./Release/InferenceCompare [filename_to_store_result]
```