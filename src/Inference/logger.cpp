//
// Created by thomas on 26/08/2021.
//

#include <numeric>
#include "logger.h"

void Logger::log(const std::string &methodName, const std::vector<double>& data) {
    *out << methodName << separator << mean(data) ;
    for (auto d : data) {
        *out << separator << d;
    }
    *out << std::endl;
}

double Logger::mean(const std::vector<double> &data) {
    auto n = data.size();
    double average = -1;
    if ( n != 0) {
        average = std::accumulate( data.begin(), data.end(), 0.0) / n;
    }
    return average;
}
