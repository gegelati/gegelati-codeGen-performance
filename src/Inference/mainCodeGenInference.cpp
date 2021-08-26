#include <iostream>
#include <cfloat>
#include <ctime>
#include <gegelati.h>

extern "C" {
#include "TPG.h"
}

#include "Environnement/stickGameAdversarial.h"
#include "logger.h"
#include "progressBar.h"

/// instantiate global variable used to communicate between the TPG and the environment
int *in1;
int *in2;



int main(int argc, char* argv[]) {

    /// Import instruction set used during training(required only for gegelati Inference)
    Instructions::Set set;
    auto minus = [](int a, int b) -> double { return (double) a - (double) b; };
    auto cast = [](int a) -> double { return (double) a; };
    auto add = [](double a, double b) -> double { return a + b; };
    auto max = [](double a, double b) -> double { return std::max(a, b); };
    auto nulltest = [](double a) -> double { return (a == 0.0) ? 10.0 : 0.0; };
    auto modulo = [](double a, double b) -> double {
        if (b != 0.0) { return fmod(a, b); }
        else { return DBL_MIN; }
    };

    set.add(*(new Instructions::LambdaInstruction<double, double>(modulo)));
    set.add(*(new Instructions::LambdaInstruction<int, int>(minus)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add)));
    set.add(*(new Instructions::LambdaInstruction<int>(cast)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(max)));
    set.add(*(new Instructions::LambdaInstruction<double>(nulltest)));

    /// initialise AdversarialLearningEnvironment
    auto le = StickGameAdversarial(false);

    /// string to find the file param.json
    std::string path(ROOT_DIR "/dat/");
    std::string example("stick-game");
    std::string filename;

    /// Instantiate an Environment and import (required only for gegelati Inference)
    filename  = path + example + "_params.json";
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(filename.c_str(), params);
    Environment env(set, le.getDataSources(), params.nbRegisters);

    /// Import to use it for the inference with intern classes
    filename = path + example + "_out_best.dot";
    auto tpg = TPG::TPGGraph(env);
    TPG::TPGExecutionEngine tee(env);
    File::TPGGraphDotImporter dotImporter(filename.c_str(), env, tpg);
    dotImporter.importGraph();
    auto rootImported = tpg.getRootVertices().back();

    /// fetch data in the environment
    auto &st = le.getDataSources().at(0).get();
    in1 = st.getDataAt(typeid(int), 0).getSharedPointer<int>().get();
    auto &st2 = le.getDataSources().at(1).get();
    in2 = st2.getDataAt(typeid(int), 0).getSharedPointer<int>().get();

    /// set the number of game
    std::ostream* out = &std::cout;
    std::ofstream csvFile;
    if (argc > 1){
        csvFile.open(argv[1], std::ofstream::app);
        out = &csvFile;
    }

    std::vector<double> data;
    Logger l(*out);
    size_t nbGames = 1000000;
    size_t nbTestOfNbGames = 5;

    time_t startTime;
    time_t endTime;
    size_t nbGameCounter;
    int playerNb = 0;
    uint64_t action;
    // let's play, the only way to leave this loop is to play nbParties games

    update_bar(0);
    for(int i = 0; i < nbTestOfNbGames; ) {
        nbGameCounter = nbGames;
        time(&startTime);
        while (nbGameCounter != 0) {

            ///inference with generated C files
            action = inferenceTPG();

            //std::cout << "player : " << playerNb << " removes : " << action + 1 << " sticks " << std::endl;
            le.doAction(action);
            playerNb = !playerNb;

            if (le.isTerminal()) {
                //std::cout << "TPG nb" << playerNb << " won !" << std::endl;
                le.reset();
                nbGameCounter--;
                continue;
            }
        }
        time(&endTime);
        i++;
        update_bar(i*100/nbTestOfNbGames);
        data.push_back(difftime(endTime, startTime));
    }
    std::cout << std::endl;
    l.log("codeGen", data);

    data.clear();
    update_bar(0);
    for(int i = 0; i < nbTestOfNbGames;) {
        nbGameCounter = nbGames;
        time(&startTime);
        while (nbGameCounter != 0) {

            ///inference with generated C files
            action = ((const TPG::TPGAction *) tee.executeFromRoot(* rootImported).back())->getActionID();

            //std::cout << "player : " << playerNb << " removes : " << action + 1 << " sticks " << std::endl;
            le.doAction(action);
            playerNb = !playerNb;

            if (le.isTerminal()) {
                //std::cout << "TPG nb" << playerNb << " won !" << std::endl;
                le.reset();
                nbGameCounter--;
                continue;
            }
        }
        time(&endTime);
        i++;
        update_bar(i*100/nbTestOfNbGames);
        data.push_back(difftime(endTime, startTime));
    }
    std::cout << std::endl;
    l.log("Gegelati", data);
}