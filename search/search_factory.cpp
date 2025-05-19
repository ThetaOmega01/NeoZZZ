#include "search_factory.hpp"
#include <algorithm>

namespace tetris {

SearchFactory& SearchFactory::getInstance() {
  static SearchFactory instance;
  return instance;
}

void SearchFactory::initialize() {
  // TODO: Register search algorithms here
}

void SearchFactory::registerSearchAlgorithm(
    std::string_view name, std::unique_ptr<SearchAlgorithm> algorithm) {
  m_searchAlgorithms.emplace(name, std::move(algorithm));
}

std::unique_ptr<SearchAlgorithm>
SearchFactory::createSearchAlgorithm(std::string_view name) const {
  auto it = m_searchAlgorithms.find(name);
  if (it == m_searchAlgorithms.end()) {
    return nullptr;
  }

  // TODO: Add actual search algorithms here (create a copy of the algorithm)

  return nullptr;
}

std::vector<std::string_view>
SearchFactory::getRegisteredAlgorithmNames() const {
  std::vector<std::string_view> names;
  names.reserve(m_searchAlgorithms.size());
  for (const auto& [name, _] : m_searchAlgorithms) {
    names.emplace_back(name);
  }

  std::ranges::sort(names);
  return names;
}

} // namespace tetris
