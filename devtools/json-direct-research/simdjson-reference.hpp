#pragma once
#include "simdjson.hpp"
namespace strong_reference {
namespace od = simdjson::ondemand;
inline void validate(od::value value, unsigned depth = 0) {
    if (depth >= 256) throw std::runtime_error("depth");
    switch (value.type().value()) {
    case od::json_type::array:
        for (auto child : value.get_array()) validate(child.value(), depth + 1);
        return;
    case od::json_type::object:
        for (auto field : value.get_object()) {
            auto key = field.unescaped_key().value();
            (void)key;
            validate(field.value(), depth + 1);
        }
        return;
    case od::json_type::number: (void)value.get_number().value(); return;
    case od::json_type::string: (void)value.get_string().value(); return;
    case od::json_type::boolean: (void)value.get_bool().value(); return;
    case od::json_type::null: if (!value.is_null().value()) throw std::runtime_error("null"); return;
    }
    throw std::runtime_error("type");
}
inline probe::point point(od::value value) {
    probe::point output;
    unsigned seen = 0;
    for (auto field : value.get_object()) {
        auto key = field.unescaped_key().value();
        if (key == "x") { output.x = field.value().get_double().value(); seen |= 1; }
        else if (key == "y") { output.y = field.value().get_double().value(); seen |= 2; }
        else validate(field.value());
    }
    if (seen != 3) throw std::runtime_error("required point field");
    return output;
}
inline probe::record record(od::value value) {
    probe::record output;
    unsigned seen = 0;
    for (auto field : value.get_object()) {
        auto key = field.unescaped_key().value();
        if (key == "id") { output.id = field.value().get_uint64().value(); seen |= 1; }
        else if (key == "sequence") { output.sequence = field.value().get_int64().value(); seen |= 2; }
        else if (key == "price") { output.price = field.value().get_double().value(); seen |= 4; }
        else if (key == "enabled") { output.enabled = field.value().get_bool().value(); seen |= 8; }
        else if (key == "name") { output.name = field.value().get_string().value(); seen |= 16; }
        else if (key == "position") { output.position = point(field.value()); seen |= 32; }
        else if (key == "samples") {
            output.samples.clear();
            for (auto child : field.value().get_array()) output.samples.emplace_back(child.get_int64().value());
            seen |= 64;
        } else if (key == "path") {
            output.path.clear();
            for (auto child : field.value().get_array()) output.path.emplace_back(point(child.value()));
            seen |= 128;
        } else validate(field.value());
    }
    if (seen != 255) throw std::runtime_error("required record field");
    return output;
}
struct parser {
    od::parser reader;
    simdjson::padded_string input;
    explicit parser(std::string_view text) : input(text) {
        (void)run();
        static bool report = true;
        if (report) {
            std::cerr << "simdjson_builtin=" << simdjson::builtin_implementation()->name() << ", active=" << simdjson::get_active_implementation()->name() << '\n';
            report = false;
        }
    }
    uint64_t run() {
        auto document = reader.iterate(input);
        std::vector<probe::record> output;
        for (auto child : document.get_array()) output.emplace_back(record(child.value()));
        if (!document.at_end()) throw std::runtime_error("trailing input");
        return probe::checksum(output);
    }
};
}
