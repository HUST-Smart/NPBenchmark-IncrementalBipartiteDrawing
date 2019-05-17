#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>

#include <cstring>

#include "Simulator.h"
#include "ThreadPool.h"


using namespace std;


namespace szx {

// EXTEND[szx][5]: read it from InstanceList.txt.
static const vector<String> instList({
    //"igdp001.l2n47e32",
    //"igdp002.l2n49e39",
    //"igdp003.l2n51e42",
    //"igdp004.l2n54e41",
    //"igdp005.l2n25e18",
    //"igdp006.l2n58e46",
    //"igdp007.l2n60e47",
    //"igdp008.l2n35e25",
    //"igdp009.l2n38e33",
    //"igdp010.l2n41e31",
    //"igdp011.l2n62e43",
    //"igdp012.l2n65e52",
    //"igdp013.l2n68e54",
    //"igdp014.l2n71e52",
    //"igdp015.l2n33e25",
    //"igdp016.l2n78e63",
    //"igdp017.l2n80e63",
    //"igdp018.l2n47e33",
    //"igdp019.l2n50e42",
    //"igdp020.l2n54e42",
    //"igdp021.l2n47e74",
    //"igdp022.l2n49e74",
    //"igdp023.l2n51e75",
    //"igdp024.l2n54e102",
    //"igdp025.l2n25e21",
    //"igdp026.l2n58e116",
    //"igdp027.l2n60e126",
    //"igdp028.l2n35e44",
    //"igdp029.l2n38e33",
    //"igdp030.l2n41e51",
    //"igdp031.l2n62e99",
    //"igdp032.l2n65e99",
    //"igdp033.l2n68e100",
    //"igdp034.l2n71e136",
    //"igdp035.l2n33e27",
    //"igdp036.l2n78e155",
    //"igdp037.l2n80e168",
    //"igdp038.l2n47e59",
    //"igdp039.l2n50e42",
    //"igdp040.l2n54e68",
    //"igdp041.l2n47e127",
    //"igdp042.l2n49e127",
    //"igdp043.l2n51e130",
    //"igdp044.l2n54e174",
    //"igdp045.l2n25e34",
    //"igdp046.l2n58e199",
    //"igdp047.l2n60e216",
    //"igdp048.l2n35e76",
    //"igdp049.l2n38e46",
    //"igdp050.l2n41e87",
    //"igdp051.l2n62e169",
    //"igdp052.l2n65e169",
    //"igdp053.l2n68e174",
    //"igdp054.l2n71e232",
    //"igdp055.l2n33e46",
    //"igdp056.l2n78e265",
    //"igdp057.l2n80e288",
    //"igdp058.l2n47e102",
    //"igdp059.l2n50e62",
    //"igdp060.l2n54e116",
    //"igdp061.l6n138e160",
    //"igdp062.l6n152e180",
    //"igdp063.l6n165e214",
    //"igdp064.l6n147e172",
    //"igdp065.l6n97e114",
    //"igdp066.l6n142e176",
    //"igdp067.l6n154e188",
    //"igdp068.l6n109e123",
    //"igdp069.l6n119e151",
    //"igdp070.l6n136e177",
    //"igdp071.l6n183e217",
    //"igdp072.l6n201e239",
    //"igdp073.l6n218e289",
    //"igdp074.l6n194e235",
    //"igdp075.l6n129e151",
    //"igdp076.l6n189e234",
    //"igdp077.l6n205e256",
    //"igdp078.l6n144e167",
    //"igdp079.l6n157e202",
    //"igdp080.l6n181e242",
    //"igdp081.l6n138e345",
    //"igdp082.l6n152e369",
    //"igdp083.l6n165e536",
    //"igdp084.l6n147e389",
    //"igdp085.l6n97e169",
    //"igdp086.l6n142e377",
    //"igdp087.l6n154e403",
    //"igdp088.l6n109e219",
    //"igdp089.l6n119e191",
    //"igdp090.l6n136e378",
    //"igdp091.l6n183e460",
    //"igdp092.l6n201e492",
    //"igdp093.l6n218e715",
    //"igdp094.l6n194e519",
    //"igdp095.l6n129e222",
    //"igdp096.l6n189e503",
    //"igdp097.l6n205e538",
    //"igdp098.l6n144e292",
    //"igdp099.l6n157e256",
    //"igdp100.l6n181e504",
    //"igdp101.l6n138e591",
    //"igdp102.l6n152e633",
    //"igdp103.l6n165e921",
    //"igdp104.l6n147e668",
    //"igdp105.l6n97e286",
    //"igdp106.l6n142e649",
    //"igdp107.l6n154e691",
    //"igdp108.l6n109e375",
    //"igdp109.l6n119e322",
    //"igdp110.l6n136e648",
    //"igdp111.l6n183e788",
    "igdp112.l6n201e845",
    "igdp113.l6n218e1228",
    "igdp114.l6n194e893",
    "igdp115.l6n129e382",
    "igdp116.l6n189e866",
    "igdp117.l6n205e923",
    "igdp118.l6n144e502",
    "igdp119.l6n157e430",
    "igdp120.l6n181e865",
    "igdp121.l13n311e413",
    "igdp122.l13n347e462",
    "igdp123.l13n289e430",
    "igdp124.l13n285e408",
    "igdp125.l13n222e316",
    "igdp126.l13n288e404",
    "igdp127.l13n311e425",
    "igdp128.l13n228e317",
    "igdp129.l13n258e364",
    "igdp130.l13n288e409",
    "igdp131.l13n412e561",
    "igdp132.l13n459e626",
    "igdp133.l13n381e568",
    "igdp134.l13n376e543",
    "igdp135.l13n294e430",
    "igdp136.l13n383e555",
    "igdp137.l13n412e580",
    "igdp138.l13n302e429",
    "igdp139.l13n339e492",
    "igdp140.l13n382e550",
    "igdp141.l13n311e981",
    "igdp142.l13n347e1141",
    "igdp143.l13n289e843",
    "igdp144.l13n285e762",
    "igdp145.l13n222e533",
    "igdp146.l13n288e846",
    "igdp147.l13n311e911",
    "igdp148.l13n228e509",
    "igdp149.l13n258e555",
    "igdp150.l13n288e887",
    "igdp151.l13n412e1310",
    "igdp152.l13n459e1522",
    "igdp153.l13n381e1128",
    "igdp154.l13n376e1018",
    "igdp155.l13n294e712",
    "igdp156.l13n383e1129",
    "igdp157.l13n412e1216",
    "igdp158.l13n302e679",
    "igdp159.l13n339e739",
    "igdp160.l13n382e1184",
    "igdp161.l13n311e1683",
    "igdp162.l13n347e1959",
    "igdp163.l13n289e1451",
    "igdp164.l13n285e1309",
    "igdp165.l13n222e917",
    "igdp166.l13n288e1455",
    "igdp167.l13n311e1564",
    "igdp168.l13n228e872",
    "igdp169.l13n258e952",
    "igdp170.l13n288e1524",
    "igdp171.l13n412e2246",
    "igdp172.l13n459e2612",
    "igdp173.l13n381e1936",
    "igdp174.l13n376e1747",
    "igdp175.l13n294e1225",
    "igdp176.l13n383e1941",
    "igdp177.l13n412e2085",
    "igdp178.l13n302e1164",
    "igdp179.l13n339e1271",
    "igdp180.l13n382e2032",
    "igdp181.l20n510e720",
    "igdp182.l20n489e687",
    "igdp183.l20n435e653",
    "igdp184.l20n465e651",
    "igdp185.l20n371e532",
    "igdp186.l20n444e646",
    "igdp187.l20n466e652",
    "igdp188.l20n422e597",
    "igdp189.l20n421e612",
    "igdp190.l20n421e596",
    "igdp191.l20n676e978",
    "igdp192.l20n646e937",
    "igdp193.l20n574e877",
    "igdp194.l20n613e879",
    "igdp195.l20n492e706",
    "igdp196.l20n590e871",
    "igdp197.l20n617e886",
    "igdp198.l20n561e811",
    "igdp199.l20n556e824",
    "igdp200.l20n559e808",
    "igdp201.l20n510e1799",
    "igdp202.l20n489e1554",
    "igdp203.l20n435e1198",
    "igdp204.l20n465e1367",
    "igdp205.l20n371e980",
    "igdp206.l20n444e1297",
    "igdp207.l20n466e1320",
    "igdp208.l20n422e1181",
    "igdp209.l20n421e1059",
    "igdp210.l20n421e1195",
    "igdp211.l20n676e2400",
    "igdp212.l20n646e2077",
    "igdp213.l20n574e1598",
    "igdp214.l20n613e1825",
    "igdp215.l20n492e1309",
    "igdp216.l20n590e1730",
    "igdp217.l20n617e1765",
    "igdp218.l20n561e1578",
    "igdp219.l20n556e1414",
    "igdp220.l20n559e1596",
    "igdp221.l20n510e3087",
    "igdp222.l20n489e2673",
    "igdp223.l20n435e2057",
    "igdp224.l20n465e2346",
    "igdp225.l20n371e1683",
    "igdp226.l20n444e2228",
    "igdp227.l20n466e2273",
    "igdp228.l20n422e2034",
    "igdp229.l20n421e1820",
    "igdp230.l20n421e2056",
    "igdp231.l20n676e4120",
    "igdp232.l20n646e3567",
    "igdp233.l20n574e2745",
    "igdp234.l20n613e3133",
    "igdp235.l20n492e2248",
    "igdp236.l20n590e2971",
    "igdp237.l20n617e3031",
    "igdp238.l20n561e2714",
    "igdp239.l20n556e2429",
    "igdp240.l20n559e2743"
});

void Simulator::initDefaultEnvironment() {
    Solver::Environment env;
    env.save(Env::DefaultEnvPath());

    Solver::Configuration cfg;
    cfg.save(Env::DefaultCfgPath());
}

void Simulator::exe(const Task &task) {
    System::makeSureDirExist(SolutionDir());

    ostringstream oss;
    oss << ProgramName()
        << " " << Cmd::InstancePathOption() << " " << InstanceDir() << task.instanceName()
        << " " << Cmd::SolutionPathOption() << " " << SolutionDir() << task.solutionName();

    auto addOption = [&](const String &key, const String &value) {
        if (!value.empty()) { oss << " " << key << " " << value; }
    };

    addOption(Cmd::RandSeedOption(), task.randSeed);
    addOption(Cmd::TimeoutOption(), task.timeout);
    addOption(Cmd::MaxIterOption(), task.maxIter);
    addOption(Cmd::JobNumOption(), task.jobNum);
    addOption(Cmd::RunIdOption(), task.runId);
    addOption(Cmd::ConfigPathOption(), task.cfgPath);
    addOption(Cmd::LogPathOption(), task.logPath);

    System::exec(oss.str());
}

void Simulator::run(const Task &task) {
    System::makeSureDirExist(SolutionDir());

    char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
    char *argv[Cmd::MaxArgNum];
    for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
    strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

    int argc = ArgIndex::ArgStart;

    strcpy(argv[argc++], Cmd::InstancePathOption().c_str());
    strcpy(argv[argc++], (InstanceDir() + task.instanceName()).c_str());

    strcpy(argv[argc++], Cmd::SolutionPathOption().c_str());
    strcpy(argv[argc++], (SolutionDir() + task.solutionName()).c_str());

    auto addOption = [&](const String &key, const String &value) {
        if (!value.empty()) {
            strcpy(argv[argc++], key.c_str());
            strcpy(argv[argc++], value.c_str());
        }
    };

    addOption(Cmd::RandSeedOption(), task.randSeed);
    addOption(Cmd::TimeoutOption(), task.timeout);
    addOption(Cmd::MaxIterOption(), task.maxIter);
    addOption(Cmd::JobNumOption(), task.jobNum);
    addOption(Cmd::RunIdOption(), task.runId);
    addOption(Cmd::ConfigPathOption(), task.cfgPath);
    addOption(Cmd::LogPathOption(), task.logPath);

    Cmd::run(argc, argv);
}

void Simulator::run(const String &envPath) {
    char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
    char *argv[Cmd::MaxArgNum];
    for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
    strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

    int argc = ArgIndex::ArgStart;

    strcpy(argv[argc++], Cmd::EnvironmentPathOption().c_str());
    strcpy(argv[argc++], envPath.c_str());

    Cmd::run(argc, argv);
}

void Simulator::debug() {
    Task task;
    task.instSet = "";
    task.instId = "igdp001.l2n47e32";
    task.randSeed = "1500972793";
    //task.randSeed = to_string(RandSeed::generate());
    task.timeout = "180";
    //task.maxIter = "1000000000";
    task.jobNum = "1";
    task.cfgPath = Env::DefaultCfgPath();
    task.logPath = Env::DefaultLogPath();
    task.runId = "0";

    run(task);
}

void Simulator::benchmark(int repeat) {
    Task task;
    task.instSet = "";
    //task.timeout = "180";
    //task.maxIter = "1000000000";
    task.timeout = "3600";
    //task.maxIter = "1000000000";
    task.jobNum = "1";
    task.cfgPath = Env::DefaultCfgPath();
    task.logPath = Env::DefaultLogPath();

    random_device rd;
    mt19937 rgen(rd());
    for (int i = 0; i < repeat; ++i) {
        //shuffle(instList.begin(), instList.end(), rgen);
        for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
            task.instId = *inst;
            task.randSeed = to_string(Random::generateSeed());
            task.runId = to_string(i);
            run(task);
        }
    }
}

