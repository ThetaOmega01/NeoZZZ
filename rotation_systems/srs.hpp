#pragma once

#include "rotation_system.hpp"

namespace tetris {

/**
 * @class SRS
 * @brief Implementation of the Super Rotation System (SRS)
 *
 * This class implements the standard SRS rotation system.
 * Reference: https://harddrop.com/wiki/SRS
 */
class SRS final : public RotationSystem {
public:
  /**
   * @brief Default constructor
   */
  SRS();

  /**
   * @brief Get the name of the rotation system
   * @return "SRS"
   */
  [[nodiscard]] std::string getName() const override { return "SRS"; }

  /**
   * @brief Get the wall kick data for a clockwise rotation
   *
   * @param type The type of the piece
   * @param fromRotation The initial rotation state
   * @return The wall kick data for the rotation
   * @throws std::invalid_argument if the piece type is invalid
   */
  [[nodiscard]] WallKickData
  getClockwiseWallKicks(PieceType type, Rotation fromRotation) const override;

  /**
   * @brief Get the wall kick data for a counter-clockwise rotation
   *
   * @param type The type of the piece
   * @param fromRotation The initial rotation state
   * @return The wall kick data for the rotation
   * @throws std::invalid_argument if the piece type is invalid
   */
  [[nodiscard]] WallKickData
  getCounterClockwiseWallKicks(PieceType type,
                               Rotation fromRotation) const override;

  /**
   * @brief Get the wall kick data for a 180-degree rotation
   *
   * @param type The type of the piece
   * @param fromRotation The initial rotation state
   * @return The wall kick data for the rotation
   */
  [[nodiscard]] WallKickData
  get180WallKicks(PieceType type, Rotation fromRotation) const override;

  /**
   * @brief Get the initial spawn state for a piece
   *
   * @param type The type of the piece
   * @param boardWidth The width of the board
   * @param boardHeight The height of the board
   * @return The initial state for the piece
   * @throws std::invalid_argument if the piece type is invalid
   */
  [[nodiscard]] PieceState
  getInitialState(PieceType type, int32_t boardWidth,
                  int32_t boardHeight) const override;

  /**
   * @brief Check if the rotation system supports 180-degree rotations
   *
   * @return false, SRS does not support 180-degree rotations.
   */
  [[nodiscard]] bool supports180Rotation() const override { return false; }

  /**
   * @brief Get the shape data for a piece type and rotation
   *
   * @param type The type of the piece
   * @param rotation The rotation state
   * @return The shape data for the piece
   * @throws std::invalid_argument if the piece type is invalid
   */
  [[nodiscard]] std::bitset<Piece::maxSize * Piece::maxSize>
  getShapeData(PieceType type, Rotation rotation) const override;

  /**
   * @brief Create a deep copy of the rotation system
   *
   * @return A shared pointer to a new rotation system instance
   */
  [[nodiscard]] std::shared_ptr<RotationSystem> clone() const override;
};

} // namespace tetris
