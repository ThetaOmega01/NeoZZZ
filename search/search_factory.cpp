#include "search_factory.hpp"

#include "path_search.hpp"

#include <algorithm>
#include <ranges>

using namespace std::string_view_literals;

namespace tetris {

SearchFactory& SearchFactory::getInstance() {
  static SearchFactory instance;
  return instance;
}

SearchFactory::SearchFactory() { initialize(); }

void SearchFactory::initialize() {
  // Register built-in search algorithms
  registerSearchAlgorithm("PathSearch"sv, std::make_unique<PathSearch>());
}

void SearchFactory::registerSearchAlgorithm(
    std::string_view name, std::unique_ptr<SearchAlgorithm> algorithm) {
  m_searchAlgorithms.emplace(name, std::move(algorithm));
}

std::shared_ptr<SearchAlgorithm>
SearchFactory::createSearchAlgorithm(const std::string_view name) const {
  auto it = m_searchAlgorithms.find(name);
  if (it == m_searchAlgorithms.end()) {
    return nullptr;
  }


  // Create a copy of the prototype
  if (name == "PathSearch") {
    return std::make_shared<PathSearch>(
        dynamic_cast<const PathSearch&>(*it->second).getConfig());
  }

  return nullptr;
}

std::vector<std::string_view>
SearchFactory::getRegisteredAlgorithmNames() const {
  std::vector<std::string_view> names;
  names.reserve(m_searchAlgorithms.size());
  for (const auto& name : m_searchAlgorithms | std::views::keys) {
    names.emplace_back(name);
  }

  std::ranges::sort(names);
  return names;
}

} // namespace tetris
