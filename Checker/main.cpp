#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>

#include "Visualizer.h"

#include "../Solver/PbReader.h"
#include "../Solver/IncrementalGraphDrawing.pb.h"


using namespace std;
using namespace szx;
using namespace pb;

int main(int argc, char *argv[]) {
    enum CheckerFlag {
        IoError = 0x0,
        UnmatchedLayerNumError = 0x1,
        NodeCombinationError = 0x2,
        OldNodeOrderError = 0x4
    };

    string inputPath;
    string outputPath;

    if (argc > 1) {
        inputPath = argv[1];
    } else {
        cerr << "input path: " << flush;
        cin >> inputPath;
    }

    if (argc > 2) {
        outputPath = argv[2];
    } else {
        cerr << "output path: " << flush;
        cin >> outputPath;
    }

    pb::IncrementalGraphDrawing::Input input;
    if (!load(inputPath, input)) { return ~CheckerFlag::IoError; }

    pb::IncrementalGraphDrawing::Output output;
    ifstream ifs(outputPath);
    if (!ifs.is_open()) { return ~CheckerFlag::IoError; }
    string submission;
    getline(ifs, submission); // skip the first line.
    ostringstream oss;
    oss << ifs.rdbuf();
    jsonToProtobuf(oss.str(), output);

    int nodeNum = input.graph().nodenum();

    // check solution.
    int error = 0;
    // check constraints.
    if (output.orders_size() != input.graph().layers_size()) { error |= CheckerFlag::UnmatchedLayerNumError; }

    vector<vector<int>> nodePositions(input.graph().layers_size());
    for (int l = 0; l < input.graph().layers_size(); ++l) {
        const auto &layer(input.graph().layers(l));
        const auto &order(output.orders(l).nodes());
        auto &pos(nodePositions[l]);
        pos.resize(layer.nodenum(), -1);
        if (order.size() != layer.nodenum()) { error |= CheckerFlag::NodeCombinationError; continue; }
        for (int i = 0; i < layer.nodenum(); ++i) {
            if ((order[i] >= 0) && (order[i] < layer.nodenum())) { pos[order[i]] = i; continue; }
            error |= CheckerFlag::NodeCombinationError;
        }
        for (int i = 1; i < layer.oldnodenum(); ++i) {
            if (pos[i] < pos[i - 1]) { error |= CheckerFlag::OldNodeOrderError; }
        }
    }
    // check objective.
    int crossNum = 0;
    for (int ll = 0, l = ll + 1; l < input.graph().layers_size(); ++ll, ++l) {
        const auto &lo(output.orders(ll).nodes()); // left (layer l) order.
        const auto &ro(output.orders(l).nodes()); // right (layer (l + 1)) order.
        const auto &adjList(input.graph().layers(ll).forwardlists());
        for (auto src = lo.begin(); src != lo.end(); ++src) {
            for (auto dst = ro.begin(); dst != ro.end(); ++dst) {
                auto srcAdj = adjList[*src].nodes();
                if (find(srcAdj.begin(), srcAdj.end(), *dst) == srcAdj.end()) { continue; }
                // count the crossing edges s-d of src-dst.
                // OPTIMIZE[szx][5]: reuse the number of edges from s@Left to [0, dst)@Right.
                for (auto s = src + 1; s != lo.end(); ++s) {
                    for (auto d = ro.begin(); d != dst; ++d) {
                        auto sAdj = adjList[*s].nodes();
                        if (find(sAdj.begin(), sAdj.end(), *d) != sAdj.end()) { ++crossNum; }
                    }
                }
            }
        }
    }

    // visualization.
    int maxLayerNodeNum = max_element(input.graph().layers().begin(), input.graph().layers().end(),
        [](const auto &l, const auto &r) { return l.nodenum() < r.nodenum(); })->nodenum();
    auto pos = outputPath.find_last_of('/');
    string outputName = (pos == string::npos) ? outputPath : outputPath.substr(pos + 1);
    constexpr double Margin = 10;
    constexpr double NodeRadius = 5;
    constexpr double LayerGap = 80;
    constexpr double NodeGap = 20;
    double Width = (input.graph().layers_size() - 1) * LayerGap;
    double Height = (maxLayerNodeNum - 1) * NodeGap;
    Drawer draw;
    draw.begin("Visualization/" + outputName + ".html", -Margin, -Margin, Width + Margin, Height + 2 * Margin);
    double srcX = 0;
    for (int ll = 0, l = 1; l < input.graph().layers_size(); ++ll, ++l, srcX += LayerGap) {
        const auto &layer(input.graph().layers(ll));
        const auto &order(output.orders(ll).nodes());
        double srcY = 0;
        double dstX = srcX + LayerGap;
        for (auto src = order.begin(); src != order.end(); ++src, srcY += NodeGap) {
            auto srcAdj = layer.forwardlists(*src).nodes();
            for (auto dst = srcAdj.begin(); dst != srcAdj.end(); ++dst) {
                double dstY = nodePositions[l][*dst] * NodeGap;
                draw.line(srcX, srcY, dstX, dstY, "000");
            }
        }
    }
    double x = 0;
    for (int l = 0; l < input.graph().layers_size(); ++l, x += LayerGap) {
        const auto &layer(input.graph().layers(l));
        const auto &order(output.orders(l).nodes());
        double y = 0;
        for (auto n = order.begin(); n != order.end(); ++n, y += NodeGap) {
            string bcolor = (*n < layer.oldnodenum()) ? "FF8000" : "FFF";
            draw.circle(x, y, NodeRadius, to_string(*n), "000", bcolor);
        }
    }
    draw.end();

    int returnCode = (error == 0) ? crossNum : ~error;
    cout << ((error == 0) ? crossNum : returnCode) << endl;
    return returnCode;
}
