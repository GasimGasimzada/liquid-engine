#ifndef LIQUID_CORE_BASE_H_
#define LIQUID_CORE_BASE_H_

#ifdef LIQUID_DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#else
#ifndef NDEBUG
#define NDEBUG
#endif
#endif

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <exception>
#include <optional>
#include <array>
#include <set>
#include <limits>
#include <fstream>
#include <memory>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <any>
#include <map>
#include <unordered_map>
#include <memory>
#include <list>
#include <type_traits>
#include <variant>
#include <random>
#include <string_view>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Assert.h"
#include "Profiler.h"
#include "Errorable.h"

namespace liquid {

using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;

template <class T> using SharedPtr = std::shared_ptr<T>;

} // namespace liquid

#endif
