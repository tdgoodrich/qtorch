#include "maxcut.h"
#include "nlopt.h"
#include "Exceptions.h"

using namespace qtorch;

void maxcutGetOptimalAngles(std::string& graphFilePath, int p, const std::string& outputPath){
    //the goal of this main class is to run the maxcut algorithm on a graph supplied in the command line arguments
    //the class will use qaoa to solve the maxcut problem, outputting a qasm circuit and simulating it for each iteration of of the maximization algorithm
    //the user will specify the "p" and the graph file in the command line, and this class will run a maxcut simulation based on the graph
    //a graph file will be in .dgf format -- an example can be found @qft8.dgf, but lines should only include e [node#1] [node#2]
    //part of the algorithm will be to generate a .qasm file for each iteration of the minimization (maximization) function
    //additionally, a simple optimization function will be needed

    Timer z;
    z.start();
    std::vector<double> betas_gammas0(2 * p);
    std::fill(betas_gammas0.begin(), betas_gammas0.begin() + betas_gammas0.size() / 2, 0.392699);
    std::fill(betas_gammas0.begin() + betas_gammas0.size() / 2, betas_gammas0.end(), 0.785399);
    ExtraData e(p,graphFilePath.c_str());
    e.outputFile = outputPath;


    auto F_p = [](const std::vector<double>& betas_gammas,std::vector<double>& grad, void * f_data)->double {
        //data
        std::ofstream maxCutCircuitQasm;
        std::ofstream tempGammasAndBetas(static_cast<ExtraData *>(f_data)->outputFile);


        //initialize and apply functions
        double f_pVal(0.0);
        int counter(0);
        std::for_each(static_cast<ExtraData *>(f_data)->pairs.begin(),static_cast<ExtraData *>(f_data)->pairs.end(), [&maxCutCircuitQasm, &f_pVal, f_data, &betas_gammas, &counter](std::pair<int,int> tem) {
            maxCutCircuitQasm.open("input/tempMaxCut.qasm");
            std::ofstream measurements("input/measureTest.txt");
            int numQubits(static_cast<ExtraData *>(f_data)->qubitsNeeded[counter]);
            maxCutCircuitQasm << numQubits << std::endl;
            outputInitialPlusStateToFile(maxCutCircuitQasm, numQubits);
            applyU_CsThenU_Bs(static_cast<ExtraData *>(f_data)->realIterations[counter], static_cast<ExtraData *>(f_data)->p, betas_gammas,numQubits,
                              maxCutCircuitQasm);
            for (int i = 0; i < numQubits; i++) {
                if (i == 0 || i == 1)
                {
                    measurements << "Z ";
                }
                else
                {
                    measurements<<"T ";
                }
            }
            measurements.close();
            maxCutCircuitQasm.close();

            ContractionTools qComputer("input/tempMaxCut.qasm", "input/measureTest.txt");
            qComputer.Contract(Stochastic);


            f_pVal += 0.5 * (1.0 - qComputer.GetFinalVal().real());
            ++counter;
        });
        for(const auto& gORb: betas_gammas) {
            tempGammasAndBetas<<gORb<<" ";
        }
        tempGammasAndBetas.close();
        return f_pVal;
    };



    //optimize(maximize)
    try {

         nlopt::opt optimization(nlopt::LN_COBYLA, 2 * p);
         optimization.set_max_objective(F_p, &e);
         std::vector<double> finalBetas_Gammas(optimization.optimize(betas_gammas0));
        std::cout<<"Took "<<z.getElapsed()<<" seconds"<<std::endl;

    }
    catch (std::runtime_error &error) {
        std::cout << error.what() << std::endl;
        std::cout<<"Took "<<z.getElapsed()<<" seconds"<<std::endl;
        return;
    }
}

