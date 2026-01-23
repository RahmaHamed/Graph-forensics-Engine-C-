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
#include <unordered_map> // Added for runRedFlag
#include "core/GraphStore.h"
#include <ctime>

using namespace std;
using namespace graph;

class CommandHandler {
public:
    // --- [SYSTEM & BUILD] ---
    static string formatTime(long long ts) {
        time_t rawtime = (time_t)ts;
        struct tm * ti = localtime(&rawtime);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ti);
        return string(buf);
    }
    static void listNodes(GraphStore* store) {
        auto& nodes = store->getNodes();
        if (nodes.empty()) { cout << "�� Graph is empty (0 nodes)." << endl; return; }
        cout << "�� --- NODE REGISTRY ---" << endl;
        for (auto const& [id, n] : nodes) {
            cout << "  [ID: " << id << "] " << n->label() << " (Img: " << n->image() << ")" << endl;
        }
    }

    static void findNode(GraphStore* store, string query) {
        bool found = false;
        for (auto const& [id, n] : store->getNodes()) {
            if (n->label().find(query) != string::npos) {
                cout << "�� Found: [ID " << id << "] " << n->label() << endl;
                found = true;
            }
        }
        if (!found) cout << "❌ No node found matching '" << query << "'" << endl;
    }

    static void renameNode(GraphStore* store, uint64_t id, string newName) {
        if (store->getNodes().count(id)) {
            string oldImg = store->getNodes()[id]->image();
            store->getNodes()[id] = make_unique<Node>(id, newName);
            store->getNodes()[id]->setImage(oldImg);
            cout << "✏️ Renamed node " << id << " -> " << newName << endl;
        } else cout << "❌ ID not found." << endl;
    }

    // --- [ANALYZE] ---
    static void showStats(GraphStore* store) {
        int edges = 0;
        for(auto const& [t, l] : store->getChronologicalEdges()) edges += l.size();
        cout << "�� STATISTICS:\n  - Nodes: " << store->getNodes().size() << "\n  - Edges: " << edges << endl;
    }

    static void showRank(GraphStore* store) {
        vector<pair<int, string>> ranks;
        for (auto const& [id, n] : store->getNodes()) {
            int deg = 0;
            for (auto const& [ts, list] : store->getChronologicalEdges())
                for (auto const& e : list) if (e->source() == id || e->target() == id) deg++;
            ranks.push_back({deg, n->label()});
        }
        sort(ranks.rbegin(), ranks.rend());
        cout << "�� --- INFLUENCE RANKING ---" << endl;
        for (auto const& r : ranks) cout << "  #" << r.first << " Links: " << r.second << endl;
    }

    static void runRedFlag(GraphStore* store) {
        cout << "�� --- CONSPIRACY SCANNER (Optimized) ---" << endl;
        unordered_map<uint64_t, set<uint64_t>> adj;
        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            for (auto const& e : list) {
                adj[e->source()].insert(e->target());
                adj[e->target()].insert(e->source());
            }
        }
        bool found = false;
        for (auto const& [u, neighbors] : adj) {
            for (uint64_t v : neighbors) {
                if (v <= u) continue;
                for (uint64_t w : neighbors) {
                    if (w <= v) continue;
                    if (adj[v].count(w)) {
                        cout << "⚠️ TRIANGLE DETECTED: "
                             << store->getNodeLabel(u) << " <-> "
                             << store->getNodeLabel(v) << " <-> "
                             << store->getNodeLabel(w) << endl;
                        found = true;
                    }
                }
            }
        }
        if (!found) cout << "✅ No suspicious triangles found." << endl;
    }

    // --- [NEW: Added for main.cpp compatibility] ---
    static void runDossier(GraphStore* store, uint64_t id) {
        if(!store->getNodes().count(id)) { cout << "❌ Invalid ID." << endl; return; }
        auto& n = store->getNodes()[id];
        int conn = 0;
        for (auto const& [ts, list] : store->getChronologicalEdges())
            for (auto const& e : list) if (e->source() == id || e->target() == id) conn++;
        cout << "�� --- INTELLIGENCE DOSSIER ---" << endl;
        cout << "  SUBJECT: " << n->label() << " | THREAT: " << (conn * 15) << "%" << endl;
        cout << "  CONNECTIONS: " << conn << endl;
    }

    // --- [NAVIGATE] ---
    static void findPath(GraphStore* store, uint64_t start, uint64_t end) {
        queue<vector<uint64_t>> q; q.push({start});
        set<uint64_t> visited = {start};
        while(!q.empty()) {
            vector<uint64_t> path = q.front(); q.pop();
            if(path.back() == end) {
                cout << "��️ PATH FOUND: ";
                for(size_t i=0; i<path.size(); ++i) cout << store->getNodeLabel(path[i]) << (i==path.size()-1?"":" -> ");
                cout << endl; return;
            }
            for(auto const& [ts, list] : store->getChronologicalEdges()) {
                for(auto const& e : list) {
                    uint64_t nxt = (e->source()==path.back())?e->target():(e->target()==path.back()?e->source():999999);
                    if(nxt!=999999 && !visited.count(nxt)) {
                        visited.insert(nxt); vector<uint64_t> np = path; np.push_back(nxt); q.push(np);
                    }
                }
            }
        }
        cout << "❌ No path exists." << endl;
    }

    static void showNeighbors(GraphStore* store, uint64_t id) {
        cout << "��️ NEIGHBORS of " << store->getNodeLabel(id) << ":" << endl;
        bool found = false;
        for(auto const& [ts, list] : store->getChronologicalEdges()) {
            for(auto const& e : list) {
                if(e->source() == id) { cout << "  -> " << store->getNodeLabel(e->target()) << endl; found = true; }
                if(e->target() == id) { cout << "  -> " << store->getNodeLabel(e->source()) << endl; found = true; }
            }
        }
        if (!found) cout << "  (Isolated Node)" << endl;
    }

    static void calculatePossibility(GraphStore* store, uint64_t u, uint64_t v) {
        auto& nodes = store->getNodes();
        if (!nodes.count(u) || !nodes.count(v)) {
            cout << "❌ Error: ID not found." << endl; return;
        }
        set<uint64_t> nU, nV;
        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            for (auto const& e : list) {
                if (e->source() == u) nU.insert(e->target());
                else if (e->target() == u) nU.insert(e->source());
                if (e->source() == v) nV.insert(e->target());
                else if (e->target() == v) nV.insert(e->source());
            }
        }
        set<uint64_t> shared;
        for (uint64_t n : nU) if (nV.count(n)) shared.insert(n);
        set<uint64_t> total = nU;
        total.insert(nV.begin(), nV.end());
        cout << "�� --- POSSIBILITY ANALYSIS ---" << endl;
        if (total.empty()) cout << "  Strength: 0%" << endl;
        else cout << "  Overlap: " << ((double)shared.size()/total.size()*100) << "%" << endl;
    }

    static void findWitness(GraphStore* store, uint64_t u, uint64_t v) {
        set<uint64_t> un, vn;
        for(auto const& [ts, l] : store->getChronologicalEdges())
            for(auto const& e : l) {
                if(e->source()==u) un.insert(e->target()); if(e->target()==u) un.insert(e->source());
                if(e->source()==v) vn.insert(e->target()); if(e->target()==v) vn.insert(e->source());
            }
        cout << "��️‍��️ SEARCHING FOR COMMON LINKS..." << endl;
        bool found = false;
        for(auto n : un) if(vn.count(n)) { cout << "  ⚠️ WITNESS: " << store->getNodeLabel(n) << endl; found = true; }
        if(!found) cout << "  No common witness found." << endl;
    }

    // --- [SECURITY] ---
    static void showBottlenecks(GraphStore* store) {
        cout << "�� --- BOTTLENECK ANALYSIS ---" << endl;
        for(auto const& [id, n] : store->getNodes()) {
            int deg=0;
            for(auto const& [ts, l] : store->getChronologicalEdges())
                for(auto const& e : l) if(e->source()==id||e->target()==id) deg++;
            if(deg > 3) cout << "  �� HIGH TRAFFIC: " << n->label() << " (" << deg << " connections)" << endl;
        }
    }

    static void isolateNode(GraphStore* store, uint64_t id) {
        cout << "��️ ISOLATING Node " << id << "..." << endl;
        cout << "  (Simulated) All edges to " << store->getNodeLabel(id) << " blocked." << endl;
    }

    // --- [HISTORY & FILE I/O] ---
    static void showTimeline(GraphStore* store) {
        auto timeline = store->getChronologicalEdges();
        if(timeline.empty()) { cout << "⏳ No events in timeline." << endl; return; }
        cout << "�� --- HUMAN-READABLE TIMELINE ---" << endl;
        for (auto const& [ts, list] : timeline) {
            string timeStr = formatTime(ts);
            for (auto const& e : list)
                cout << "  [" << timeStr << "] " << store->getNodeLabel(e->source()) << " <---> " << store->getNodeLabel(e->target()) << endl;
        }
    }

    static void runForensics(GraphStore* store, long long s, long long e) {
        cout << "�� FORENSIC WINDOW: " << formatTime(s) << " to " << formatTime(e) << endl;
        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            if(ts >= s && ts <= e) {
                for(auto const& ed : list)
                    cout << "  MATCH: [" << formatTime(ts) << "] " << store->getNodeLabel(ed->source()) << " <-> " << store->getNodeLabel(ed->target()) << endl;
            }
        }
    }

    static void saveSnapshot(GraphStore* store) {
        ofstream out("graph_snapshot.txt");
        for(auto const& [id, n] : store->getNodes()) out << "NODE " << id << " " << n->label() << " " << n->image() << endl;
        for(auto const& [ts, list] : store->getChronologicalEdges())
            for(auto const& e : list) out << "EDGE " << e->source() << " " << e->target() << " " << ts << endl;
        out.close();
        cout << "�� Snapshot saved to graph_snapshot.txt" << endl;
    }

    static void loadSnapshot(GraphStore* store) {
        ifstream in("graph_snapshot.txt");
        if (!in) { cout << "❌ No snapshot found." << endl; return; }
        string type, label, img; uint64_t id, u, v; long long ts;
        int nc=0, ec=0;
        while (in >> type) {
            if (type == "NODE") {
                in >> id >> label >> img;
                store->addNode(label);
                store->getNodes()[id]->setImage(img);
                nc++;
            }
            else if (type == "EDGE") { in >> u >> v >> ts; store->addEdge(u, v, ts); ec++; }
        }
        cout << "�� Loaded " << nc << " nodes and " << ec << " edges." << endl;
    }

    static void exportJSON(GraphStore* store) {
        ofstream out("graph_data.json");
        out << "{\"nodes\":[";
        auto& ns = store->getNodes(); size_t i=0;
        for(auto const& [id, n] : ns) {
            out << "{\"id\":" << id << ",\"label\":\"" << n->label() << "\",\"image\":\"" << n->image() << "\"}";
            if(++i < ns.size()) out << ",";
        }
        out << "],\"edges\":[";
        auto tl = store->getChronologicalEdges(); vector<shared_ptr<Edge>> edgs;
        for(auto const& [ts, l] : tl) for(auto const& e : l) edgs.push_back(e);
        for(size_t j=0; j<edgs.size(); ++j) {
            out << "{\"from\":" << edgs[j]->source() << ",\"to\":" << edgs[j]->target() << ",\"ts\":" << edgs[j]->timestamp() << "}";
            if(j < edgs.size()-1) out << ",";
        }
        out << "]}";
        out.close();
        cout << "�� Data exported to graph_data.json" << endl;
    }
};

#endif
