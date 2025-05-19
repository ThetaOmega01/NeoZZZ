#pragma once

#include "search_algorithm.hpp"
#include <memory>
#include <string_view>
#include <unordered_map>

namespace tetris {

/**
 * @class SearchFactory
 * @brief Factory for creating search algorithm instances
 *
 * This class provides a centralized way to create and manage
 * different search algorithms.
 */
class SearchFactory {
public:
  /**
   * @brief Get the singleton instance of the factory
   *
   * @return Reference to the singleton instance
   */
  static SearchFactory& getInstance();

  /**
   * @brief Register a search algorithm with the factory
   *
   * @param name The name of the search algorithm
   * @param algorithm The search algorithm instance
   */
  void registerSearchAlgorithm(std::string_view name,
                               std::unique_ptr<SearchAlgorithm> algorithm);

  /**
   * @brief Create a search algorithm by name
   *
   * @param name The name of the search algorithm to create
   * @return Shared pointer to the created search algorithm, or nullptr if not
   * found
   */
  [[nodiscard]] std::unique_ptr<SearchAlgorithm>
  createSearchAlgorithm(std::string_view name) const;

  /**
   * @brief Get the names of all registered search algorithms
   *
   * @return Vector of search algorithm names
   */
  [[nodiscard]] std::vector<std::string_view>
  getRegisteredAlgorithmNames() const;

private:
  /**
   * @brief Private constructor for singleton pattern
   */
  SearchFactory();

  /**
   * @brief Initialize the factory with built-in search algorithms
   */
  static void initialize();

  std::unordered_map<std::string_view, std::unique_ptr<SearchAlgorithm>>
      m_searchAlgorithms;
};

} // namespace tetris
