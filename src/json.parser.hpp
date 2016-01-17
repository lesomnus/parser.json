#pragma once

#include <iostream>
#include <string>

#include "json.reader.hpp"
#include "json.elem.hpp"

namespace JSON{
	namespace parser{
		using iterator = reader::iterator;

		elem parse_number(iterator& it, const iterator& end){
			iterator begin = it;
			if(reader::is_number(it, end)){
				return elem(TYPE::FRAC, std::string(begin, it));
			} else return elem();
		}
		elem parse_string(iterator& it, const iterator& end){
			iterator begin = it;
			if(reader::is_string(it, end)){
				return elem(TYPE::STRING, std::string(begin + 1, it - 1));
			} else return elem();
		}
		elem parse_bool(iterator& it, const iterator& end){
			iterator begin = it;
			if(reader::is_bool(it, end)){
				return elem(TYPE::BOOL, std::string(begin, it));
			} else return elem();
		}

		elem parse_array(iterator& it, const iterator& end);
		elem parse_object(iterator& it, const iterator& end);

		elem parse_array(iterator& it, const iterator& end){
			elem result(TYPE::ARRAY, "");
			size_t index = 0;

			do{
				++it;
				if(isdigit(*it)){
					result[index] = parse_number(it, end);
				} else if(*it == '\"'){
					result[index] = parse_string(it, end);
				} else if(*it == 'n' || *it == 't' || *it == 'f'){
					result[index] = parse_bool(it, end);
				} else if(*it == '['){
					result[index] = parse_array(it, end);
				} else if(*it == '{'){
					result[index] = parse_object(it, end);
				} else reader::go_until(it, end, ']', true);
				++index;
			} while(*it != ']');

			++it;
			return result;
		}
		elem parse_object(iterator& it, const iterator& end){
			elem result(TYPE::OBJECT, "");
			iterator begin;
			std::string key;

			do{
				begin = ++it;
				if(reader::is_string(it, end)){
					key = std::string(begin + 1, it - 1);
				} else reader::go_until(it, end, '}', true);
				++it;

				if(isdigit(*it)){
					result[key] = parse_number(it, end);
				} else if(*it == '\"'){
					result[key] = parse_string(it, end);
				} else if(*it == 'n' || *it == 't' || *it == 'f'){
					result[key] = parse_bool(it, end);
				} else if(*it == '['){
					result[key] = parse_array(it, end);
				} else if(*it == '{'){
					result[key] = parse_object(it, end);
				} else reader::go_until(it, end, '}', true);
			} while(*it != '}');

			++it;
			return result;
		}
	}

	elem parse(const std::string& json){
		if(!JSON::util::good(json)) return elem();
		std::string src = JSON::util::pack(json);
		auto begin = src.cbegin();
		auto end = src.cend();

		if(*begin == '{') return parser::parse_object(begin, end);
		else if(*begin == '[') return parser::parse_array(begin, end);
		else if(isdigit(*begin)) return parser::parse_number(begin, end);
		else if(*begin == '\"') return parser::parse_string(begin, end);
		else if(*begin == 'n' || *begin == 't' || *begin == 'f')
			return parser::parse_bool(begin, end);
		else return elem();
	}
}