void Simulator::parallelBenchmark(int repeat) {
    Task task;
    task.instSet = "";
    //task.timeout = "180";
    //task.maxIter = "1000000000";
    task.timeout = "3600";
    //task.maxIter = "1000000000";
    task.jobNum = "1";
    task.cfgPath = Env::DefaultCfgPath();
    task.logPath = Env::DefaultLogPath();

    ThreadPool<> tp(4);

    random_device rd;
    mt19937 rgen(rd());
    for (int i = 0; i < repeat; ++i) {
        //shuffle(instList.begin(), instList.end(), rgen);
        for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
            task.instId = *inst;
            task.randSeed = to_string(Random::generateSeed());
            task.runId = to_string(i);
            tp.push([=]() { exe(task); });
            this_thread::sleep_for(1s);
        }
    }
}

void Simulator::generateInstance(const InstanceTrait &trait) {
    Random rand;

    Problem::Input input;

    // EXTEND[szx][5]: generate random instances.
    ID edgeNum = 0;

    ostringstream path;
    path << InstanceDir() << "rand.n" << input.graph().nodenum()
        << "e" << edgeNum << "l" << input.layerNum() << ".json";
    save(path.str(), input);
}

void Simulator::convertIgdpInstance(const String &igdpPath, int index) {
    Log(Log::Info) << "converting " << igdpPath << endl;

    ifstream ifs(igdpPath);

    Problem::Input input;
    auto &graph(*input.mutable_graph());

    int layerNum;
    ifs >> layerNum;

    int nodeNum = 0;
    int oldNodeNum = 0;
    int edgeNum = 0;
    for (int l = 0; l < layerNum; ++l) {
        int layerNodeNum;
        ifs >> layerNodeNum;
        graph.add_layers()->set_nodenum(layerNodeNum);
        nodeNum += layerNodeNum;
    }
    graph.set_nodenum(nodeNum);

    pb::IncrementalGraphDrawing::Layer &l0(*(graph.mutable_layers(0)));
    vector<string> lastAdjList(l0.nodenum());
    int layerOldNodeNum = 0;
    bool isOldNode;
    int pos;
    for (int n = 0; n < l0.nodenum(); ++n) {
        ifs >> isOldNode >> pos;
        getline(ifs, lastAdjList[pos]);
        layerOldNodeNum += isOldNode;
    }
    l0.set_oldnodenum(layerOldNodeNum);
    oldNodeNum += layerOldNodeNum;
    for (int l = 1; l < layerNum; ++l) {
        pb::IncrementalGraphDrawing::Layer &layer(*(graph.mutable_layers(l)));
        vector<int> idMap(layer.nodenum());
        vector<string> adjList(layer.nodenum());
        layerOldNodeNum = 0;
        for (int n = 0; n < layer.nodenum(); ++n) {
            ifs >> isOldNode >> pos;
            getline(ifs, adjList[pos]);
            layerOldNodeNum += isOldNode;
            idMap[n] = pos;
        }
        layer.set_oldnodenum(layerOldNodeNum);
        oldNodeNum += layerOldNodeNum;

        // transform the node id.
        pb::IncrementalGraphDrawing::Layer &lastLayer(*(graph.mutable_layers(l - 1)));
        for (auto src = lastAdjList.begin(); src != lastAdjList.end(); ++src) {
            auto &forwardList(*(lastLayer.add_forwardlists()->mutable_nodes()));
            istringstream iss(*src);
            for (int dst; iss >> dst; ++edgeNum) { forwardList.Add(idMap[dst]); }
        }

        lastAdjList = adjList;
    }
    graph.set_oldnodenum(oldNodeNum);

    ostringstream path;
    path << InstanceDir() << "igdp" << setfill('0') << setw(3) << index << ".l" << input.layerNum()
        << "n" << input.graph().nodenum() << "e" << edgeNum << ".json";
    save(path.str(), input);
}

void Simulator::convertIgdpInstances() {
    int i = 0;
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.06_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.17_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_2_0.30_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.06_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.17_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_6_0.30_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.06_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.17_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_13_0.30_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.06_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.17_5_30_1.60_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.20_10.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_1.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_2.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_3.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_4.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_5.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_6.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_7.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_8.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_9.txt", ++i);
    convertIgdpInstance("Instance/igdplib/incgraph_20_0.30_5_30_1.60_10.txt", ++i);
}

}
