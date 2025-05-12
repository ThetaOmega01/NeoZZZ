#pragma once

#include "../core/move.hpp"
#include "../core/tetris_piece.hpp"

namespace tetris {

/**
 * @class RotationSystem
 * @brief Abstract interface for rotation systems
 */
class RotationSystem {
public:
  /**
   * @brief Virtual destructor
   */
  virtual ~RotationSystem() = default;

  /**
   * @brief Get the name of the rotation system
   *
   * @return The name of the rotation system
   */
  [[nodiscard]] virtual std::string getName() const = 0;

  /**
   * @brief Get the wall kick data for a clockwise rotation
   *
   * @param type The type of the piece
   * @param fromRotation The initial rotation state
   * @return The wall kick data for the rotation
   */
  [[nodiscard]] virtual WallKickData
  getClockwiseWallKicks(PieceType type, Rotation fromRotation) const = 0;

  /**
   * @brief Get the wall kick data for a counter-clockwise rotation
   *
   * @param type The type of the piece
   * @param fromRotation The initial rotation state
   * @return The wall kick data for the rotation
   */
  [[nodiscard]] virtual WallKickData
  getCounterClockwiseWallKicks(PieceType type, Rotation fromRotation) const = 0;

  /**
   * @brief Get the wall kick data for a 180-degree rotation
   *
   * @param type The type of the piece
   * @param fromRotation The initial rotation state
   * @return The wall kick data for the rotation
   */
  [[nodiscard]] virtual WallKickData
  get180WallKicks(PieceType type, Rotation fromRotation) const = 0;

  /**
   * @brief Get the shape data for a piece type and rotation
   *
   * @param type The type of the piece
   * @param rotation The rotation state
   * @return The shape data for the piece
   */
  [[nodiscard]] virtual std::bitset<Piece::maxSize * Piece::maxSize>
  getShapeData(PieceType type, Rotation rotation) const = 0;

  /**
   * @brief Get the initial spawn state for a piece
   *
   * @param type The type of the piece
   * @param boardWidth The width of the board
   * @param boardHeight The height of the board
   * @return The initial state for the piece
   */
  [[nodiscard]] virtual PieceState
  getInitialState(PieceType type, int boardWidth, int boardHeight) const = 0;

  /**
   * @brief Check if the rotation system supports 180-degree rotations
   *
   * @return true if 180-degree rotations are supported, false otherwise
   */
  [[nodiscard]] virtual bool supports180Rotation() const = 0;
};

} // namespace tetris
