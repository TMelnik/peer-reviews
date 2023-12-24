//---------------Функция 1 (LoadPerson.cpp):------------------
// Comments
// Используем auto вместо явного указания типов для улучшения читаемости
vector<Person> LoadPersons(string_view db_name, int db_connection_timeout, bool db_allow_exceptions,
                           DBLogLevel db_log_level, int min_age, int max_age, string_view name_filter) {
    // Используем auto для уменьшения дублирования кода
    auto db = DBConnector(db_allow_exceptions, db_log_level)
                  .Connect(db_name.starts_with("tmp."s) ? "Tmp."s + db_name : db_name, db_connection_timeout);

    // Используем std::ostringstream вместо ostringstream
    std::ostringstream query_str;
    query_str << "from Persons "s
              << "select Name, Age "s
              << "where Age between "s << min_age << " and "s << max_age << " "s
              << "and Name like '%"s << DBHandler::Quote(name_filter) << "%'"s;
    DBQuery query(query_str.str());

    // Используем auto в цикле для улучшения читаемости
    vector<Person> persons;
    for (const auto& [name, age] : db.LoadRows<string, int>(query)) {
        persons.push_back({name, age});
    }
    return persons;
}


//---------------Функция 2 (CheckDateTimeValidity.cpp):-----------------
void CheckDateTimeValidity(const DateTime& dt) {
    // Извлекаем логику проверки в отдельную lambda-функцию
    auto check_range = [](int value, int min_val, int max_val, const string& field_name) {
        if (value < min_val || value > max_val) {
            throw domain_error(field_name + " is out of range");
        }
    };

    // Применяем lambda-функцию для каждой проверки
    check_range(dt.year, 1, 9999, "year");
    check_range(dt.month, 1, 12, "month");

    const bool is_leap_year = (dt.year % 4 == 0) && !(dt.year % 100 == 0 && dt.year % 400 != 0);
    const array<int, 12> month_lengths = {31, 28 + is_leap_year, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    check_range(dt.day, 1, month_lengths[dt.month - 1], "day");
    check_range(dt.hour, 0, 23, "hour");
    check_range(dt.minute, 0, 59, "minute");
    check_range(dt.second, 0, 59, "second");
}
//------------------Функция 3 (ParseCitySubjson.cpp):----------------------
void ParseCitySubjson(vector<City>& cities, const Json& json, const string& country_name,
                      const string& country_iso_code, const string& country_phone_code, const string& country_time_zone,
                      const vector<Language>& languages) {
    // Заменяем AsString на метод As<std::string>() для явной конвертации типа
    for (const auto& city_json : json.AsList()) {
        const auto& city_obj = city_json.AsObject();
        cities.push_back({
            city_obj["name"s].As<std::string>(),
            city_obj["iso_code"s].As<std::string>(),
            country_phone_code + city_obj["phone_code"s].As<std::string>(),
            country_name,
            country_iso_code,
            country_time_zone,
            languages
        });
    }
}

void ParseCountryJson(vector<Country>& countries, vector<City>& cities, const Json& json) {
    for (const auto& country_json : json.AsList()) {
        const auto& country_obj = country_json.AsObject();
        countries.push_back({
            country_obj["name"s].As<std::string>(),
            country_obj["iso_code"s].As<std::string>(),
            country_obj["phone_code"s].As<std::string>(),
            country_obj["time_zone"s].As<std::string>(),
        });
        Country& country = countries.back();
        for (const auto& lang_obj : country_obj["languages"s].AsList()) {
            // Заменяем FromString на метод As<Language>() для явной конвертации типа
            country.languages.push_back(lang_obj.As<Language>());
        }
        ParseCitySubjson(cities, country_obj["cities"s], country.name, country.iso_code, country.phone_code,
                         country.time_zone, country.languages);
    }
}