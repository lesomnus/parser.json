#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

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

	class elem{
	public:
		using string = std::string;
		using elems = std::vector<elem>;
		using members = std::unordered_map <std::string, elem>;


		/* constructor */

		// copy

		elem(const elem& src){
			this->copy(src);
		}
		elem(elem&& src){
			this->copy(std::move(src));
		}

		elem():_type(TYPE::EMPTY){}

		elem(const string& value)
			:_type(TYPE::STRING), _value(value){}
		elem(string&& value)
			:_type(TYPE::STRING), _value(std::move(value)){}
		elem(const char* value)
			:_type(TYPE::STRING), _value(string(value)){}

		elem(const int value)
			:_type(TYPE::INT), _value(std::to_string(value)){}
		elem(const double value)
			:_type(TYPE::FRAC), _value(std::to_string(value)){}
		elem(const bool value)
			:_type(TYPE::BOOL), _value(std::to_string(value)){}

		/* method */

		elem& type_to(const TYPE type){
			if(_type == type) return (*this);

			if(_type == TYPE::OBJECT) _members.reset();
			else if(_type == TYPE::ARRAY) _elems.reset();

			if(type == TYPE::OBJECT)
				_members = std::make_unique<members>();
			else if(type == TYPE::ARRAY)
				_elems = std::make_unique<elems>();

			_value.clear();
			_type = type;
			return (*this);
		}
		string stringify() const{
			if(_type == TYPE::OBJECT){
				std::string val("{");
				auto is_empty = !(_members->size() > 0);
				auto end = _members->end();
				auto pend = end;
				if(!is_empty) --pend;

				for(auto it = _members->begin(); it != end; ++it){
					std::string key("\"");
					key = key + it->first + "\"";
					auto value = it->second.stringify();
					val = val + key + ":" + value;
					if(!is_empty && (it != pend)) val += ",";
				}
				val += "}";
				return val;
			} else if(_type == TYPE::ARRAY){
				std::string val("[");
				auto is_empty = !(_elems->size() > 0);
				auto end = _elems->end();
				auto pend = end;
				if(!is_empty) --pend;

				for(auto it = _elems->begin(); it != end; ++it){
					switch(it->_type){
					case TYPE::EMPTY:
						val += "null";
						break;
					case TYPE::ARRAY:
						val += it->stringify();
						break;
					case TYPE::STRING:
						val = val + "\"" + it->_value + "\"";
						break;
					default:
						if(it->_value.empty()) val += "null";
						else val += it->_value;
						break;
					}
					if(!is_empty && (it != pend)) val += ",";
				}
				val += "]";
				return val;
			} else{
				if(_type != TYPE::STRING) return _value;
				return std::string("\"") + _value + "\"";
			}
		}
		void clear(){
			_type = TYPE::EMPTY;
			_value.clear();
			_elems.reset();
			_members.reset();
		}
		elem& copy(const elem& src){
			type_to(src._type);
			if(_type == TYPE::ARRAY){
				_elems = std::make_unique<elems>(src._elems->size());
				auto it = _elems->begin();
				auto end = src._elems->end();
				auto that = src._elems->begin();
				for(; that != end; ++it, ++that)
					it->copy(*that);
			} else if(_type == TYPE::OBJECT){
				_members = std::make_unique<members>(src._members->size());
				auto end = src._members->end();
				auto that = src._members->begin();
				for(; that != end; ++that){
					(*_members)[that->first].copy(that->second);
				}
			} else _value = src._value;
			return (*this);
		}
		elem& copy(elem&& src){
			_type = src._type;
			_value = std::move(src._value);
			_elems = std::move(src._elems);
			_members = std::move(src._members);
			src.type_to(TYPE::EMPTY);
			return (*this);
		}

		/* overloading */

		elem& operator [] (const string& key){
			if(_type != TYPE::OBJECT)
				type_to(TYPE::OBJECT);
			return (*_members)[key];
		}
		elem& operator [] (string&& key){
			if(_type != TYPE::OBJECT)
				type_to(TYPE::OBJECT);
			return (*_members)[std::move(key)];
		}
		elem& operator [] (const size_t index){
			if(_type != TYPE::ARRAY)
				type_to(TYPE::ARRAY);

			auto size = _elems->size();
			if(!(index < size))
				_elems->resize(index + 1, elem());

			return (*_elems)[index];
		}

		elem& operator = (const elem& rhs){
			this->copy(rhs);
			return (*this);
		}
		elem& operator = (elem&& rhs){
			this->copy(std::move(rhs));
			return (*this);
		}

		elem& operator = (const string& rhs){
			this->type_to(TYPE::STRING);
			_value = rhs;
			return (*this);
		}
		elem& operator = (string&& rhs){
			this->type_to(TYPE::STRING);
			_value = std::move(rhs);
			return (*this);
		}
		elem& operator = (const char* rhs){
			this->type_to(TYPE::STRING);
			_value = std::string(rhs);
			return (*this);
		}

		elem& operator = (const int rhs){
			this->type_to(TYPE::INT);
			_value = std::to_string(rhs);
			return (*this);
		}
		elem& operator = (const double rhs){
			this->type_to(TYPE::FRAC);
			_value = std::to_string(rhs);
			return (*this);
		}
		elem& operator = (const bool rhs){
			this->type_to(TYPE::BOOL);
			_value = std::to_string(rhs);
			return (*this);
		}

	private:
		TYPE	_type;
		string	_value;
		std::unique_ptr< elems
		>		_elems;
		std::unique_ptr< members
		>		_members;

	public:	// static
		static elem parse(const string& json){
			std::stack<char> stck;
			string src = JSON::pack(json);
			elem rst("qwer");
			return rst;
		}
	};

	elem create_array(){
		return std::move(elem().type_to(TYPE::ARRAY));
	}
	elem create_object(){
		return std::move(elem().type_to(TYPE::OBJECT));
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