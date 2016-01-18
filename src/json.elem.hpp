#pragma once
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "json.reader.hpp"
#include "json.util.hpp"

namespace JSON{
	class elem{
	public:
		using string = std::string;
		using elems = std::vector<elem>;
		using members = std::unordered_map <std::string, elem>;


		/* constructor */

		elem(TYPE type, string&& value)
			:_type(type), _value(std::move(value)){

			if(type == TYPE::OBJECT)
				_members = std::make_unique<members>();
			else if(type == TYPE::ARRAY)
				_elems = std::make_unique<elems>();
		}

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
					case TYPE::OBJECT:
						val += it->stringify();
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
				if(_type == TYPE::STRING)
					return std::string("\"") + _value + "\"";
				else if(_type == TYPE::EMPTY)
					return std::string("null");
				else return _value;
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
		elem& operator [] (const char* key){
			if(_type != TYPE::OBJECT)
				type_to(TYPE::OBJECT);
			return (*_members)[std::move(string(key))];
		}
		elem& operator [] (const size_t index){
			if(_type != TYPE::ARRAY)
				type_to(TYPE::ARRAY);

			auto size = _elems->size();
			if(!(index < size))
				_elems->resize(index + 1, elem());

			return (*_elems)[index];
		}
		elem& operator [] (const int index){
			return (*this)[static_cast<size_t>(index)];
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

		operator string(){ return this->_value; }
		operator double(){ return std::stod(this->_value); }
		operator int(){ return std::stoi(this->_value); }
		operator bool(){ return std::stoi(this->_value) == 1; }
		
	private:
		TYPE	_type;
		string	_value;
		std::unique_ptr< elems
		>		_elems;
		std::unique_ptr< members
		>		_members;
	};
}