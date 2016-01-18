#pragma once

#include "json.reader.hpp"
#include "json.elem.hpp"
#include "json.parser.hpp"

namespace JSON{

	elem create_array(){
		return std::move(elem().type_to(TYPE::ARRAY));
	}
	elem create_object(){
		return std::move(elem().type_to(TYPE::OBJECT));
	}

}