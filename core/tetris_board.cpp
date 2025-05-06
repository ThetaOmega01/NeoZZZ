#include "tetris_board.hpp"

#include <algorithm>
#include <stdexcept>

namespace tetris {

Board::Board(int width, int height)
    : m_width{width}, m_height{height}, m_filledCellCount{0}, m_roof{0} {

  // Validate dimensions
  [[unlikely]] if (width > maxWidth || height > maxHeight || width < 4 ||
                   height < 4) {
    throw std::invalid_argument("Invalid board dimensions");
  }

  // Clear all data
  std::fill(m_rows.begin(), m_rows.end(), 0);
  std::fill(m_columnHeights.begin(), m_columnHeights.end(), 0);

  // Create bit mask for a full row
  m_fullRowMask = (width == maxWidth) ? 0xFFFFFFFFU : (1U << width) - 1;
}

bool Board::operator==(const Board &other) const {
  if (m_width != other.m_width || m_height != other.m_height) {
    return false;
  }

  // Compare only the active part of the board
  return std::ranges::equal(
      std::span<const std::uint32_t>(m_rows.data(), m_height),
      std::span<const std::uint32_t>(other.m_rows.data(), other.m_height));
}

bool Board::operator!=(const Board &other) const { return !(*this == other); }

bool Board::isFilled(int x, int y) const {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
    return false;
  }

  return (m_rows[y] >> x) & 1;
}

void Board::fillCell(int x, int y) {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
    return;
  }

  // If the cell is already filled, do nothing
  if (isFilled(x, y)) {
    return;
  }

  // Set the bit for this cell
  m_rows[y] |= (1U << x);

  // Update column height if needed
  if (y + 1 > m_columnHeights[x]) {
    m_columnHeights[x] = y + 1;
    m_roof = std::max(m_roof, y + 1);
  }

  // Increment filled cell count
  ++m_filledCellCount;
}

void Board::clearCell(int x, int y) {
  [[unlikely]] if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
    return;
  }

  // If the cell is already empty, do nothing
  if (!isFilled(x, y)) {
    return;
  }

  // Clear the bit for this cell
  m_rows[y] &= ~(1U << x);

  // Decrement filled cell count
  --m_filledCellCount;

  // Update column height if needed
  if (y + 1 == m_columnHeights[x]) {
    // Find the new height for this column
    int newHeight{0};
    for (int i{y - 1}; i >= 0; --i) {
      if (isFilled(x, i)) {
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

int Board::getColumnHeight(int column) const {
  [[unlikely]] if (column < 0 || column >= m_width) {
    return 0;
  }

  return m_columnHeights[column];
}

int Board::clearFilledRows() {
  int rowsCleared{0};

  // Check each row from bottom to top
  for (int y{0}; y < m_height; ++y) {
    if (m_rows[y] == m_fullRowMask) {
      // This row is full, clear it and shift rows down
      for (int i{y}; i < m_height - 1; ++i) {
        m_rows[i] = m_rows[i + 1];
      }

      // Clear the top row
      m_rows[m_height - 1] = 0;

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

bool Board::isRowFilled(int row) const {
  [[unlikely]] if (row < 0 || row >= m_height) {
    return false;
  }

  return m_rows[row] == m_fullRowMask;
}

std::span<const std::uint32_t> Board::getRowData() const {
  return std::span<const std::uint32_t>(m_rows.data(), m_height);
}

std::span<const int> Board::getColumnHeights() const {
  return std::span<const int>(m_columnHeights.data(), m_width);
}

void Board::updateHeights() {
  m_roof = 0;

  // Recalculate all column heights
  for (int x{0}; x < m_width; ++x) {
    m_columnHeights[x] = 0;
    for (int y{m_height - 1}; y >= 0; --y) {
      if (isFilled(x, y)) {
        m_columnHeights[x] = y + 1;
        m_roof = std::max(m_roof, y + 1);
        break;
      }
    }
  }
}

void Board::updateHeights(int column) {
  [[unlikely]] if (column < 0 || column >= m_width) {
    return;
  }

  // Recalculate the height just for this column
  m_columnHeights[column] = 0;
  for (int y{m_height - 1}; y >= 0; --y) {
    if (isFilled(column, y)) {
      m_columnHeights[column] = y + 1;
      break;
    }
  }

  // Recalculate the roof
  for (int x{0}; x < m_width; ++x) {
    m_roof = std::max(m_roof, m_columnHeights[x]);
  }
}

} // namespace tetris
