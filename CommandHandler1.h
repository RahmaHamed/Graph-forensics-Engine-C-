#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <ctime>
#include "core/GraphStore.h"

using namespace std;
using namespace graph;

class CommandHandler {
public:
    // --- [HELPERS] ---
    static string formatTime(long long ts) {
        time_t rawtime = (time_t)ts;
        struct tm * ti = localtime(&rawtime);
        char buf[80];
        if (!ti) return "1970-01-01 00:00:00";
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ti);
        return string(buf);
    }

    static int64_t findIdByName(GraphStore* store, string name) {
        for (auto const& [id, n] : store->getNodes()) {
            if (n->label() == name) return (int64_t)id;
        }
        return -1;
    }

    // --- [SYSTEM & BUILD] ---
    static void listNodes(GraphStore* store) {
        auto& nodes = store->getNodes();
        if (nodes.empty()) { cout << "�� Graph is empty." << endl; return; }
        cout << "�� --- NODE REGISTRY ---" << endl;
        for (auto const& [id, n] : nodes) {
            cout << "  [ID: " << id << "] " << n->label() << " (Img: " << n->image() << ")" << endl;
        }
    }

    static void renameNode(GraphStore* store, uint64_t id, string newName) {
        if (store->getNodes().count(id)) {
            string oldImg = store->getNodes()[id]->image();
            store->getNodes()[id] = make_unique<Node>(id, newName);
            store->getNodes()[id]->setImage(oldImg);
            cout << "✏️ Renamed node " << id << " -> " << newName << endl;
        } else cout << "❌ ID not found." << endl;
    }

    // --- [ANALYZE & CONNECTIVITY] ---
    static void calculatePossibility(GraphStore* store, uint64_t u, uint64_t v) {
        if (!store->getNodes().count(u) || !store->getNodes().count(v)) {
            cout << "❌ ID(s) do not exist." << endl; return;
        }
        set<uint64_t> nU, nV;
        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            for (auto const& e : list) {
                if (e->source() == u) nU.insert(e->target());
                if (e->target() == u) nU.insert(e->source());
                if (e->source() == v) nV.insert(e->target());
                if (e->target() == v) nV.insert(e->source());
            }
        }
        set<uint64_t> shared;
        for (uint64_t n : nU) if (nV.count(n)) shared.insert(n);
        set<uint64_t> total = nU;
        total.insert(nV.begin(), nV.end());

        double score = total.empty() ? 0 : (double)shared.size() / total.size();
        cout << "�� --- POSSIBILITY: " << (score * 100) << "% ---" << endl;
        if (score > 0.5) cout << "  ⚠️ HIGH PROBABILITY of collaboration." << endl;
    }

    static void runRedFlag(GraphStore* store) {
        cout << "�� --- CONSPIRACY SCANNER ---" << endl;
        bool found = false;
        for (auto const& [id1, n1] : store->getNodes()) {
            set<uint64_t> nbrs;
            for (auto const& [ts, list] : store->getChronologicalEdges())
                for (auto const& e : list) {
                    if (e->source() == id1) nbrs.insert(e->target());
                    if (e->target() == id1) nbrs.insert(e->source());
                }
            for (uint64_t id2 : nbrs) {
                for (uint64_t id3 : nbrs) {
                    if (id2 >= id3) continue;
                    for (auto const& [ts, list] : store->getChronologicalEdges())
                        for (auto const& e : list)
                            if ((e->source()==id2 && e->target()==id3) || (e->source()==id3 && e->target()==id2)) {
                                cout << "⚠️ TRIANGLE: " << n1->label() << " <-> " << store->getNodeLabel(id2) << " <-> " << store->getNodeLabel(id3) << endl;
                                found = true;
                            }
                }
            }
        }
        if (!found) cout << "✅ No suspicious triangles." << endl;
    }

    static void findPath(GraphStore* store, uint64_t start, uint64_t end) {
        queue<vector<uint64_t>> q; q.push({start});
        set<uint64_t> visited = {start};
        while(!q.empty()) {
            vector<uint64_t> path = q.front(); q.pop();
            if(path.back() == end) {
                cout << "�� PATH: ";
                for(size_t i=0; i<path.size(); ++i) cout << store->getNodeLabel(path[i]) << (i==path.size()-1?"":" -> ");
                cout << endl; return;
            }
            for(auto const& [ts, list] : store->getChronologicalEdges())
                for(auto const& e : list) {
                    uint64_t nxt = (e->source()==path.back())?e->target():(e->target()==path.back()?e->source():999999);
                    if(nxt!=999999 && !visited.count(nxt)) {
                        visited.insert(nxt); vector<uint64_t> np = path; np.push_back(nxt); q.push(np);
                    }
                }
        }
        cout << "❌ No path." << endl;
    }

    // --- [HISTORY & FILE I/O] ---
    static void loadSnapshot(GraphStore* store) {
        ifstream in("graph_snapshot.txt");
        if (!in) { cout << "❌ No file." << endl; return; }
        string type, label, img; uint64_t id, u, v; long long ts;
        int nc=0, ec=0;
        while (in >> type) {
            if (type == "NODE") {
                in >> id >> label >> img;
                store->addNode(label);
                if(store->getNodes().count(id)) store->getNodes()[id]->setImage(img);
                nc++;
            }
            else if (type == "EDGE") { in >> u >> v >> ts; store->addEdge(u, v, ts); ec++; }
        }
        cout << "�� Loaded " << nc << " nodes, " << ec << " edges." << endl;
    }

    static void showTimeline(GraphStore* store) {
        auto timeline = store->getChronologicalEdges();
        if(timeline.empty()) { cout << "⏳ Empty." << endl; return; }
        for (auto const& [ts, list] : timeline)
            for (auto const& e : list)
                cout << "  [" << formatTime(ts) << "] " << store->getNodeLabel(e->source()) << " <-> " << store->getNodeLabel(e->target()) << endl;
    }
};

#endif
