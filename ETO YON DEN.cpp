#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <climits>
#include <cctype>
#include <algorithm>
using namespace std;

// Helper function to convert string to lower case
string toLower(const string& s) {
    string res = s;
    for (char& c : res) c = tolower(c);
    return res;
}

// Location group mapping
unordered_map<string, string> locationGroup = {
    // Ground Floor
    {"Main Gate", "Ground Floor"}, {"Clinic", "Ground Floor"}, {"Main Admission", "Ground Floor"},
    {"Gazebo", "Ground Floor"}, {"Gym", "Ground Floor"}, {"Back Gate", "Ground Floor"},
    {"Waiting Shed", "Ground Floor"}, {"TYK Stairs Left", "Ground Floor"}, {"TYK Elevator Left", "Ground Floor"},
    {"TYK Admission", "Ground Floor"}, {"Field", "Ground Floor"}, {"SAO", "Ground Floor"},
    {"LCT Stairs Mid", "Ground Floor"}, {"LCT Elevator Mid", "Ground Floor"}, {"EN Stairs Left", "Ground Floor"},
    {"RM103", "Ground Floor"}, {"EN Stairs Back Left", "Ground Floor"}, {"Parking", "Ground Floor"},
    {"TYK Stairs Mid", "Ground Floor"}, {"TYK Elevator Mid", "Ground Floor"}, {"Canteen", "Ground Floor"},
    {"TYK Stairs Right", "Ground Floor"}, {"HM Building", "Ground Floor"}, {"EN Stairs Right", "Ground Floor"},
    {"RM117", "Ground Floor"}, {"EN Stairs Back Right", "Ground Floor"}, {"RM100", "Ground Floor"},
    {"EN Admission", "Ground Floor"}, {"CpE Faculty", "Ground Floor"}, {"ME Lab", "Ground Floor"},

    // Second Floor
    {"TYK Stairs Left 2F", "Second Floor"}, {"TYK Elevator Left 2F", "Second Floor"}, {"RM200", "Second Floor"},
    {"RM201", "Second Floor"}, {"TYK Stairs Mid 2F", "Second Floor"}, {"TYK Elevator Mid 2F", "Second Floor"},
    {"RM203", "Second Floor"}, {"RM204", "Second Floor"}, {"TYK Stairs Right 2F", "Second Floor"},
    {"RM205", "Second Floor"}, {"RM206", "Second Floor"}, {"LCT Stairs Mid 2F", "Second Floor"},
    {"LCT Elevator Mid 2F", "Second Floor"}, {"RM208", "Second Floor"}, {"RM209", "Second Floor"},
    {"EN Stairs Left 2F", "Second Floor"}, {"Chemistry Tool Room", "Second Floor"}, {"EN Stairs Back Left 2F", "Second Floor"},
    {"CLR1", "Second Floor"}, {"CLR2", "Second Floor"}, {"ECE1", "Second Floor"}, {"EEPL1", "Second Floor"},
    {"EN Stairs Right 2F", "Second Floor"}, {"Physics Tool Room", "Second Floor"}, {"EN Stairs Back Right 2F", "Second Floor"},

    // Third Floor
    {"TYK Stairs Left 3F", "Third Floor"}, {"TYK Elevator Left 3F", "Third Floor"}, {"RM300", "Third Floor"},
    {"RM301", "Third Floor"}, {"TYK Stairs Mid 3F", "Third Floor"}, {"TYK Elevator Mid 3F", "Third Floor"},
    {"RM303", "Third Floor"}, {"RM304", "Third Floor"}, {"TYK Stairs Right 3F", "Third Floor"}, {"Bridge", "Third Floor"},
    {"RM305", "Third Floor"}, {"RM306", "Third Floor"}, {"LCT Stairs Mid 3F", "Third Floor"}, {"LCT Elevator Mid 3F", "Third Floor"},
    {"RM308", "Third Floor"}, {"RM309", "Third Floor"}, {"EN Stairs Left 3F", "Third Floor"}, {"Stock Room", "Third Floor"},
    {"EN Stairs Back Left 3F", "Third Floor"}, {"EN MPH", "Third Floor"}, {"EN Library", "Third Floor"}, {"RM314", "Third Floor"},
    {"EN Stairs Right 3F", "Third Floor"}, {"DAWN Room", "Third Floor"}, {"EN Stairs Back Right 3F", "Third Floor"},

    // Fourth Floor
    {"TYK Stairs Left 4F", "Fourth Floor"}, {"TYK Elevator Left 4F", "Fourth Floor"}, {"BGC LIBRARY", "Fourth Floor"},
    {"TYK Stairs Mid 4F", "Fourth Floor"}, {"TYK Elevator Mid 4F", "Fourth Floor"}, {"TYK Stairs Right 4F", "Fourth Floor"},
    {"LCT LIBRARY", "Fourth Floor"}, {"LCT Stairs Mid 4F", "Fourth Floor"}, {"LCT Elevator Mid 4F", "Fourth Floor"},
    {"EN Stairs Left 4F", "Fourth Floor"}, {"DR400", "Fourth Floor"}, {"EN Stairs Back Left 4F", "Fourth Floor"},
    {"RM400", "Fourth Floor"}, {"RM401", "Fourth Floor"}, {"RM402", "Fourth Floor"}, {"RM403", "Fourth Floor"},
    {"RM404", "Fourth Floor"}, {"EN Stairs Right 4F", "Fourth Floor"}, {"DR401", "Fourth Floor"}, {"EN Stairs Back Right 4F", "Fourth Floor"},

    // Fifth Floor
    {"TYK Stairs Left 5F", "Fifth Floor"}, {"TYK Elevator Left 5F", "Fifth Floor"}, {"TYK Stairs Mid 5F", "Fifth Floor"},
    {"TYK Elevator Mid 5F", "Fifth Floor"}, {"TYK MPH", "Fifth Floor"}, {"LCT Stairs Mid 5F", "Fifth Floor"},
    {"LCT Elevator Mid 5F", "Fifth Floor"}, {"LCT MPH", "Fifth Floor"}
};

