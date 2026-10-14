#pragma once // Prevents the header from being included multiple times

namespace DatabaseSchema {

    inline const char* seniority =
        "CREATE TABLE IF NOT EXISTS seniority ("
        "id   INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL);";

    inline const char* engineers =
        "CREATE TABLE IF NOT EXISTS engineers ("
        "id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name      TEXT NOT NULL,"
        "seniority INTEGER NOT NULL,"
        "FOREIGN KEY (seniority) REFERENCES seniority(id));";

    inline const char* equipment =
        "CREATE TABLE IF NOT EXISTS equipment ("
        "id                   INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name                 TEXT NOT NULL,"
        "seniorityRequirement INTEGER NOT NULL,"
        "FOREIGN KEY (seniorityRequirement) REFERENCES seniority(id));";

    inline const char* missions =
        "CREATE TABLE IF NOT EXISTS missions ("
        "id                   INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name                 TEXT NOT NULL,"
        "equipmentId          INTEGER NOT NULL,"
        "seniorityRequirement INTEGER NOT NULL,"
        "FOREIGN KEY (equipmentId) REFERENCES equipment(id),"
        "FOREIGN KEY (seniorityRequirement) REFERENCES seniority(id));";

    inline const char* assignedEngineers =
        "CREATE TABLE IF NOT EXISTS assigned_engineers ("
        "engineerId INTEGER NOT NULL,"
        "missionId  INTEGER NOT NULL,"
        "PRIMARY KEY (engineerId, missionId),"
        "FOREIGN KEY (engineerId) REFERENCES engineers(id),"
        "FOREIGN KEY (missionId) REFERENCES missions(id));";

} // namespace DatabaseSchema
