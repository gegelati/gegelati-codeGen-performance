//
// Created by thomas on 26/08/2021.
//

#ifndef PERFORMANCE_CODEGEN_LOGGER_H
#define PERFORMANCE_CODEGEN_LOGGER_H

#include <string>
#include <iostream>
#include <vector>
#include <ctime>

class Logger {
    /** Class used to log the result of the measures.
     * By default the class output on the standard output but it can output in a csv file
    **/
protected:
    std::ostream* out;

    std::string separator;
public:
    /**
     * Constructor of the logger.
     *
     * @param out std::stream where the results are outputed, by default stdout
     * @param separator std::string that separates the data when they are outputed. by default "\t".
     */
    Logger(std::ostream& out = std::cout, std::string separator = "\t") : out(&out), separator(std::move(separator)){

        *(this->out) << "method used" << separator << "means" << separator << " data " << std::endl;
    }

    /**
     * Function that print on the output stream the means and the data given as parameter
     *
     * @param methodName first element of the line printed, used as a legend
     * @param data std::vector holding the times measured that have to be printed
     */
    void log(const std::string& methodName, const std::vector<double>& data);

private:
    /**
     * Function that return the mean of the values of a std::vector<double>
     *
     * @param data std::vector that we want the mean
     * @return the mean of data
     */
    double mean(const std::vector<double>& data );
};


#endif //PERFORMANCE_CODEGEN_LOGGER_H