struct Edge {
    int to;
    int weight;
};

class Graph {
    unordered_map<string, int> nameToIndex; // lower-case name -> index
    vector<string> indexToName;             // index -> original name
    vector<vector<Edge>> adj;
    int nodeCount = 0;

public:
    int addNode(const string& name) {
        string lname = toLower(name);
        if (nameToIndex.count(lname)) return nameToIndex[lname];
        nameToIndex[lname] = nodeCount++;
        indexToName.push_back(name); // Store original name for display
        adj.push_back({});
        return nodeCount - 1;
    }

    void addEdge(const string& from, const string& to, int weight) {
        int u = addNode(from);
        int v = addNode(to);
        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight}); // undirected
    }

    void dijkstra(const string& start, const string& end, const string& mode = "any") {
        string lstart = toLower(start), lend = toLower(end);
        if (!nameToIndex.count(lstart) || !nameToIndex.count(lend)) {
            cout << "Invalid location(s).\n";
            return;
        }
        int n = nodeCount;
        vector<int> dist(n, INT_MAX), prev(n, -1);
        int src = nameToIndex[lstart], dest = nameToIndex[lend];
        dist[src] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            for (auto& e : adj[u]) {
                string fromName = indexToName[u];
                string toName = indexToName[e.to];

                // Get group/floor for each node
                string fromGroup = locationGroup.count(fromName) ? locationGroup[fromName] : "";
                string toGroup = locationGroup.count(toName) ? locationGroup[toName] : "";

                bool isStairsEdge = (fromName.find("Stair") != string::npos && toName.find("Stair") != string::npos);
                bool isElevatorEdge = (fromName.find("Elevator") != string::npos && toName.find("Elevator") != string::npos);

                bool isVertical = (fromGroup != toGroup);

                // Only filter vertical transitions
                if (isVertical) {
                    if (mode == "stairs" && isElevatorEdge) continue;
                    if (mode == "elevator" && isStairsEdge) continue;
                }

                if (dist[e.to] > dist[u] + e.weight) {
                    dist[e.to] = dist[u] + e.weight;
                    prev[e.to] = u;
                    pq.push({dist[e.to], e.to});
                }
            }
        }

        if (dist[dest] == INT_MAX) {
            cout << "No path found.\n";
            return;
        }

        // Reconstruct path
        vector<int> path;
        for (int at = dest; at != -1; at = prev[at])
            path.push_back(at);
        cout << "\033[38;2;255;165;0m\nShortest path from " << indexToName[src] << " to " << indexToName[dest] << ":\n";
        for (int i = path.size() - 1; i >= 0; --i) {
            cout << indexToName[path[i]];
            if (i) cout << " -> ";
        }
        cout << "\033[38;2;102;204;255m\n\nTotal distance [M]: " << dist[dest] << endl;
    }

    void listLocations() {
        // Group locations by floor
        unordered_map<string, vector<string>> groups;
        for (auto& name : indexToName) {
            string group = "Other";
            auto it = locationGroup.find(name);
            if (it != locationGroup.end()) group = it->second;
            groups[group].push_back(name);
        }
        // Print by group in order
        vector<string> order = {
            "Ground Floor", "Second Floor", "Third Floor", "Fourth Floor", "Fifth Floor"
        };
        for (const auto& group : order) {
            if (groups.count(group)) {
                // Group header (floor) in orange
                cout << "\033[38;2;255;165;0m\n[" << group << "]\033[0m\n";
                for (auto& name : groups[group])
                    // Location name in light blue
                    cout << "\033[38;2;102;204;255m- " << name << "\033[0m" << endl;
            }
        }
    }
};

