#include "Solver.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>

#include <cmath>

#include "MpSolver.h"
#include "CsvReader.h"


using namespace std;


namespace szx {

#pragma region Solver::Cli
int Solver::Cli::run(int argc, char * argv[]) {
    Log(LogSwitch::Szx::Cli) << "parse command line arguments." << endl;
    Set<String> switchSet;
    Map<String, char*> optionMap({ // use string as key to compare string contents instead of pointers.
        { InstancePathOption(), nullptr },
        { SolutionPathOption(), nullptr },
        { RandSeedOption(), nullptr },
        { TimeoutOption(), nullptr },
        { MaxIterOption(), nullptr },
        { JobNumOption(), nullptr },
        { RunIdOption(), nullptr },
        { EnvironmentPathOption(), nullptr },
        { ConfigPathOption(), nullptr },
        { LogPathOption(), nullptr }
    });

    for (int i = 1; i < argc; ++i) { // skip executable name.
        auto mapIter = optionMap.find(argv[i]);
        if (mapIter != optionMap.end()) { // option argument.
            mapIter->second = argv[++i];
        } else { // switch argument.
            switchSet.insert(argv[i]);
        }
    }

    Log(LogSwitch::Szx::Cli) << "execute commands." << endl;
    if (switchSet.find(HelpSwitch()) != switchSet.end()) {
        cout << HelpInfo() << endl;
    }

    if (switchSet.find(AuthorNameSwitch()) != switchSet.end()) {
        cout << AuthorName() << endl;
    }

    Solver::Environment env;
    env.load(optionMap);
    if (env.instPath.empty() || env.slnPath.empty()) { return -1; }

    Solver::Configuration cfg;
    cfg.load(env.cfgPath);

    Log(LogSwitch::Szx::Input) << "load instance " << env.instPath << " (seed=" << env.randSeed << ")." << endl;
    Problem::Input input;
    if (!input.load(env.instPath)) { return -1; }

    Solver solver(input, env, cfg);
    solver.solve();

    pb::Submission submission;
    submission.set_thread(to_string(env.jobNum));
    submission.set_instance(env.friendlyInstName());
    submission.set_duration(to_string(solver.timer.elapsedSeconds()) + "s");

    solver.output.save(env.slnPath, submission);
    #if SZX_DEBUG
    solver.output.save(env.solutionPathWithTime(), submission);
    solver.record();
    #endif // SZX_DEBUG

    return 0;
}
#pragma endregion Solver::Cli

#pragma region Solver::Environment
void Solver::Environment::load(const Map<String, char*> &optionMap) {
    char *str;

    str = optionMap.at(Cli::EnvironmentPathOption());
    if (str != nullptr) { loadWithoutCalibrate(str); }

    str = optionMap.at(Cli::InstancePathOption());
    if (str != nullptr) { instPath = str; }

    str = optionMap.at(Cli::SolutionPathOption());
    if (str != nullptr) { slnPath = str; }

    str = optionMap.at(Cli::RandSeedOption());
    if (str != nullptr) { randSeed = atoi(str); }

    str = optionMap.at(Cli::TimeoutOption());
    if (str != nullptr) { msTimeout = static_cast<Duration>(atof(str) * Timer::MillisecondsPerSecond); }

    str = optionMap.at(Cli::MaxIterOption());
    if (str != nullptr) { maxIter = atoi(str); }

    str = optionMap.at(Cli::JobNumOption());
    if (str != nullptr) { jobNum = atoi(str); }

    str = optionMap.at(Cli::RunIdOption());
    if (str != nullptr) { rid = str; }

    str = optionMap.at(Cli::ConfigPathOption());
    if (str != nullptr) { cfgPath = str; }

    str = optionMap.at(Cli::LogPathOption());
    if (str != nullptr) { logPath = str; }

    calibrate();
}

void Solver::Environment::load(const String &filePath) {
    loadWithoutCalibrate(filePath);
    calibrate();
}

void Solver::Environment::loadWithoutCalibrate(const String &filePath) {
    // EXTEND[szx][8]: load environment from file.
    // EXTEND[szx][8]: check file existence first.
}

void Solver::Environment::save(const String &filePath) const {
    // EXTEND[szx][8]: save environment to file.
}
void Solver::Environment::calibrate() {
    // adjust thread number.
    int threadNum = thread::hardware_concurrency();
    if ((jobNum <= 0) || (jobNum > threadNum)) { jobNum = threadNum; }

    // adjust timeout.
    msTimeout -= Environment::SaveSolutionTimeInMillisecond;
}
#pragma endregion Solver::Environment

#pragma region Solver::Configuration
void Solver::Configuration::load(const String &filePath) {
    // EXTEND[szx][5]: load configuration from file.
    // EXTEND[szx][8]: check file existence first.
}

void Solver::Configuration::save(const String &filePath) const {
    // EXTEND[szx][5]: save configuration to file.
}
#pragma endregion Solver::Configuration

#pragma region Solver
bool Solver::solve() {
    if (input.layerNum() < 2) { return true; }

    init();

    int workerNum = (max)(1, env.jobNum / cfg.threadNumPerWorker);
    cfg.threadNumPerWorker = env.jobNum / workerNum;
    List<Solution> solutions(workerNum, Solution(this));
    List<bool> success(workerNum);

    Log(LogSwitch::Szx::Framework) << "launch " << workerNum << " workers." << endl;
    List<thread> threadList;
    threadList.reserve(workerNum);
    for (int i = 0; i < workerNum; ++i) {
        // TODO[szx][2]: as *this is captured by ref, the solver should support concurrency itself, i.e., data members should be read-only or independent for each worker.
        // OPTIMIZE[szx][3]: add a list to specify a series of algorithm to be used by each threads in sequence.
        threadList.emplace_back([&, i]() { success[i] = optimize(solutions[i], i); });
    }
    for (int i = 0; i < workerNum; ++i) { threadList.at(i).join(); }

    Log(LogSwitch::Szx::Framework) << "collect best result among all workers." << endl;
    int bestIndex = -1;
    Length bestValue = Problem::MaxCrossNum;
    for (int i = 0; i < workerNum; ++i) {
        if (!success[i]) { continue; }
        Log(LogSwitch::Szx::Framework) << "worker " << i << " got " << solutions[i].crossNum << endl;
        if (solutions[i].crossNum >= bestValue) { continue; }
        bestIndex = i;
        bestValue = solutions[i].crossNum;
    }

    env.rid = to_string(bestIndex);
    if (bestIndex < 0) { return false; }
    output = solutions[bestIndex];
    return true;
}

void Solver::record() const {
    #if SZX_DEBUG
    int generation = 0;

    ostringstream log;

    System::MemoryUsage mu = System::peakMemoryUsage();

    Length obj = output.crossNum;
    Length checkerObj = -1;
    bool feasible = check(checkerObj);

    // record basic information.
    log << env.friendlyLocalTime() << ","
        << env.rid << ","
        << env.instPath << ","
        << feasible << "," << (obj - checkerObj) << ","
        << checkerObj << ","
        << timer.elapsedSeconds() << ","
        << mu.physicalMemory << "," << mu.virtualMemory << ","
        << env.randSeed << ","
        << cfg.toBriefStr() << ","
        << generation << "," << iteration << ",";

    // record solution vector.
    for (auto o = output.orders().begin(); o != output.orders().end(); ++o) {
        for (auto n = o->nodes().begin(); n != o->nodes().end(); ++n) {
            log << *n << " ";
        }
    }
    log << endl;

    // append all text atomically.
    static mutex logFileMutex;
    lock_guard<mutex> logFileGuard(logFileMutex);

    ofstream logFile(env.logPath, ios::app);
    logFile.seekp(0, ios::end);
    if (logFile.tellp() <= 0) {
        logFile << "Time,ID,Instance,Feasible,ObjMatch,Cross,Duration,PhysMem,VirtMem,RandSeed,Config,Generation,Iteration,Solution" << endl;
    }
    logFile << log.str();
    logFile.close();
    #endif // SZX_DEBUG
}

bool Solver::check(Length &checkerObj) const {
    #if SZX_DEBUG
    enum CheckerFlag {
        IoError = 0x0,
        UnmatchedLayerNumError = 0x1,
        NodeCombinationError = 0x2,
        OldNodeOrderError = 0x4
    };

    checkerObj = System::exec("Checker.exe " + env.instPath + " " + env.solutionPathWithTime());
    if (checkerObj >= 0) { return true; }
    checkerObj = ~checkerObj;
    if (checkerObj == CheckerFlag::IoError) { Log(LogSwitch::Checker) << "IoError." << endl; }
    if (checkerObj & CheckerFlag::UnmatchedLayerNumError) { Log(LogSwitch::Checker) << "UnmatchedLayerNumError." << endl; }
    if (checkerObj & CheckerFlag::NodeCombinationError) { Log(LogSwitch::Checker) << "NodeCombinationError." << endl; }
    if (checkerObj & CheckerFlag::OldNodeOrderError) { Log(LogSwitch::Checker) << "OldNodeOrderError." << endl; }
    return false;
    #else
    checkerObj = 0;
    return true;
    #endif // SZX_DEBUG
}

void Solver::init() {
    aux.adjList.init(input.layerNum(), Side::Size);
    aux.adjMats.init(input.layerNum(), Side::Size);

    auto adjMat = aux.adjMats.begin();
    for (ID ll = 0, l = ll + 1; l < input.layerNum(); ++ll, ++l) {
        const pb::IncrementalGraphDrawing::Layer &layer(input.layers(ll)); // left/last layer.
        const pb::IncrementalGraphDrawing::Layer &nextLayer(input.layers(l));
        aux.adjList[ll][Side::Right].resize(layer.nodenum());
        aux.adjList[l][Side::Left].resize(nextLayer.nodenum());
        aux.adjMats[ll][Side::Right].resize(layer.nodenum());
        aux.adjMats[l][Side::Left].resize(nextLayer.nodenum());
        for (ID n = 0; n < layer.nodenum(); ++n) {
            aux.adjMats[ll][Side::Right][n].resize(nextLayer.nodenum(), false);
        }
        for (ID n = 0; n < nextLayer.nodenum(); ++n) {
            aux.adjMats[l][Side::Left][n].resize(layer.nodenum(), false);
        }
        for (ID src = 0; src < layer.nodenum(); ++src) {
            for (auto dst = layer.forwardlists(src).nodes().begin(); dst != layer.forwardlists(src).nodes().end(); ++dst) {
                aux.adjList[ll][Side::Right][src].push_back(*dst);
                aux.adjList[l][Side::Left][*dst].push_back(src);
                aux.adjMats[ll][Side::Right][src][*dst] = true;
                aux.adjMats[l][Side::Left][*dst][src] = true;
            }
        }
    }

    // load reference results.
    CsvReader cr;
    ifstream ifs(Environment::DefaultInstanceDir() + "Baseline.csv");
    if (!ifs.is_open()) { return; }
    const List<CsvReader::Row> &rows(cr.scan(ifs));
    ifs.close();
    for (auto r = rows.begin(); r != rows.end(); ++r) {
        if (env.friendlyInstName() != r->front()) { continue; }
        aux.refCrossNum = stoi((*r)[1]);
        break;
    }
}

bool Solver::optimize(Solution &sln, ID workerId) {
    Log(LogSwitch::Szx::Framework) << "worker " << workerId << " starts." << endl;

    bool status = false;

    //status = optimizeAsIs(sln);
    //status = optimizeRandomly(sln);
    status = optimizePlainModel(sln);

    Log(LogSwitch::Szx::Framework) << "worker " << workerId << " ends." << endl;
    return status;
}

bool Solver::optimizeAsIs(Solution &sln) {
    // reset solution state.
    bool status = true;
    auto &orders(*sln.mutable_orders());
    orders.Clear();
    orders.Reserve(input.layerNum());

    // TODO[0]: replace the following as-is assignment with your own algorithm.
    for (auto l = input.layers().begin(); !timer.isTimeOut() && (l != input.layers().end()); ++l) {
        auto &slots(*(orders.Add()->mutable_nodes()));
        slots.Resize(l->nodenum(), Problem::InvalidId);
        // place nodes by their original order.
        ID n = 0;
        for (auto s = slots.begin(); s != slots.end(); ++s) { *s = n++; }
    }

    sln.crossNum = countCrossEdge(sln); // record obj.
    return true;
}

bool Solver::optimizeRandomly(Solution &sln) {
    // reset solution state.
    bool status = true;
    auto &orders(*sln.mutable_orders());
    orders.Clear();
    orders.Reserve(input.layerNum());

    // TODO[0]: replace the following random assignment with your own algorithm.
    for (auto l = input.layers().begin(); !timer.isTimeOut() && (l != input.layers().end()); ++l) {
        auto &slots(*(orders.Add()->mutable_nodes()));
        slots.Resize(l->nodenum(), Problem::InvalidId);
        auto slot = slots.begin();
        ID placedNewNode = l->oldnodenum();
        for (ID n = l->oldnodenum(); n < l->nodenum(); ++n, ++slot) { *slot = placedNewNode++; }
        // place new nodes randomly.
        shuffle(slots.begin(), slots.end(), rand.rgen);
        // place old nodes by their original order.
        ID placedOldNode = 0;
        for (slot = slots.begin(); slot != slots.end(); ++slot) {
            if (*slot == Problem::InvalidId) { *slot = placedOldNode++; }
        }
    }

    sln.crossNum = countCrossEdge(sln); // record obj.
    return true;
}

bool Solver::optimizePlainModel(Solution &sln) {
    // reset solution state.
    auto &orders(*sln.mutable_orders());
    orders.Clear();
    orders.Reserve(input.layerNum());

    MpSolver mp;

    // add decision variables.
    Arr<Arr<MpSolver::DecisionVar>> position(input.layerNum()); // position[l][n] is the position of node n in layer l.
    Arr<Arr2D<MpSolver::DecisionVar>> isPrior(input.layerNum()); // isPrior[l][n][m] == true means node n is prior to node m in layer l.
    Arr<Arr2D<Arr2D<MpSolver::DecisionVar>>> isCrossing(input.layerNum()); // isCrossing[l][n][m][u][v] == ture means edge i-u and j-v cross where node i, j in layer l and node u, v in layer (l + 1).
    for (ID l = 0; l < input.layerNum(); ++l) {
        const pb::IncrementalGraphDrawing::Layer &layer(input.layers(l));
        if (cfg.model == Configuration::Model::SequentialModel) { position[l].init(layer.nodenum()); }
        isPrior[l].init(layer.nodenum(), layer.nodenum());
        for (ID n = 0; n < layer.nodenum(); ++n) {
            if(cfg.model == Configuration::Model::SequentialModel) {
                position[l][n] = mp.addVar(MpSolver::VariableType::Real, 0, layer.nodenum(), 0);
            }
            //for (ID m = max(n + 1, layer.oldnodenum()); m < layer.nodenum(); ++m) { // skip node n >= m or they are both old nodes.
            //    isPrior[l][n][m] = mp.addVar(MpSolver::VariableType::Bool, 0, 1, 0);
            //}
            for (ID m = n + 1; m < layer.nodenum(); ++m) {
                isPrior[l][n][m] = mp.addVar(MpSolver::VariableType::Bool, (m < layer.oldnodenum()), 1, 0);
            }
        }
    }
    for (ID ll = 0, l = 1; l < input.layerNum(); ++ll, ++l) {
        const pb::IncrementalGraphDrawing::Layer &layer(input.layers(ll));
        const pb::IncrementalGraphDrawing::Layer &nextLayer(input.layers(l));
        isCrossing[ll].init(layer.nodenum(), layer.nodenum());
        for (ID n = 0; n < layer.nodenum(); ++n) {
            const List<ID> &nAdjList(aux.adjList[ll][Side::Right][n]);
            for (ID m = n + 1; m < layer.nodenum(); ++m) {
                isCrossing[ll][n][m].init(nextLayer.nodenum(), nextLayer.nodenum());
                const List<ID> &mAdjList(aux.adjList[ll][Side::Right][m]);
                for (auto u = nAdjList.begin(); u != nAdjList.end(); ++u) {
                    for (auto v = mAdjList.begin(); v != mAdjList.end(); ++v) {
                        if (*u == *v) { continue; }
                        isCrossing[ll][n][m][*u][*v] = mp.addVar(MpSolver::VariableType::Bool, 0, 1, 1);
                    }
                }
            }
        }
    }

    // add constraints.
    // cross counting.
    for (ID ll = 0, l = 1; l < input.layerNum(); ++ll, ++l) {
        const pb::IncrementalGraphDrawing::Layer &layer(input.layers(ll));
        for (ID n = 0; n < layer.nodenum(); ++n) {
            const List<ID> &nAdjList(aux.adjList[ll][Side::Right][n]);
            for (ID m = n + 1; m < layer.nodenum(); ++m) {
                const List<ID> &mAdjList(aux.adjList[ll][Side::Right][m]);
                for (auto u = nAdjList.begin(); u != nAdjList.end(); ++u) {
                    for (auto v = mAdjList.begin(); v != mAdjList.end(); ++v) {
                        if (*u == *v) { continue; }
                        MpSolver::LinearExpr diff = isPrior[ll][n][m]
                            - ((*u < *v) ? isPrior[l][*u][*v] : (1 - isPrior[l][*v][*u]));
                        mp.addConstraint(-isCrossing[ll][n][m][*u][*v] <= diff);
                        mp.addConstraint(diff <= isCrossing[ll][n][m][*u][*v]);
                    }
                }
            }
        }
    }

    // ordering.
    if (cfg.model == Configuration::Model::TransitivityModel) {
        for (ID l = 0; l < input.layerNum(); ++l) {
            const pb::IncrementalGraphDrawing::Layer &layer(input.layers(l));
            for (ID n = 0; n < layer.nodenum(); ++n) {
                for (ID m = n + 1; m < layer.nodenum(); ++m) {
                    for (ID u = m + 1; u < layer.nodenum(); ++u) {
                        MpSolver::LinearExpr diff = isPrior[l][n][m] + isPrior[l][m][u] - isPrior[l][n][u];
                        mp.addConstraint(0 <= diff);
                        mp.addConstraint(diff <= 1);
                    }
                }
            }
        }
    } else { // if (cfg.model == Configuration::Model::SequentialModel)
        for (ID l = 0; l < input.layerNum(); ++l) {
            const pb::IncrementalGraphDrawing::Layer &layer(input.layers(l));
            for (ID n = 0; n < layer.nodenum(); ++n) {
                for (ID m = n + 1; m < layer.nodenum(); ++m) {
                    MpSolver::LinearExpr diff = position[l][m] - position[l][n];
                    mp.addConstraint(1 - layer.nodenum() * (1 - isPrior[l][n][m]) <= diff);
                    mp.addConstraint(diff <= layer.nodenum() * isPrior[l][n][m] - 1);
                }
            }
        }
    }

    // set objective.
    mp.setOptimaOrientation(MpSolver::OptimaOrientation::Minimize);

    // solve model.
    mp.setOutput(true);
    //mp.setMaxThread(1);
    mp.setTimeLimitInSecond(1800);

    // record decision.
    if (mp.optimize()) {
        sln.crossNum = lround(mp.getObjectiveValue());
        for (ID l = 0; l < input.layerNum(); ++l) {
            const pb::IncrementalGraphDrawing::Layer &layer(input.layers(l));
            auto &order(*(sln.add_orders()->mutable_nodes()));
            order.Resize(layer.nodenum(), Problem::InvalidId);
            if (cfg.model == Configuration::Model::TransitivityModel) {
                for (ID n = 0; n < layer.nodenum(); ++n) { order[n] = n; }
                sort(order.begin(), order.end(), [&](ID n, ID m) {
                    return (n < m) ? mp.isTrue(isPrior[l][n][m]) : !mp.isTrue(isPrior[l][m][n]);
                });
            } else { // if (cfg.model == Configuration::Model::SequentialModel)
                for (ID n = 0; n < layer.nodenum(); ++n) {
                    ID i = lround(mp.getValue(position[l][n]));
                    order[i] = n;
                }
            }
        }
        return true;
    }

    return optimizeAsIs(sln);
}

ID Solver::countCrossEdge(Solution &sln) const {
    ID crossNum = 0;
    // {0,1,1,2,3,3} = accEdgeNums[X]
    //  O O O O O O
    //     \  | |
    //      \ | |
    //       \|/
    //  O O O X O O
    List<List<ID>> accEdgeNums(input.nodeNum());
    // OPTIMIZE[szx][5]: for each pair of layers with node number <N, M>,
    //                   let the cross number on edge <i, 1> be c(i, 1) = 0,
    //                   c(i, j + 1) = c(i, j) + (accEdgeNums[j, N] - accEdgeNums[j, i]).
    for (ID ll = 0, l = ll + 1; l < input.layerNum(); ++ll, ++l) {
        const auto &lo(sln.orders(ll).nodes()); // left (layer l) order.
        const auto &ro(sln.orders(l).nodes()); // right (layer (l + 1)) order.
        for (auto dst = ro.begin(); dst != ro.end(); ++dst) {
            for (auto src = lo.begin(); src != lo.end(); ++src) {
                if (!aux.adjMats[l][Side::Left][*dst][*src]) { continue; }
                // count the crossing edges s-d of src-dst.
                for (auto s = src + 1; s != lo.end(); ++s) {
                    for (auto d = ro.begin(); d != dst; ++d) {
                        if (aux.adjMats[ll][Side::Right][*s][*d]) { ++crossNum; }
                    }
                }
            }
        }
    }
    return crossNum;
}

#pragma endregion Solver

}
