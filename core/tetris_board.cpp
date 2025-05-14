#include "tetris_board.hpp"

#include <algorithm>
#include <stdexcept>

namespace tetris {

Board::Board(std::int32_t width, std::int32_t height)
    : m_width{width}, m_height{height}, m_roof{0}, m_filledCellCount{0} {

  // Validate dimensions
  [[unlikely]] if (width > maxWidth || height > maxHeight || width < 4 ||
                   height < 4) {
    throw std::invalid_argument("Invalid board dimensions");
  }

  // Clear all data
  m_cells.reset();
  std::ranges::fill(m_columnHeights, 0);
}

bool Board::operator==(const Board& other) const {
  [[unlikely]] if (m_width != other.m_width || m_height != other.m_height) {
    return false;
  }

  // Compare only the active part of the board by checking row by row
  for (std::int32_t y{0}; y < m_height; ++y) {
    std::size_t rowStart{static_cast<std::size_t>(y * maxWidth)};

    // Compare only the cells within the width of the board
    for (std::int32_t x{0}; x < m_width; ++x) {
      if (isFilled(x, y) != other.isFilled(x, y)) {
        return false;
      }
    }
  }

  return true;
}

bool Board::operator!=(const Board& other) const { return !(*this == other); }

bool Board::isFilled(std::int32_t x, std::int32_t y) const {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
    return false;
  }

  return m_cells.test(static_cast<std::size_t>(y * m_width + x));
}

void Board::fillCell(std::int32_t x, std::int32_t y) {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) { return; }

  // Calculate the bit position in the bitset
  std::size_t bitPos{static_cast<std::size_t>(y * maxWidth + x)};

  // If the cell is already filled, do nothing
  if (isFilled(x, y)) {
    return;
  }

  // Set the bit for this cell
  m_cells.set(bitPos);

  // Update column height if needed
  if (y + 1 > m_columnHeights.at(x)) {
    m_columnHeights.at(x) = y + 1;
    m_roof = std::max(m_roof, y + 1);
  }

  // Increment filled cell count
  ++m_filledCellCount;
}

void Board::clearCell(std::int32_t x, std::int32_t y) {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) { return; }

  // If the cell is already empty, do nothing
  if (!isFilled(x, y)) {
    return;
  }

  // Clear the bit for this cell
  m_cells.reset(static_cast<std::size_t>(y * m_width + x));

  // Decrement filled cell count
  --m_filledCellCount;

  // Update column height if needed
  if (y + 1 == m_columnHeights.at(x)) {
    // Find the new height for this column
    std::int32_t newHeight{0};
    for (std::int32_t i{y - 1}; i >= 0; --i) {
      if (isFilled(x, i)) {
        newHeight = i + 1;
        break;
      }
    }
    m_columnHeights.at(x) = newHeight;

    // Recalculate roof if needed
    if (y + 1 == m_roof) {
      updateHeights();
    }
  }
}

std::int32_t Board::getColumnHeight(std::int32_t column) const {
  [[unlikely]] if (column < 0 || column >= m_width) { return 0; }

  return m_columnHeights.at(column);
}

std::int32_t Board::clearFilledRows() {
  std::int32_t rowsCleared{0};

  // Check each row from bottom to top
  for (std::int32_t y{0}; y < m_height; ++y) {
    if (isRowFilled(y)) {
      // This row is full, shift rows down by copying entire rows at once
      for (std::int32_t i{y}; i < m_height - 1; ++i) {
        // Copy entire row at once using a loop over just the width
        for (std::int32_t x{0}; x < m_width; ++x) {
          if (isFilled(x, i + 1)) {
            fillCell(x, i);
          } else {
            clearCell(x, i);
          }
        }
      }

      // Clear the top row
      for (std::int32_t x{0}; x < m_width; ++x) {
        clearCell(x, m_height - 1);
      }

      // Update counters
      ++rowsCleared;
      m_filledCellCount -= m_width;

      // Stay on the same row index since we shifted everything down
      --y;
    }
  }

  // If we cleared any rows, update heights
  if (rowsCleared > 0) {
    updateHeights();
  }

  return rowsCleared;
}

bool Board::isRowFilled(std::int32_t row) const {
  [[unlikely]] if (row < 0 || row >= m_height) { return false; }

  // Check each cell in the row directly
  for (std::int32_t x{0}; x < m_width; ++x) {
    if (!isFilled(x, row)) {
      return false;
    }
  }

  return true;
}

const std::bitset<maxWidth * maxHeight>& Board::getCells() const {
  return m_cells;
}

std::span<const std::int32_t> Board::getColumnHeights() const {
  return {m_columnHeights.data(),
          static_cast<std::span<const std::int32_t>::size_type>(m_width)};
}

void Board::updateHeights() {
  m_roof = 0;

  // Recalculate all column heights
  for (std::int32_t x{0}; x < m_width; ++x) {
    m_columnHeights.at(x) = 0;
    for (std::int32_t y{m_height - 1}; y >= 0; --y) {
      if (isFilled(x, y)) {
        m_columnHeights.at(x) = y + 1;
        m_roof = std::max(m_roof, y + 1);
        break;
      }
    }
  }
}

void Board::updateHeights(std::int32_t column) {
  [[unlikely]] if (column < 0 || column >= m_width) { return; }

  // Recalculate the height just for this column
  m_columnHeights.at(column) = 0;
  for (std::int32_t y{m_height - 1}; y >= 0; --y) {
    if (isFilled(column, y)) {
      m_columnHeights.at(column) = y + 1;
      break;
    }
  }

  // Recalculate the roof
  m_roof = 0;
  for (std::int32_t x{0}; x < m_width; ++x) {
    m_roof = std::max(m_roof, m_columnHeights.at(x));
  }
}

} // namespace tetris