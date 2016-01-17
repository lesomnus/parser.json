#pragma once

#include <string>

namespace JSON{
	enum class TYPE: unsigned short{
		EMPTY = 0, OBJECT, ARRAY, STRING, INT, FRAC, EXP, BOOL
	};

	namespace reader{
		using iterator = std::string::const_iterator;
		using checker = decltype(isspace);

		bool go_until(iterator& it, iterator& end, char is, bool what){
			++it;
			while(it != end){
				if((is == *it) == what) return true;
				else ++it;
			} return false;
		}
		bool go_until(iterator& it, iterator& end, checker is, bool what){
			++it;
			while(it != end){
				if(is(*it) == (what ? 1 : 0)) return true;
				else ++it;
			} return false;
		}

		bool is_(iterator& it, const std::string& src){
			iterator begin = it;
			iterator src_it = src.cbegin();
			iterator src_end = src.end();

			while(src_it != src_end){
				if(*it != *src_it){
					it = begin;
					return false;
				} else{
					++it;
					++src_it;
				}
			} return true;
		}
		bool is_(iterator& it, std::string&& src){
			return is_(it, src);
		}
		bool is_(iterator& it, const char* src){
			return is_(it, std::string(src));
		}

		bool is_string(iterator& it, iterator& end){
			if(it == end) return false;
			auto begine = it;
			if((*it == '\"') && go_until(it, end, '\"', true)){
				++it;
				return true;
			}
			it = begine;
			return false;
		}
		bool is_number(iterator& it, iterator& end){
			if(it == end) return false;
			auto begin = it;
			auto returner = [&it, &begin](){
				it = begin;
				return false;
			};

			if((*it == '-') && (++it == end)){
				return returner();
			} else if(isdigit(*it)){
				if(!go_until(it, end, isdigit, false))
					return true;

				if((*it == '.') && (++it == end)){
					return returner();
				} else if(isdigit(*it)){
					if(!go_until(it, end, isdigit, false))
						return true;
				} else if(((*it == 'e') || (*it == 'E')) && (++it == end)){
					return returner();
				} else if(isdigit(*it)){
					if(!go_until(it, end, isdigit, false))
						return true;
				} else if(((*it == '-') || (*it == '+')) && (++it == end)){
					return returner();
				} else if(isdigit(*it)){
					if(!go_until(it, end, isdigit, false))
						return true;
				} else return true;
			} else return returner();
			return true;
		}
		bool is_bool(iterator& it, iterator& end){
			//if(isspace(*it)) go_until(it, end, isspace, false);
			if(it == end) return false;
			auto begin = it;
			if(is_(it, "null") ||
			   is_(it, "true") ||
			   is_(it, "false")) return true;
			it = begin;
			return false;
		}

		bool is_array(iterator& it, iterator& end);
		bool is_object(iterator& it, iterator& end);

		bool is_elem(iterator& it, iterator& end){
			if(it == end) return false;
			auto begin = it;

			if((isdigit(*it) || (*it == '-')) && is_number(it, end))
				return true;

			switch(*it){
			case 'n': case 't': case 'f':
				if(is_bool(it, end))
					return true;
				break;
			case '\"':
				if(is_string(it, end))
					return true;
				break;
			case '[':
				if(is_array(it, end))
					return true;
				break;
			case '{':
				if(is_object(it, end))
					return true;
				break;
			default:
				break;
			}

			it = begin;
			return false;
		};

		bool is_array(iterator& it, iterator& end){
			if(it == end) return false;
			auto begin = it;

			if(*it == '[') while(true){
				if(++it == end) break;
				if(isspace(*it) &&
				   !go_until(it, end, isspace, false)) break;
				if(!is_elem(it, end)) break;
				if(isspace(*it) &&
				   !go_until(it, end, isspace, false)) break;
				if(*it == ','){
					continue;
				} else if(*it == ']'){
					++it;
					return true;
				} else break;
			}

			it = begin;
			return false;
		};
		bool is_object(iterator& it, iterator& end){
			if(it == end) return false;
			auto begin = it;

			if(*it == '{') while(true){
				if(++it == end) break;
				if(isspace(*it) &&
				   !go_until(it, end, isspace, false)) break;
				if(!is_string(it, end)) break;
				if((*it != ':') &&
				   !go_until(it, end, isspace, false)) break;
				if(*it != ':') break;
				if(!isspace(*it) &&
				   !go_until(it, end, isspace, false)) break;
				if(!is_elem(it, end)) break;
				if(isspace(*it) &&
				   !go_until(it, end, isspace, false)) break;
				if(*it == ','){
					continue;
				} else if(*it == '}'){
					++it;
					return true;
				} else break;
			}

			it = begin;
			return false;
		};
	}
}