#include "srs.hpp"
#include <stdexcept>

/* Reference: https://harddrop.com/wiki/SRS */

namespace {
// Define the shape data for each piece type and rotation

using ShapeDatum = std::bitset<tetris::Piece::maxSize * tetris::Piece::maxSize>;
using ShapeData = std::array<ShapeDatum, 4>;

// I piece
constexpr ShapeData iShapeData = {{// Rotation 0 (spawn)
                                   // □ □ □ □
                                   // ■ ■ ■ ■
                                   // □ □ □ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000111100000000),
                                   // Rotation R
                                   // □ □ ■ □
                                   // □ □ ■ □
                                   // □ □ ■ □
                                   // □ □ ■ □
                                   ShapeDatum(0b0010001000100010),
                                   // Rotation 2
                                   // □ □ □ □
                                   // □ □ □ □
                                   // ■ ■ ■ ■
                                   // □ □ □ □
                                   ShapeDatum(0b0000000011110000),
                                   // Rotation L
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0100010001000100)}};

// O piece (same for all rotations)
constexpr ShapeData oShapeData = {{// □ □ □ □
                                   // □ ■ ■ □
                                   // □ ■ ■ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000011001100000),
                                   // Same shape for all rotations
                                   ShapeDatum(0b0000011001100000),
                                   ShapeDatum(0b0000011001100000),
                                   ShapeDatum(0b0000011001100000)}};

// T piece
constexpr ShapeData tShapeData = {{// Rotation 0 (spawn)
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // ■ ■ ■ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000010011100000),
                                   // Rotation R
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // □ ■ ■ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000010001100100),
                                   // Rotation 2
                                   // □ □ □ □
                                   // □ □ □ □
                                   // ■ ■ ■ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000000011100100),
                                   // Rotation L
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // ■ ■ □ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000010011000100)}};

// L piece
constexpr ShapeData lShapeData = {{// Rotation 0 (spawn)
                                   // □ □ □ □
                                   // □ □ ■ □
                                   // ■ ■ ■ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000001011100000),
                                   // Rotation R
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   // □ ■ ■ □
                                   ShapeDatum(0b0000010001000110),
                                   // Rotation 2
                                   // □ □ □ □
                                   // □ □ □ □
                                   // ■ ■ ■ □
                                   // ■ □ □ □
                                   ShapeDatum(0b0000000011101000),
                                   // Rotation L
                                   // □ □ □ □
                                   // ■ ■ □ □
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000110001000100)}};

// J piece
constexpr ShapeData jShapeData = {{// Rotation 0 (spawn)
                                   // □ □ □ □
                                   // ■ □ □ □
                                   // ■ ■ ■ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000100011100000),
                                   // Rotation R
                                   // □ □ □ □
                                   // □ ■ ■ □
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000011001000100),
                                   // Rotation 2
                                   // □ □ □ □
                                   // □ □ □ □
                                   // ■ ■ ■ □
                                   // □ □ ■ □
                                   ShapeDatum(0b0000000011100010),
                                   // Rotation L
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // □ ■ □ □
                                   // ■ ■ □ □
                                   ShapeDatum(0b0000010001001100)}};

