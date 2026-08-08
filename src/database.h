#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <mutex>

// One row in a table: ordered field values. Convention for "users":
//   [0] = id, [1] = name, [2] = age
using Record = std::vector<std::string>;

// Minimal file-backed table store (no SQL).
// On-disk format (data/database.db):
//
//   users:
//   1,John,20
//   2,Alice,21
//
//   other_table:
//   1,foo
//
class Database {
public:
    explicit Database(const std::string& path);

    // Read database.db into memory. Creates an empty file if missing.
    bool load();

    // Write the in-memory tables back to disk.
    bool save();

    // Ensure a named table exists (no-op if it already does).
    void createTable(const std::string& name);

    // Insert a row. Auto-assigns the next numeric id as the first column.
    // fields maps column names (excluding id) to values; for users we use
    // insertion order: name, age.
    // Returns the new id, or -1 on failure.
    int insert(const std::string& table, const std::vector<std::string>& values);

    // Return a copy of every record in the table (empty if table missing).
    std::vector<Record> select(const std::string& table);

private:
    std::string path_;
    // table name → list of records
    std::unordered_map<std::string, std::vector<Record>> tables_;
    // Protects tables_ for concurrent request threads.
    std::mutex mutex_;
};
