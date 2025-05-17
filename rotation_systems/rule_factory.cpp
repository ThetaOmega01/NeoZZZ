#include "rule_factory.hpp"
#include <algorithm>
#include <ranges>

namespace tetris {

RuleFactory& RuleFactory::getInstance() {
  static RuleFactory instance;
  return instance;
}

RuleFactory::RuleFactory() { initialize(); }

void RuleFactory::initialize() {
  // Register built-in rotation systems
  // registerRotationSystem("SRS", std::make_unique<SRS>());

  // Additional rotation systems can be registered here
  // Example usage:
  // registerRotationSystem("ARS", std::make_unique<ARS>());
  // registerRotationSystem("TGM", std::make_unique<TGM>());
}

void RuleFactory::registerRotationSystem(
    const std::string_view name, std::unique_ptr<RotationSystem> system) {
  m_rotationSystems[name] = std::move(system);
}

std::shared_ptr<RotationSystem>
RuleFactory::createRotationSystem(const std::string_view name) const {
  const auto it = m_rotationSystems.find(name);
  if (it == m_rotationSystems.end()) {
    return nullptr;
  }
  // Clone the prototype from the map
  return it->second->clone();
}

std::vector<std::string_view> RuleFactory::getRegisteredSystemNames() const {
  std::vector<std::string_view> names;
  names.reserve(m_rotationSystems.size());

  for (const auto& key : m_rotationSystems | std::views::keys) {
    names.emplace_back(key);
  }

  std::ranges::sort(names);
  return names;
}

} // namespace tetris
