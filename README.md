#  Graph Forensics & Security Engine
**A High-Performance C++ Framework for Digital Investigation & Network Analysis**

---

##  Project Overview
Traditional graph systems treat data as static snapshots. This **Forensics Engine** introduces a temporal (time-based) dimension, treating nodes as entities and edges as timed interactions. 

By mimicking a **"Forensic Investigation Board,"** the engine allows users to reconstruct timelines, identify hidden conspiracies (cliques), and isolate threats within a network.



---

##  System Architecture
The engine is built with a modular, thread-safe C++ architecture designed for memory safety and efficiency.

* **Core Logic:** Implemented with a decoupled design where `GraphStore` handles data and `CommandHandler` serves as the analytical brain.
* **Memory Safety:** Utilizes `std::unique_ptr` for Nodes and `std::shared_ptr` for Edges to ensure a **zero-leak** footprint.
* **Concurrency:** Features `std::mutex` and `lock_guard` implementations to prevent data races during real-time graph modifications.
* **Performance:** Built on `std::unordered_map` for **$O(1)$** average-time entity lookups.

---

##  Forensic & Security Toolset

### **1. Relationship Analysis (`analyze`)**
Calculates the probability of collaboration between two entities by analyzing shared neighbors and network overlap.

### **2. Conspiracy Scanner (`redflag`)**
Scans for **"Triangle Cliques"**—interconnected loops where three or more nodes are all linked, often indicating coordinated malicious activity.

### **3. Isolation Protocol (`isolate`)**
Acting as "Digital Handcuffs," this command severs all active connections for a specific node to prevent "lateral movement" during an investigation.



---

##  Command Reference

| Category | Command | Forensic Purpose |
| :--- | :--- | :--- |
| **Search** | `find <text>` | Search for entities by label or metadata. |
| **Analysis** | `analyze <u> <v>` | Generate a relationship report with confidence scores. |
| **Navigation**| `path <u> <v>` | Find the shortest **chronologically valid** link. |
| **Security** | `redflag` | Identify high-risk cliques and network kingpins. |
| **Evidence** | `dossier <id>` | Compile a full profile including all "first/last seen" events. |
| **Temporal** | `forensics <s> <e>`| Reconstruct events within a specific time window. |

---

##  Visualization & Persistence
The engine supports multiple formats for reporting and external analysis:
* **Graphviz Integration:** Exports to `.dot` files for professional network mapping.
* **JSON Export:** Generates structured data for web-based forensic dashboards (`index.html`).
* **Snapshots:** Save and load full graph states to resume investigations.



---

##  Build & Usage
Ensure you have a C++17 compatible compiler (like GCC or Clang).

This project uses a **Makefile** for optimized compilation. Ensure you have `g++` and `make` installed.

#  Build the project using the Makefile
make

#  Run the engine
./graph_engine
