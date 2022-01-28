A library that I wrote myself.

Now there is a relatively complete "String" class.

---

Functions & dependencies :

```
grisu 	    : floating point to string Library by using grisu algorithm
mallocator  : memory pool
mySTL       : provide some simple basic information such as byte order,array hash,type_traits
typeInfo    : simple reflection
mtool       : simple tool,for simple testing and debugging
thread_pool :  [progschj/ThreadPool: A simple C++11 Thread Pool implementation (github.com)](https://github.com/progschj/ThreadPool)
String      : self implemented string class ,  many functions have been implemented such as 	  replace,split,trim,connect,contains,convert_to,convert_from and so on. but the COW version will not be implemented temporarily.
json   	    : serialization and deserialization and so on, and passed the JSON_cheker test,and you can customize the type.

thread_pool -- mallcator (because the memory of function <void> needs to be applied frequently,so use mallocator as default allocator)
mString     -- mallocator
		   -- mySTL.h
		   -- grisu
json        -- typeInfo
		   -- mString 
```

---

mString 

The first part is the string search tool , use a algorithm that I came up based on KMP and Sunday algorithm.

At present, there is no more constant optimization，it has a large constant(I hope someone can help me optimize it).

For special cases, the efficiency is better, and the worst complexity is O(n).

If is_default_equal<Pred_eq>::value is true,the value_type is integral and the sizeof(value_type) == 1 then more optimization can be done. As default,is_default_equal<equal_to> and is_default_equal<char_traits<Char>> is true.There is the best performance in String.

The second part is String_core，SSO is used,and the COW is disabled.

The third part is about some type_traits and functions about String.

The forth part is basic_String_view and basic_String.

---

json

