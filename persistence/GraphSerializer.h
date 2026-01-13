#pragma once
#include "core/GraphStore.h"
#include <fstream>

namespace graph {
    class GraphSerializer {
    public:
        static void toCSV(const GraphStore& store, const std::string& path) {
            std::ofstream f(path);
            f << "Source,Target,Timestamp\n";
            // Logic to iterate and save...
            f.close();
        }
    };
}
