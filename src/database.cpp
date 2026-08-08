#include "database.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <sstream>


Database::Database(const std::string& path) : path_(path) {}

bool Database::load() {
    std::lock_guard<std::mutex> lock(mutex_);
    tables_.clear();

    std::ifstream in(path_);
    if (!in.is_open()) {
        // First run: start empty; save() will create the file later.
        std::cout << "[db] No existing database at " << path_
                  << " — starting empty.\n";
        return true;
    }

    std::string line;
    std::string currentTable;
    while (std::getline(in, line)) {
        // Drop Windows-style trailing CR if present.
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        // Table header lines end with ':'.
        if (line.back() == ':') {
            currentTable = line.substr(0, line.size() - 1);
            tables_[currentTable]; // ensure entry exists
            continue;
        }

        if (currentTable.empty()) continue;

        tables_[currentTable].push_back(split(line, ','));
    }

    std::cout << "[db] Loaded " << tables_.size() << " table(s) from "
              << path_ << "\n";
    return true;
}

bool Database::save() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream out(path_, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "[db] Failed to open " << path_ << " for writing.\n";
        return false;
    }

    for (const auto& [name, records] : tables_) {
        out << name << ":\n";
        for (const auto& rec : records) {
            for (size_t i = 0; i < rec.size(); ++i) {
                if (i) out << ',';
                out << rec[i];
            }
            out << '\n';
        }
        out << '\n';
    }
    return true;
}

void Database::createTable(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    tables_[name]; // default-construct empty vector if missing
}

int Database::insert(const std::string& table, const std::vector<std::string>& values) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto& records = tables_[table];

    // Next id = max existing id + 1 (or 1 if empty).
    int nextId = 1;
    for (const auto& rec : records) {
        if (!rec.empty()) {
            try {
                int id = std::stoi(rec[0]);
                if (id >= nextId) nextId = id + 1;
            } catch (...) {
                // Ignore non-numeric ids.
            }
        }
    }

    Record row;
    row.push_back(std::to_string(nextId));
    for (const auto& v : values) {
        row.push_back(v);
    }
    records.push_back(row);

    // Persist immediately so data survives a crash/restart.
    // Note: save() below needs unlocked path — write inline to avoid deadlock
    // since we already hold mutex_.
    std::ofstream out(path_, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "[db] Failed to persist after insert.\n";
        return -1;
    }
    for (const auto& [name, recs] : tables_) {
        out << name << ":\n";
        for (const auto& rec : recs) {
            for (size_t i = 0; i < rec.size(); ++i) {
                if (i) out << ',';
                out << rec[i];
            }
            out << '\n';
        }
        out << '\n';
    }

    return nextId;
}

std::vector<Record> Database::select(const std::string& table) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tables_.find(table);
    if (it == tables_.end()) {
        return {};
    }
    return it->second;
}
