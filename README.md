parser.JSON
==========

Introduction
----------
I made this for just practice. But works good I think.

Example
----------

```cpp
#include <iostream>
#include <string>
#include "json.hpp"

int main(){
	using name

	auto rjson
		= JSON::parse("								\
	{\"menu\": {									\
		\"id\": \"file\",							\
			\"value\" : \"File\",					\
			\"popup\" : {							\
			\"menuitem\": [							\
			{										\
				\"value\": \"New\",					\
				\"onclick\" : \"CreateNewDoc()\"},	\
			{										\
				\"value\": \"Open\",				\
				\"onclick\" : \"OpenDoc()\"},		\
			{										\
				\"value\": \"Close\",				\
				\"onclick\" : \"CloseDoc()\"}		\
			]										\
		}											\
	}}");

	std::string id = rjson["menu"]["id"];
	cout << id << endl;

	auto wjson = JSON::elem();
	wjson["a1"] = 1;
	wjson["a2"] = 2;
	wjson["a3"]["b"] = "ab";
	wjson["a3"]["c"][3] = 123;

	cout << wjson.stringify() << endl;

	return 0;
}
```

It's all. But I have plan to make more comportable for user.