int main() {
    Graph g;
    //Ground Floor
    // Main Gate
    g.addEdge("Main Gate", "Clinic", 38.25);

    // Clinic
    g.addEdge("Clinic", "Main Gate", 38.25);
    g.addEdge("Clinic", "Main Admission", 64.41);
    g.addEdge("Clinic", "Waiting Shed", 39.48);

    // Main Admission
    g.addEdge("Main Admission", "Gazebo", 49.03);
    g.addEdge("Main Admission", "Clinic", 64.41);
    g.addEdge("Main Admission", "Waiting Shed", 85.62);

    // Gazebo
    g.addEdge("Gazebo", "Main Admission", 49.03);
    g.addEdge("Gazebo", "Gym", 65.21);
    g.addEdge("Gazebo", "SAO", 124);

    // Gym
    g.addEdge("Gym", "Back Gate", 55.80);
    g.addEdge("Gym", "SAO", 152.48);
    g.addEdge("Gym", "Gazebo", 65.21);

    // Back Gate
    g.addEdge("Back Gate", "Gym", 55.80);

    // Waiting Shed
    g.addEdge("Waiting Shed", "Main Admission", 85.62);
    g.addEdge("Waiting Shed", "Clinic", 39.48);
    g.addEdge("Waiting Shed", "TYK Stairs Left", 19.83);

    // TYK Stairs Left
    g.addEdge("TYK Stairs Left", "TYK Elevator Left", 5.09);
    g.addEdge("TYK Stairs Left", "Waiting Shed", 19.83);
    g.addEdge("TYK Stairs Left", "TYK Stairs Left 2F", 19.812);

    // TYK Elevator Left
    g.addEdge("TYK Elevator Left", "TYK Admission", 35.59);
    g.addEdge("TYK Elevator Left", "TYK Stairs Left", 5.09);
    g.addEdge("TYK Elevator Left", "TYK Elevator Left 2F", 19.812);

    // TYK Admission
    g.addEdge("TYK Admission", "TYK Stairs Mid", 28.42);
    g.addEdge("TYK Admission", "TYK Elevator Left", 35.59);
    g.addEdge("TYK Admission", "Field", 28.42);

    // Field
    g.addEdge("Field", "SAO", 84.08);
    g.addEdge("Field", "LCT Stairs Mid", 155.55);
    g.addEdge("Field", "TYK Stairs Mid", 95.95);
    g.addEdge("Field", "TYK Admission", 28.42);

    // SAO
    g.addEdge("SAO", "Gym", 152.48);
    g.addEdge("SAO", "LCT Stairs Mid", 97.75);
    g.addEdge("SAO", "Gazebo", 124);
    g.addEdge("SAO", "Field", 84.08);

    // LCT Stairs Mid
    g.addEdge("LCT Stairs Mid", "LCT Elevator Mid", 8.30);
    g.addEdge("LCT Stairs Mid", "SAO", 97.75);
    g.addEdge("LCT Stairs Mid", "LCT Stairs Mid 2F", 19.812);
    g.addEdge("LCT Stairs Mid", "Field", 155.55);

    // LCT Elevator Mid
    g.addEdge("LCT Elevator Mid", "EN Stairs Left", 92.09);
    g.addEdge("LCT Elevator Mid", "LCT Stairs Mid", 8.30);
    g.addEdge("LCT Elevator Mid", "LCT Elevator Mid 2F", 19.812);

    // EN Stairs Left
    g.addEdge("EN Stairs Left", "RM103", 23.74);
    g.addEdge("EN Stairs Left", "LCT Elevator Mid", 92.09);
    g.addEdge("EN Stairs Left", "RM100", 53.716);
    g.addEdge("EN Stairs Left", "EN Stairs Left 2F", 19.05);

    // RM103
    g.addEdge("RM103", "EN Stairs Back Left", 9.99);
    g.addEdge("RM103", "EN Stairs Left", 23.74);
    g.addEdge("RM103", "RM100", 46.726);

    // EN Stairs Back Left
    g.addEdge("EN Stairs Back Left", "RM103", 9.99);
    g.addEdge("EN Stairs Back Left", "Parking", 72.755);
    g.addEdge("EN Stairs Back Left", "EN Stairs Back Left 2F", 19.05);

    // Parking
    g.addEdge("Parking", "EN Stairs Back Right", 72.755);
    g.addEdge("Parking", "EN Stairs Back Left", 72.755);

    // TYK Stairs Mid
    g.addEdge("TYK Stairs Mid", "TYK Elevator Mid", 20.24);
    g.addEdge("TYK Stairs Mid", "TYK Admission", 28.42);
    g.addEdge("TYK Stairs Mid", "TYK Stairs Mid 2F", 19.812);
    g.addEdge("TYK Stairs Mid", "Field", 95.95);

    // TYK Elevator Mid
    g.addEdge("TYK Elevator Mid", "Canteen", 32.63);
    g.addEdge("TYK Elevator Mid", "TYK Stairs Mid", 20.24);
    g.addEdge("TYK Elevator Mid", "TYK Elevator Mid 2F", 19.812);

    // Canteen
    g.addEdge("Canteen", "TYK Stairs Right", 18.72);
    g.addEdge("Canteen", "TYK Elevator Mid", 32.63);

    // TYK Stairs Right
    g.addEdge("TYK Stairs Right", "HM Building", 29.62);
    g.addEdge("TYK Stairs Right", "Canteen", 18.72);
    g.addEdge("TYK Stairs Right", "TYK Stairs Right 2F", 22.098);

    // HM Building
    g.addEdge("HM Building", "EN Stairs Right", 55.31);
    g.addEdge("HM Building", "TYK Stairs Right", 29.62);

    // EN Stairs Right
    g.addEdge("EN Stairs Right", "RM117", 23.74);
    g.addEdge("EN Stairs Right", "HM Building", 55.31);
    g.addEdge("EN Stairs Right", "ME Lab", 53.716);
    g.addEdge("EN Stairs Right", "EN Stairs Right 2F", 19.05);

    // RM117
    g.addEdge("RM117", "EN Stairs Back Right", 9.99);
    g.addEdge("RM117", "EN Stairs Right", 23.74);
    g.addEdge("RM117", "ME Lab", 46.726);

    // EN Stairs Back Right
    g.addEdge("EN Stairs Back Right", "RM117", 9.99);
    g.addEdge("EN Stairs Back Right", "Parking", 72.755);
    g.addEdge("EN Stairs Back Right", "EN Stairs Back Right 2F", 19.05);

    // RM100
    g.addEdge("RM100", "EN Admission", 29.346);
    g.addEdge("RM100", "EN Stairs Left", 53.716);
    g.addEdge("RM100", "RM103", 46.726);

    // EN Admission
    g.addEdge("EN Admission", "RM100", 29.346);
    g.addEdge("EN Admission", "CpE Faculty", 29.346);

    // CpE Faculty
    g.addEdge("CpE Faculty", "EN Admission", 29.346);
    g.addEdge("CpE Faculty", "ME Lab", 29.346);

    // ME Lab
    g.addEdge("ME Lab", "CpE Faculty", 29.346);
    g.addEdge("ME Lab", "RM117", 46.726);
    g.addEdge("ME Lab", "EN Stairs Right", 53.716);

    // 2nd Floor
    // TYK Stairs Left 2F
    g.addEdge("TYK Stairs Left 2F", "TYK Elevator Left 2F", 5.09);
    g.addEdge("TYK Stairs Left 2F", "TYK Stairs Left", 19.812);
    g.addEdge("TYK Stairs Left 2F", "TYK Stairs Left 3F", 19.812);

    // TYK Elevator Left 2F
    g.addEdge("TYK Elevator Left 2F", "RM200", 21.633);
    g.addEdge("TYK Elevator Left 2F", "TYK Stairs Left 2F", 5.09);
    g.addEdge("TYK Elevator Left 2F", "TYK Elevator Left", 19.812);
    g.addEdge("TYK Elevator Left 2F", "TYK Elevator Left 3F", 19.812);

    // RM200
    g.addEdge("RM200", "RM201", 21.633);
    g.addEdge("RM200", "TYK Elevator Left 2F", 21.633);

    // RM201
    g.addEdge("RM201", "TYK Stairs Mid 2F", 21.633);
    g.addEdge("RM201", "RM200", 21.633);

    // TYK Stairs Mid 2F
    g.addEdge("TYK Stairs Mid 2F", "TYK Elevator Mid 2F", 20.24);
    g.addEdge("TYK Stairs Mid 2F", "RM201", 21.633);
    g.addEdge("TYK Stairs Mid 2F", "TYK Stairs Mid", 19.812);
    g.addEdge("TYK Stairs Mid 2F", "TYK Stairs Mid 3F", 19.812);

    // TYK Elevator Mid 2F
    g.addEdge("TYK Elevator Mid 2F", "RM203", 14.436);
    g.addEdge("TYK Elevator Mid 2F", "TYK Stairs Mid 2F", 20.24);
    g.addEdge("TYK Elevator Mid 2F", "TYK Elevator Mid", 19.812);
    g.addEdge("TYK Elevator Mid 2F", "TYK Elevator Mid 3F", 19.812);

    // RM203
    g.addEdge("RM203", "RM204", 14.436);
    g.addEdge("RM203", "TYK Elevator Mid 2F", 14.436);

    // RM204
    g.addEdge("RM204", "TYK Stairs Right 2F", 14.436);
    g.addEdge("RM204", "RM203", 14.436);

    // TYK Stairs Right 2F
    g.addEdge("TYK Stairs Right 2F", "RM204", 14.436);
    g.addEdge("TYK Stairs Right 2F", "TYK Stairs Right", 22.098);
    g.addEdge("TYK Stairs Right 2F", "TYK Stairs Right 3F", 22.098);

    // RM205
    g.addEdge("RM205", "RM206", 14.545);

    // RM206
    g.addEdge("RM206", "RM205", 14.545);
    g.addEdge("RM206", "LCT Stairs Mid 2F", 14.545);

    // LCT Stairs Mid 2F
    g.addEdge("LCT Stairs Mid 2F", "RM206", 14.545);
    g.addEdge("LCT Stairs Mid 2F", "LCT Elevator Mid 2F", 8.30);
    g.addEdge("LCT Stairs Mid 2F", "LCT Stairs Mid", 19.812);
    g.addEdge("LCT Stairs Mid 2F", "LCT Stairs Mid 3F", 19.812);

    // LCT Elevator Mid 2F
    g.addEdge("LCT Elevator Mid 2F", "LCT Stairs Mid 2F", 8.30);
    g.addEdge("LCT Elevator Mid 2F", "RM208", 26.945);
    g.addEdge("LCT Elevator Mid 2F", "LCT Elevator Mid", 19.812);
    g.addEdge("LCT Elevator Mid 2F", "LCT Elevator Mid 3F", 19.812);

    // RM208
    g.addEdge("RM208", "LCT Elevator Mid 2F", 26.945);
    g.addEdge("RM208", "RM209", 26.945);

    // RM209
    g.addEdge("RM209", "RM208", 26.945);

    // EN Stairs Left 2F
    g.addEdge("EN Stairs Left 2F", "Chemistry Tool Room", 23.74);
    g.addEdge("EN Stairs Left 2F", "CLR1", 53.716);
    g.addEdge("EN Stairs Left 2F", "EN Stairs Left", 19.05);
    g.addEdge("EN Stairs Left 2F", "EN Stairs Left 3F", 19.05);

    // Chemistry Tool Room
    g.addEdge("Chemistry Tool Room", "EN Stairs Back Left 2F", 9.99);
    g.addEdge("Chemistry Tool Room", "EN Stairs Left 2F", 23.74);
    g.addEdge("Chemistry Tool Room", "CLR1", 46.726);

    // EN Stairs Back Left 2F
    g.addEdge("EN Stairs Back Left 2F", "Chemistry Tool Room", 9.99);
    g.addEdge("EN Stairs Back Left 2F", "EN Stairs Back Left", 19.05);
    g.addEdge("EN Stairs Back Left 2F", "EN Stairs Back Left 3F", 19.05);

    // CLR1
    g.addEdge("CLR1", "CLR2", 29.346);
    g.addEdge("CLR1", "EN Stairs Left 2F", 53.716);
    g.addEdge("CLR1", "Chemistry Tool Room", 46.726);

    // CLR2
    g.addEdge("CLR2", "CLR1", 29.346);
    g.addEdge("CLR2", "ECE1", 29.346);

    // ECE1
    g.addEdge("ECE1", "EEPL1", 29.346);
    g.addEdge("ECE1", "CLR2", 29.346);

    // EEPL1
    g.addEdge("EEPL1", "ECE1", 29.346);
    g.addEdge("EEPL1", "Physics Tool Room", 46.726);
    g.addEdge("EEPL1", "EN Stairs Right 2F", 53.716);

    // EN Stairs Right 2F
    g.addEdge("EN Stairs Right 2F", "Physics Tool Room", 23.74);
    g.addEdge("EN Stairs Right 2F", "EEPL1", 53.716);
    g.addEdge("EN Stairs Right 2F", "EN Stairs Right", 19.05);
    g.addEdge("EN Stairs Right 2F", "EN Stairs Right 3F", 19.05);

    // Physics Tool Room
    g.addEdge("Physics Tool Room", "EN Stairs Back Right 2F", 9.99);
    g.addEdge("Physics Tool Room", "EN Stairs Right 2F", 23.74);
    g.addEdge("Physics Tool Room", "EEPL1", 46.726);

    // EN Stairs Back Right 2F
    g.addEdge("EN Stairs Back Right 2F", "Physics Tool Room", 9.99);
    g.addEdge("EN Stairs Back Right 2F", "EN Stairs Back Right", 19.05);
    g.addEdge("EN Stairs Back Right 2F", "EN Stairs Back Right 3F", 19.05);

    // 3rd Floor
    // TYK Stairs Left 3F
    g.addEdge("TYK Stairs Left 3F", "TYK Elevator Left 3F", 5.09);
    g.addEdge("TYK Stairs Left 3F", "TYK Stairs Left 2F", 19.812);
    g.addEdge("TYK Stairs Left 3F", "TYK Stairs Left 4F", 19.812);

    // TYK Elevator Left 3F
    g.addEdge("TYK Elevator Left 3F", "RM300", 21.633);
    g.addEdge("TYK Elevator Left 3F", "TYK Stairs Left 3F", 5.09);
    g.addEdge("TYK Elevator Left 3F", "TYK Elevator Left 2F", 19.812);
    g.addEdge("TYK Elevator Left 3F", "TYK Elevator Left 4F", 19.812);

    // RM300
    g.addEdge("RM300", "RM301", 21.633);
    g.addEdge("RM300", "TYK Elevator Left 3F", 21.633);

    // RM301
    g.addEdge("RM301", "TYK Stairs Mid 3F", 21.633);
    g.addEdge("RM301", "RM300", 21.633);

    // TYK Stairs Mid 3F
    g.addEdge("TYK Stairs Mid 3F", "TYK Elevator Mid 3F", 20.24);
    g.addEdge("TYK Stairs Mid 3F", "RM301", 21.633);
    g.addEdge("TYK Stairs Mid 3F", "TYK Stairs Mid 2F", 19.812);
    g.addEdge("TYK Stairs Mid 3F", "TYK Stairs Mid 4F", 19.812);

    // TYK Elevator Mid 3F
    g.addEdge("TYK Elevator Mid 3F", "RM303", 14.436);
    g.addEdge("TYK Elevator Mid 3F", "TYK Stairs Mid 3F", 20.24);
    g.addEdge("TYK Elevator Mid 3F", "TYK Elevator Mid 2F", 19.812);
    g.addEdge("TYK Elevator Mid 3F", "TYK Elevator Mid 4F", 19.812);

    // RM303
    g.addEdge("RM303", "RM304", 14.436);
    g.addEdge("RM303", "TYK Elevator Mid 3F", 14.436);

    // RM304
    g.addEdge("RM304", "TYK Stairs Right 3F", 14.436);
    g.addEdge("RM304", "RM303", 14.436);

    // TYK Stairs Right 3F
    g.addEdge("TYK Stairs Right 3F", "RM304", 14.436);
    g.addEdge("TYK Stairs Right 3F", "Bridge", 33.81);
    g.addEdge("TYK Stairs Right 3F", "TYK Stairs Right 2F", 22.098);
    g.addEdge("TYK Stairs Right 3F", "TYK Stairs Right 4F", 22.098);

    // Bridge
    g.addEdge("Bridge", "EN Stairs Right 3F", 43.165);
    g.addEdge("Bridge", "TYK Stairs Right 3F", 33.81);

    // RM305
    g.addEdge("RM305", "RM306", 14.545);

    // RM306
    g.addEdge("RM306", "RM305", 14.545);
    g.addEdge("RM306", "LCT Stairs Mid 3F", 14.545);

    // LCT Stairs Mid 3F
    g.addEdge("LCT Stairs Mid 3F", "RM306", 14.545);
    g.addEdge("LCT Stairs Mid 3F", "LCT Elevator Mid 3F", 8.30);
    g.addEdge("LCT Stairs Mid 3F", "LCT Stairs Mid 2F", 19.812);
    g.addEdge("LCT Stairs Mid 3F", "LCT Stairs Mid 4F", 19.812);

    // LCT Elevator Mid 3F
    g.addEdge("LCT Elevator Mid 3F", "LCT Stairs Mid 3F", 8.30);
    g.addEdge("LCT Elevator Mid 3F", "RM308", 26.945);
    g.addEdge("LCT Elevator Mid 3F", "LCT Elevator Mid 2F", 19.812);
    g.addEdge("LCT Elevator Mid 3F", "LCT Elevator Mid 4F", 19.812);

    // RM308
    g.addEdge("RM308", "LCT Elevator Mid 3F", 26.945);
    g.addEdge("RM308", "RM309", 26.945);

    // RM309
    g.addEdge("RM309", "RM308", 26.945);

    // EN Stairs Left 3F
    g.addEdge("EN Stairs Left 3F", "Stock Room", 23.74);
    g.addEdge("EN Stairs Left 3F", "EN MPH", 53.716);
    g.addEdge("EN Stairs Left 3F", "EN Stairs Left 2F", 19.05);
    g.addEdge("EN Stairs Left 3F", "EN Stairs Left 4F", 19.05);

    // Stock Room
    g.addEdge("Stock Room", "EN Stairs Left 3F", 23.74);
    g.addEdge("Stock Room", "EN Stairs Back Left 3F", 9.99);
    g.addEdge("Stock Room", "EN MPH", 46.726);

    // EN Stairs Back Left 3F
    g.addEdge("EN Stairs Back Left 3F", "Stock Room", 9.99);
    g.addEdge("EN Stairs Back Left 3F", "EN Stairs Back Left 2F", 19.05);
    g.addEdge("EN Stairs Back Left 3F", "EN Stairs Back Left 4F", 19.05);

    // EN MPH
    g.addEdge("EN MPH", "EN Library", 44.02);
    g.addEdge("EN MPH", "EN Stairs Left 3F", 53.716);
    g.addEdge("EN MPH", "Stock Room", 46.726);

    // EN Library
    g.addEdge("EN Library", "EN MPH", 44.02);
    g.addEdge("EN Library", "RM314", 44.02);

    // RM314
    g.addEdge("RM314", "EN Library", 44.02);
    g.addEdge("RM314", "DAWN Room", 46.726);
    g.addEdge("RM314", "EN Stairs Right 3F", 53.716);

    // EN Stairs Right 3F
    g.addEdge("EN Stairs Right 3F", "DAWN Room", 23.74);
    g.addEdge("EN Stairs Right 3F", "Bridge", 43.165);
    g.addEdge("EN Stairs Right 3F", "RM314", 53.716);
    g.addEdge("EN Stairs Right 3F", "EN Stairs Right 2F", 19.05);
    g.addEdge("EN Stairs Right 3F", "EN Stairs Right 4F", 19.05);

    // DAWN Room
    g.addEdge("DAWN Room", "EN Stairs Right 3F", 23.74);
    g.addEdge("DAWN Room", "EN Stairs Back Right 3F", 9.99);
    g.addEdge("DAWN Room", "RM314", 46.726);

    // EN Stairs Back Right 3F
    g.addEdge("EN Stairs Back Right 3F", "DAWN Room", 9.99);
    g.addEdge("EN Stairs Back Right 3F", "EN Stairs Back Right 2F", 19.05);
    g.addEdge("EN Stairs Back Right 3F", "EN Stairs Back Right 4F", 19.05);

    // 4th Floor
    // TYK Stairs Left 4F
    g.addEdge("TYK Stairs Left 4F", "TYK Elevator Left 4F", 5.09);
    g.addEdge("TYK Stairs Left 4F", "TYK Stairs Left 3F", 19.812);
    g.addEdge("TYK Stairs Left 4F", "TYK Stairs Left 5F", 19.812);

    // TYK Elevator Left 4F
    g.addEdge("TYK Elevator Left 4F", "BGC LIBRARY", 32.45);
    g.addEdge("TYK Elevator Left 4F", "TYK Stairs Left 4F", 5.09);
    g.addEdge("TYK Elevator Left 4F", "TYK Elevator Left 3F", 19.812);
    g.addEdge("TYK Elevator Left 4F", "TYK Elevator Left 5F", 19.812);

    // BGC LIBRARY
    g.addEdge("BGC LIBRARY", "TYK Stairs Mid 4F", 32.45);
    g.addEdge("BGC LIBRARY", "TYK Elevator Left 4F", 32.45);

    // TYK Stairs Mid 4F
    g.addEdge("TYK Stairs Mid 4F", "TYK Elevator Mid 4F", 20.24);
    g.addEdge("TYK Stairs Mid 4F", "BGC LIBRARY", 32.45);
    g.addEdge("TYK Stairs Mid 4F", "TYK Stairs Mid 3F", 19.812);
    g.addEdge("TYK Stairs Mid 4F", "TYK Stairs Mid 5F", 19.812);

    // TYK Elevator Mid 4F
    g.addEdge("TYK Elevator Mid 4F", "TYK Stairs Right 4F", 43.41);
    g.addEdge("TYK Elevator Mid 4F", "TYK Stairs Mid 4F", 20.24);
    g.addEdge("TYK Elevator Mid 4F", "TYK Elevator Mid 3F", 19.812);
    g.addEdge("TYK Elevator Mid 4F", "TYK Elevator Mid 5F", 19.812);

    // TYK Stairs Right 4F
    g.addEdge("TYK Stairs Right 4F", "TYK Elevator Mid 4F", 43.41);
    g.addEdge("TYK Stairs Right 4F", "TYK Stairs Right 3F", 22.098);

    // LCT LIBRARY
    g.addEdge("LCT LIBRARY", "LCT Stairs Mid 4F", 14.545);

    // LCT Stairs Mid 4F
    g.addEdge("LCT Stairs Mid 4F", "LCT LIBRARY", 14.545);
    g.addEdge("LCT Stairs Mid 4F", "LCT Elevator Mid 4F", 8.30);
    g.addEdge("LCT Stairs Mid 4F", "LCT Stairs Mid 3F", 19.812);
    g.addEdge("LCT Stairs Mid 4F", "LCT Stairs Mid 5F", 19.812);

    // LCT Elevator Mid 4F
    g.addEdge("LCT Elevator Mid 4F", "LCT Stairs Mid 4F", 8.30);
    g.addEdge("LCT Elevator Mid 4F", "LCT Elevator Mid 3F", 19.812);
    g.addEdge("LCT Elevator Mid 4F", "LCT Elevator Mid 5F", 19.812);

    // EN Stairs Left 4F
    g.addEdge("EN Stairs Left 4F", "DR400", 23.74);
    g.addEdge("EN Stairs Left 4F", "RM400", 53.716);
    g.addEdge("EN Stairs Left 4F", "EN Stairs Left 3F", 19.05);

    // DR400
    g.addEdge("DR400", "EN Stairs Left 4F", 23.74);
    g.addEdge("DR400", "EN Stairs Back Left 4F", 9.99);
    g.addEdge("DR400", "RM400", 46.726);

    // EN Stairs Back Left 4F
    g.addEdge("EN Stairs Back Left 4F", "DR400", 9.99);
    g.addEdge("EN Stairs Back Left 4F", "EN Stairs Back Left 3F", 19.05);

    // RM400
    g.addEdge("RM400", "RM401", 22.01);
    g.addEdge("RM400", "EN Stairs Left 4F", 53.716);
    g.addEdge("RM400", "DR400", 46.726);

    // RM401
    g.addEdge("RM401", "RM402", 22.01);
    g.addEdge("RM401", "RM400", 22.01);

    // RM402
    g.addEdge("RM402", "RM403", 22.01);
    g.addEdge("RM402", "RM401", 22.01);

    // RM403
    g.addEdge("RM403", "RM404", 22.01);
    g.addEdge("RM403", "RM402", 22.01);

    // RM404
    g.addEdge("RM404", "RM403", 22.01);
    g.addEdge("RM404", "DR401", 46.726);
    g.addEdge("RM404", "EN Stairs Right 4F", 53.716);

    // EN Stairs Right 4F
    g.addEdge("EN Stairs Right 4F", "DR401", 23.74);
    g.addEdge("EN Stairs Right 4F", "RM404", 53.716);
    g.addEdge("EN Stairs Right 4F", "EN Stairs Right 3F", 19.05);

    // DR401
    g.addEdge("DR401", "EN Stairs Right 4F", 23.74);
    g.addEdge("DR401", "EN Stairs Back Right 4F", 9.99);
    g.addEdge("DR401", "RM404", 46.726);

    // EN Stairs Back Right 4F
    g.addEdge("EN Stairs Back Right 4F", "DR401", 9.99);
    g.addEdge("EN Stairs Back Right 4F", "EN Stairs Back Right 3F", 19.05);

    // 5th Floor
    // TYK Stairs Left 5F
    g.addEdge("TYK Stairs Left 5F", "TYK Elevator Left 5F", 5.09);
    g.addEdge("TYK Stairs Left 5F", "TYK Stairs Left 4F", 19.812);

    // TYK Elevator Left 5F
    g.addEdge("TYK Elevator Left 5F", "TYK Stairs Mid 5F", 64.90);
    g.addEdge("TYK Elevator Left 5F", "TYK Stairs Left 5F", 5.09);
    g.addEdge("TYK Elevator Left 5F", "TYK Elevator Left 4F", 19.812);

    // TYK Stairs Mid 5F
    g.addEdge("TYK Stairs Mid 5F", "TYK Elevator Mid 5F", 20.24);
    g.addEdge("TYK Stairs Mid 5F", "TYK Elevator Left 5F", 64.90);
    g.addEdge("TYK Stairs Mid 5F", "TYK Stairs Mid 4F", 19.812);

    // TYK Elevator Mid 5F
    g.addEdge("TYK Elevator Mid 5F", "TYK MPH", 14.436);
    g.addEdge("TYK Elevator Mid 5F", "TYK Stairs Mid 5F", 20.24);
    g.addEdge("TYK Elevator Mid 5F", "TYK Elevator Mid 4F", 19.812);

    // TYK MPH
    g.addEdge("TYK MPH", "TYK Elevator Mid 5F", 14.436);

    // LCT Stairs Mid 5F
    g.addEdge("LCT Stairs Mid 5F", "LCT Elevator Mid 5F", 8.30);
    g.addEdge("LCT Stairs Mid 5F", "LCT Stairs Mid 4F", 19.812);

    // LCT Elevator Mid 5F
    g.addEdge("LCT Elevator Mid 5F", "LCT Stairs Mid 5F", 8.30);
    g.addEdge("LCT Elevator Mid 5F", "LCT MPH", 26.945);
    g.addEdge("LCT Elevator Mid 5F", "LCT Elevator Mid 4F", 19.812);

    // LCT MPH
    g.addEdge("LCT MPH", "LCT Elevator Mid 5F", 26.945);

    rpt:
    // --- Display the ASCII Art Banner in light blue ---
    cout << "\033[38;2;102;204;255m"
".--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--.\n"
"/ .. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\\n"
"\\ \\/\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ \\/ /\n"
" \\/ /`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'\\/ / \n"
" / /\\                                                                                / /\\ \n"
"/ /\\ \\                                                                              / /\\ \\\n"
"\\ \\/ /    __  __   ____                                                             \\ \\/ /\n"
" \\/ /    / / / /  / __/                                                              \\/ / \n"
" / /\\   / /_/ /  / _/                                                                / /\\ \n"
"/ /\\ \\  \\____/  /___/                                                               / /\\ \\\n"
"\\ \\/ /                                                                              \\ \\/ /\n"
" \\/ /     _____   ___    __   ____   ____   _____   ___    _  __                     \\/ / \n"
" / /\\    / ___/  / _ |  / /  / __ \\ / __ \\ / ___/  / _ |  / |/ /                     / /\\ \n"
"/ /\\ \\  / /__   / __ | / /__/ /_/ // /_/ // /__   / __ | /    /                     / /\\ \\\n"
"\\ \\/ /  \\___/  /_/ |_|/____/\\____/ \\____/ \\___/  /_/ |_|/_/|_/                      \\ \\/ /\n"
" \\/ /                                                                                \\/ / \n"
" / /\\      _  __   ___   _   __   ____  _____   ___  ______   ____  ____    _  __    / /\\ \n"
"/ /\\ \\    / |/ /  / _ | | | / /  /  _/ / ___/  / _ |/_  __/  /  _/ / __ \\  / |/ /   / /\\ \\\n"
"\\ \\/ /   /    /  / __ | | |/ /  _/ /  / (_ /  / __ | / /    _/ /  / /_/ / /    /    \\ \\/ /\n"
" \\/ /   /_/|_/  /_/ |_| |___/  /___/  \\___/  /_/ |_|/_/    /___/  \\____/ /_/|_/      \\/ / \n"
" / /\\                                                                                / /\\ \n"
"/ /\\ \\     ____ __  __   ____ ______   ____   __  ___                               / /\\ \\\n"
"\\ \\/ /    / __/ \\ \\/ /  / __//_  __/  / __/  /  |/  /                               \\ \\/ /\n"
" \\/ /    _\\ \\    \\  /  _\\ \\   / /    / _/   / /|_/ /                                 \\/ / \n"
" / /\\   /___/    /_/  /___/  /_/    /___/  /_/  /_/                                  / /\\ \n"
"/ /\\ \\                                                                              / /\\ \\\n"
"\\ \\/ /                                                                              \\ \\/ /\n"
" \\/ /                                                                                \\/ / \n"
" / /\\.--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--./ /\\ \n"
"/ /\\ \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\/\\ \\\n"
"\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `' /\n"
" `--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'\n"
"\033[0m";

    // --- Print the orange header for the location list ---
    cout << "\033[38;2;255;165;0m\nLIST OF LOCATIONS BY FLOOR: \033[0m\n";

    // --- Print all locations grouped by floor (each floor in orange, each location in light blue) ---
    g.listLocations();

    // --- Prompt user to enter start and destination locations ---
    cout << "\033[38;2;255;165;0m\nPlease enter your locations as shown above.\n";
    string from, to;
    cout << "\033[38;2;102;204;255m\nEnter start location: ";
    getline(cin, from);
    cout << "\033[38;2;102;204;255m\nEnter destination: ";
    getline(cin, to);

    // --- Ask user for preferred mode of vertical travel (stairs/elevator/any) ---
    string mode;
    cout << "\033[38;2;255;165;0m\nHow do you want to travel between floors?\n";
    cout << "\033[38;2;102;204;255m\n1. Stairs only\n";
    cout << "\033[38;2;102;204;255m\n2. Elevator only\n";
    cout << "\033[38;2;102;204;255m\n3. No preference (I'm taking the ground floor)\n";
    cout << "\033[38;2;102;204;255m\nEnter choice (1/2/3): ";
    getline(cin, mode);

    // --- Set the mode string for pathfinding based on user input ---
    string modeStr = "any";
    if (mode == "1") modeStr = "stairs";
    else if (mode == "2") modeStr = "elevator";
    // else keep as "any"

    // --- Find and display the shortest path using Dijkstra's algorithm ---
    g.dijkstra(from, to, modeStr);

    // --- Ask user if they want to search for another path ---
    char choice;
    cout << "\033[38;2;255;165;0m\nDo you want to find another path? (y/n): ";
    cin >> choice;
    cin.ignore(); // To consume the newline character left in the input buffer
    if (tolower(choice) == 'y') goto rpt; // Repeat if user wants another search

    return 0;
}
