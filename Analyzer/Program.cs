using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace Analyzer {
    class Program {
        static void Main(string[] args) {
            //InstanceAnalyzer.analyzeAllInstances("Instances.xlsx");
            LogAnalyzer.mapName("log.600s.csv", "log.map.csv");
        }
    }
}
