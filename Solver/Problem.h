////////////////////////////////
/// usage : 1.	data that identifies an incremental graph drawing problem and its solution.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_INCREMENTAL_GRAPH_DRAWING_PROBLEM_H
#define SMART_SZX_INCREMENTAL_GRAPH_DRAWING_PROBLEM_H


#include "Config.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Common.h"
#include "PbReader.h"
#include "IncrementalGraphDrawing.pb.h"


namespace szx {

class Problem {
    #pragma region Type
public:
    struct Input : public pb::IncrementalGraphDrawing::Input {
        bool load(const String &path) { return pb::load(path, *this); }

        //auto &layers() { return *(mutable_graph()->mutable_layers()); }
        const auto &layers() const { return graph().layers(); }
        const pb::IncrementalGraphDrawing::Layer &layers(ID l) const { return graph().layers(l); }

        ID nodeNum() const { return graph().nodenum(); }
        ID layerNum() const { return graph().layers_size(); }
    };

    struct Output : public pb::IncrementalGraphDrawing::Output {
        bool save(const String &path, pb::Submission &submission) const {
            std::ofstream ofs(path);
            if (!ofs.is_open()) { return false; }

            // TODO[0]: fill the submission information.
            submission.set_author("szx");
            submission.set_algorithm("rand");
            submission.set_cpu("Intel Core i5-7400 3.00GHz");
            submission.set_ram("16G 2400MHz");
            submission.set_language("C++");
            submission.set_compiler("VS2017");
            submission.set_os("Windows 10");
            submission.set_problem("IncrementalGraphDrawing");

            ofs << protobufToJson(submission, false) << std::endl << protobufToJson(*this);
            return true;
        }

        ID crossNum = 0;
    };
    #pragma endregion Type

    #pragma region Constant
public:
    enum {
        MaxLayerNum = 256,
        MaxNodeNumPerLayer = 512,
        MaxNodeNum = 4096,
        MaxEdgeNumPerLayer = MaxNodeNumPerLayer * MaxNodeNumPerLayer,
        MaxEdgeNum = MaxEdgeNumPerLayer * MaxLayerNum,
        MaxCrossNum = (1 << 28),

        InvalidId = -1,
    };
    #pragma endregion Constant

    #pragma region Constructor
public:
    #pragma endregion Constructor

    #pragma region Method
public:
    #pragma endregion Method

    #pragma region Field
public:
    #pragma endregion Field
}; // Problem

}


#endif // SMART_SZX_INCREMENTAL_GRAPH_DRAWING_PROBLEM_H
