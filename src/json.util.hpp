#pragma once

#include <cctype>
#include <string>

#include "json.reader.hpp"

namespace JSON{
	namespace util{
		std::string pack(const std::string& json){
			std::string result;

			for(auto it : json){
				if(isspace(it)) continue;
				result.push_back(it);
			}

			return result;
		}
		std::string pack(std::string&& json){
			return pack(json);
		}

		bool good(const std::string& json){
			auto it = json.cbegin();
			auto end = json.cend();
			if(it == end) return false;

			using namespace JSON::reader;

			if(isspace(*it) && !go_until(it, end, isspace, false))
				return false;
			if(!is_elem(it, end))
				return false;
			if(it == end) return true;
			if(!isspace(*it)) return false;
			if(!go_until(it, end, isspace, false))
				return true;
			return false;
		}
	}
}