void maxcutGetFinalStringUDS(std::string &graphFilePath, int p,std::string &contractionSequence,
        std::vector<double>& gAndB, const std::string& outfilePath){
    Timer z;
    z.start();
    ExtraData e(p,graphFilePath.c_str());
    e.outputFile = outfilePath;

    auto calculateFinalString = [&z, contractionSequence](const std::vector<double>& betas_gammas, ExtraData * f_data) {
        srand(time(NULL));
        std::vector<bool> answerString;
        std::ofstream maxCutCircuitQasm;
        std::ofstream maxCutAnswer(f_data->outputFile);
        maxCutAnswer<<f_data->fileName<<std::endl;
        double currentProb(1.0);

        maxCutCircuitQasm.open("input/tempMaxCut.qasm");
        maxCutCircuitQasm<<f_data->numQubits<<std::endl;
        outputInitialPlusStateToFile(maxCutCircuitQasm,f_data->numQubits);
        applyU_CsThenU_Bs(f_data->pairs,static_cast<ExtraData *>(f_data)->p,betas_gammas,f_data->numQubits,maxCutCircuitQasm);
        maxCutCircuitQasm.close();

        for(int i=0; i<f_data->numQubits; i++)
        {
            std::ofstream measurements("input/measureTest.txt");
            for(int j=0; j<static_cast<ExtraData *>(f_data)->numQubits; j++)
            {
                if(j<answerString.size())
                {
                    if(answerString[j])
                    {
                        measurements <<"1 ";
                    }
                    else
                    {
                        measurements<<"0 ";
                    }
                }
                else if(j==i)
                {
                    measurements<<"0 ";
                }
                else
                {
                    measurements<<"T ";
                }
            }
            measurements.close();
            ContractionTools p ("input/tempMaxCut.qasm","input/measureTest.txt");
            p.ContractUserDefinedSequenceOfWires( contractionSequence);

            double probNextGivenPrev(p.GetFinalVal().real()/currentProb);
                if (probNextGivenPrev > 0.5)
                {
                    currentProb *= probNextGivenPrev;
                    answerString.push_back(false);
                }
                else if (probNextGivenPrev < 0.5)
                {
                    currentProb *= (1.0-probNextGivenPrev);
                    answerString.push_back(true);
                }
                else
                {
                    int r = rand() % 2;
                    if (r == 1) {
                        answerString.push_back(true);
                    } else {
                        answerString.push_back(false);
                    }
                    currentProb*=0.5;
                }
        }
        int counter(0);
        std::for_each(answerString.begin(),answerString.end(),[&maxCutAnswer](bool b){
           maxCutAnswer<<b<<" ";
        });

        int cutEdgeCount(0);
        std::for_each(f_data->pairs.begin(),f_data->pairs.end(),[&answerString,&cutEdgeCount,&f_data](std::pair<int,int> edge){
            if(answerString[edge.first]!=answerString[edge.second])
            {
                cutEdgeCount++;
            }
        });
        maxCutAnswer<<std::endl<<"Cut edges: "<<cutEdgeCount<<"/"<<f_data->numQubits*3/2<<std::endl;
        maxCutAnswer<<"Time elapsed: "<<z.getElapsed()<<std::endl;
        maxCutAnswer.close();
    };

    calculateFinalString(gAndB,&e);
}

int main(int argc, char *argv[]) {
    if(argc < 5){
        std::cout<<"Not enough arguments"<<std::endl;
        return -1;
    }
    int pVal = atoi(argv[2]);
    int action = atoi(argv[3]);

    std::string graphFilePath(argv[1]);
    mkdir("output", 0755);
    mkdir("input", 0755);
    int procSec = 6000;

    if(action == 2){
        std::ifstream inAngles(argv[4]);
        std::string outputFile(argv[5]);
        std::string conseq(argv[6]);
        std::vector<double> gammasAndBetas;

        for(int i=0; i < 2*pVal; i++){
            double z;
            inAngles>>z;
            gammasAndBetas.push_back(z);
        }
        inAngles.close();
        maxcutGetFinalStringUDS(graphFilePath, pVal, conseq, gammasAndBetas, outputFile);

    }

    if(action == 1){
        std::ifstream inAngles(argv[4]);
        std::string outputQASM(argv[5]);

        std::vector<double> gammasAndBetas;

        for(int i=0; i < 2*pVal; i++){
            double z;
            inAngles>>z;
            gammasAndBetas.push_back(z);
        }
        inAngles.close();
        ExtraData e(pVal, graphFilePath.c_str());
        std::ofstream maxCutCircuitQasm(outputQASM);
        maxCutCircuitQasm << e.numQubits << std::endl;
        outputInitialPlusStateToFile( maxCutCircuitQasm, e.numQubits);
        applyU_CsThenU_Bs(e.pairs, pVal, gammasAndBetas, e.numQubits, maxCutCircuitQasm);
        maxCutCircuitQasm.close();

    }

    if(action == 0){
        std::string outputPath(argv[4]);
        std::string outputQASM(argv[5]);
        maxcutGetOptimalAngles(graphFilePath, pVal, outputPath);
        std::ifstream inAngles(outputPath);
        std::vector<double> gammasAndBetas;

        for(int i=0; i<2*pVal; i++){
            double z;
            inAngles>>z;
            gammasAndBetas.push_back(z);
        }
        inAngles.close();
        ExtraData e(pVal, graphFilePath.c_str());
        std::ofstream maxCutCircuitQasm(outputQASM);
        maxCutCircuitQasm << e.numQubits << std::endl;
        outputInitialPlusStateToFile( maxCutCircuitQasm, e.numQubits);
        applyU_CsThenU_Bs(e.pairs, pVal, gammasAndBetas, e.numQubits, maxCutCircuitQasm);
        maxCutCircuitQasm.close();
    }
}
