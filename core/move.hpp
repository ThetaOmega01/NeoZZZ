#pragma once

#include "tetris_piece.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace tetris {

/**
 * @brief Maximum number of wall kick tests
 */
constexpr int32_t maxWallKickTests = 16;

/**
 * @brief Represents a wall kick offset
 */
struct WallKickOffset {
  int32_t xOffset{0};
  int32_t yOffset{0};

  /**
   * @brief Construct a wall kick offset with given values
   */
  constexpr WallKickOffset(const int32_t x, const int32_t y)
      : xOffset{x}, yOffset{y} {}

  /**
   * @brief Convert to Position
   */
  [[nodiscard]] constexpr Position toPosition() const {
    return Position(xOffset, yOffset);
  }
};

/**
 * @brief Represents a set of wall kick tests for a rotation
 */
class WallKickData {
public:
  /**
   * @brief Default constructor
   */
  WallKickData() = default;
  /**
   * @brief Construct with a list of offsets
   */
  WallKickData(std::initializer_list<WallKickOffset> offsets);

  /**
   * @brief Get the number of tests
   */
  [[nodiscard]] size_t getTestCount() const { return m_offsets.size(); }

  /**
   * @brief Get the offset at the given index
   */
  [[nodiscard]] const WallKickOffset& getOffset(size_t index) const;

  /**
   * @brief Get all offsets
   */
  [[nodiscard]] const std::vector<WallKickOffset>& getOffsets() const {
    return m_offsets;
  }

private:
  std::vector<WallKickOffset> m_offsets; ///< List of wall kick offsets
};

/**
 * @brief Enumeration of move types
 */
enum class MoveType {
  Left,                   ///< Move left
  Right,                  ///< Move right
  Down,                   ///< Move down
  Up,                     ///< Move up (for testing)
  RotateClockwise,        ///< Rotate clockwise
  RotateCounterClockwise, ///< Rotate counter-clockwise
  Rotate180,              ///< Rotate 180 degrees
  HardDrop,               ///< Hard drop
  SoftDrop,               ///< Soft drop
  Hold                    ///< Hold piece
};

/**
 * @brief Represents a move operation
 */
class Move {
public:
  /**
   * @brief Construct a move with the given type
   */
  explicit Move(MoveType type);

  /**
   * @brief Construct a move with the given type and wall kick index
   */
  Move(MoveType type, int32_t wallKickIndex);

  /**
   * @brief Get the move type
   */
  [[nodiscard]] MoveType getType() const { return m_type; }

  /**
   * @brief Get the wall kick index
   */
  [[nodiscard]] int32_t getWallKickIndex() const { return m_wallKickIndex; }

  /**
   * @brief Check if this is a rotation move
   */
  [[nodiscard]] bool isRotation() const;

  /**
   * @brief Check if this is a translation move
   */
  [[nodiscard]] bool isTranslation() const;

  /**
   * @brief Get a string representation of the move
   */
  [[nodiscard]] std::string toString() const;

private:
  MoveType m_type{MoveType::Down}; ///< Type of move
  int32_t m_wallKickIndex{-1};     ///< Wall kick index for rotation moves
};
} // namespace tetris
