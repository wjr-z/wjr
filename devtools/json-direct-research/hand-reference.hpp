#pragma once
namespace hand_reference { class context; }
namespace wjr::json::visitor_detail {
template<> inline constexpr bool parse_is_nothrow<hand_reference::context> = false;
}
namespace hand_reference {
using namespace wjr::json;
class context {
    template<class Parser>
    friend result<void> wjr::json::visitor_detail::parse(Parser&&, ondemand_reader&) noexcept(
        ::wjr::json::visitor_detail::parse_is_nothrow<::wjr::remove_cvref_t<Parser>>);
    enum class kind { records, record, point, samples, path, unknown_object, unknown_array };
    enum class field { unknown, id, sequence, price, enabled, name, position, samples, path, x, y };
    enum class location { root, object, array };
    struct frame { kind type; void* output; field pending = field::unknown; unsigned seen = 0; };
public:
    std::vector<probe::record> output;
    result<void> run(ondemand_reader& reader) { return visitor_detail::parse(*this, reader); }
private:
    bool discarded(location where) const {
        if (where == location::root) return false;
        const auto& parent = frames.back();
        return parent.type == kind::unknown_object || parent.type == kind::unknown_array ||
               (where == location::object && parent.pending == field::unknown);
    }
    result<void> key(const char* first, const char* last) {
        auto& current = frames.back();
        current.pending = field::unknown;
        if (current.type == kind::unknown_object) return detail::check_string(first,last);
        auto parsed = detail::decode_typed_string(key_text,first,last);
        if (!parsed) return parsed;
        unsigned bit = 0;
        if (current.type == kind::record) {
            if (key_text == "id") { current.pending=field::id; bit=1; }
            else if (key_text == "sequence") { current.pending=field::sequence; bit=2; }
            else if (key_text == "price") { current.pending=field::price; bit=4; }
            else if (key_text == "enabled") { current.pending=field::enabled; bit=8; }
            else if (key_text == "name") { current.pending=field::name; bit=16; }
            else if (key_text == "position") { current.pending=field::position; bit=32; }
            else if (key_text == "samples") { current.pending=field::samples; bit=64; }
            else if (key_text == "path") { current.pending=field::path; bit=128; }
        } else {
            if (key_text == "x") { current.pending=field::x; bit=1; }
            else if (key_text == "y") { current.pending=field::y; bit=2; }
        }
        current.seen |= bit;
        return {};
    }
    result<void> null(location where,const char* first) {
        auto checked=detail::check_null(first);
        if (!checked) return checked;
        return discarded(where) ? result<void>{} : wjr::unexpected(error_code::INCORRECT_TYPE);
    }
    result<void> boolean(location where,const char* first,bool value) {
        auto checked=value ? detail::check_true(first) : detail::check_false(first);
        if (!checked || discarded(where)) return checked;
        if (where==location::object && frames.back().type==kind::record && frames.back().pending==field::enabled) {
            static_cast<probe::record*>(frames.back().output)->enabled=value;
            return {};
        }
        return wjr::unexpected(error_code::INCORRECT_TYPE);
    }
    result<void> number(location where,const char* first,const char* last) {
        if (where==location::object) {
            auto& current=frames.back();
            if (current.type==kind::record && current.pending==field::price)
                return detail::parse_typed_float<double>(&static_cast<probe::record*>(current.output)->price,first,last);
            if (current.type==kind::point && current.pending==field::x)
                return detail::parse_typed_float<double>(&static_cast<probe::point*>(current.output)->x,first,last);
            if (current.type==kind::point && current.pending==field::y)
                return detail::parse_typed_float<double>(&static_cast<probe::point*>(current.output)->y,first,last);
        }
        basic_value value(wjr::default_construct);
        auto parsed=detail::parse_number(first,last,value);
        if (!parsed || discarded(where)) return parsed;
        if (value.m_type==value_t::number_float) {
            const bool fraction=std::find_if(first,last,[](char ch) {return ch=='.'||ch=='e'||ch=='E';})!=last;
            return wjr::unexpected(fraction ? error_code::INCORRECT_TYPE : error_code::NUMBER_OUT_OF_RANGE);
        }
        if (where==location::object) {
            auto& current=frames.back();
            if (current.type==kind::record && current.pending==field::id)
                return detail::assign_typed_number<uint64_t>(&static_cast<probe::record*>(current.output)->id,value);
            if (current.type==kind::record && current.pending==field::sequence)
                return detail::assign_typed_number<int64_t>(&static_cast<probe::record*>(current.output)->sequence,value);
        } else if (where==location::array && frames.back().type==kind::samples) {
            int64_t integer=0;
            auto assigned=detail::assign_typed_number<int64_t>(&integer,value);
            if (assigned) static_cast<std::vector<int64_t>*>(frames.back().output)->emplace_back(integer);
            return assigned;
        }
        return wjr::unexpected(error_code::INCORRECT_TYPE);
    }
    result<void> string(location where,const char* first,const char* last) {
        if (discarded(where)) return detail::check_string(first,last);
        if (where==location::object && frames.back().type==kind::record && frames.back().pending==field::name)
            return detail::decode_typed_string(static_cast<probe::record*>(frames.back().output)->name,first,last);
        return wjr::unexpected(error_code::INCORRECT_TYPE);
    }
    result<void> begin(location where,bool object) {
        if (discarded(where)) {
            frames.emplace_back(frame{object ? kind::unknown_object : kind::unknown_array,nullptr});
            return {};
        }
        if (where==location::root) {
            if (object) return wjr::unexpected(error_code::INCORRECT_TYPE);
            output.clear();
            frames.emplace_back(frame{kind::records,&output});
            return {};
        }
        auto& current=frames.back();
        if (where==location::array && object) {
            if (current.type==kind::records) {
                auto& child=static_cast<std::vector<probe::record>*>(current.output)->emplace_back();
                frames.emplace_back(frame{kind::record,&child});
                return {};
            }
            if (current.type==kind::path) {
                auto& child=static_cast<std::vector<probe::point>*>(current.output)->emplace_back();
                frames.emplace_back(frame{kind::point,&child});
                return {};
            }
        }
        if (where==location::object && current.type==kind::record) {
            auto& record=*static_cast<probe::record*>(current.output);
            if (current.pending==field::position && object) {
                frames.emplace_back(frame{kind::point,&record.position});
                return {};
            }
            if (current.pending==field::samples && !object) {
                record.samples.clear();
                frames.emplace_back(frame{kind::samples,&record.samples});
                return {};
            }
            if (current.pending==field::path && !object) {
                record.path.clear();
                frames.emplace_back(frame{kind::path,&record.path});
                return {};
            }
        }
        return wjr::unexpected(error_code::INCORRECT_TYPE);
    }
    result<void> end() {
        auto& current=frames.back();
        if ((current.type==kind::record && current.seen!=255) || (current.type==kind::point && current.seen!=3))
            return wjr::unexpected(error_code::NO_SUCH_FIELD);
        frames.pop_back();
        return {};
    }
    result<void> visit_object_key_string(const char* first,const char* last) { return key(first,last); }
#define HAND_CALLBACKS(place) \
    result<void> visit_##place##_null(const char* first) {return null(location::place,first);} \
    result<void> visit_##place##_true(const char* first) {return boolean(location::place,first,true);} \
    result<void> visit_##place##_false(const char* first) {return boolean(location::place,first,false);} \
    result<void> visit_##place##_number(const char* first,const char* last) {return number(location::place,first,last);} \
    result<void> visit_##place##_string(const char* first,const char* last) {return string(location::place,first,last);} \
    result<void> visit_##place##_start_object(uint32_t) {return begin(location::place,true);} \
    result<void> visit_##place##_start_array(uint32_t) {return begin(location::place,false);}
    HAND_CALLBACKS(root)
    HAND_CALLBACKS(object)
    HAND_CALLBACKS(array)
#undef HAND_CALLBACKS
#define HAND_END(container) \
    result<void> visit_end_##container##_to_root(uint32_t) {return end();} \
    result<void> visit_end_##container##_to_object(uint32_t) {return end();} \
    result<void> visit_end_##container##_to_array(uint32_t) {return end();}
    HAND_END(object)
    HAND_END(array)
#undef HAND_END
    wjr::small_vector<frame,16> frames;
    std::string key_text;
};
}
namespace strong_reference {
struct parser {
    wjr::json::ondemand_reader reader;
    std::string_view input;
    explicit parser(std::string_view text):input(text) { (void)run(); }
    uint64_t run() {
        reader.read(input);
        hand_reference::context parsed;
        if (!parsed.run(reader)) throw std::runtime_error("hand-written WJR parse failure");
        return probe::checksum(parsed.output);
    }
};
}
