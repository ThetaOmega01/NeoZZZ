#pragma once

#include "rotation_system.hpp"
#include <string_view>
#include <unordered_map>

namespace tetris {

/**
 * @class RuleFactory
 * @brief Factory for creating rotation system instances
 *
 * This class provides a centralized way to create and manage
 * different rotation systems.
 */
class RuleFactory {
public:
  /**
   * @brief Get the singleton instance of the factory
   *
   * @return Reference to the singleton instance
   */
  static RuleFactory& getInstance();

  /**
   * @brief Register a rotation system with the factory
   *
   * @param name The name of the rotation system
   * @param system The rotation system instance
   */
  void registerRotationSystem(std::string_view name,
                              std::unique_ptr<RotationSystem> system);

  /**
   * @brief Create a rotation system by name
   *
   * @param name The name of the rotation system to create
   * @return Shared pointer to the created rotation system, or nullptr if not
   * found
   */
  [[nodiscard]] std::shared_ptr<RotationSystem>
  createRotationSystem(std::string_view name) const;

  /**
   * @brief Get the names of all registered rotation systems
   *
   * @return Vector of string_view of rotation system names
   */
  [[nodiscard]] std::vector<std::string_view> getRegisteredSystemNames() const;

private:
  /**
   * @brief Private constructor for singleton pattern
   */
  RuleFactory();

  /**
   * @brief Initialize the factory with built-in rotation systems
   */
  static void initialize();

  std::unordered_map<std::string_view, std::unique_ptr<RotationSystem>>
      m_rotationSystems;
};

} // namespace tetris
