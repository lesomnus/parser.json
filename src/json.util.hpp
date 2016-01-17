#pragma once

namespace JSON{
	namespace util{
		std::string pack(const std::string& json){
			std::string result;
			bool is_qScp = false; // is " scope?
			bool is_escp = false; // is escape?

			for(auto it : json){
				if(is_qScp){
					if(it == '\"') is_qScp = false;
					else is_qScp = true;
				} else{
					is_escp = false;
					switch(it){
					case '\"':
						is_qScp = true;
						break;
					case '\\':
					case '\b':
					case '\f':
					case '\n':
					case '\r':
					case '\t':
					case ' ':	// except: space
						is_escp = true;
						break;
					default:
						break;
					}
					if(is_escp) continue;
				}

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