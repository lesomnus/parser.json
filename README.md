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
	using namespace std;

	auto rjson = JSON::parse(fstream("example.json"));
	cout << rjson.stringify() << endl;

	auto wjson = JSON::elem();
	wjson["a"] = 1;
	wjson["b"] = true;
	wjson["c"] = {1,2,3};

	auto subjson = JSON::elem();
	subjson["foo"] = "bar";
	subjson["baz"] = {1,2,"three", {1,"two", 3, 4, 5}};

	wjson["d"] = subjson;

	cout << wjson.stringify() << endl;

	return 0;
}
```

It's all. But I have plan to make more comportable for user.