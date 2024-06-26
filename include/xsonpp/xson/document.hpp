#pragma once
#include <map>

#include "value/object.hpp"

namespace xson {
	template<template<typename...> class KVMapTy = std::map> 
	using document = object<KVMapTy>;
}