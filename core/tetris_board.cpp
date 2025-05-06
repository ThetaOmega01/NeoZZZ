#include "tetris_board.hpp"

#include <algorithm>
#include <stdexcept>

namespace tetris {

Board::Board(std::int64_t width, std::int64_t height)
    : m_width{width}, m_height{height}, m_filledCellCount{0}, m_roof{0} {

  // Validate dimensions
  [[unlikely]] if (width > maxWidth || height > maxHeight || width < 4 ||
                   height < 4) {
    throw std::invalid_argument("Invalid board dimensions");
  }

  // Clear all data
  m_cells.reset();
  std::fill(m_columnHeights.begin(), m_columnHeights.end(), 0);
}

bool Board::operator==(const Board &other) const {
  [[unlikely]] if (m_width != other.m_width || m_height != other.m_height) {
    return false;
  }

  // Compare only the active part of the board by checking row by row
  for (std::int64_t y{0}; y < m_height; ++y) {
    std::size_t rowStart{static_cast<std::size_t>(y * maxWidth)};

    // Compare only the cells within the width of the board
    for (std::int64_t x{0}; x < m_width; ++x) {
      if (m_cells[rowStart + x] != other.m_cells[rowStart + x]) {
        return false;
      }
    }
  }

  return true;
}

bool Board::operator!=(const Board &other) const { return !(*this == other); }

void Board::fillCell(std::int64_t x, std::int64_t y) {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) { return; }

  // Calculate the bit position in the bitset
  std::size_t bitPos{static_cast<std::size_t>(y * maxWidth + x)};

  // If the cell is already filled, do nothing
  if (m_cells[bitPos]) {
    return;
  }

  // Set the bit for this cell
  m_cells.set(bitPos);

  // Update column height if needed
  if (y + 1 > m_columnHeights[x]) {
    m_columnHeights[x] = y + 1;
    m_roof = std::max(m_roof, y + 1);
  }

  // Increment filled cell count
  ++m_filledCellCount;
}

void Board::clearCell(std::int64_t x, std::int64_t y) {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) { return; }

  // Calculate the bit position in the bitset
  std::size_t bitPos{static_cast<std::size_t>(y * maxWidth + x)};

  // If the cell is already empty, do nothing
  if (!m_cells[bitPos]) {
    return;
  }

  // Clear the bit for this cell
  m_cells.reset(bitPos);

  // Decrement filled cell count
  --m_filledCellCount;

  // Update column height if needed
  if (y + 1 == m_columnHeights[x]) {
    // Find the new height for this column
    std::int64_t newHeight{0};
    for (std::int64_t i{y - 1}; i >= 0; --i) {
      std::size_t checkPos{static_cast<std::size_t>(i * maxWidth + x)};
      if (m_cells[checkPos]) {
        newHeight = i + 1;
        break;
      }
    }
    m_columnHeights[x] = newHeight;

    // Recalculate roof if needed
    if (y + 1 == m_roof) {
      updateHeights();
    }
  }
}

std::int64_t Board::getColumnHeight(std::int64_t column) const {
  [[unlikely]] if (column < 0 || column >= m_width) { return 0; }

  return m_columnHeights[column];
}

std::int64_t Board::clearFilledRows() {
  std::int64_t rowsCleared{0};

  // Check each row from bottom to top
  for (std::int64_t y{0}; y < m_height; ++y) {
    if (isRowFilled(y)) {
      // This row is full, shift rows down by copying entire rows at once
      for (std::int64_t i{y}; i < m_height - 1; ++i) {
        // Calculate start positions for current row and row above
        std::size_t currentRowStart{static_cast<std::size_t>(i * maxWidth)};
        std::size_t aboveRowStart{static_cast<std::size_t>((i + 1) * maxWidth)};

        // Copy entire row at once using a loop over just the width
        for (std::int64_t x{0}; x < m_width; ++x) {
          if (m_cells[aboveRowStart + x]) {
            m_cells.set(currentRowStart + x);
          } else {
            m_cells.reset(currentRowStart + x);
          }
        }
      }

      // Clear the top row
      std::size_t topRowStart{
          static_cast<std::size_t>((m_height - 1) * maxWidth)};
      for (std::int64_t x{0}; x < m_width; ++x) {
        m_cells.reset(topRowStart + x);
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

bool Board::isRowFilled(std::int64_t row) const {
  [[unlikely]] if (row < 0 || row >= m_height) { return false; }

  // Calculate the start position for this row
  std::size_t rowStart{static_cast<std::size_t>(row * maxWidth)};

  // Check each cell in the row directly
  for (std::int64_t x{0}; x < m_width; ++x) {
    if (!m_cells[rowStart + x]) {
      return false;
    }
  }

  return true;
}

const std::bitset<maxWidth * maxHeight> &Board::getCells() const {
  return m_cells;
}

std::span<const std::int64_t> Board::getColumnHeights() const {
  return std::span<const std::int64_t>(m_columnHeights.data(), m_width);
}

void Board::updateHeights() {
  m_roof = 0;

  // Recalculate all column heights
  for (std::int64_t x{0}; x < m_width; ++x) {
    m_columnHeights[x] = 0;
    for (std::int64_t y{m_height - 1}; y >= 0; --y) {
      std::size_t bitPos{static_cast<std::size_t>(y * maxWidth + x)};
      if (m_cells[bitPos]) {
        m_columnHeights[x] = y + 1;
        m_roof = std::max(m_roof, y + 1);
        break;
      }
    }
  }
}

void Board::updateHeights(std::int64_t column) {
  [[unlikely]] if (column < 0 || column >= m_width) { return; }

  // Recalculate the height just for this column
  m_columnHeights[column] = 0;
  for (std::int64_t y{m_height - 1}; y >= 0; --y) {
    std::size_t bitPos{static_cast<std::size_t>(y * maxWidth + column)};
    if (m_cells[bitPos]) {
      m_columnHeights[column] = y + 1;
      break;
    }
  }

  // Recalculate the roof
  m_roof = 0;
  for (std::int64_t x{0}; x < m_width; ++x) {
    m_roof = std::max(m_roof, m_columnHeights[x]);
  }
}

} // namespace tetris
