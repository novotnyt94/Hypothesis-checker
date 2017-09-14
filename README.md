# Hypothesis-checker
Source code of the checker of induction assumption for an article by P. Gregor, T. Novotný, R. Škrekovski: "Extending perfect matchings to Gray codes with prescribed ends".

Used language: C++, standard C++11.

Compilation and run: 

1. Download the Hypothesis-checker directory and change values in settings.hpp to required (especially DIMENSION and HYPER_EDGE_CNT). 

2. Compile it with any C++11 compatible compiler (e.g. by g++ -std=c++11 -O2 for GCC GNU Compiler or by loading a project into Microsoft Visual Studio). No non-standard libraries are required.

3. Run the program with optional parameters -i (input matchings file), -m (output matchings file) and/or -o (results).

Detailed description of the program is provided in the enclosed documentation.

I would like to thank to "Studentský fakultní grant" with name "Publikace vlastních výsledků o strukturálních vlastnostech hyperkrychlí" by the Faculty of Mathematics and Physics, Charles University, which supported this project.
