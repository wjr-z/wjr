#pragma once
#include <bit>
#include <stdexcept>
#include <string>
#include <vector>
#include <wjr/json/document.hpp>
#include <wjr/json/inplace_parser.hpp>
namespace probe {
using namespace wjr::json;
struct numeric8 {
 WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(numeric8);
 WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(numeric8, f00,f01,f02,f03,f04,f05,f06,f07)
 uint64_t f00=0;
 uint64_t f01=0;
 uint64_t f02=0;
 uint64_t f03=0;
 uint64_t f04=0;
 uint64_t f05=0;
 uint64_t f06=0;
 uint64_t f07=0;
};
struct numeric32 {
 WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(numeric32);
 WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(numeric32, f00,f01,f02,f03,f04,f05,f06,f07,f08,f09,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,f29,f30,f31)
 uint64_t f00=0;
 uint64_t f01=0;
 uint64_t f02=0;
 uint64_t f03=0;
 uint64_t f04=0;
 uint64_t f05=0;
 uint64_t f06=0;
 uint64_t f07=0;
 uint64_t f08=0;
 uint64_t f09=0;
 uint64_t f10=0;
 uint64_t f11=0;
 uint64_t f12=0;
 uint64_t f13=0;
 uint64_t f14=0;
 uint64_t f15=0;
 uint64_t f16=0;
 uint64_t f17=0;
 uint64_t f18=0;
 uint64_t f19=0;
 uint64_t f20=0;
 uint64_t f21=0;
 uint64_t f22=0;
 uint64_t f23=0;
 uint64_t f24=0;
 uint64_t f25=0;
 uint64_t f26=0;
 uint64_t f27=0;
 uint64_t f28=0;
 uint64_t f29=0;
 uint64_t f30=0;
 uint64_t f31=0;
};
struct numeric64 {
 WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(numeric64);
 template<class Owner=numeric64> static constexpr auto wjr_json_fields() {
  return std::tuple{field_descriptor<&Owner::f00>{"f00"},field_descriptor<&Owner::f01>{"f01"},field_descriptor<&Owner::f02>{"f02"},field_descriptor<&Owner::f03>{"f03"},field_descriptor<&Owner::f04>{"f04"},field_descriptor<&Owner::f05>{"f05"},field_descriptor<&Owner::f06>{"f06"},field_descriptor<&Owner::f07>{"f07"},field_descriptor<&Owner::f08>{"f08"},field_descriptor<&Owner::f09>{"f09"},field_descriptor<&Owner::f10>{"f10"},field_descriptor<&Owner::f11>{"f11"},field_descriptor<&Owner::f12>{"f12"},field_descriptor<&Owner::f13>{"f13"},field_descriptor<&Owner::f14>{"f14"},field_descriptor<&Owner::f15>{"f15"},field_descriptor<&Owner::f16>{"f16"},field_descriptor<&Owner::f17>{"f17"},field_descriptor<&Owner::f18>{"f18"},field_descriptor<&Owner::f19>{"f19"},field_descriptor<&Owner::f20>{"f20"},field_descriptor<&Owner::f21>{"f21"},field_descriptor<&Owner::f22>{"f22"},field_descriptor<&Owner::f23>{"f23"},field_descriptor<&Owner::f24>{"f24"},field_descriptor<&Owner::f25>{"f25"},field_descriptor<&Owner::f26>{"f26"},field_descriptor<&Owner::f27>{"f27"},field_descriptor<&Owner::f28>{"f28"},field_descriptor<&Owner::f29>{"f29"},field_descriptor<&Owner::f30>{"f30"},field_descriptor<&Owner::f31>{"f31"},field_descriptor<&Owner::f32>{"f32"},field_descriptor<&Owner::f33>{"f33"},field_descriptor<&Owner::f34>{"f34"},field_descriptor<&Owner::f35>{"f35"},field_descriptor<&Owner::f36>{"f36"},field_descriptor<&Owner::f37>{"f37"},field_descriptor<&Owner::f38>{"f38"},field_descriptor<&Owner::f39>{"f39"},field_descriptor<&Owner::f40>{"f40"},field_descriptor<&Owner::f41>{"f41"},field_descriptor<&Owner::f42>{"f42"},field_descriptor<&Owner::f43>{"f43"},field_descriptor<&Owner::f44>{"f44"},field_descriptor<&Owner::f45>{"f45"},field_descriptor<&Owner::f46>{"f46"},field_descriptor<&Owner::f47>{"f47"},field_descriptor<&Owner::f48>{"f48"},field_descriptor<&Owner::f49>{"f49"},field_descriptor<&Owner::f50>{"f50"},field_descriptor<&Owner::f51>{"f51"},field_descriptor<&Owner::f52>{"f52"},field_descriptor<&Owner::f53>{"f53"},field_descriptor<&Owner::f54>{"f54"},field_descriptor<&Owner::f55>{"f55"},field_descriptor<&Owner::f56>{"f56"},field_descriptor<&Owner::f57>{"f57"},field_descriptor<&Owner::f58>{"f58"},field_descriptor<&Owner::f59>{"f59"},field_descriptor<&Owner::f60>{"f60"},field_descriptor<&Owner::f61>{"f61"},field_descriptor<&Owner::f62>{"f62"},field_descriptor<&Owner::f63>{"f63"}};
 }
 template<class Document> explicit numeric64(Document&& document,in_place_document_serializer_from_t)
  : numeric64(document.template get<object_t>(), in_place_document_serializer_object_copy_t{}) {}
 template<class Object> explicit numeric64(const Object& object,in_place_document_serializer_object_copy_t)
  : f00(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f00"))),f01(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f01"))),f02(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f02"))),f03(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f03"))),f04(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f04"))),f05(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f05"))),f06(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f06"))),f07(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f07"))),f08(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f08"))),f09(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f09"))),f10(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f10"))),f11(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f11"))),f12(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f12"))),f13(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f13"))),f14(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f14"))),f15(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f15"))),f16(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f16"))),f17(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f17"))),f18(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f18"))),f19(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f19"))),f20(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f20"))),f21(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f21"))),f22(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f22"))),f23(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f23"))),f24(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f24"))),f25(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f25"))),f26(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f26"))),f27(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f27"))),f28(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f28"))),f29(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f29"))),f30(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f30"))),f31(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f31"))),f32(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f32"))),f33(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f33"))),f34(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f34"))),f35(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f35"))),f36(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f36"))),f37(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f37"))),f38(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f38"))),f39(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f39"))),f40(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f40"))),f41(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f41"))),f42(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f42"))),f43(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f43"))),f44(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f44"))),f45(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f45"))),f46(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f46"))),f47(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f47"))),f48(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f48"))),f49(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f49"))),f50(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f50"))),f51(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f51"))),f52(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f52"))),f53(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f53"))),f54(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f54"))),f55(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f55"))),f56(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f56"))),f57(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f57"))),f58(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f58"))),f59(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f59"))),f60(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f60"))),f61(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f61"))),f62(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f62"))),f63(static_cast<uint64_t>(wjr::json::detail::document_field_at(object,"f63"))) {}
 template<class Document> static Document construct(const numeric64& value,in_place_document_serializer_to_t) {
  Document document=Document::_construct_object();
  auto& object=document.template get_unsafe<object_t>();
 object.emplace("f00",value.f00);
 object.emplace("f01",value.f01);
 object.emplace("f02",value.f02);
 object.emplace("f03",value.f03);
 object.emplace("f04",value.f04);
 object.emplace("f05",value.f05);
 object.emplace("f06",value.f06);
 object.emplace("f07",value.f07);
 object.emplace("f08",value.f08);
 object.emplace("f09",value.f09);
 object.emplace("f10",value.f10);
 object.emplace("f11",value.f11);
 object.emplace("f12",value.f12);
 object.emplace("f13",value.f13);
 object.emplace("f14",value.f14);
 object.emplace("f15",value.f15);
 object.emplace("f16",value.f16);
 object.emplace("f17",value.f17);
 object.emplace("f18",value.f18);
 object.emplace("f19",value.f19);
 object.emplace("f20",value.f20);
 object.emplace("f21",value.f21);
 object.emplace("f22",value.f22);
 object.emplace("f23",value.f23);
 object.emplace("f24",value.f24);
 object.emplace("f25",value.f25);
 object.emplace("f26",value.f26);
 object.emplace("f27",value.f27);
 object.emplace("f28",value.f28);
 object.emplace("f29",value.f29);
 object.emplace("f30",value.f30);
 object.emplace("f31",value.f31);
 object.emplace("f32",value.f32);
 object.emplace("f33",value.f33);
 object.emplace("f34",value.f34);
 object.emplace("f35",value.f35);
 object.emplace("f36",value.f36);
 object.emplace("f37",value.f37);
 object.emplace("f38",value.f38);
 object.emplace("f39",value.f39);
 object.emplace("f40",value.f40);
 object.emplace("f41",value.f41);
 object.emplace("f42",value.f42);
 object.emplace("f43",value.f43);
 object.emplace("f44",value.f44);
 object.emplace("f45",value.f45);
 object.emplace("f46",value.f46);
 object.emplace("f47",value.f47);
 object.emplace("f48",value.f48);
 object.emplace("f49",value.f49);
 object.emplace("f50",value.f50);
 object.emplace("f51",value.f51);
 object.emplace("f52",value.f52);
 object.emplace("f53",value.f53);
 object.emplace("f54",value.f54);
 object.emplace("f55",value.f55);
 object.emplace("f56",value.f56);
 object.emplace("f57",value.f57);
 object.emplace("f58",value.f58);
 object.emplace("f59",value.f59);
 object.emplace("f60",value.f60);
 object.emplace("f61",value.f61);
 object.emplace("f62",value.f62);
 object.emplace("f63",value.f63);
  return document;
 }

 uint64_t f00=0;
 uint64_t f01=0;
 uint64_t f02=0;
 uint64_t f03=0;
 uint64_t f04=0;
 uint64_t f05=0;
 uint64_t f06=0;
 uint64_t f07=0;
 uint64_t f08=0;
 uint64_t f09=0;
 uint64_t f10=0;
 uint64_t f11=0;
 uint64_t f12=0;
 uint64_t f13=0;
 uint64_t f14=0;
 uint64_t f15=0;
 uint64_t f16=0;
 uint64_t f17=0;
 uint64_t f18=0;
 uint64_t f19=0;
 uint64_t f20=0;
 uint64_t f21=0;
 uint64_t f22=0;
 uint64_t f23=0;
 uint64_t f24=0;
 uint64_t f25=0;
 uint64_t f26=0;
 uint64_t f27=0;
 uint64_t f28=0;
 uint64_t f29=0;
 uint64_t f30=0;
 uint64_t f31=0;
 uint64_t f32=0;
 uint64_t f33=0;
 uint64_t f34=0;
 uint64_t f35=0;
 uint64_t f36=0;
 uint64_t f37=0;
 uint64_t f38=0;
 uint64_t f39=0;
 uint64_t f40=0;
 uint64_t f41=0;
 uint64_t f42=0;
 uint64_t f43=0;
 uint64_t f44=0;
 uint64_t f45=0;
 uint64_t f46=0;
 uint64_t f47=0;
 uint64_t f48=0;
 uint64_t f49=0;
 uint64_t f50=0;
 uint64_t f51=0;
 uint64_t f52=0;
 uint64_t f53=0;
 uint64_t f54=0;
 uint64_t f55=0;
 uint64_t f56=0;
 uint64_t f57=0;
 uint64_t f58=0;
 uint64_t f59=0;
 uint64_t f60=0;
 uint64_t f61=0;
 uint64_t f62=0;
 uint64_t f63=0;
};
struct point {
 WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(point);
 WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(point,x,y)
 double x=0,y=0;
};
struct record {
 WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(record);
 WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(record,id,sequence,price,enabled,name,position,samples,path)
 uint64_t id=0; int64_t sequence=0; double price=0; bool enabled=false;
 std::string name; point position; std::vector<int64_t> samples; std::vector<point> path;
};
inline uint64_t fold(uint64_t sum,uint64_t value) { return (sum^value)*UINT64_C(1099511628211); }
template<class T> uint64_t checksum(const T& value) {
 uint64_t sum=UINT64_C(1469598103934665603);
 if constexpr(std::is_integral_v<T>) return fold(sum,static_cast<uint64_t>(value));
 else if constexpr(std::same_as<T,float>) return fold(sum,std::bit_cast<uint32_t>(value));
 else if constexpr(std::same_as<T,double>) return fold(sum,std::bit_cast<uint64_t>(value));
 else if constexpr(std::same_as<T,std::string>) {
  for(unsigned char ch:value) sum=fold(sum,ch);
  return fold(sum,value.size());
 } else if constexpr(requires { T::template wjr_json_fields<>(); }) {
  std::apply([&](auto... field) { ((sum=fold(sum,checksum(value.*decltype(field)::member))),...); },T::template wjr_json_fields<>());
  return sum;
 } else {
  for(const auto& element:value) sum=fold(sum,checksum(element));
  return fold(sum,value.size());
 }
}
template<class T> uint64_t native(ondemand_reader& reader) {
 auto result=wjr::json::parse<T>(reader);
 if(!result) throw std::runtime_error("native parse failure");
 return checksum(*result);
}
template<class T> uint64_t dom(ondemand_reader& reader) {
 auto document=wjr::json::document::parse(reader);
 if(!document) throw std::runtime_error("DOM parse failure");
 auto result=wjr::json::value_to<T>(std::move(*document));
 return checksum(result);
}
struct fixture {
 std::string name,input;
 uint64_t(*direct)(ondemand_reader&);
 uint64_t(*reference)(ondemand_reader&);
 uint64_t expected;
};
template<class T> fixture make_fixture(std::string name,std::string input) {
 ondemand_reader reader;
 reader.read(input); auto direct=wjr::json::parse<T>(reader);
 reader.read(input); auto document=wjr::json::document::parse(reader);
 if(!direct||!document) throw std::runtime_error("fixture parse failure");
 auto reference=wjr::json::value_to<T>(std::move(*document));
 if(wjr::json::document(*direct)!=wjr::json::document(reference)) throw std::runtime_error("canonical output mismatch");
 auto expected=checksum(reference);
 if(checksum(*direct)!=expected) throw std::runtime_error("checksum mismatch");
 return {std::move(name),std::move(input),native<T>,dom<T>,expected};
}
inline std::string numeric_input(size_t count,unsigned fields) {
 std::string input="[";
 for(size_t record=0;record<count;++record) {
  if(record) input+=',';
  input+='{';
  std::vector<unsigned> order(fields);
  for(unsigned i=0;i<fields;++i) order[i]=i;
  uint32_t random=73+static_cast<uint32_t>(record);
  for(unsigned i=fields-1;i>0;--i) {random=random*1664525u+1013904223u; std::swap(order[i],order[random%(i+1)]);}
  for(unsigned index=0;index<fields;++index) {
   if(index) input+=',';
   unsigned field=order[index];
   input+="\"f";
   input+=char('0'+field/10); input+=char('0'+field%10);
   input+="\":"+std::to_string(record*fields+field+1);
  }
  input+='}';
 }
 input+=']'; return input;
}
inline std::string records_input(size_t count,size_t text_size,bool unknown) {
 std::string input="[";
 for(size_t i=0;i<count;++i) {
  if(i) input+=',';
  input+="{\"samples\":[1,-2,3,4,5,6,7,8],\"name\":\""+std::string(text_size,'x')+"\\n\\u4e2d\",\"price\":1.25,\"position\":{\"y\":2.5,\"x\":1.5},\"enabled\":true,\"id\":"+std::to_string(i)+",\"sequence\":-7,\"path\":[{\"x\":1,\"y\":2},{\"y\":4,\"x\":3}]";
  if(unknown) input+=",\"unknown\":{\"payload\":\""+std::string(128,'u')+"\",\"items\":[1.25,null,true,{\"x\":\"\\u4e2d\"}]}";
  input+='}';
 }
 input+=']'; return input;
}
inline std::vector<fixture> fixtures() {
 std::vector<fixture> output;
 output.push_back(make_fixture<std::vector<numeric8>>("fields8-small",numeric_input(1,8)));
 output.push_back(make_fixture<std::vector<numeric8>>("fields8",numeric_input(128,8)));
 output.push_back(make_fixture<std::vector<numeric32>>("fields32",numeric_input(64,32)));
 output.push_back(make_fixture<std::vector<numeric64>>("fields64",numeric_input(64,64)));
 output.push_back(make_fixture<std::vector<record>>("records-short",records_input(128,8,false)));
 output.push_back(make_fixture<std::vector<record>>("records-long",records_input(64,256,false)));
 output.push_back(make_fixture<std::vector<record>>("records-unknown",records_input(64,8,true)));
 std::string numbers="[",floats="[";
 const char* fractions[]={".0",".25",".5",".75"};
 for(int i=0;i<4096;++i) {
  if(i) {numbers+=',';floats+=',';}
  numbers+=std::to_string(i-2048);
  floats+=std::to_string((i%512)/4)+fractions[i%4];
 }
 numbers+=']';floats+=']';
 output.push_back(make_fixture<std::vector<int64_t>>("numbers4096",std::move(numbers)));
 output.push_back(make_fixture<std::vector<float>>("floats4096",std::move(floats)));
 return output;
}
} // namespace probe
