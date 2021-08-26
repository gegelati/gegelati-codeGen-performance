//
// Created by thomas on 26/08/2021.
//

#include <iostream>
#include "progressBar.h"

void update_bar(size_t percent_done){
    int num_chars = percent_done * PROGRESS_BAR_WIDTH / 100;
    std::cout << "\r[";
    for (int i = 0; i < num_chars; ++i) {
        std::cout << "#";
    }
    for (int i = num_chars+1; i < PROGRESS_BAR_WIDTH; ++i) {
        std::cout << " ";
    }
    std::cout << "] " << percent_done << "% Done";
    std::flush(std::cout);
}