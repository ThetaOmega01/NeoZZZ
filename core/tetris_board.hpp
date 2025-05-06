#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <span>

namespace tetris {

/**
 * @brief Maximum height of a Tetris board
 */
constexpr std::int64_t maxHeight{40};

/**
 * @brief Maximum width of a Tetris board
 */
constexpr std::int64_t maxWidth{32};

/**
 * @class Board
 * @brief Tetris game board
 *
 * (0,0) is the bottom-left corner.
 * The board supports a maximum size of maxHeight * maxWidth.
 */
class Board {
public:
  /**
   * @brief Construct a board with specified dimensions
   *
   * @param width The width of the board
   * @param height The height of the board
   */
  Board(std::int64_t width, std::int64_t height);

  /**
   * @brief Equality operator
   */
  bool operator==(const Board &other) const;

  /**
   * @brief Inequality operator
   */
  bool operator!=(const Board &other) const;

  /**
   * @brief Fill a cell at the given coordinates
   *
   * @param x X-coordinate (column)
   * @param y Y-coordinate (row)
   */
  void fillCell(std::int64_t x, std::int64_t y);

  /**
   * @brief Clear a cell at the given coordinates
   *
   * @param x X-coordinate (column)
   * @param y Y-coordinate (row)
   */
  void clearCell(std::int64_t x, std::int64_t y);

  /**
   * @brief Get the width of the board
   *
   * @return The width
   */
  [[nodiscard]] std::int64_t getWidth() const { return m_width; }

  /**
   * @brief Get the height of the board
   *
   * @return The height
   */
  [[nodiscard]] std::int64_t getHeight() const { return m_height; }

  /**
   * @brief Get the current highest filled cell in the board
   *
   * @return The height of the highest filled cell
   */
  [[nodiscard]] std::int64_t getRoof() const { return m_roof; }

  /**
   * @brief Get the number of filled cells in the board
   *
   * @return The count of filled cells
   */
  [[nodiscard]] std::int64_t getFilledCellCount() const {
    return m_filledCellCount;
  }

  /**
   * @brief Get the height of the highest filled cell in a column
   *
   * @param column The column index
   * @return The height of the highest filled cell in the column
   */
  [[nodiscard]] std::int64_t getColumnHeight(std::int64_t column) const;

  /**
   * @brief Clear filled rows and update the board state
   *
   * @return The number of rows cleared
   */
  std::int64_t clearFilledRows();

  /**
   * @brief Check if a row is completely filled
   *
   * @param row The row index
   * @return true if the row is filled, false otherwise
   */
  [[nodiscard]] bool isRowFilled(std::int64_t row) const;

  /**
   * @brief Get a read-only reference to the board cells
   *
   * @return A reference to the bitset representing the board
   */
  [[nodiscard]] const std::bitset<maxWidth * maxHeight> &getCells() const;

  /**
   * @brief Get a read-only view of the column heights
   *
   * @return A span of the column heights
   */
  [[nodiscard]] std::span<const std::int64_t> getColumnHeights() const;

private:
  /**
   * @brief Update all column heights and roof after board changes
   */
  void updateHeights();

  /**
   * @brief Update a specific column height and recalculate roof
   *
   * @param column The column to update
   */
  void updateHeights(std::int64_t column);

private:
  std::bitset<maxWidth * maxHeight>
      m_cells{}; ///< Bit representation of the board
  std::array<std::int64_t, maxWidth>
      m_columnHeights{};            ///< Height of each column
  std::int64_t m_width{};           ///< Width of the board
  std::int64_t m_height{};          ///< Height of the board
  std::int64_t m_roof{};            ///< Current highest filled cell
  std::int64_t m_filledCellCount{}; ///< Number of filled cells
};

} // namespace tetris
