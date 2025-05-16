#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

namespace tetris {

/**
 * @brief Enumeration of all tetromino types
 */
enum class PieceType {
  I,
  J,
  L,
  O,
  S,
  T,
  Z,
};

/**
 * @brief Enumeration of rotation states
 */
enum class Rotation {
  R0,   ///< Initial rotation (0 degree)
  R90,  ///< 90 degrees clockwise
  R180, ///< 180 degrees
  R270  ///< 270 degrees clockwise (90 degrees anti-clockwise)
};

/**
 * @brief Get the next rotation state when rotating clockwise
 *
 * @param rotation Current rotation state
 * @return Next rotation state
 */
constexpr Rotation rotateClockwise(const Rotation rotation) {
  return static_cast<Rotation>((std::to_underlying(rotation) + 1) % 4);
}

/**
 * @brief Get the next rotation state when rotating anti-clockwise
 *
 * @param rotation Current rotation state
 * @return Next rotation state
 */
constexpr Rotation rotateCounterClockwise(const Rotation rotation) {
  return static_cast<Rotation>((std::to_underlying(rotation) + 3) % 4);
}

/**
 * @brief Get the next rotation state when rotating 180 degrees
 *
 * @param rotation Current rotation state
 * @return Next rotation state
 */
constexpr Rotation rotate180(const Rotation rotation) {
  return static_cast<Rotation>((std::to_underlying(rotation) + 2) % 4);
}

/**
 * @brief Represents a position on the board
 */
struct Position {
  int32_t xPos{};
  int32_t yPos{};

  /**
   * @brief Equality operator
   */
  bool operator==(const Position& other) const {
    return xPos == other.xPos && yPos == other.yPos;
  }

  /**
   * @brief Addition operator
   */
  Position operator+(const Position& other) const {
    return Position(xPos + other.xPos, yPos + other.yPos);
  }
};

/**
 * @brief Represents the state of a tetromino
 */
class PieceState {
public:
  /**
   * @brief Default constructor
   */
  PieceState() = default;

  /**
   * @brief Construct a piece state with given parameters
   *
   * @param type PieceType
   * @param position Position on the board
   * @param rotation Rotation state
   */
  PieceState(const PieceType type, const Position position, const Rotation rotation)
      : m_type{type}, m_position{position}, m_rotation{rotation} {}

  /**
   * @brief Get the type of the tetromino
   */
  [[nodiscard]] PieceType getType() const { return m_type; }

  /**
   * @brief Get the position of the tetromino
   */
  [[nodiscard]] Position getPosition() const { return m_position; }

  /**
   * @brief Get the rotation state of the tetromino
   */
  [[nodiscard]] Rotation getRotation() const { return m_rotation; }

  /**
   * @brief Set the type of the tetromino
   */
  void setType(const PieceType type) { m_type = type; }

  /**
   * @brief Set the position of the tetromino
   */
  void setPosition(const Position position) { m_position = position; }

  /**
   * @brief Set the rotation state of the tetromino
   */
  void setRotation(const Rotation rotation) { m_rotation = rotation; }

  /**
   * @brief Equality operator
   */
  bool operator==(const PieceState& other) const {
    return m_type == other.m_type && m_position == other.m_position &&
           m_rotation == other.m_rotation;
  }

  /**
   * @brief Hash function for PieceState
   */
  struct Hash {
    /**
     * @brief Helper function to combine hash values
     *
     * Uses the FNV-1a inspired algorithm similar to boost::hash_combine
     */
    static void hash_combine(size_t& seed, const size_t value) {
      seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t operator()(const PieceState& state) const {
      size_t seed{0x9e3779b9};

      // Combine the hash of all components
      hash_combine(seed, std::hash<int32_t>{}(
                             static_cast<int32_t>(state.getType())));
      hash_combine(seed, std::hash<int32_t>{}(state.getPosition().xPos));
      hash_combine(seed, std::hash<int32_t>{}(state.getPosition().yPos));
      hash_combine(seed, std::hash<int32_t>{}(
                             static_cast<int32_t>(state.getRotation())));

      return seed;
    }
  };

private:
  PieceType m_type{};
  Position m_position{};
  Rotation m_rotation{};
};

// Forward declaration
class RotationSystem;

/**
 * @brief Represents a tetromino with its shape and properties
 */
class Piece {
public:
  /**
   * @brief Max size of a tetromino (4x4 grid)
   */
  static constexpr size_t maxSize{4};

  /**
   * @brief Default constructor
   */
  Piece() = default;

  /**
   * @brief Construct a piece with a given state
   *
   * @param state The initial state of the piece
   * @param rotationSystem The rotation system for the piece
   */
  Piece(const PieceState& state,
        std::shared_ptr<RotationSystem> rotationSystem);

  /**
   * @brief Get the current state of the piece
   */
  [[nodiscard]] const PieceState& getState() const { return m_state; }

  /**
   * @brief Set the state of the piece
   */
  void setState(const PieceState& state);

  /**
   * @brief Set the rotation systems for the piece
   */
  void setRotationSystem(const std::shared_ptr<RotationSystem>& rotationSystem);

  /**
   * @brief Get the rotation systems for the piece
   */
  [[nodiscard]] const std::shared_ptr<RotationSystem>&
  getRotationSystem() const {
    return m_rotationSystem;
  }

  /**
   * @brief Get the width of the piece in its current rotation
   */
  [[nodiscard]] int32_t getWidth() const { return m_width; }

  /**
   * @brief Get the height of the piece in its current rotation
   */
  [[nodiscard]] int32_t getHeight() const { return m_height; }

  /**
   * @brief Get the shape data for the piece
   */
  [[nodiscard]] const std::bitset<maxSize * maxSize>& getShapeData() const {
    return m_shapeData;
  }

  /**
   * @brief Get the column heights of the piece
   *
   * Each value represents the height of a column in the piece.
   */
  [[nodiscard]] const std::array<int32_t, maxSize>&
  getColumnHeights() const {
    return m_columnHeights;
  }

  /**
   * @brief Get the column bottoms of the piece
   *
   * Each value represents the bottom position of a column in the piece.
   */
  [[nodiscard]] const std::array<int32_t, maxSize>&
  getColumnBottoms() const {
    return m_columnBottoms;
  }

  /**
   * @brief Get all filled cell positions in the piece
   *
   * @return Vector of positions relative to the piece's top-left corner
   */
  [[nodiscard]] std::vector<Position> getFilledCells() const;

  /**
   * @brief Get all filled cell positions in absolute board coordinates
   *
   * @return Vector of positions in board coordinates
   */
  [[nodiscard]] std::vector<Position> getAbsoluteFilledCells() const;

private:
  /**
   * @brief Update the piece's shape data based on its state
   */
  void updateShapeData();

  /**
   * @brief Update the piece's dimensions and column properties
   */
  void updateDimensions();

  PieceState m_state; ///< Current state of the piece
  std::shared_ptr<RotationSystem> m_rotationSystem; ///< Rotation systems
  std::bitset<maxSize * maxSize> m_shapeData{};     ///< Shape data for each row
  std::array<int32_t, maxSize>
      m_columnHeights{}; ///< Height of each column
  std::array<int32_t, maxSize>
      m_columnBottoms{};   ///< Bottom position of each column
  int32_t m_width{};  ///< Width of the piece
  int32_t m_height{}; ///< Height of the piece
};

} // namespace tetris