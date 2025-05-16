#include "move.hpp"
#include <stdexcept>
#include <string>

namespace tetris {

WallKickData::WallKickData(const std::initializer_list<WallKickOffset> offsets)
    : m_offsets{offsets} {
  if (m_offsets.size() > maxWallKickTests) {
    throw std::invalid_argument("Too many wall kick tests");
  }
}

const WallKickOffset& WallKickData::getOffset(const size_t index) const {
  if (index >= m_offsets.size()) {
    throw std::out_of_range("Wall kick index out of range");
  }
  return m_offsets.at(index);
}

Move::Move(const MoveType type) : m_type{type} {}

Move::Move(const MoveType type, const int32_t wallKickIndex)
    : m_type{type} {
  if (!isRotation() && wallKickIndex >= 0) {
    throw std::invalid_argument(
        "Wall kick index only valid for rotation moves");
  }
  m_wallKickIndex = wallKickIndex;
}

bool Move::isRotation() const {
  return m_type == MoveType::RotateClockwise ||
         m_type == MoveType::RotateCounterClockwise ||
         m_type == MoveType::Rotate180;
}

bool Move::isTranslation() const {
  return m_type == MoveType::Left || m_type == MoveType::Right ||
         m_type == MoveType::Down || m_type == MoveType::Up ||
         m_type == MoveType::HardDrop || m_type == MoveType::SoftDrop;
}

std::string Move::toString() const {
  switch (m_type) {
  case MoveType::Left:
    return "Left";
  case MoveType::Right:
    return "Right";
  case MoveType::Down:
    return "Down";
  case MoveType::Up:
    return "Up";
  case MoveType::RotateClockwise:
    return m_wallKickIndex >= 0
               ? "RotateClockwise(WK:" + std::to_string(m_wallKickIndex) + ")"
               : "RotateClockwise";
  case MoveType::RotateCounterClockwise:
    return m_wallKickIndex >= 0 ? "RotateCounterClockwise(WK:" +
                                      std::to_string(m_wallKickIndex) + ")"
                                : "RotateCounterClockwise";
  case MoveType::Rotate180:
    return m_wallKickIndex >= 0
               ? "Rotate180(WK:" + std::to_string(m_wallKickIndex) + ")"
               : "Rotate180";
  case MoveType::HardDrop:
    return "HardDrop";
  case MoveType::SoftDrop:
    return "SoftDrop";
  case MoveType::Hold:
    return "Hold";
  default:
    return "Unknown";
  }
}

} // namespace tetris