// S piece
constexpr ShapeData sShapeData = {{// Rotation 0 (spawn)
                                   // □ □ □ □
                                   // □ ■ ■ □
                                   // ■ ■ □ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000011011000000),
                                   // Rotation R
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // □ ■ ■ □
                                   // □ □ ■ □
                                   ShapeDatum(0b0000010001100010),
                                   // Rotation 2
                                   // □ □ □ □
                                   // □ □ □ □
                                   // □ ■ ■ □
                                   // ■ ■ □ □
                                   ShapeDatum(0b0000000001101100),
                                   // Rotation L
                                   // □ □ □ □
                                   // ■ □ □ □
                                   // ■ ■ □ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000100011000100)}};

// Z piece
constexpr ShapeData zShapeData = {{// Rotation 0 (spawn)
                                   // □ □ □ □
                                   // ■ ■ □ □
                                   // □ ■ ■ □
                                   // □ □ □ □
                                   ShapeDatum(0b0000110001100000),
                                   // Rotation R
                                   // □ □ □ □
                                   // □ □ ■ □
                                   // □ ■ ■ □
                                   // □ ■ □ □
                                   ShapeDatum(0b0000001001100100),
                                   // Rotation 2
                                   // □ □ □ □
                                   // □ □ □ □
                                   // ■ ■ □ □
                                   // □ ■ ■ □
                                   ShapeDatum(0b0000000011000110),
                                   // Rotation L
                                   // □ □ □ □
                                   // □ ■ □ □
                                   // ■ ■ □ □
                                   // ■ □ □ □
                                   ShapeDatum(0b0000010011001000)}};

// J/L/S/T/Z Pieces
const std::array<std::array<tetris::WallKickData, 4>, 2>
    jlstzWallKickData = {
        {// Clockwise rotation (0->R, R->2, 2->L, L->0)
         {{tetris::WallKickData(
               {{+0, +0}, {-1, +0}, {-1, +1}, {+0, -2}, {-1, -2}}),
           tetris::WallKickData(
               {{+0, +0}, {+1, +0}, {+1, -1}, {+0, +2}, {+1, +2}}),
           tetris::WallKickData(
               {{+0, +0}, {+1, +0}, {+1, +1}, {+0, -2}, {+1, -2}}),
           tetris::WallKickData(
               {{+0, +0}, {-1, +0}, {-1, -1}, {+0, +2}, {-1, +2}})}},
         // Counterclockwise rotation (0->L, L->2, 2->R, R->0)
         {{tetris::WallKickData(
               {{+0, +0}, {+1, +0}, {+1, +1}, {+0, -2}, {+1, -2}}),
           tetris::WallKickData(
               {{+0, +0}, {+1, +0}, {+1, -1}, {+0, +2}, {+1, +2}}),
           tetris::WallKickData(
               {{+0, +0}, {-1, +0}, {-1, +1}, {+0, -2}, {-1, -2}}),
           tetris::WallKickData(
               {{+0, +0}, {-1, +0}, {-1, -1}, {+0, +2}, {-1, +2}})}}}};

// I Piece
const std::array<std::array<tetris::WallKickData, 4>, 2> iWallKickData =
    {{// Clockwise rotation (0->R, R->2, 2->L, L->0)
      {{tetris::WallKickData(
            {{+0, +0}, {-2, +0}, {+1, +0}, {-2, -1}, {+1, +2}}),
        tetris::WallKickData(
            {{+0, +0}, {-1, +0}, {+2, +0}, {-1, +2}, {+2, -1}}),
        tetris::WallKickData(
            {{+0, +0}, {+2, +0}, {-1, +0}, {+2, +1}, {-1, -2}}),
        tetris::WallKickData(
            {{+0, +0}, {+1, +0}, {-2, +0}, {+1, -2}, {-2, +1}})}},
      // Counterclockwise rotation (0->L, L->2, 2->R, R->0)
      {{tetris::WallKickData(
            {{+0, +0}, {-1, +0}, {+2, +0}, {-1, +2}, {+2, -1}}),
        tetris::WallKickData(
            {{+0, +0}, {+2, +0}, {-1, +0}, {+2, +1}, {-1, -2}}),
        tetris::WallKickData(
            {{+0, +0}, {+1, +0}, {-2, +0}, {+1, -2}, {-2, +1}}),
        tetris::WallKickData(
            {{+0, +0}, {-2, +0}, {+1, +0}, {-2, -1}, {+1, +2}})}}}};

// O Piece (and empty kicks)
const tetris::WallKickData emptyWallKicksData({{+0, +0}});

} // namespace

namespace tetris {

SRS::SRS() = default;

WallKickData SRS::getClockwiseWallKicks(const PieceType type,
                                        Rotation fromRotation) const {
  const auto rotIndex = static_cast<std::size_t>(fromRotation);

  switch (type) {
  case PieceType::I:
    return iWallKickData.at(0).at(rotIndex);
  case PieceType::O:
    return emptyWallKicksData;
  case PieceType::J:
  case PieceType::L:
  case PieceType::S:
  case PieceType::T:
  case PieceType::Z:
    return jlstzWallKickData.at(0).at(rotIndex);
  default:
    throw std::invalid_argument("Invalid piece type for wall kicks");
  }
}

WallKickData SRS::getCounterClockwiseWallKicks(const PieceType type,
                                               Rotation fromRotation) const {
  const auto rotIndex = static_cast<std::size_t>(fromRotation);

  switch (type) {
  case PieceType::I:
    return iWallKickData.at(1).at(rotIndex);
  case PieceType::O:
    return emptyWallKicksData;
  case PieceType::J:
  case PieceType::L:
  case PieceType::S:
  case PieceType::T:
  case PieceType::Z:
    return jlstzWallKickData.at(1).at(rotIndex);
  default:
    throw std::invalid_argument("Invalid piece type for wall kicks");
  }
}

WallKickData SRS::get180WallKicks(PieceType, Rotation) const {
  return emptyWallKicksData;
}

PieceState SRS::getInitialState(PieceType type, const int32_t boardWidth,
                                const int32_t boardHeight) const {
  // Calculate spawn position based on the SRS rules
  // Pieces spawn centered at the top of the board
  const int32_t x = (boardWidth - 4) / 2;

  // In SRS, pieces spawn with their bottom at y=21 (0-indexed)
  // Adjust for the actual board height
  const int32_t y = std::min(21, boardHeight - 1);

  return {type, Position(x, y), Rotation::R0};
}

std::bitset<Piece::maxSize * Piece::maxSize>
SRS::getShapeData(const PieceType type, Rotation rotation) const {
  const auto rotIndex = static_cast<std::size_t>(rotation);

  switch (type) {
  case PieceType::I:
    return iShapeData[rotIndex];
  case PieceType::O:
    return oShapeData[rotIndex];
  case PieceType::T:
    return tShapeData[rotIndex];
  case PieceType::L:
    return lShapeData[rotIndex];
  case PieceType::J:
    return jShapeData[rotIndex];
  case PieceType::S:
    return sShapeData[rotIndex];
  case PieceType::Z:
    return zShapeData[rotIndex];
  default:
    throw std::invalid_argument("Invalid piece type");
  }
}

std::shared_ptr<RotationSystem> SRS::clone() const {
  return std::make_shared<SRS>();
}

} // namespace tetris
