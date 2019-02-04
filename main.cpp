#include <iostream>
#include <queue>

#include <jsoncpp/json/json.h>
#include <sqlite3.h>

struct sm1pro_data_t final
{
    std::string iso_date;
    std::string sensor_name;
    std::string sensor_type;
    std::string data_type;
    double value;
    std::string value_raw;
    std::string full_scale;

public:
    explicit sm1pro_data_t(Json::Value const &json_value) :
            iso_date(json_value["iso_date"].asString()), sensor_name(json_value["sensor_name"].asString()),
            sensor_type(json_value["sensor_type"].asString()), data_type(json_value["data_type"].asString()),
            value(json_value["value"].asDouble()), value_raw(json_value["value_raw"].asString()),
            full_scale(json_value["full_scale"].asString())
    {

    }

    Json::Value toJsonValue() const
    {
        Json::Value jsonValue;
        jsonValue["iso_date"] = iso_date;
        jsonValue["sensor_name"] = sensor_name;
        jsonValue["sensor_type"] = sensor_type;
        jsonValue["data_type"] = data_type;
        jsonValue["value"] = value;
        jsonValue["value_raw"] = value_raw;
        jsonValue["full_scale"] = full_scale;
        return jsonValue;
    }
};

template<typename T>
class sm1pro_t final
{
    std::queue<T> __queue;
public:
    sm1pro_t() :
            __queue(std::queue<T>())
    {};

    void push(T const &t)
    { __queue.push(t); }

    void emplace(T &&t)
    { __queue.emplace(std::forward<T>(t)); }

    const T &front()
    { return __queue.front(); }

    void pop()
    { __queue.pop(); }

    size_t size()
    { return __queue.size(); }

    bool empty()
    { return __queue.empty(); }
};

int main()
{
    const auto filename = "2019-01-01.sqlite";
    sqlite3 *persistentDB; // needs mutex
    sqlite3_stmt *stmt;

    std::remove(filename);
    auto rc = sqlite3_open(filename, &persistentDB);
    if (rc)
    {
        exit(rc);
    }
    auto create_query = R"(
    CREATE TABLE IF NOT EXISTS sm1pro_data
    (
        iso_date TEXT NOT NULL,
        sensor_name TEXT NOT NULL,
        sensor_type TEXT NOT NULL,
        data_type TEXT NOT NULL,
        value REAL NOT NULL,
        value_raw TEXT NOT NULL,
        full_scale TEXT NOT NULL
))";

    sqlite3_prepare_v2(persistentDB, create_query, -1, &stmt, nullptr);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_OK && rc != SQLITE_DONE)
    {
        exit(rc);
    }

    Json::Value jsonValue;
    jsonValue["iso_date"] = "2019-01-01T00:00:00.123Z";
    jsonValue["sensor_name"] = "01";
    jsonValue["sensor_type"] = "SEN";
    jsonValue["data_type"] = "pressure";
    jsonValue["value"] = 33.3f;
    jsonValue["value_raw"] = Json::nullValue;
    jsonValue["full_scale"] = Json::nullValue;
    sm1pro_data_t sm1pro_data(jsonValue);

    sm1pro_t<sm1pro_data_t> sm1pro;

    for (auto it = 0u; it < 3600; ++it)
    {
        sm1pro.push(sm1pro_data);
    }
    while (!sm1pro.empty())
    {
        std::cout << sm1pro.front().toJsonValue().toStyledString() << std::endl;
        auto insert_data = R"(INSERT INTO sm1pro_data VALUES (?, ?, ?, ?, ?, ?, ?))";
        sqlite3_prepare_v2(persistentDB, insert_data, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, sm1pro.front().iso_date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, sm1pro.front().sensor_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, sm1pro.front().sensor_type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, sm1pro.front().data_type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 5, sm1pro.front().value);
        sqlite3_bind_text(stmt, 6, sm1pro.front().value_raw.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, sm1pro.front().full_scale.c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_OK && rc != SQLITE_DONE)
        {
            auto err = const_cast<char *>(sqlite3_errmsg(persistentDB));
            std::cerr << err << std::endl;
            exit(rc);
        }
        sm1pro.pop();
    }

    return 0;
}