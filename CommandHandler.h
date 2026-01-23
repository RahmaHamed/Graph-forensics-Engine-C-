#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <cctype>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <memory> 
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
            // FIX: Used make_unique instead of make_shared
            string oldImg = store->getNodes()[id]->image(); // Keep the old image
            store->getNodes()[id] = make_unique<Node>(id, newName); 
            store->getNodes()[id]->setImage(oldImg); // Restore image
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
    
    // 1. Pre-calculate Adjacency List for O(1) lookups
    unordered_map<uint64_t, set<uint64_t>> adj;
    for (auto const& [ts, list] : store->getChronologicalEdges()) {
        for (auto const& e : list) {
            adj[e->source()].insert(e->target());
            adj[e->target()].insert(e->source());
        }
    }

    bool found = false;
    // 2. Iterate through nodes and their neighbors
    for (auto const& [u, neighbors] : adj) {
        for (uint64_t v : neighbors) {
            if (v <= u) continue; // Avoid duplicate pairs
            
            for (uint64_t w : neighbors) {
                if (w <= v) continue; // Ensure u < v < w order
                
                // 3. Check if v and w are also connected
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
    // --- [NAVIGATE] ---
struct TemporalState {
    uint64_t node;
    long long lastTs;
    vector<uint64_t> path;
};
static void findPath(GraphStore* store, uint64_t start, uint64_t end) {
    queue<TemporalState> q;
    q.push({start, LLONG_MIN, {start}});

    set<pair<uint64_t, long long>> visited;

    while (!q.empty()) {
        auto cur = q.front(); q.pop();

        if (cur.node == end) {
            cout << "�� TIME-VALID PATH FOUND:\n  ";
            for (size_t i = 0; i < cur.path.size(); ++i)
                cout << store->getNodeLabel(cur.path[i])
                     << (i + 1 < cur.path.size() ? " → " : "");
            cout << endl;
            return;
        }

        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            if (ts < cur.lastTs) continue; // ⛔ NO TIME TRAVEL

            for (auto const& e : list) {
                uint64_t next = 0;
                if (e->source() == cur.node) next = e->target();
                else if (e->target() == cur.node) next = e->source();
                else continue;

                if (!visited.count({next, ts})) {
                    visited.insert({next, ts});
                    auto newPath = cur.path;
                    newPath.push_back(next);
                    q.push({next, ts, newPath});
                }
            }
        }
    }

    cout << "❌ No chronologically valid path found." << endl;
}

static long long findSmallestGap(vector<long long>& timesA, vector<long long>& timesB) {
    if (timesA.empty() || timesB.empty()) return LONG_MAX;

    sort(timesA.begin(), timesA.end());
    sort(timesB.begin(), timesB.end());

    long long minGap = LONG_MAX;
    size_t i = 0, j = 0;

    // Two-pointer approach to find the closest pair in O(N + M)
    while (i < timesA.size() && j < timesB.size()) {
        long long currentGap = abs(timesA[i] - timesB[j]);
        if (currentGap < minGap) minGap = currentGap;

        if (timesA[i] < timesB[j]) i++;
        else j++;
    }
    return minGap;
}
struct Finding {
    string witnessName;
    long long minGapSeconds;
    double localizedConfidence;
    string narrative;
};

struct InvestigationReport {
    string leadSummary;
    vector<Finding> allFindings;
    double globalConfidence; // Strictly 0.0 to 1.0
};

static InvestigationReport analyzeRelationship(GraphStore* store, uint64_t a, uint64_t b) {
    InvestigationReport report;
    report.globalConfidence = 0.0;

    // Fix 3: Get all witnesses instead of overwriting
    vector<uint64_t> witnesses = store->getCommonNeighbors(a, b);

    for (uint64_t w : witnesses) {
        Finding f;
        f.witnessName = store->getNodeLabel(w);

        // Fix 1: Find the absolute minimum temporal gap (Temporal Minimization)
        auto timesA = store->getAllTimestamps(a, w);
        auto timesB = store->getAllTimestamps(b, w);
        f.minGapSeconds = findSmallestGap(timesA, timesB); 

        // Fix 4: Explain the "Low Traffic" logic in the narrative
        int activity = store->getDegree(w);
        bool isPrivateLink = (activity < 10);
        
        f.narrative = f.witnessName + " acted as a bridge within " + to_string(f.minGapSeconds / 3600) + "h. ";
        f.narrative += isPrivateLink ? "This low-traffic link suggests a private channel." : "High traffic at this node increases the chance of coincidence.";

        // Fix 2: Bounded Confidence (Sigmoid or Weighted Sum)
        double timeScore = (f.minGapSeconds < 172800) ? 0.5 : 0.1;
        double privacyScore = isPrivateLink ? 0.4 : 0.1;
        f.localizedConfidence = min(1.0, timeScore + privacyScore);

        report.allFindings.push_back(f);
    }

    // Aggregate: Use the strongest finding as the "Lead"
    if (!report.allFindings.empty()) {
        sort(report.allFindings.begin(), report.allFindings.end(), [](const Finding& x, const Finding& y) {
            return x.localizedConfidence > y.localizedConfidence;
        });
        report.leadSummary = "Primary Lead: " + report.allFindings[0].narrative;
        report.globalConfidence = report.allFindings[0].localizedConfidence;
    }

    return report;
}
static void runDossier(GraphStore* store, uint64_t id) {
        auto& nodes = store->getNodes();
        if (!nodes.count(id)) {
            cout << "❌ Error: Node ID " << id << " not found." << endl;
            return;
        }

        auto& node = nodes.at(id);
        cout << "\n�� --- DOSSIER: " << node->label() << " ---" << endl;
        cout << "  [ID]        : " << id << endl;
        cout << "  [IMAGE]     : " << (node->image().empty() ? "None" : node->image()) << endl;

        int connections = 0;
        long long firstSeen = -1, lastSeen = -1;
        vector<string> neighbors;

        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            for (auto const& e : list) {
                uint64_t target = 0;
                if (e->source() == id) target = e->target();
                else if (e->target() == id) target = e->source();
                else continue;

                connections++;
                neighbors.push_back(store->getNodeLabel(target));
                if (firstSeen == -1 || ts < firstSeen) firstSeen = ts;
                if (ts > lastSeen) lastSeen = ts;
            }
        }

        cout << "  [CONNECTIONS]: " << connections << endl;
        if (connections > 0) {
            cout << "  [FIRST ACT]  : " << formatTime(firstSeen) << endl;
            cout << "  [LAST ACT]   : " << formatTime(lastSeen) << endl;
            cout << "  [RELATIONS]  : ";
            for (size_t i = 0; i < neighbors.size(); ++i) {
                cout << neighbors[i] << (i == neighbors.size() - 1 ? "" : ", ");
            }
            cout << endl;
        }
        cout << "-------------------------------------\n" << endl;
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

    // 1. Validation: Ensure both nodes exist
    if (!nodes.count(u) || !nodes.count(v)) {
        cout << "❌ Error: One or both Node IDs do not exist." << endl;
        return;
    }

    // 2. Map the "Neighborhood" for both nodes
    set<uint64_t> neighborsU, neighborsV;
    for (auto const& [ts, list] : store->getChronologicalEdges()) {
        for (auto const& e : list) {
            if (e->source() == u) neighborsU.insert(e->target());
            else if (e->target() == u) neighborsU.insert(e->source());

            if (e->source() == v) neighborsV.insert(e->target());
            else if (e->target() == v) neighborsV.insert(e->source());
        }
    }

    // 3. Calculate Intersection (Shared Neighbors)
    set<uint64_t> shared;
    for (uint64_t n : neighborsU) {
        if (neighborsV.count(n)) shared.insert(n);
    }

    // 4. Calculate Union (Total Unique Neighbors)
    set<uint64_t> totalUnique = neighborsU;
    totalUnique.insert(neighborsV.begin(), neighborsV.end());

    // 5. Calculate & Display Results
    cout << "�� --- POSSIBILITY ANALYSIS ---" << endl;
    cout << "  Target A: " << store->getNodeLabel(u) << endl;
    cout << "  Target B: " << store->getNodeLabel(v) << endl;

    if (totalUnique.empty()) {
        cout << "  Strength: 0% (Isolated nodes)" << endl;
    } else {
        double score = (double)shared.size() / totalUnique.size();
        cout << "  Shared Partners: " << shared.size() << endl;
        cout << "  Network Overlap: " << (score * 100) << "%" << endl;

        if (score > 0.6) cout << "  ⚠️  STATUS: Extremely High Probability of direct collaboration." << endl;
        else if (score > 0.2) cout << "  �� STATUS: Probable indirect link detected." << endl;
        else cout << "  �� STATUS: Weak/No correlation." << endl;
    }
}
    static void findWitness(GraphStore* store, uint64_t u, uint64_t v) {
        set<uint64_t> un, vn;
        for(auto const& [ts, l] : store->getChronologicalEdges()) 
            for(auto const& e : l) {
                if(e->source()==u) un.insert(e->target()); if(e->target()==u) un.insert(e->source());
                if(e->source()==v) vn.insert(e->target()); if(e->target()==v) vn.insert(e->source());
            }
        cout << "��️ SEARCHING FOR COMMON LINKS..." << endl;
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
static void loadSnapshot(GraphStore* store) {
        const std::string filename = "graph_snapshot.txt";
        std::ifstream in(filename);
        if (!in.is_open()) {
            std::cout << "❌ File not found: " << filename << std::endl;
            return;
        }

        auto parseToken = [&](const std::string &line, size_t &pos) -> std::string {
            while (pos < line.size() && std::isspace((unsigned char)line[pos])) ++pos;
            if (pos >= line.size()) return "";
            std::string out;
            if (line[pos] == '"') {
                ++pos;
                while (pos < line.size()) {
                    char c = line[pos++];
                    if (c == '\\' && pos < line.size()) out.push_back(line[pos++]);
                    else if (c == '"') break;
                    else out.push_back(c);
                }
            } else {
                while (pos < line.size() && !std::isspace((unsigned char)line[pos])) out.push_back(line[pos++]);
            }
            return out;
        };

        struct NodeRec { uint64_t fileId; std::string label; std::string img; };
        struct EdgeRec { uint64_t u; uint64_t v; long long ts; };
        std::vector<NodeRec> nodeRecs;
        std::vector<EdgeRec> edgeRecs;

        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            size_t pos = 0;
            std::string typ = parseToken(line, pos);
            if (typ == "NODE") {
                uint64_t fid = std::stoull(parseToken(line, pos));
                std::string lbl = parseToken(line, pos);
                std::string img = parseToken(line, pos);
                nodeRecs.push_back({fid, lbl, img});
            } else if (typ == "EDGE") {
                uint64_t u = std::stoull(parseToken(line, pos));
                uint64_t v = std::stoull(parseToken(line, pos));
                long long ts = std::stoll(parseToken(line, pos));
                edgeRecs.push_back({u, v, ts});
            }
        }

        store->getNodes().clear();
        store->getChronologicalEdges().clear();

        std::unordered_map<uint64_t, uint64_t> idMap;
        for (auto &nr : nodeRecs) {
            uint64_t rid = store->addNode(nr.label);
            if (!nr.img.empty()) store->getNodes()[rid]->setImage(nr.img);
            idMap[nr.fileId] = rid;
        }
        for (auto &er : edgeRecs) {
            store->addEdge(idMap[er.u], idMap[er.v], er.ts);
        }
        std::cout << "✅ Snapshot loaded successfully." << std::endl;
    }
// Real Isolation: Removes all edges connected to a specific node
    static void isolateNode(GraphStore* store, uint64_t id) {
        if (!store->getNodes().count(id)) {
            cout << "❌ ID not found." << endl;
            return;
        }

        auto& chronologicalEdges = store->getChronologicalEdges();
        int removedCount = 0;

        for (auto& [ts, edgeList] : chronologicalEdges) {
            auto it = edgeList.begin();
            while (it != edgeList.end()) {
                if ((*it)->source() == id || (*it)->target() == id) {
                    it = edgeList.erase(it);
                    removedCount++;
                } else {
                    ++it;
                }
            }
        }
        cout << "��️ ISOLATED: Removed " << removedCount << " active connections for " << store->getNodeLabel(id) << "." << endl;
    }

    // Real Purge: Wipes the entire graph from memory
    static void purgeGraph(GraphStore* store) {
        store->getNodes().clear();
        store->getChronologicalEdges().clear();
        cout << "♻️ Memory Purged. Graph is now empty." << endl;
    }
static void saveSnapshot(GraphStore* store) {
        const std::string filename = "graph_snapshot.txt";
        std::ofstream out(filename);
        if (!out.is_open()) {
            std::cout << "❌ Could not open " << filename << " for writing." << std::endl;
            return;
        }

        // Write Nodes: NODE <id> "<label>" "<image>"
        for (auto const& [id, n] : store->getNodes()) {
            out << "NODE " << id << " \"" << n->label() << "\" \"" << n->image() << "\"\n";
        }

        // Write Edges: EDGE <u> <v> <timestamp>
        for (auto const& [ts, list] : store->getChronologicalEdges()) {
            for (auto const& e : list) {
                out << "EDGE " << e->source() << " " << e->target() << " " << ts << "\n";
            }
        }

        out.close();
        std::cout << "�� Snapshot saved to " << filename << std::endl;
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
