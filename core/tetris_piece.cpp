#include "tetris_piece.hpp"
#include "../rotatoinal_systems/rotation_system.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace tetris {

Piece::Piece(const PieceState& state,
             std::shared_ptr<RotationSystem> rotationSystem)
    : m_state{state}, m_rotationSystem{std::move(rotationSystem)} {
  [[unlikely]] if (!m_rotationSystem) {
    throw std::invalid_argument("Rotation system cannot be null");
  }
  updateShapeData();
  updateDimensions();
}

void Piece::setState(const PieceState& state) {
  m_state = state;
  updateShapeData();
  updateDimensions();
}

void Piece::setRotationSystem(const std::shared_ptr<RotationSystem>& rotationSystem) {
  [[unlikely]] if (!rotationSystem) {
    throw std::invalid_argument("Rotation system cannot be null");
  }
  m_rotationSystem = rotationSystem;
  updateShapeData();
  updateDimensions();
}

std::vector<Position> Piece::getFilledCells() const {
  std::vector<Position> filledCells;
  filledCells.reserve(4); // Tetrominoes have 4 cells

  for (std::int32_t y{0}; y < m_height; ++y) {
    for (std::int32_t x{0}; x < m_width; ++x) {
      if (m_shapeData.test(y * m_width + x)) {
        filledCells.emplace_back(x, y);
      }
    }
  }

  return filledCells;
}

std::vector<Position> Piece::getAbsoluteFilledCells() const {
  std::vector<Position> absoluteFilledCells{getFilledCells()};
  const Position& pos = m_state.getPosition();

  for (auto& cell : absoluteFilledCells) {
    cell.xPos += pos.xPos;
    cell.yPos += pos.yPos;
  }

  return absoluteFilledCells;
}

void Piece::updateShapeData() {
  [[unlikely]] if (!m_rotationSystem) {
    throw std::runtime_error("Rotation system not set");
  }

  const auto& shape =
      m_rotationSystem->getShapeData(m_state.getType(), m_state.getRotation());
  m_shapeData = shape;
}

void Piece::updateDimensions() {
  // Reset dimensions
  m_width = 0;
  m_height = 0;
  std::ranges::fill(m_columnHeights, 0);
  std::ranges::fill(m_columnBottoms, maxSize);

  // Find the actual dimensions of the piece
  for (std::int32_t y{0}; y < static_cast<std::int32_t>(maxSize); ++y) {
    bool rowHasFilledCells = false;

    for (std::int32_t x{0}; x < static_cast<std::int32_t>(maxSize); ++x) {
      if (m_shapeData.test(y * maxSize + x)) {
        rowHasFilledCells = true;
        m_width = std::max(m_width, x + 1);
        m_columnHeights.at(x) = std::max(m_columnHeights.at(x), y + 1);
        m_columnBottoms.at(x) = std::min(m_columnBottoms.at(x), y);
      }
    }

    if (rowHasFilledCells) {
      m_height = y + 1;
    }
  }
}

} // namespace tetris
