#include <iostream>
#include <sstream>
#include <memory>
#include "core/GraphStore.h"
#include "CommandHandler.h" // Include the new brain
//#include "CommandHandler1.h"
using namespace std;

int main() {
    auto store = make_unique<GraphStore>();
    string line, cmd;
        cout << "\n--- �   GRAPH ENGINE MASTER CLI v3.8 [COMPLETE] ---" << endl;              cout << "  [BUILD]    add <n> | connect <u,v> | rename <id,n> | set-img <id,p>" << endl;                                                                                    cout << "  [ANALYZE]  rank    | stats         | redflag       | bottleneck" << endl;                                                                                        cout << "  [NAVIGATE] path    | neighbors     | find <txt>    | witness <u,v>" << endl;                                                                                     cout << "  [SECURITY] isolate | purge         | dossier <id>" << endl;                cout << "  [HISTORY]  save    | load          | timeline      | forensics <s,e>" << endl;                                                                                   cout << "  [SYSTEM]   list    | export        | clear         | exit" << endl;        cout << "--------------------------------------------------------" << endl;
    while (true) {

        cout << "graph-engine> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;
        stringstream ss(line); ss >> cmd;
        if (cmd == "exit") break;

        // --- [BUILD] ---
        else if (cmd == "add") { string n; ss >> n; if(!n.empty()) { store->addNode(n); cout << "✅ Added " << n << endl; } }
        else if (cmd == "connect") { uint64_t u, v; if(ss >> u >> v) { store->addEdge(u, v, time(0)); cout << "�� Connected " << u << " <-> " << v << endl; } }
        else if (cmd == "rename") { uint64_t id; string n; if(ss >> id >> n) CommandHandler::renameNode(store.get(), id, n); }
        else if (cmd == "set-img") { uint64_t id; string p; if(ss >> id >> p && store->getNodes().count(id)) { store->getNodes()[id]->setImage(p); cout << "��️ Image set." << endl; } }

        // --- [ANALYZE] ---
        else if (cmd == "rank") CommandHandler::showRank(store.get());
        else if (cmd == "stats") CommandHandler::showStats(store.get());
        else if (cmd == "redflag") CommandHandler::runRedFlag(store.get());
        else if (cmd == "bottleneck") CommandHandler::showBottlenecks(store.get());

        // --- [NAVIGATE] ---
        else if (cmd == "path") { uint64_t u, v; if(ss >> u >> v) CommandHandler::findPath(store.get(), u, v); }
        else if (cmd == "neighbors") { uint64_t id; if(ss >> id) CommandHandler::showNeighbors(store.get(), id); }
        else if (cmd == "find") { string q; ss >> q; CommandHandler::findNode(store.get(), q); }
        else if (cmd == "witness") { uint64_t u, v; if(ss >> u >> v) CommandHandler::findWitness(store.get(), u, v); }

        // --- [SECURITY] ---
        else if (cmd == "isolate") { uint64_t id; if(ss >> id) CommandHandler::isolateNode(store.get(), id); }
        else if (cmd == "dossier") { uint64_t id; if(ss >> id) CommandHandler::runDossier(store.get(), id); }
        else if (cmd == "purge" || cmd == "clear") { store = make_unique<GraphStore>(); cout << "♻️ Memory Purged." << endl; }

        // --- [HISTORY & SYSTEM] ---
        else if (cmd == "save") CommandHandler::saveSnapshot(store.get());
        else if (cmd == "load") CommandHandler::loadSnapshot(store.get());
        else if (cmd == "list") CommandHandler::listNodes(store.get());
        else if (cmd == "timeline") CommandHandler::showTimeline(store.get());
        else if (cmd == "forensics") { long long s, e; if(ss >> s >> e) CommandHandler::runForensics(store.get(), s, e); }
        else if (cmd == "export" || cmd == "json") CommandHandler::exportJSON(store.get());
                else if (cmd == "possibility") {
    uint64_t u, v;
    if (ss >> u >> v) CommandHandler::calculatePossibility(store.get(), u, v);
}
      
        else { cout << "❓ Unknown command: " << cmd << endl; }
    }
    return 0;
}
